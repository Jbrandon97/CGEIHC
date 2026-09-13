#include "Window.h"
Window::Window() : Window(1280,800) {}
Window::Window(GLint w,GLint h):mainWindow(nullptr),width(w),height(h),rotax(0),rotay(0),rotaz(0),bufferWidth(w),bufferHeight(h),lastX(0),lastY(0),xChange(0),yChange(0),mouseFirstMoved(true){for(auto& k:keys)k=false;}
int Window::Initialise(bool hidden){
 if(!glfwInit())return 1;
 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
 glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
 glfwWindowHint(GLFW_VISIBLE,hidden?GLFW_FALSE:GLFW_TRUE);
 mainWindow=glfwCreateWindow(width,height,"P03 | Cohete y cruz | Flechas: orbitar | WASD: mover | Rueda: acercar | Espacio: giro | R: inicio",nullptr,nullptr);
 if(!mainWindow)return 1;
 glfwMakeContextCurrent(mainWindow);glfwSetWindowUserPointer(mainWindow,this);createCallbacks();
 glewExperimental=GL_TRUE;if(glewInit()!=GLEW_OK)return 1;
 while(glGetError()!=GL_NO_ERROR){}
 glEnable(GL_DEPTH_TEST);glfwSwapInterval(hidden?0:1);return 0;
}
void Window::createCallbacks(){glfwSetKeyCallback(mainWindow,ManejaTeclado);glfwSetCursorPosCallback(mainWindow,ManejaMouse);glfwSetScrollCallback(mainWindow,ManejaRueda);}
void Window::pollEvents(){glfwPollEvents();glfwGetFramebufferSize(mainWindow,&bufferWidth,&bufferHeight);}
GLfloat Window::getXChange(){float v=xChange;xChange=0;return v;}
GLfloat Window::getYChange(){float v=yChange;yChange=0;return v;}
float Window::getScroll(){float v=scroll;scroll=0;return v;}
bool Window::takeReset(){bool v=reset;reset=false;return v;}
void Window::ManejaTeclado(GLFWwindow* w,int key,int,int action,int){
 auto* self=static_cast<Window*>(glfwGetWindowUserPointer(w));
 if(key>=0&&key<1024)self->keys[key]=action!=GLFW_RELEASE;
 if(action!=GLFW_PRESS)return;
 if(key==GLFW_KEY_ESCAPE)glfwSetWindowShouldClose(w,1);
 if(key==GLFW_KEY_R){self->reset=true;self->automatic=false;}
 if(key==GLFW_KEY_SPACE)self->automatic=!self->automatic;
}
void Window::ManejaMouse(GLFWwindow* w,double x,double y){
 auto* self=static_cast<Window*>(glfwGetWindowUserPointer(w));
 if(self->mouseFirstMoved){self->lastX=float(x);self->lastY=float(y);self->mouseFirstMoved=false;}
 if(glfwGetMouseButton(w,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS){self->xChange+=float(x)-self->lastX;self->yChange+=self->lastY-float(y);}
 self->lastX=float(x);self->lastY=float(y);
}
void Window::ManejaRueda(GLFWwindow* w,double,double y){static_cast<Window*>(glfwGetWindowUserPointer(w))->scroll+=float(y);}
Window::~Window(){if(mainWindow)glfwDestroyWindow(mainWindow);glfwTerminate();}
