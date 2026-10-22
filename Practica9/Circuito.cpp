#include "Circuito.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
namespace { constexpr double pi=3.14159265358979323846;
void vert(std::vector<float>&v,glm::vec3 p,float u,float t,glm::vec3 n){v.insert(v.end(),{p.x,p.y,p.z,u,t,n.x,n.y,n.z});}
void caja(std::vector<float>&v,std::vector<unsigned>&idx,glm::vec3 lo,glm::vec3 hi){
    glm::vec3 p[8]={{lo.x,lo.y,lo.z},{hi.x,lo.y,lo.z},{hi.x,hi.y,lo.z},{lo.x,hi.y,lo.z},
        {lo.x,lo.y,hi.z},{hi.x,lo.y,hi.z},{hi.x,hi.y,hi.z},{lo.x,hi.y,hi.z}};
    int f[6][4]={{0,3,2,1},{4,5,6,7},{0,4,7,3},{1,2,6,5},{3,7,6,2},{0,1,5,4}};
    for(auto&q:f){auto n=glm::normalize(glm::cross(p[q[1]]-p[q[0]],p[q[2]]-p[q[0]]));unsigned b=unsigned(v.size()/8);
        for(int k:q)vert(v,p[k],0,0,n);idx.insert(idx.end(),{b,b+1,b+2,b,b+2,b+3});}
}
}
PuntoRuta Circuito::Evaluar(double s) const {
    s=std::fmod(s,longitudPlana);if(s<0)s+=longitudPlana;
    double x=0,z=0,dx=0,dz=0,alt=0,pendiente=0;const double a=25*pi/2;
    auto curva=[&](double cx,double cz,double ang){x=cx+25*std::cos(ang);z=cz+25*std::sin(ang);dx=-std::sin(ang);dz=std::cos(ang);};
    if(s<130){x=-65+s;z=-55;dx=1;alt=6;}
    else if((s-=130)<a)curva(65,-30,-pi/2+s/25);
    else if((s-=a)<60){x=90;z=-30+s;dz=1;}
    else if((s-=60)<a)curva(65,30,s/25);
    else if((s-=a)<130){x=65-s;z=55;dx=-1;alt=4;}
    else if((s-=130)<a)curva(-65,30,pi/2+s/25);
    else if((s-=a)<60){x=-90;z=30-s;dz=-1;}
    else {s-=60;curva(-65,-30,pi+s/25);}
    double y=0;
    // Seno al cuadrado: altura y pendiente continuas al entrar y salir.
    if(alt>0&&x>-30&&x<30){double t=pi*(x+30)/60;y=alt*std::sin(t)*std::sin(t);pendiente=alt*pi/60*std::sin(2*t)*dx;}
    PuntoRuta p;p.posicion={float(x),float(y),float(z)};
    p.frente=glm::normalize(glm::vec3(float(dx),float(pendiente),float(dz)));
    p.derecha=glm::normalize(glm::cross(glm::vec3(0,1,0),p.frente));p.arriba=glm::cross(p.frente,p.derecha);return p;
}
Circuito::Circuito(){
    const int n=12000;parametros.reserve(n+1);acumuladas.reserve(n+1);
    parametros.push_back(0);acumuladas.push_back(0);auto anterior=Evaluar(0).posicion;
    for(int i=1;i<=n;i++){double s=longitudPlana*i/n;auto p=Evaluar(s).posicion;
        parametros.push_back(s);acumuladas.push_back(acumuladas.back()+glm::length(p-anterior));anterior=p;}
}
PuntoRuta Circuito::PorDistancia(double d) const{
    d=std::fmod(d,Longitud());if(d<0)d+=Longitud();
    size_t i=std::upper_bound(acumuladas.begin(),acumuladas.end(),d)-acumuladas.begin();
    i=std::min(i,acumuladas.size()-1);double f=(d-acumuladas[i-1])/(acumuladas[i]-acumuladas[i-1]);
    return Evaluar(parametros[i-1]+f*(parametros[i]-parametros[i-1]));
}
double Circuito::DistanciaEn(double s) const{
    s=std::max(0.0,std::min(s,longitudPlana));size_t i=std::min(size_t(s/longitudPlana*(parametros.size()-1)),parametros.size()-2);
    double f=(s-parametros[i])/(parametros[i+1]-parametros[i]);return acumuladas[i]+f*(acumuladas[i+1]-acumuladas[i]);
}
void Circuito::Franja(MeshModel&mesh,float izq,float der,float altura){
    std::vector<float>v;std::vector<unsigned>idx;const int n=2400;
    for(int i=0;i<=n;i++){auto p=Evaluar(longitudPlana*i/n);
        for(float lado:{izq,der})vert(v,p.posicion+p.derecha*lado+glm::vec3(0,altura,0),(lado-izq)/(der-izq),float(i)*.16f,p.arriba);
        if(i<n){unsigned a=2*i;idx.insert(idx.end(),{a,a+2,a+1,a+1,a+2,a+3});}}
    mesh.CreateMeshModel(v.data(),idx.data(),unsigned(v.size()),unsigned(idx.size()));
}
void Circuito::CrearMallas(){
    Franja(superficie,-7,7,0);Franja(bordeInterior,-7,-6.55f,.035f);Franja(bordeExterior,6.55f,7,.035f);
    float v[]={-120,-.35f,-85,0,0,0,1,0,120,-.35f,-85,24,0,0,1,0,120,-.35f,85,24,17,0,1,0,-120,-.35f,85,0,17,0,1,0};
    unsigned idx[]={0,2,1,0,3,2};piso.CreateMeshModel(v,idx,32,6);
    std::vector<float>a;std::vector<unsigned>b;
    // Luz libre interior: 18 de ancho y 13 de altura, sobre la recta X=90.
    caja(a,b,{79,0,-1.2f},{81,13,1.2f});caja(a,b,{99,0,-1.2f},{101,13,1.2f});caja(a,b,{79,13,-1.2f},{101,15,1.2f});
    arco.CreateMeshModel(a.data(),b.data(),unsigned(a.size()),unsigned(b.size()));
}
bool Circuito::Verificar() const{
    const double a=25*pi/2;
    for(double s:{0.,130.,130+a,190+a,190+2*a,320+2*a,320+3*a,380+3*a,longitudPlana}){
        auto l=Evaluar(s-.0001),r=Evaluar(s+.0001);
        if(glm::length(l.posicion-r.posicion)>.001f||glm::dot(l.frente,r.frente)<.99999f)return false;}
    if(std::abs(Evaluar(65).posicion.y-6)>.0001||std::abs(Evaluar(255+2*a).posicion.y-4)>.0001)return false;
    float xmin=1000,xmax=-1000,zmin=1000,zmax=-1000;
    for(int i=0;i<20000;i++){auto p=Evaluar(longitudPlana*i/20000);xmin=std::min(xmin,p.posicion.x);xmax=std::max(xmax,p.posicion.x);zmin=std::min(zmin,p.posicion.z);zmax=std::max(zmax,p.posicion.z);
        if(std::abs(glm::length(p.derecha*14.f)-14)>.0001)return false;
        if(p.posicion.y<-.0001||p.posicion.y>6.0001)return false;}
    if(std::abs(xmin+90)+std::abs(xmax-90)+std::abs(zmin+55)+std::abs(zmax-55)>.001)return false;
    for(auto c:std::vector<glm::vec2>{{65,-30},{65,30},{-65,30},{-65,-30}}){
        bool encontrado=false;for(int i=0;i<2000;i++){auto p=Evaluar(longitudPlana*i/2000).posicion;
            if(std::abs(p.x)>65.01&&std::abs(p.z)>30.01&&((p.x>0)==(c.x>0))&&((p.z>0)==(c.y>0))){
                if(std::abs(glm::length(glm::vec2(p.x,p.z)-c)-25)>.001)return false;encontrado=true;}}
        if(!encontrado)return false;}
    printf("Pista OK: X +/-90, Z +/-55, radio 25, ancho 14; rampas 6/4; longitud 3D %.4f\n",Longitud());return true;
}
