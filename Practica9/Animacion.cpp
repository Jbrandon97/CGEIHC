#include "Animacion.h"
#include <cmath>
#include <algorithm>
#include <cstdio>
namespace {
glm::mat4 orientar(glm::vec3 pos,glm::vec3 frente){
    auto derecha=glm::normalize(glm::cross(glm::vec3(0,1,0),frente));auto arriba=glm::cross(frente,derecha);
    glm::mat4 m(1);m[0]=glm::vec4(derecha,0);m[1]=glm::vec4(arriba,0);m[2]=glm::vec4(frente,0);m[3]=glm::vec4(pos,1);return m;
}
}
void Animacion::Actualizar(double dt,const Circuito&c){dt=std::max(0.0,dt);tiempo+=dt;distancia=std::fmod(distancia+velocidad*dt,c.Longitud());}
glm::mat4 Animacion::Nave(const Circuito&c) const{
    auto p=c.PorDistancia(distancia);
    // Morro de la Naboo original hacia +Z. Centrar X/Z sin modificar el OBJ.
    auto m=orientar(p.posicion+glm::vec3(0,1.5f,0),p.frente);
    return glm::translate(glm::scale(m,glm::vec3(.24f)),glm::vec3(.7748545f,-.928624f,-.124113f));
}
glm::vec3 Animacion::PosicionSatelite() const {double t=std::fmod(tiempo*.12,6.283185307179586);return {float(35*cos(t)),26,float(22*sin(t))};}
glm::vec3 Animacion::Pivote(int i){
    // Centros de bisagras Y y del anillo de base de antena, medidos en los OBJ originales.
    if(i==1)return {-2.358219f,-.000921f,-.8970145f};
    if(i==2)return {2.358219f,-.000921f,-.8970145f};
    return {-2.370542f,-.00012f,-.084987f};
}
void Animacion::Satelite(glm::mat4*m) const{
    double t=std::fmod(tiempo*.12,6.283185307179586);
    glm::vec3 frente=glm::normalize(glm::vec3(float(-35*sin(t)),0,float(22*cos(t))));
    // Las mallas del satelite estan exportadas con Z vertical; convertir una sola vez en la raiz.
    m[0]=glm::scale(glm::rotate(orientar(PosicionSatelite(),frente),glm::radians(-90.f),glm::vec3(1,0,0)),glm::vec3(.65f));
    float angulos[]={0,float(8*sin(tiempo*.55)),float(-8*sin(tiempo*.55)),float(7*sin(tiempo*.4))};
    for(int i=1;i<4;i++){
        auto p=Pivote(i);m[i]=glm::translate(glm::rotate(glm::translate(m[0],p),glm::radians(angulos[i]),glm::vec3(0,1,0)),-p);
    }
}
bool VerificarAnimacion(const Circuito&c){
    Animacion finales[3];int tasas[]={30,60,144};
    for(int k=0;k<3;k++){
        auto&a=finales[k];auto previo=c.PorDistancia(0).posicion;
        for(int j=0;j<180*tasas[k];j++){
            a.Actualizar(1.0/tasas[k],c);auto p=c.PorDistancia(a.distancia);auto nave=a.Nave(c);
            if(glm::length(p.posicion-previo)>18.1f/tasas[k])return false;previo=p.posicion;
            if(glm::dot(glm::normalize(glm::vec3(nave*glm::vec4(0,0,1,0))),p.frente)<.99999f)return false;
            auto base=glm::vec3(nave*glm::vec4(-.7748545f,.928624f,.124113f,1));
            if(glm::length(base-p.posicion-glm::vec3(0,1.5f,0))>.0001)return false;
            glm::mat4 m[4];a.Satelite(m);
            for(int i=1;i<4;i++)if(glm::length(glm::vec3(m[0]*glm::vec4(Animacion::Pivote(i),1)-m[i]*glm::vec4(Animacion::Pivote(i),1)))>.0001)return false;
        }
    }
    for(int k=1;k<3;k++)if(std::abs(finales[k].distancia-finales[0].distancia)>1e-6)return false;
    puts("Animacion OK: 180 s a 30/60/144 FPS, continuidad, frente, altura y pivotes fijos.");return true;
}
