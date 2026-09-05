// Practica 2: Proyeccion ortogonal y transformaciones geometricas.
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
const float toRadians = 3.14159265f / 180.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
glm::mat4 projection;
enum MeshId { PIRAMIDE = 0, CUBO = 1 };
enum ShaderId { ROJO = 0, VERDE, AZUL, CAFE, MAGENTA, AMARILLO };
struct ShaderFiles { const char* vertex; const char* fragment; };
const ShaderFiles shaderFiles[] = {
	{ "shaders/shaderrojo.vert", "shaders/shaderrojo.frag" }, { "shaders/shaderverde.vert", "shaders/shaderverde.frag" },
	{ "shaders/shaderazul.vert", "shaders/shaderazul.frag" }, { "shaders/shadercafe.vert", "shaders/shadercafe.frag" },
	{ "shaders/shadermagenta.vert", "shaders/shadermagenta.frag" }, { "shaders/shaderamarillo.vert", "shaders/shaderamarillo.frag" }
};
void CrearPiramideCuadrangular() {
	const unsigned int i[] = {0,3,4,3,2,4,2,1,4,1,0,4,0,1,2,0,2,3};
	GLfloat v[] = {.5f,-.5f,.5f,1,1,1,.5f,-.5f,-.5f,1,1,1,-.5f,-.5f,-.5f,1,1,1,-.5f,-.5f,.5f,1,1,1,0,.5f,0,1,1,1};
	Mesh* p = new Mesh(); p->CreateMesh(v,const_cast<unsigned int*>(i),30,18); meshList.push_back(p);
}
void CrearCubo() {
	const unsigned int i[] = {0,1,2,2,3,0,1,5,6,6,2,1,7,6,5,5,4,7,4,0,3,3,7,4,4,5,1,1,0,4,3,2,6,6,7,3};
	GLfloat v[] = {-.5f,-.5f,.5f,1,1,1,.5f,-.5f,.5f,1,1,1,.5f,.5f,.5f,1,1,1,-.5f,.5f,.5f,1,1,1,-.5f,-.5f,-.5f,1,1,1,.5f,-.5f,-.5f,1,1,1,.5f,.5f,-.5f,1,1,1,-.5f,.5f,-.5f,1,1,1};
	Mesh* c = new Mesh(); c->CreateMesh(v,const_cast<unsigned int*>(i),48,36); meshList.push_back(c);
}
void CreateShaders() {
	shaderList.reserve(6);
	for (const ShaderFiles& f : shaderFiles) {
		shaderList.emplace_back();
		shaderList.back().CreateFromFiles(f.vertex, f.fragment);
	}
}
void RenderMesh(ShaderId sid, MeshId mid, float x,float y,float z,float sx,float sy,float sz,float rz=0) {
	// Cada llamada crea una instancia: la mueve, gira y escala sin modificar la figura original.
	Shader& s=shaderList[sid]; s.useShader(); glm::mat4 m(1); m=glm::translate(m,glm::vec3(x,y,z)); m=glm::rotate(m,rz*toRadians,glm::vec3(0,0,1)); m=glm::scale(m,glm::vec3(sx,sy,sz));
	glUniformMatrix4fv(s.getModelLocation(),1,GL_FALSE,glm::value_ptr(m)); glUniformMatrix4fv(s.getProjectLocation(),1,GL_FALSE,glm::value_ptr(projection)); meshList[mid]->RenderMesh();
}
void Trazo(ShaderId c,float x,float y,float w,float h,float z,float r=0) { RenderMesh(c,CUBO,x,y,z,w,h,.08f,r); }
// Separa las iniciales de las figuras para que se lean como un bloque independiente.
void TrazoInicial(ShaderId c,float x,float y,float w,float h,float z,float r=0) { Trazo(c,x,y+.10f,w,h,z,r); }
void DibujarIniciales(float z) {
	// Las cuatro letras se forman con cubos escalados y cada una usa un shader distinto.
	// J verde
	TrazoInicial(VERDE,-1.08f,.78f,.26f,.05f,z); TrazoInicial(VERDE,-.97f,.62f,.05f,.31f,z); TrazoInicial(VERDE,-1.08f,.45f,.23f,.05f,z); TrazoInicial(VERDE,-1.18f,.50f,.05f,.14f,z);
	// B azul
	TrazoInicial(AZUL,-.64f,.61f,.05f,.36f,z); TrazoInicial(AZUL,-.54f,.78f,.20f,.05f,z); TrazoInicial(AZUL,-.54f,.61f,.20f,.05f,z); TrazoInicial(AZUL,-.54f,.44f,.20f,.05f,z); TrazoInicial(AZUL,-.44f,.70f,.05f,.12f,z); TrazoInicial(AZUL,-.44f,.52f,.05f,.12f,z);
	// C magenta, abierta a la derecha
	TrazoInicial(MAGENTA,-.08f,.78f,.24f,.05f,z); TrazoInicial(MAGENTA,-.19f,.61f,.05f,.36f,z); TrazoInicial(MAGENTA,-.08f,.44f,.24f,.05f,z);
	// M cafe
	TrazoInicial(CAFE,.25f,.61f,.05f,.36f,z); TrazoInicial(CAFE,.55f,.61f,.05f,.36f,z); TrazoInicial(CAFE,.34f,.68f,.05f,.21f,z,35); TrazoInicial(CAFE,.46f,.68f,.05f,.21f,z,-35);
}
void DibujarFiguras(float z) {
	// Columna de piramides y postes
	RenderMesh(AMARILLO,PIRAMIDE,-.95f,.19f,z,.44f,.25f,.25f,180); RenderMesh(ROJO,PIRAMIDE,-.95f,-.10f,z,.44f,.25f,.25f,180); RenderMesh(VERDE,PIRAMIDE,-.95f,-.39f,z,.44f,.25f,.25f,180); RenderMesh(CAFE,CUBO,-1.20f,-.10f,z,.06f,1.10f,.10f); RenderMesh(CAFE,CUBO,-.70f,-.10f,z,.06f,1.10f,.10f);
	// Cuadrado central, rombo azul y rombo cafe
	RenderMesh(AMARILLO,CUBO,-.15f,.02f,z,.30f,.30f,.08f); RenderMesh(ROJO,CUBO,.15f,.02f,z,.30f,.30f,.08f); RenderMesh(MAGENTA,CUBO,-.15f,-.28f,z,.30f,.30f,.08f); RenderMesh(VERDE,CUBO,.15f,-.28f,z,.30f,.30f,.08f); RenderMesh(AZUL,CUBO,0,-.13f,z+.01f,.43f,.43f,.10f,45); RenderMesh(CAFE,CUBO,0,-.13f,z+.02f,.19f,.19f,.11f,45);
	// Trifuerza: las puntas laterales coinciden con los extremos de las dos piramides centrales.
	RenderMesh(MAGENTA,PIRAMIDE,.88f,.12f,z,.24f,.24f,.24f); RenderMesh(VERDE,PIRAMIDE,.76f,-.12f,z,.24f,.24f,.24f); RenderMesh(AMARILLO,PIRAMIDE,.88f,-.12f,z,.24f,.24f,.24f,180); RenderMesh(ROJO,PIRAMIDE,1.00f,-.12f,z,.24f,.24f,.24f);
}
int main() {
	mainWindow=Window(1000,750); mainWindow.Initialise(); CrearPiramideCuadrangular(); CrearCubo(); CreateShaders();
	// Proyeccion ortogonal: conserva el tamano de las figuras aunque cambie su posicion en Z.
	projection=glm::ortho(-1.3333f,1.3333f,-1.0f,1.0f,.1f,100.0f);
	while(!mainWindow.getShouldClose()) {
		glfwPollEvents(); glClearColor(.12f,.12f,.12f,1); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		// Se dibujan las letras y las composiciones en cada cuadro de la ventana.
		DibujarIniciales(-3); DibujarFiguras(-3); glUseProgram(0); mainWindow.swapBuffers();
	}
	return 0;
}
