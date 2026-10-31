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
#include "HolocronAnimado.h"
#include "CoheteAnimado.h"

int main(int argc,char**argv){
    wchar_t ruta[MAX_PATH];GetModuleFileNameW(nullptr,ruta,MAX_PATH);
    std::filesystem::current_path(std::filesystem::path(ruta).parent_path());
    Window ventana;if(ventana.Initialise())return 1;
    ventana.MostrarControles();glfwSwapInterval(1);
    Shader shader;shader.CreateFromFiles("shaders/shader_light.vert","shaders/shader_light.frag");
    shader.UseShader();GLint programa=0;glGetIntegerv(GL_CURRENT_PROGRAM,&programa);
    GLint enlazado=0;glGetProgramiv(programa,GL_LINK_STATUS,&enlazado);if(!enlazado)return 2;
    Model centro,cristal,esquinas[8],cohete;
    if(!centro.LoadModel("Models/Holocron_Centro.obj"))return 3;
    if(!cristal.LoadModel("Models/Holocron_Cristal.obj"))return 3;
    for(int i=0;i<8;i++){
        char rutaModelo[100];sprintf_s(rutaModelo,"Models/Holocron_Esquina_%02d.obj",i+1);
        if(!esquinas[i].LoadModel(rutaModelo))return 3;
    }
    if(!cohete.LoadModel("Models/saturn_v.obj"))return 4;
    // Piso amplio, 600 x 600, conservando el criterio del profesor.
    float v[]={-300,-2,-300, 0,0,0,1,0, 300,-2,-300, 30,0,0,1,0,
                300,-2,300, 30,30,0,1,0, -300,-2,300, 0,30,0,1,0};
    unsigned ind[]={0,2,1,0,3,2};MeshModel piso;piso.CreateMeshModel(v,ind,32,6);
    Texture texturaPiso("Textures/piso.tga");if(!texturaPiso.LoadTextureA())return 5;
    std::vector<std::string>caras;
    for(const char*n:{"rt","lf","up","dn","bk","ft"})caras.push_back(std::string("Textures/Skybox/cupertin-lake_")+n+".tga");
    Skybox cielo(caras);if(!cielo.IsReady())return 6;
    Camera cam({115,105,170},{0,1,0},-122,-18,55,.16f);
    HolocronAnimado holocron;CoheteAnimado lanzamiento;
    double anterior=glfwGetTime();bool focoAnterior=ventana.EnFoco();
    puts("Practica 9-2: holocron con ocho esquinas moviles y cohete Saturn V. R: reiniciar despegue.");
    while(!ventana.getShouldClose()){
        glfwPollEvents();double ahora=glfwGetTime(),dt=ahora-anterior;anterior=ahora;
        bool foco=ventana.EnFoco();if(!foco||!focoAnterior)dt=0;else dt=std::min(dt,.08);focoAnterior=foco;
        if(ventana.getsKeys()[GLFW_KEY_R])lanzamiento.Reiniciar();
        holocron.Actualizar(dt);lanzamiento.Actualizar(dt);
        ventana.ControlarCamara(cam,float(dt));
        int w,h;glfwGetFramebufferSize(glfwGetCurrentContext(),&w,&h);if(w<1||h<1){glfwWaitEvents();continue;}
        glViewport(0,0,w,h);glm::mat4 proy=glm::perspective(glm::radians(50.f),float(w)/h,.1f,1600.f);
        glm::vec3 eye=cam.getCameraPosition(),objetivo;glm::mat4 vista=cam.calculateViewMatrix();
        // 1 observa el holocron; 2 sigue el cohete; 3 general; 4 regresa a libre.
        int camara=ventana.Vista();
        if(camara==1){eye={-38,44,67};objetivo={-65,23,0};}
        if(camara==2){objetivo=lanzamiento.Posicion()+glm::vec3(0,20,0);eye=objetivo+glm::vec3(55,29,80);}
        if(camara==3){eye={155,130,170};objetivo={0,55,0};}
        if(camara!=0)vista=glm::lookAt(eye,objetivo,glm::vec3(0,1,0));
        glClearColor(.05f,.06f,.09f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        cielo.DrawSkybox(vista,proy);shader.UseShader();
        PointLight luces[]={
            PointLight(1,.88f,.78f,.25f,1.3f,0,120,90,1,.002f,.00002f),
            PointLight(.45f,.75f,1.f,.30f,1.8f,-65,40,30,1,.025f,.0008f),
            PointLight(1.f,.9f,.55f,.20f,1.0f,65,65,20,1,.018f,.0007f)};
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
        auto base=holocron.Base();
        matriz(base);centro.RenderModel(shader.getColorLocation(),tex,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        matriz(base);cristal.RenderModel(shader.getColorLocation(),tex,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        for(int i=0;i<8;i++){
            matriz(base*holocron.Pieza(i));
            esquinas[i].RenderModel(shader.getColorLocation(),tex,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        }
        matriz(lanzamiento.Matriz());
        cohete.RenderModel(shader.getColorLocation(),tex,shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        if(glGetError()!=GL_NO_ERROR){puts("Error de OpenGL en el renderizado");return 7;}
        ventana.swapBuffers();
    }
    centro.ClearModel();cristal.ClearModel();for(auto&m:esquinas)m.ClearModel();cohete.ClearModel();
    return 0;
}
