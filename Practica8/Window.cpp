#include "Window.h"
#include <cstdio>
#include <cmath>

Window::Window(GLint w,GLint h):width(w),height(h) {}
int Window::Initialise() {
    if(!glfwInit())return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    handle=glfwCreateWindow(width,height,"P08 - Naboo N-1 | 1 principales, 2 izquierda, 3 derecha",nullptr,nullptr);
    if(!handle)return 1;
    glfwMakeContextCurrent(handle);
    glfwSetWindowUserPointer(handle,this);
    glfwSetKeyCallback(handle,ManejaTeclado);
    glfwSetCursorPosCallback(handle,ManejaMouse);
    glfwSetInputMode(handle,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glewExperimental=GL_TRUE;
    if(glewInit()!=GLEW_OK)return 1;
    glEnable(GL_DEPTH_TEST);
    return 0;
}
void Window::RestablecerLuces(){principales=true;motores[0]=motores[1]=true;}
void Window::ProcesarTecla(int key,int action) {
    if(key<0||key>=1024)return;
    if(action==GLFW_RELEASE){keys[key]=false;return;}
    if(action!=GLFW_PRESS||keys[key])return;
    keys[key]=true;
    if(key==GLFW_KEY_1)principales=!principales;
    if(key==GLFW_KEY_2)motores[0]=!motores[0];
    if(key==GLFW_KEY_3)motores[1]=!motores[1];
    if(key>=GLFW_KEY_F5&&key<=GLFW_KEY_F8)vista=key-GLFW_KEY_F5;
    if(key==GLFW_KEY_1||key==GLFW_KEY_2||key==GLFW_KEY_3)
        printf("Principales %s | Izquierdo %s | Derecho %s\n",principales?"ON":"OFF",motores[0]?"ON":"OFF",motores[1]?"ON":"OFF");
}
void Window::ManejaTeclado(GLFWwindow* h,int key,int,int action,int) {
    auto w=static_cast<Window*>(glfwGetWindowUserPointer(h));
    if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)glfwSetWindowShouldClose(h,1);
    w->ProcesarTecla(key,action);
}
void Window::ManejaMouse(GLFWwindow* h,double x,double y) {
    auto w=static_cast<Window*>(glfwGetWindowUserPointer(h));
    if(w->primerMouse){w->lastX=x;w->lastY=y;w->primerMouse=false;}
    double dx=x-w->lastX,dy=w->lastY-y;
    if(std::abs(dx)<100&&std::abs(dy)<100){w->xChange+=float(dx);w->yChange+=float(dy);}
    w->lastX=x;w->lastY=y;
}
GLfloat Window::getXChange(){float v=xChange;xChange=0;return v;}
GLfloat Window::getYChange(){float v=yChange;yChange=0;return v;}
bool Window::getShouldClose() const{return glfwWindowShouldClose(handle)!=0;}
void Window::swapBuffers(){glfwSwapBuffers(handle);}
Window::~Window(){if(handle)glfwDestroyWindow(handle);glfwTerminate();}

void Pulso(Window& w,int key) {w.ProcesarTecla(key,GLFW_PRESS);w.ProcesarTecla(key,GLFW_RELEASE);}
bool Window::ProbarControles() {
    Window& w=*this;
    for(int key=GLFW_KEY_1;key<=GLFW_KEY_3;key++){
        w.RestablecerLuces();
        for(int i=0;i<20;i++){
            w.ProcesarTecla(key,GLFW_PRESS);
            for(int j=0;j<10;j++)w.ProcesarTecla(key,GLFW_REPEAT);
            w.ProcesarTecla(key,GLFW_PRESS);
            bool estado=key==GLFW_KEY_1?w.principalesActivas():w.motorActivo(key-GLFW_KEY_2);
            if(estado!=(i%2==1))return false;
            if(key!=GLFW_KEY_1&&!w.principalesActivas())return false;
            if(key!=GLFW_KEY_2&&!w.motorActivo(0))return false;
            if(key!=GLFW_KEY_3&&!w.motorActivo(1))return false;
            w.ProcesarTecla(key,GLFW_RELEASE);
        }
    }
    w.RestablecerLuces();puts("60 pulsaciones, repeticion e independencia: OK");return true;
}

void Window::AplicarCasoPrueba(int mask) {
    RestablecerLuces();
    for(int bit=0;bit<3;bit++)if(!(mask&(1<<bit)))Pulso(*this,GLFW_KEY_1+bit);
}
