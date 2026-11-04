#pragma once
#include <glew.h>
#include <glfw3.h>
class Camera;
class Window {
public:
    Window(GLint w=1280,GLint h=800):width(w),height(h){}
    int Initialise();
    bool getShouldClose() const;
    bool* getsKeys(){return keys;}
    GLfloat getXChange();
    GLfloat getYChange();
    void swapBuffers();
    bool EnFoco() const;
    int Vista() const {return vista;}
    void ControlarCamara(Camera&,float dt);
    bool ProbarControles();
    bool ConsumirTecla(int);
    void MostrarTiempo(int);
    void MostrarControles() const;
    ~Window();
private:
    GLFWwindow* handle=nullptr;
    GLint width,height;
    int vista=0;
    bool keys[1024]={},primerMouse=true;
    bool pulsaciones[1024]={};
    double lastX=0,lastY=0;
    GLfloat xChange=0,yChange=0;
    static void ManejaTeclado(GLFWwindow*,int,int,int,int);
    static void ManejaMouse(GLFWwindow*,double,double);
    static void ManejaFoco(GLFWwindow*,int);
};
