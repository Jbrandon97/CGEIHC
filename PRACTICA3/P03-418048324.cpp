// Reporte de practica 3 - 418048324.
// Base: clases Window, Camera, Shader, Mesh y Sphere del laboratorio.
// Una sola escena: cohete a la izquierda y cruz de ocho piramides a la derecha.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <vector>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Sphere.h"

const float PI=3.14159265359f;
glm::mat4 mover(float x,float y,float z){return glm::translate(glm::mat4(1),glm::vec3(x,y,z));}
glm::mat4 escalar(float x,float y,float z){return glm::scale(glm::mat4(1),glm::vec3(x,y,z));}
glm::mat4 girar(float ang,glm::vec3 eje){return glm::rotate(glm::mat4(1),glm::radians(ang),eje);}

void crearCubo(Mesh& cubo){
 GLfloat v[]={-.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,.5f,.5f,-.5f,.5f,.5f,
 -.5f,-.5f,-.5f,.5f,-.5f,-.5f,.5f,.5f,-.5f,-.5f,.5f,-.5f};
 unsigned i[]={0,1,2,2,3,0,1,5,6,6,2,1,5,4,7,7,6,5,4,0,3,3,7,4,4,5,1,1,0,4,3,2,6,6,7,3};
 cubo.CreateMesh(v,i,24,36);
}
// Una piramide cuadrangular COMPLETA: 4 triangulos y base de 2 triangulos.
// Se duplican vertices para que los colores de caras vecinas no se mezclen.
void crearPiramide(MeshColor& piramide){
 const glm::vec3 p[]={{-.62f,0,.62f},{.62f,0,.62f},{.62f,0,-.62f},{-.62f,0,-.62f},{0,1.08f,0}};
 const glm::vec3 c[]={{1,0,0},{0,1,0},{1,1,0},{1,0,1},{0,0,1}};
 std::vector<float> v;
 auto vertice=[&](int n,int color){for(int k=0;k<3;++k)v.push_back(p[n][k]);for(int k=0;k<3;++k)v.push_back(c[color][k]);};
 for(int cara=0;cara<4;++cara){vertice(cara,cara);vertice((cara+1)%4,cara);vertice(4,cara);}
 for(int n:{0,3,2,2,1,0})vertice(n,4);
 piramide.CreateMeshColor(v.data(),unsigned(v.size()));
}
// Cilindro/con o cerrados, centrados en Y; radios y altura unitarios.
void crearRedondo(Mesh& pieza,bool cono){
 std::vector<float> v;std::vector<unsigned> ind;
 auto tri=[&](glm::vec3 a,glm::vec3 b,glm::vec3 c){
  for(auto p:{a,b,c}){ind.push_back(unsigned(v.size()/3));v.insert(v.end(),{p.x,p.y,p.z});}
 };
 for(int k=0;k<40;++k){
  float a=2*PI*k/40,b=2*PI*(k+1)/40;
  glm::vec3 p(cos(a),-.5f,sin(a)),q(cos(b),-.5f,sin(b));
  tri({0,-.5f,0},q,p);
  if(cono)tri(p,q,{0,.5f,0});
  else {glm::vec3 r(q.x,.5f,q.z),s(p.x,.5f,p.z);tri(p,q,r);tri(p,r,s);tri({0,.5f,0},s,r);}
 }
 pieza.CreateMesh(v.data(),ind.data(),unsigned(v.size()),unsigned(ind.size()));
}
void matriz(Shader& shader,const glm::mat4& m){glUniformMatrix4fv(shader.getModelLocation(),1,GL_FALSE,glm::value_ptr(m));}
void dibujar(Mesh& m,Shader& shader,const glm::mat4& model,glm::vec3 color){
 matriz(shader,model);glUniform3fv(shader.getColorLocation(),1,glm::value_ptr(color));m.RenderMesh();
}
void usar(Shader& shader,const glm::mat4& view,const glm::mat4& projection){
 shader.useShader();glUniformMatrix4fv(shader.getViewLocation(),1,GL_FALSE,glm::value_ptr(view));
 glUniformMatrix4fv(shader.getProjectLocation(),1,GL_FALSE,glm::value_ptr(projection));
}
void cruz(MeshColor& piramide,Shader& shader,const glm::mat4& posicion){
 const float separacion=.16f,centro=1.34f;
 // Cuatro pares: arriba, abajo, izquierda y derecha. Ocho llamadas en total.
 const glm::mat4 pares[]={mover(0,centro,0),mover(0,-centro,0),
  mover(-centro,0,0)*girar(90,{0,0,1}),mover(centro,0,0)*girar(90,{0,0,1})};
 for(const auto& par:pares){
  matriz(shader,posicion*par*mover(0,separacion/2,0));piramide.RenderMeshColor();
  matriz(shader,posicion*par*mover(0,-separacion/2,0)*girar(180,{1,0,0}));piramide.RenderMeshColor();
 }
}
void cohete(Mesh& cubo,Mesh& cilindro,Mesh& cono,Sphere& esfera,Shader& shader){
 const glm::vec3 blanco(.82f,.85f,.9f),rojo(.88f,.08f,.08f),gris(.22f,.26f,.32f);
 dibujar(cilindro,shader,escalar(.58f,2.6f,.58f),blanco);
 dibujar(cono,shader,mover(0,1.8f,0)*escalar(.58f,1,.58f),rojo);
 for(float lado:{-1.f,1.f}){
  dibujar(cilindro,shader,mover(lado*.96f,-.52f,0)*escalar(.25f,1.8f,.25f),gris);
  dibujar(cono,shader,mover(lado*.96f,.58f,0)*escalar(.25f,.4f,.25f),rojo);
  dibujar(cubo,shader,mover(lado*.74f,-.5f,0)*escalar(.45f,.18f,.3f),rojo);
  dibujar(cubo,shader,mover(lado*.96f,-1.5f,0)*escalar(.65f,.16f,.65f),gris);
 }
 // Ventanilla: esfera aplanada, parcialmente incrustada en el cilindro.
 matriz(shader,mover(0,.5f,.55f)*escalar(.25f,.25f,.1f));
 glm::vec3 azul(.03f,.5f,.95f);glUniform3fv(shader.getColorLocation(),1,glm::value_ptr(azul));esfera.render();
}
void captura(const std::filesystem::path& path,int w,int h){
 int stride=(w*3+3)&~3;std::vector<unsigned char> p(stride*h),head(54);
 glPixelStorei(GL_PACK_ALIGNMENT,4);glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE,p.data());
 head[0]='B';head[1]='M';auto n=[&](int i,unsigned v){for(int k=0;k<4;++k)head[i+k]=static_cast<unsigned char>(v>>(8*k));};
 n(2,54+unsigned(p.size()));n(10,54);n(14,40);n(18,w);n(22,h);head[26]=1;head[28]=24;
 std::ofstream f(path,std::ios::binary);f.write((char*)head.data(),54);f.write((char*)p.data(),p.size());
 if(!f)throw std::runtime_error("No se pudo guardar captura");
}
int main(int argc,char** argv){
 const bool test=argc==3&&std::string(argv[1])=="--verify";
 const bool record=argc==2&&std::string(argv[1])=="--record";
 if(record)_setmode(_fileno(stdout),_O_BINARY);
 std::filesystem::path evidencia;if(test){evidencia=std::filesystem::absolute(argv[2]);std::filesystem::create_directories(evidencia);}
 // Los shaders se buscan junto al EXE tanto al abrirlo como desde Visual Studio.
 wchar_t ruta[MAX_PATH];GetModuleFileNameW(nullptr,ruta,MAX_PATH);
 std::filesystem::current_path(std::filesystem::path(ruta).parent_path());
 Window ventana;if(ventana.Initialise(test||record))return 1;
 try{
  Mesh cubo,cilindro,cono;MeshColor piramide;Sphere esfera(1,24,16);
  crearCubo(cubo);crearRedondo(cilindro,false);crearRedondo(cono,true);crearPiramide(piramide);esfera.init();esfera.load();
  Shader uniforme,colores;uniforme.CreateFromFiles("shaders/shader.vert","shaders/shader.frag");
  colores.CreateFromFiles("shaders/shadercolor.vert","shaders/shadercolor.frag");
  Camera camera({0,0,14},{0,1,0},-90,0,3,.18f);camera.orbit(10,12,14);
  double last=glfwGetTime();float giro=10;int frame=0;
  while(!ventana.getShouldClose()){
   ventana.pollEvents();double now=glfwGetTime();float dt=std::min(float(now-last),.05f);last=now;
   if(ventana.takeReset()){camera.orbit(10,12,14);giro=10;}
   camera.keyControl(ventana.getsKeys(),dt);camera.mouseControl(ventana.getXChange(),ventana.getYChange());camera.zoom(ventana.getScroll()*.35f);
   if(ventana.getAutomatic()){giro+=25*dt;camera.orbit(giro,18,14);}
   if(test){const float ang[]={10,35,90,180,270,30,0,0};const float elev[]={12,25,0,0,0,-40,65,-65};camera.orbit(ang[frame],elev[frame],14);}
   // Video de la misma escena, generado desde el framebuffer real.
   if(record){float t=frame/24.f;
    if(t<5)camera.orbit(10,12,14);
    else if(t<25)camera.orbit(10+(t-5)*18,18,14);
    else if(t<32)camera.orbit(30,-35,14);
    else if(t<39)camera.orbit(-25,45,14);
    else camera.orbit(10,12,14);
   }
   int w=int(ventana.getBufferWidth()),h=int(ventana.getBufferHeight());if(w<=0||h<=0)continue;
   glViewport(0,0,w,h);glClearColor(.92f,.94f,.97f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glm::mat4 view=camera.calculateViewMatrix(),projection=glm::perspective(glm::radians(42.f),float(w)/h,.05f,100.f);
   const glm::mat4 izquierda=mover(-2.7f,0,0),derecha=mover(2.0f,0,0);
   usar(uniforme,view*izquierda,projection);
   cohete(cubo,cilindro,cono,esfera,uniforme);
   usar(colores,view,projection);
   // Una piramide como aleta del cohete, mas ocho en la cruz. Sin pieza aislada.
   matriz(colores,izquierda*mover(0,-.95f,-.5f)*girar(-90,{1,0,0})*escalar(.65f,.85f,.7f));piramide.RenderMeshColor();
   cruz(piramide,colores,derecha);
   if(test){
    if(glGetError()!=GL_NO_ERROR)throw std::runtime_error("Error de OpenGL");
    captura(evidencia/("vista-"+std::to_string(frame)+".bmp"),w,h);
    if(++frame==8)break;
   }
   if(record){
    std::vector<unsigned char> rgb(w*h*3);glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,rgb.data());
    if(glGetError()!=GL_NO_ERROR||std::fwrite(rgb.data(),1,rgb.size(),stdout)!=rgb.size())throw std::runtime_error("Error grabando video");
    if(++frame==45*24)break;
   }
   ventana.swapBuffers();
  }
  if(test)std::puts("OK: 8 vistas de la escena conjunta, sin errores OpenGL.");
 }catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}
 return 0;
}
