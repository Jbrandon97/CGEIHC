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
    puts("1: luces principales | 2: motor izquierdo | 3: motor derecho\nF5: libre WASD + mouse | F6: posterior | F7: frontal | F8: superior | Esc: salir");
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
    Camera cam({13,10,-17},{0,1,0},128,-17,10,.15f);
    std::ofstream log;
    if(pruebas){std::filesystem::create_directories(destino);log.open(destino/"estados.csv");log<<"caso,principales,izquierdo,derecho,plasmas,puntuales,reflectores\n";}
    const char* nombres[]={"A_todo_apagado","principales_sin_motores","izquierdo_sin_principales","C_solo_izquierdo","derecho_sin_principales","D_solo_derecho","E_motores_sin_principales","B_todo_encendido","vista_frontal","vista_superior"};
    double anterior=glfwGetTime();int caso=0;
    while(!window.getShouldClose()){
        glfwPollEvents();double ahora=glfwGetTime();float dt=float(std::min(ahora-anterior,.05));anterior=ahora;
        int vista=window.vistaActual();
        if(pruebas){
            window.AplicarCasoPrueba(caso<8?caso:7);
            vista=caso==8?2:caso==9?3:1;
        }
        cam.keyControl(window.getsKeys(),dt);cam.mouseControl(window.getXChange(),window.getYChange());
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
        PointLight puntos[3]={
            PointLight(1,.96f,.88f,principal?.35f:0,principal?3.2f:0,puntoPrincipal.x,puntoPrincipal.y,puntoPrincipal.z,1,.04f,.008f),
            PointLight(.05f,.32f,1,0,window.motorActivo(0)?3.0f:0,motores[0].x,motores[0].y,motores[0].z-.25f,1,.18f,.10f),
            PointLight(.05f,.32f,1,0,window.motorActivo(1)?3.0f:0,motores[1].x,motores[1].y,motores[1].z-.25f,1,.18f,.10f)
        };
        SpotLight reflector(1,.93f,.75f,0,principal?3.5f:0,faro.x,faro.y,faro.z,direccion.x,direccion.y,direccion.z,1,.06f,.02f,28);
        shader.UseShader();shader.SetPointLights(puntos,3);shader.SetSpotLights(&reflector,1);
        glm::mat4 projection=glm::perspective(glm::radians(45.f),float(width)/height,.1f,180.f);
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
        int dibujados=0;
        for(int i=0;i<2;i++)if(window.motorActivo(i)){
            model(glm::translate(glm::mat4(1),motores[i]+glm::vec3(0,0,-.025f)));chorro.RenderMeshModel();dibujados++;
        }
        glDepthMask(GL_TRUE);glDisable(GL_BLEND);glUniform1i(emissive,0);
        if(pruebas){
            GLint pc,sc;glGetUniformiv(id,glGetUniformLocation(id,"pointLightCount"),&pc);glGetUniformiv(id,glGetUniformLocation(id,"spotLightCount"),&sc);
            if(pc!=3||sc!=1||dibujados!=int(window.motorActivo(0))+int(window.motorActivo(1)))return 6;
            for(int i=0;i<4;i++){
                std::string base=i<3?"pointLights["+std::to_string(i)+"]":"spotLights[0].base";
                float intensidad,con,lin,exp;
                glGetUniformfv(id,glGetUniformLocation(id,(base+".base.diffuseIntensity").c_str()),&intensidad);
                glGetUniformfv(id,glGetUniformLocation(id,(base+".constant").c_str()),&con);
                glGetUniformfv(id,glGetUniformLocation(id,(base+".linear").c_str()),&lin);
                glGetUniformfv(id,glGetUniformLocation(id,(base+".exponent").c_str()),&exp);
                bool activo=i==0||i==3?principal:window.motorActivo(i-1);
                if((intensidad>0)!=activo||con<=0||lin<=0||exp<=0)return 7;
            }
            log<<nombres[caso]<<","<<principal<<","<<window.motorActivo(0)<<","<<window.motorActivo(1)<<","<<dibujados<<","<<pc<<","<<sc<<"\n";
            Capturar(destino/(std::string(nombres[caso])+".ppm"),width,height);printf("PRUEBA %s OK\n",nombres[caso]);
        }
        if(glGetError()!=GL_NO_ERROR)return 8;
        window.swapBuffers();if(pruebas&&++caso==10)break;
    }
    nave.ClearModel();puts("CIERRE CORRECTO, sin errores OpenGL");return 0;
}

