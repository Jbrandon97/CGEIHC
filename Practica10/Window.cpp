#include "Window.h"
#include "Camera.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
int Window::Initialise(){
    if(!glfwInit())return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    handle=glfwCreateWindow(width,height,u8"Práctica 10 - Animación por Keyframes",nullptr,nullptr);
    if(!handle)return 1;
    glfwMakeContextCurrent(handle);glfwSetWindowUserPointer(handle,this);
    glfwSetKeyCallback(handle,ManejaTeclado);glfwSetCursorPosCallback(handle,ManejaMouse);glfwSetWindowFocusCallback(handle,ManejaFoco);
    glfwSetInputMode(handle,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glewExperimental=GL_TRUE;if(glewInit()!=GLEW_OK)return 1;
    glEnable(GL_DEPTH_TEST);return 0;
}
void Window::ManejaTeclado(GLFWwindow* h,int key,int,int action,int){
    auto w=static_cast<Window*>(glfwGetWindowUserPointer(h));
    if(key<0||key>=1024)return;
    if(action==GLFW_RELEASE){w->keys[key]=false;return;}
    if(action!=GLFW_PRESS||w->keys[key])return;
    w->keys[key]=true;w->pulsaciones[key]=true;
    {
        if(key==GLFW_KEY_1)w->vista=1;
        if(key==GLFW_KEY_2)w->vista=2;
        if(key==GLFW_KEY_3)w->vista=3;
        if(key==GLFW_KEY_4)w->vista=0;
    }
    if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)glfwSetWindowShouldClose(h,1);
}
void Window::ManejaMouse(GLFWwindow* h,double x,double y){
    auto w=static_cast<Window*>(glfwGetWindowUserPointer(h));
    if(w->primerMouse){w->lastX=x;w->lastY=y;w->primerMouse=false;}
    double dx=x-w->lastX,dy=w->lastY-y;
    if(std::abs(dx)<100&&std::abs(dy)<100){w->xChange+=float(dx);w->yChange+=float(dy);}
    w->lastX=x;w->lastY=y;
}
void Window::ManejaFoco(GLFWwindow* h,int){
    auto w=static_cast<Window*>(glfwGetWindowUserPointer(h));
    std::fill(w->pulsaciones,w->pulsaciones+1024,false);
    std::fill(w->keys,w->keys+1024,false);w->primerMouse=true;w->xChange=w->yChange=0;
}
GLfloat Window::getXChange(){float x=xChange;xChange=0;return x;}
GLfloat Window::getYChange(){float y=yChange;yChange=0;return y;}
bool Window::EnFoco() const{return glfwGetWindowAttrib(handle,GLFW_FOCUSED)!=0;}
void Window::ControlarCamara(Camera& c,float dt){
    float dx=getXChange(),dy=getYChange();
    if(vista!=0)return;
    c.keyControl(float(keys[GLFW_KEY_W])-float(keys[GLFW_KEY_S]),float(keys[GLFW_KEY_D])-float(keys[GLFW_KEY_A]),dt);
    c.mouseControl(dx,dy);
}
bool Window::getShouldClose() const{return glfwWindowShouldClose(handle)!=0;}
void Window::swapBuffers(){glfwSwapBuffers(handle);}
Window::~Window(){if(handle)glfwDestroyWindow(handle);glfwTerminate();}
void Window::MostrarControles() const{
    puts("P10: ESPACIO reproducir | R reiniciar | G guardar TXT | O leer TXT | L listar cuadros");
    puts("1 seguimiento | 2 capsula | 3 vista general para grabar | 4 libre WASD y raton | Esc salir");
}
bool Window::ConsumirTecla(int k){if(k<0||k>=1024)return false;bool p=pulsaciones[k];pulsaciones[k]=false;return p;}
bool Window::ProbarControles(){
    if(vista!=0)return false;
    Camera c({140,100,140},{0,1,0},-135,-27,45,.15f);
    int teclas[]={GLFW_KEY_W,GLFW_KEY_S,GLFW_KEY_A,GLFW_KEY_D};
    for(int tecla:teclas){
        auto p=c.getCameraPosition();ManejaTeclado(handle,tecla,0,GLFW_PRESS,0);ControlarCamara(c,.1f);ManejaTeclado(handle,tecla,0,GLFW_RELEASE,0);
        if(glm::length(p-c.getCameraPosition())<1.19f)return false;
    }
    primerMouse=true;ManejaMouse(handle,100,100);ManejaMouse(handle,120,110);
    auto d=c.getCameraDirection();c.mouseControl(getXChange(),getYChange());
    if(glm::length(d-c.getCameraDirection())<.01f)return false;
    for(int key:{GLFW_KEY_1,GLFW_KEY_2,GLFW_KEY_3}){
        ManejaTeclado(handle,key,0,GLFW_PRESS,0);ManejaTeclado(handle,key,0,GLFW_RELEASE,0);if(vista!=key-GLFW_KEY_1+1)return false;
        ManejaTeclado(handle,GLFW_KEY_4,0,GLFW_PRESS,0);ManejaTeclado(handle,GLFW_KEY_4,0,GLFW_RELEASE,0);if(vista!=0)return false;
    }
    // Mantener una tecla no debe reinstalar su vista tras otra pulsacion.
    ManejaTeclado(handle,GLFW_KEY_1,0,GLFW_PRESS,0);
    ManejaTeclado(handle,GLFW_KEY_2,0,GLFW_PRESS,0);
    for(int i=0;i<20;i++){
        ManejaTeclado(handle,GLFW_KEY_1,0,GLFW_REPEAT,0);
        ManejaTeclado(handle,GLFW_KEY_1,0,GLFW_PRESS,0);
        if(vista!=2)return false;
    }
    ManejaTeclado(handle,GLFW_KEY_1,0,GLFW_RELEASE,0);
    ManejaTeclado(handle,GLFW_KEY_2,0,GLFW_RELEASE,0);
    ManejaTeclado(handle,GLFW_KEY_4,0,GLFW_PRESS,0);
    ManejaTeclado(handle,GLFW_KEY_4,0,GLFW_RELEASE,0);
    if(vista!=0)return false;
    ManejaFoco(handle,1);puts("Camara: WASD y mouse desde inicio; 1/2/3 y retorno 4 OK");return true;
}
