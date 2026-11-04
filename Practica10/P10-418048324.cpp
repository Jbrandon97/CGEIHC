#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX
#include <windows.h>
#include <filesystem>
#include <vector>
#include <cmath>
#include <cstdio>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Camera.h"
#include "Model.h"
#include "Shader_light.h"
#include "Texture.h"
#include "Skybox.h"
#include "KeyframesCohete.h"

int main(int argc,char**argv){
    wchar_t ruta[MAX_PATH];GetModuleFileNameW(nullptr,ruta,MAX_PATH);
    std::filesystem::current_path(std::filesystem::path(ruta).parent_path());
    Window ventana;if(ventana.Initialise())return 1;
    ventana.MostrarControles();glfwSwapInterval(1);
    Shader shader;shader.CreateFromFiles("shaders/shader_light.vert","shaders/shader_light.frag");
    shader.UseShader();GLint programa=0;glGetIntegerv(GL_CURRENT_PROGRAM,&programa);
    GLint enlazado=0;glGetProgramiv(programa,GL_LINK_STATUS,&enlazado);if(!enlazado)return 2;
    Model piezas[4];
    const char* nombres[]={"Saturn_Etapa_0","Saturn_Etapa_1","Saturn_Etapa_2","Saturn_Capsula"};
    for(int i=0;i<4;i++)if(!piezas[i].LoadModel(std::string("Models/")+nombres[i]+".obj"))return 4;
    // Piso amplio, 600 x 600, conservando el criterio del profesor.
    float v[]={-300,-2,-300, 0,0,0,1,0, 300,-2,-300, 30,0,0,1,0,
                300,-2,300, 30,30,0,1,0, -300,-2,300, 0,30,0,1,0};
    unsigned ind[]={0,2,1,0,3,2};MeshModel piso;piso.CreateMeshModel(v,ind,32,6);
    Texture texturaPiso("Textures/piso.tga");if(!texturaPiso.LoadTextureA())return 5;
    std::vector<std::string>caras;
    for(const char*n:{"rt","lf","up","dn","bk","ft"})caras.push_back(std::string("Textures/Skybox/cupertin-lake_")+n+".tga");
    Skybox cielo(caras);if(!cielo.IsReady())return 6;
    Camera cam({115,90,190},{0,1,0},-121,-15,55,.16f);
    KeyframesCohete keyframes;
    if(!keyframes.Leer("keyframescohete.txt")){puts("ERROR: keyframescohete.txt V2 ausente o invalido");return 8;}
    const glm::vec3 origen(0,-1.95f,0); // 0.05 unidades sobre el piso original.
    double anterior=glfwGetTime();bool focoAnterior=ventana.EnFoco();
    int segundoAnterior=-1;
    while(!ventana.getShouldClose()){
        glfwPollEvents();double ahora=glfwGetTime(),dt=ahora-anterior;anterior=ahora;
        bool foco=ventana.EnFoco();if(!foco||!focoAnterior)dt=0;else dt=std::min(dt,.08);focoAnterior=foco;
        if(ventana.ConsumirTecla(GLFW_KEY_G))puts(keyframes.Guardar("keyframescohete.txt")?"TXT sobrescrito correctamente":"ERROR al guardar TXT");
        if(ventana.ConsumirTecla(GLFW_KEY_O))puts(keyframes.Leer("keyframescohete.txt")?"TXT leido correctamente":"ERROR: TXT invalido; se conservan los cuadros anteriores");
        if(ventana.ConsumirTecla(GLFW_KEY_SPACE))puts(keyframes.Reproducir()?"Reproduciendo keyframes desde el inicio":"ERROR: faltan keyframes");
        if(ventana.ConsumirTecla(GLFW_KEY_R)){keyframes.Reiniciar();puts("Animacion reiniciada; ESPACIO para reproducir");}
        if(ventana.ConsumirTecla(GLFW_KEY_L))keyframes.Listar();
        keyframes.Actualizar(float(dt));
        int segundo=int(keyframes.Tiempo());
        if(segundo!=segundoAnterior){ventana.MostrarTiempo(segundo);segundoAnterior=segundo;}
        ventana.ControlarCamara(cam,float(dt));
        int w,h;glfwGetFramebufferSize(glfwGetCurrentContext(),&w,&h);if(w<1||h<1){glfwWaitEvents();continue;}
        glViewport(0,0,w,h);glm::mat4 proy=glm::perspective(glm::radians(50.f),float(w)/h,.1f,1600.f);
        glm::vec3 eye=cam.getCameraPosition();glm::mat4 vista=cam.calculateViewMatrix();
        // Las cuatro teclas de vista permanecen disponibles en Window.
        int camara=ventana.Vista();
        glm::vec3 objetivo=origen+keyframes.Actual(3).posicion;
        if(camara==1){objetivo.y-=17;eye=objetivo+glm::vec3(65,28,115);}
        if(camara==2){eye=objetivo+glm::vec3(22,12,32);}
        if(camara==3){eye={145,120,230};objetivo={5,66,0};}
        if(camara!=0)vista=glm::lookAt(eye,objetivo,glm::vec3(0,1,0));
        glClearColor(.05f,.06f,.09f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        cielo.DrawSkybox(vista,proy);shader.UseShader();
        PointLight luces[]={
            PointLight(1,.88f,.78f,.15f,.70f,0,120,90,1,.002f,.00002f),
            PointLight(.45f,.75f,1.f,.12f,.65f,0,15,30,1,.025f,.0008f),
            PointLight(1.f,.9f,.55f,.10f,.50f,30,12,20,1,.018f,.0007f)};
        shader.SetPointLights(luces,3);shader.SetSpotLights(nullptr,0);
        glUniformMatrix4fv(shader.GetProjectionLocation(),1,GL_FALSE,glm::value_ptr(proy));
        glUniformMatrix4fv(shader.GetViewLocation(),1,GL_FALSE,glm::value_ptr(vista));
        glUniform3fv(shader.GetEyePositionLocation(),1,glm::value_ptr(eye));
        GLint tex=glGetUniformLocation(programa,"useMaterialTexture");
        glUniform1i(glGetUniformLocation(programa,"alphaCutout"),0);
        glUniform1i(glGetUniformLocation(programa,"emissive"),0);
        auto matriz=[&](glm::mat4 m){glUniformMatrix4fv(shader.GetModelLocation(),1,GL_FALSE,glm::value_ptr(m));};
        matriz(glm::mat4(1));glUniform1i(tex,1);texturaPiso.UseTexture();
        glUniform3f(shader.getColorLocation(),.35f,.38f,.42f);
        glUniform1f(shader.GetSpecularIntensityLocation(),.04f);glUniform1f(shader.GetShininessLocation(),16);
        piso.RenderMeshModel();
        for(int i=0;i<4;i++){
            matriz(keyframes.Matriz(i,origen));
            piezas[i].RenderModel(shader.getColorLocation(),tex,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        }
        if(glGetError()!=GL_NO_ERROR){puts("Error de OpenGL en el renderizado");return 7;}
        ventana.swapBuffers();
    }
    for(auto& pieza:piezas)pieza.ClearModel();
    return 0;
}
