#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Camera.h"
#include "Model.h"
#include "Shader_light.h"
#include "Texture.h"
#include "Skybox.h"
#include "Animacion.h"

void Capturar(const std::filesystem::path&path,int w,int h){
    std::vector<unsigned char>p(w*h*3);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadBuffer(GL_BACK);glFinish();
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,p.data());std::ofstream f(path,std::ios::binary);f<<"P6\n"<<w<<" "<<h<<"\n255\n";
    for(int y=h-1;y>=0;y--)f.write(reinterpret_cast<char*>(p.data()+y*w*3),w*3);
}
int main(int argc,char**argv){
    bool pruebas=argc>1&&std::string(argv[1])=="--pruebas";
    auto destino=std::filesystem::absolute(argc>2?argv[2]:"evidencias");
    wchar_t ruta[MAX_PATH];GetModuleFileNameW(nullptr,ruta,MAX_PATH);std::filesystem::current_path(std::filesystem::path(ruta).parent_path());
    Circuito circuito;if(pruebas&&(!circuito.Verificar()||!VerificarAnimacion(circuito)))return 2;
    Window window;if(window.Initialise())return 1;while(glGetError()!=GL_NO_ERROR){}
    glfwSwapInterval(pruebas?0:1);window.MostrarControles();
    printf("OpenGL %s | %s\n",glGetString(GL_VERSION),glGetString(GL_RENDERER));
    if(pruebas&&!window.ProbarControles())return 3;
    Shader shader;shader.CreateFromFiles("shaders/shader_light.vert","shaders/shader_light.frag");shader.UseShader();
    GLint id=0,ok=0;glGetIntegerv(GL_CURRENT_PROGRAM,&id);glGetProgramiv(id,GL_LINK_STATUS,&ok);if(!ok)return 4;
    Model nave,satelite[4];if(!nave.LoadModel("Models/naboo_n1.obj"))return 5;
    const char*nombres[]={"Cuerpo","Panel_Izquierdo","Panel_Derecho","Antena"};
    for(int i=0;i<4;i++)if(!satelite[i].LoadModel(std::string("Models/Satelite_")+nombres[i]+".obj"))return 5;
    Texture suelo("Textures/piso.tga");if(!suelo.LoadTextureA())return 6;
    circuito.CrearMallas();std::vector<std::string>faces;
    for(const char*n:{"rt","lf","up","dn","bk","ft"})faces.push_back(std::string("Textures/Skybox/cupertin-lake_")+n+".tga");
    Skybox sky(faces);if(!sky.IsReady())return 7;
    Camera cam({140,100,140},{0,1,0},-135,-27,45,.15f);Animacion anim;
    struct Caso {const char*nombre;double parametro;int vista;double tiempo;};
    const double a=25*3.14159265358979323846/2;
    std::vector<Caso>casos={{"01_general",0,3,0},{"02_recta",15,1,1},
        {"03_subida_6",50,1,2},{"04_cima_6",65,1,3},{"05_bajada_6",80,1,4},
        {"06_curva",130+a*.5,1,5},{"07_arco",130+a+30,1,6},
        {"08_subida_4",190+2*a+50,1,7},{"09_cima_4",190+2*a+65,1,8},
        {"10_bajada_4",190+2*a+80,1,9},{"11_cierre",Circuito::longitudPlana-.1,1,10},
        {"12_satelite_neutro",0,2,0},{"13_satelite_paneles",0,2,2.856},
        {"14_satelite_opuesto",0,2,8.568},{"15_rampas_perfil",65,4,12}};
    size_t caso=0;double anterior=glfwGetTime();bool teniaFoco=window.EnFoco();
    if(pruebas)std::filesystem::create_directories(destino);
    while(!window.getShouldClose()){
        glfwPollEvents();double ahora=glfwGetTime(),dt=ahora-anterior;anterior=ahora;
        bool foco=window.EnFoco();if(!foco||!teniaFoco)dt=0;else if(dt>.25)dt=.05;teniaFoco=foco;
        if(pruebas){anim.distancia=circuito.DistanciaEn(casos[caso].parametro);anim.tiempo=casos[caso].tiempo;}
        else anim.Actualizar(dt,circuito);
        int vista=pruebas?casos[caso].vista:window.Vista();
        if(!pruebas)window.ControlarCamara(cam,float(dt));
        auto punto=circuito.PorDistancia(anim.distancia);glm::vec3 eye=cam.getCameraPosition(),target;
        glm::mat4 view=cam.calculateViewMatrix();
        if(vista){
            if(vista==1){target=punto.posicion+glm::vec3(0,3,0);eye=target-punto.frente*24.f+punto.derecha*17.f+glm::vec3(0,14,0);}
            if(vista==2){target=anim.PosicionSatelite();glm::mat4 m[4];anim.Satelite(m);eye=target+glm::normalize(glm::vec3(m[2][2]))*20.f+glm::vec3(25,14,27);}
            if(vista==3){eye={145,130,160};target={0,0,0};}
            if(vista==4){eye={0,17,-112};target={0,2,-55};}
            view=glm::lookAt(eye,target,glm::vec3(0,1,0));
        }
        int w,h;glfwGetFramebufferSize(glfwGetCurrentContext(),&w,&h);if(!w||!h){glfwWaitEvents();continue;}
        glViewport(0,0,w,h);auto proy=glm::perspective(glm::radians(48.f),float(w)/h,.1f,650.f);
        glClearColor(.04f,.05f,.08f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);sky.DrawSkybox(view,proy);
        shader.UseShader();
        auto sp=anim.PosicionSatelite();
        PointLight luces[]={PointLight(1,.96f,.9f,.28f,1.5f,0,100,0,1,.003f,.00004f),
            PointLight(.80f,.90f,1,.16f,.8f,punto.posicion.x,25+punto.posicion.y,punto.posicion.z,1,.014f,.0008f),
            PointLight(1,.96f,.85f,.16f,.7f,sp.x,sp.y+15,sp.z+8,1,.018f,.001f)};
        shader.SetPointLights(luces,3);shader.SetSpotLights(nullptr,0);
        glUniformMatrix4fv(shader.GetProjectionLocation(),1,GL_FALSE,glm::value_ptr(proy));glUniformMatrix4fv(shader.GetViewLocation(),1,GL_FALSE,glm::value_ptr(view));
        glUniform3fv(shader.GetEyePositionLocation(),1,glm::value_ptr(eye));
        GLint tex=glGetUniformLocation(id,"useMaterialTexture");glUniform1i(glGetUniformLocation(id,"alphaCutout"),0);glUniform1i(glGetUniformLocation(id,"emissive"),0);
        auto matriz=[&](const glm::mat4&m){glUniformMatrix4fv(shader.GetModelLocation(),1,GL_FALSE,glm::value_ptr(m));};
        auto malla=[&](MeshModel&m,glm::vec3 color,bool textura=false){
            matriz(glm::mat4(1));glUniform1i(tex,textura);glUniform3fv(shader.getColorLocation(),1,glm::value_ptr(color));
            glUniform1f(shader.GetSpecularIntensityLocation(),.04f);glUniform1f(shader.GetShininessLocation(),16);if(textura)suelo.UseTexture();m.RenderMeshModel();};
        malla(circuito.piso,{.34f,.39f,.43f},true);
        malla(circuito.superficie,{.13f,.18f,.24f});
        malla(circuito.bordeInterior,{.03f,.75f,.95f});malla(circuito.bordeExterior,{.95f,.52f,.08f});
        malla(circuito.arco,{.16f,.6f,.72f});
        matriz(anim.Nave(circuito));nave.RenderModel(shader.getColorLocation(),tex,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        glm::mat4 matrices[4];anim.Satelite(matrices);
        for(int i=0;i<4;i++){matriz(matrices[i]);satelite[i].RenderModel(shader.getColorLocation(),tex,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());}
        if(glGetError()!=GL_NO_ERROR)return 8;
        if(pruebas){Capturar(destino/(std::string(casos[caso].nombre)+".ppm"),w,h);printf("Captura real %s OK\n",casos[caso].nombre);if(++caso==casos.size())break;}
        window.swapBuffers();
    }
    nave.ClearModel();for(auto&m:satelite)m.ClearModel();puts("CIERRE CORRECTO: sin errores OpenGL.");return 0;
}
