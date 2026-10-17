#pragma once
#include <glew.h>
#include <glfw3.h>

class Window {
public:
    Window(GLint width=1280, GLint height=800);
    int Initialise();
    void MostrarControles() const;
    bool getShouldClose() const;
    bool* getsKeys() { return keys; }
    GLfloat getXChange();
    GLfloat getYChange();
    void swapBuffers();
    void ProcesarTecla(int key, int action);
    bool principalesActivas() const { return principales; }
    bool motorActivo(int i) const { return motores[i]; }
    int vistaActual() const { return vista; }
    void RestablecerLuces();
    bool ProbarControles();
    void AplicarCasoPrueba(int mask);
    ~Window();
private:
    GLFWwindow* handle=nullptr;
    GLint width,height;
    bool keys[1024]={};
    bool principales=true, motores[2]={true,true};
    int vista=0;
    bool primerMouse=true;
    double lastX=0,lastY=0;
    GLfloat xChange=0,yChange=0;
    static void ManejaTeclado(GLFWwindow*,int,int,int,int);
    static void ManejaMouse(GLFWwindow*,double,double);
};
