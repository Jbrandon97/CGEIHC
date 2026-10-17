#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Camera.h"
#include "Model.h"
#include "Shader_light.h"
#include "Texture.h"
#include "Skybox.h"

void Capturar(const std::filesystem::path& path,int w,int h) {
    std::vector<unsigned char> pixels(w*h*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);glReadBuffer(GL_BACK);glFinish();
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::ofstream f(path,std::ios::binary);f<<"P6\n"<<w<<" "<<h<<"\n255\n";
    for(int y=h-1;y>=0;y--)f.write(reinterpret_cast<char*>(pixels.data()+y*w*3),w*3);
}
int main(int argc,char** argv) {
    bool pruebas=argc>1&&std::string(argv[1])=="--pruebas";
    auto destino=std::filesystem::absolute(argc>2?argv[2]:"evidencias");
    wchar_t ruta[MAX_PATH];GetModuleFileNameW(nullptr,ruta,MAX_PATH);
    std::filesystem::current_path(std::filesystem::path(ruta).parent_path());
    Window window;
    if(window.Initialise())return 1;
    while(glGetError()!=GL_NO_ERROR){}
    glfwSwapInterval(pruebas?0:1);
    printf("OpenGL %s | %s\n",glGetString(GL_VERSION),glGetString(GL_RENDERER));
    window.MostrarControles();
    if(pruebas&&!window.ProbarControles())return 2;
    Shader shader;shader.CreateFromFiles("shaders/shader_light.vert","shaders/shader_light.frag");shader.UseShader();
    GLint id=0,ok=0;glGetIntegerv(GL_CURRENT_PROGRAM,&id);glGetProgramiv(id,GL_LINK_STATUS,&ok);if(!ok)return 3;
    Model nave;if(!nave.LoadModel("Models/naboo_n1.obj"))return 4;
    Texture plasma("Textures/plasma_azul.png");if(!plasma.LoadTextureA())return 5;
    MeshModel piso,chorro;
    GLfloat floorVertices[]={-30,0,-30,0,0,0,1,0, 30,0,-30,1,0,0,1,0, 30,0,30,1,1,0,1,0, -30,0,30,0,1,0,1,0};
    unsigned floorIndices[]={0,2,1,0,3,2};piso.CreateMeshModel(floorVertices,floorIndices,32,6);
    GLfloat pv[]={-.5f,0,0,0,0,0,1,0, .5f,0,0,1,0,0,1,0, .5f,0,-3,1,1,0,1,0, -.5f,0,-3,0,1,0,1,0,
        0,-.5f,0,0,0,1,0,0, 0,.5f,0,1,0,1,0,0, 0,.5f,-3,1,1,1,0,0, 0,-.5f,-3,0,1,1,0,0};
    unsigned pi[]={0,1,2,0,2,3,4,5,6,4,6,7};chorro.CreateMeshModel(pv,pi,64,12);
    // OBJ exportado con X=x, Y=z, Z=-y; izquierda y derecha vistas desde la cabina.
    const glm::vec3 salidas[2]={{12.93335f,4.30253f,5.31969f},{-14.51758f,4.30119f,5.32504f}};
    glm::mat4 ship=glm::scale(glm::translate(glm::mat4(1),glm::vec3(.248f,3,0)),glm::vec3(.32f));
    auto posicion=[&](glm::vec3 p){return glm::vec3(ship*glm::vec4(p,1));};
    glm::vec3 motores[2]={posicion(salidas[0]),posicion(salidas[1])};
    glm::vec3 puntoPrincipal=posicion({0,23,0}),faro=posicion({-.7f,2,25});
    glm::vec3 direccion=glm::normalize(glm::mat3(ship)*glm::vec3(0,-.65f,1));
    std::vector<std::string> faces;
    for(const char* face:{"rt","lf","up","dn","bk","ft"})faces.push_back(std::string("Textures/Skybox/cupertin-lake_")+face+".tga");
    Skybox sky(faces);if(!sky.IsReady())return 9;
    if(pruebas){
        glViewport(0,0,128,128);
        auto fondo=[&](glm::vec3 eye,glm::vec3 dir){
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            sky.DrawSkybox(glm::lookAt(eye,eye+dir,glm::vec3(0,1,0)),glm::perspective(glm::radians(45.f),1.f,.1f,180.f));
            std::vector<unsigned char> pixels(128*128*3);
            glReadPixels(0,0,128,128,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());return pixels;
        };
        auto a=fondo({0,0,0},{0,0,-1}),b=fondo({7,2,3},{0,0,-1}),c=fondo({0,0,0},{1,0,0});
        GLint depth;GLboolean mask;glGetIntegerv(GL_DEPTH_FUNC,&depth);glGetBooleanv(GL_DEPTH_WRITEMASK,&mask);
        if(a!=b||a==c||depth!=GL_LESS||mask!=GL_TRUE)return 10;
        puts("Skybox: centrado, rotacion y restauracion de profundidad OK");
    }
    // Configuracion permanente: apagar una luz no modifica su intensidad.
    PointLight configuradas[3]={
        PointLight(1,.96f,.88f,.35f,3.2f,puntoPrincipal.x,puntoPrincipal.y,puntoPrincipal.z,1,.04f,.008f),
        PointLight(.05f,.32f,1,0,3.0f,motores[0].x,motores[0].y,motores[0].z-.25f,1,.18f,.10f),
        PointLight(.05f,.32f,1,0,3.0f,motores[1].x,motores[1].y,motores[1].z-.25f,1,.18f,.10f)
    };
    SpotLight reflector(1,.93f,.75f,0,3.5f,faro.x,faro.y,faro.z,direccion.x,direccion.y,direccion.z,1,.06f,.02f,28);
    Camera cam({13,10,-17},{0,1,0},128,-17,10,.15f);
    std::ofstream log;
    if(pruebas){std::filesystem::create_directories(destino);log.open(destino/"estados.csv");log<<"caso,principales,izquierdo,derecho,plasmas,puntuales,reflectores\n";}
    const char* nombres[]={"A_todo_apagado","principales_sin_motores","izquierdo_sin_principales","C_solo_izquierdo","derecho_sin_principales","D_solo_derecho","E_motores_sin_principales","B_todo_encendido","vista_frontal","vista_superior","inicio_libre"};
    double anterior=glfwGetTime();int caso=0;
    while(!window.getShouldClose()){
        glfwPollEvents();double ahora=glfwGetTime();float dt=float(std::min(ahora-anterior,.05));anterior=ahora;
        int vista=window.vistaActual();
        if(pruebas){
            // Ida y vuelta: tambien probar el cero despues de cuadros con luces.
            window.AplicarCasoPrueba(caso<8?caso:caso<16?15-caso:7);
            vista=caso==16?2:caso==17?3:caso==18?0:1;
        }
        float mouseX=window.getXChange(),mouseY=window.getYChange();
        if(vista==0){cam.keyControl(window.getsKeys(),dt);cam.mouseControl(mouseX,mouseY);}
        glm::vec3 eye=cam.getCameraPosition(),target(0,4,0);glm::mat4 view=cam.calculateViewMatrix();
        if(vista){
            if(vista==1)eye={13,10,-17};
            if(vista==2)eye={13,10,20};
            if(vista==3)eye={0,30,-1};
            view=glm::lookAt(eye,target,glm::vec3(0,1,0));
        }
        int width,height;glfwGetFramebufferSize(glfwGetCurrentContext(),&width,&height);if(!width||!height){glfwWaitEvents();continue;}
        glViewport(0,0,width,height);glClearColor(.035f,.048f,.075f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        bool principal=window.principalesActivas();
        PointLight puntos[3];
        unsigned int pointLightCount=0;
        if(principal)puntos[pointLightCount++]=configuradas[0];
        for(int i=0;i<2;i++)if(window.motorActivo(i))puntos[pointLightCount++]=configuradas[i+1];
        unsigned int spotLightCount=principal?1u:0u;

        glm::mat4 projection=glm::perspective(glm::radians(45.f),float(width)/height,.1f,180.f);
        int dibujados=0;
        auto renderizar=[&](){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        sky.DrawSkybox(view,projection);
        shader.UseShader();shader.SetPointLights(puntos,pointLightCount);shader.SetSpotLights(&reflector,spotLightCount);
        glUniformMatrix4fv(shader.GetProjectionLocation(),1,GL_FALSE,glm::value_ptr(projection));glUniformMatrix4fv(shader.GetViewLocation(),1,GL_FALSE,glm::value_ptr(view));
        glUniform3fv(shader.GetEyePositionLocation(),1,glm::value_ptr(eye));
        auto model=[&](const glm::mat4& m){glUniformMatrix4fv(shader.GetModelLocation(),1,GL_FALSE,glm::value_ptr(m));};
        GLint emissive=glGetUniformLocation(id,"emissive"),useTexture=glGetUniformLocation(id,"useMaterialTexture");
        glUniform1i(emissive,0);glUniform1i(glGetUniformLocation(id,"alphaCutout"),0);glUniform1i(useTexture,0);
        model(glm::mat4(1));glUniform3f(shader.getColorLocation(),.23f,.26f,.30f);glUniform1f(shader.GetSpecularIntensityLocation(),.12f);glUniform1f(shader.GetShininessLocation(),24);piso.RenderMeshModel();
        model(ship);nave.RenderModel(shader.getColorLocation(),useTexture,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        // Plasma emisivo: mezcla aditiva tras los objetos opacos, sin escribir profundidad.
        glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE);glDepthMask(GL_FALSE);
        glUniform1i(emissive,1);glUniform1i(useTexture,1);glUniform3f(shader.getColorLocation(),1,1,1);plasma.UseTexture();
        dibujados=0;
        for(int i=0;i<2;i++)if(window.motorActivo(i)){
            model(glm::translate(glm::mat4(1),motores[i]+glm::vec3(0,0,-.025f)));chorro.RenderMeshModel();dibujados++;
        }
        glDepthMask(GL_TRUE);glDisable(GL_BLEND);glUniform1i(emissive,0);
        };
        renderizar();
        if(pruebas){
            GLint pc,sc;glGetUniformiv(id,glGetUniformLocation(id,"pointLightCount"),&pc);glGetUniformiv(id,glGetUniformLocation(id,"spotLightCount"),&sc);
            int mask=(principal?1:0)|(window.motorActivo(0)?2:0)|(window.motorActivo(1)?4:0);
            const int esperadosP[8]={0,1,1,2,1,2,2,3};
            const int esperadosS[8]={0,1,0,1,0,1,0,1};
            if(pc!=esperadosP[mask]||sc!=esperadosS[mask]||dibujados!=int(window.motorActivo(0))+int(window.motorActivo(1)))return 6;
            // Leer los uniformes reales de la GPU: configuracion y orden compacto.
            int slot=0;
            for(int fuente=0;fuente<4;fuente++){
                if(!(fuente==0||fuente==3?principal:window.motorActivo(fuente-1)))continue;
                std::string base=fuente<3?"pointLights["+std::to_string(slot++)+"]":"spotLights[0].base";
                auto verificar=[&](std::string campo,float esperado){
                    float actual;glGetUniformfv(id,glGetUniformLocation(id,(base+campo).c_str()),&actual);
                    return std::abs(actual-esperado)<.0001f;
                };
                glm::vec3 pos=fuente==0?puntoPrincipal:fuente==3?faro:motores[fuente-1]+glm::vec3(0,0,-.25f);
                glm::vec3 col=fuente==0?glm::vec3(1,.96f,.88f):fuente==3?glm::vec3(1,.93f,.75f):glm::vec3(.05f,.32f,1);
                glm::vec3 actualPos,actualCol;
                glGetUniformfv(id,glGetUniformLocation(id,(base+".position").c_str()),glm::value_ptr(actualPos));
                glGetUniformfv(id,glGetUniformLocation(id,(base+".base.color").c_str()),glm::value_ptr(actualCol));
                if(glm::length(pos-actualPos)>.0001f||glm::length(col-actualCol)>.0001f||
                   !verificar(".base.diffuseIntensity",fuente==0?3.2f:fuente==3?3.5f:3.f)||
                   !verificar(".base.ambientIntensity",fuente==0?.35f:0.f)||
                   !verificar(".constant",1)||!verificar(".linear",fuente==0?.04f:fuente==3?.06f:.18f)||
                   !verificar(".exponent",fuente==0?.008f:fuente==3?.02f:.10f))return 7;
                if(fuente==3){
                    glm::vec3 dir;float edge;
                    glGetUniformfv(id,glGetUniformLocation(id,"spotLights[0].direction"),glm::value_ptr(dir));
                    glGetUniformfv(id,glGetUniformLocation(id,"spotLights[0].edge"),&edge);
                    if(glm::length(dir-direccion)>.0001f||std::abs(edge-std::cos(glm::radians(28.f)))>.0001f)return 7;
                }
            }
            // Prueba negativa: valores enormes fuera del rango activo no cambian ningun pixel.
            auto leerPixels=[&](){
                std::vector<unsigned char> pix(width*height*3);
                glPixelStorei(GL_PACK_ALIGNMENT,1);glReadBuffer(GL_BACK);glFinish();
                glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pix.data());return pix;
            };
            auto antes=leerPixels();
            auto contaminar=[&](std::string base){
                glUniform3f(glGetUniformLocation(id,(base+".base.color").c_str()),1,1,1);
                glUniform1f(glGetUniformLocation(id,(base+".base.ambientIntensity").c_str()),1000000);
                glUniform1f(glGetUniformLocation(id,(base+".base.diffuseIntensity").c_str()),1000000);
                glUniform3f(glGetUniformLocation(id,(base+".position").c_str()),0,10,0);
                glUniform1f(glGetUniformLocation(id,(base+".constant").c_str()),1);
                glUniform1f(glGetUniformLocation(id,(base+".linear").c_str()),0);
                glUniform1f(glGetUniformLocation(id,(base+".exponent").c_str()),0);
            };
            for(int i=pc;i<3;i++)contaminar("pointLights["+std::to_string(i)+"]");
            if(!sc){
                contaminar("spotLights[0].base");
                glUniform3f(glGetUniformLocation(id,"spotLights[0].direction"),0,-1,0);
                glUniform1f(glGetUniformLocation(id,"spotLights[0].edge"),-2);
            }
            renderizar();
            if(antes!=leerPixels())return 11;
            int indice=caso<8?caso:caso<16?15-caso:caso-8;
            std::string nombre=std::to_string(caso)+"_"+nombres[indice];
            log<<nombre<<","<<principal<<","<<window.motorActivo(0)<<","<<window.motorActivo(1)<<","<<dibujados<<","<<pc<<","<<sc<<"\n";
            Capturar(destino/(nombre+".ppm"),width,height);printf("PRUEBA %s OK: %d puntuales, %d reflectores; slots inactivos sin efecto\n",nombre.c_str(),pc,sc);
        }
        if(glGetError()!=GL_NO_ERROR)return 8;
        window.swapBuffers();if(pruebas&&++caso==19)break;
    }
    nave.ClearModel();puts("CIERRE CORRECTO, sin errores OpenGL");return 0;
}

