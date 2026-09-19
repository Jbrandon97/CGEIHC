/*
 Practica 4: Modelado jerarquico
 Rover de seis ruedas y sonda espacial inspirada en Voyager.

 Controles:
   W, A, S, D y mouse: mover la camara.
   Mantener R: girar continuamente el rover y la sonda.
   F, G, H: girar las tres articulaciones del brazo.
   1, 2, 3, 4, 5, 6: girar cada llanta por separado.
   Esc: cerrar el programa.
*/

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere esfera = Sphere(1.0f, 24, 24);

void CrearCubo()
{
	unsigned int indices[] = {
		0,1,2, 2,3,0, 1,5,6, 6,2,1,
		7,6,5, 5,4,7, 4,0,3, 3,7,4,
		4,5,1, 1,0,4, 3,2,6, 6,7,3
	};
	GLfloat vertices[] = {
		-0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
		 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(vertices, indices, 24, 36);
	meshList.push_back(cubo);
}

// El cilindro se usa para ejes, ruedas, antenas y brazos.
void CrearCilindro(int resolucion, float radio)
{
	vector<GLfloat> vertices;
	vector<unsigned int> indices;
	float paso = 2.0f * PI / resolucion;
	for (int n = 0; n <= resolucion; n++)
	{
		float angulo = (n == resolucion ? 0.0f : n * paso);
		float x = radio * cos(angulo);
		float z = radio * sin(angulo);
		vertices.push_back(x); vertices.push_back(-0.5f); vertices.push_back(z);
		vertices.push_back(x); vertices.push_back(0.5f); vertices.push_back(z);
	}
	for (int n = 0; n <= resolucion; n++)
	{
		float x = radio * cos(n * paso);
		float z = radio * sin(n * paso);
		vertices.push_back(x); vertices.push_back(-0.5f); vertices.push_back(z);
	}
	for (int n = 0; n <= resolucion; n++)
	{
		float x = radio * cos(n * paso);
		float z = radio * sin(n * paso);
		vertices.push_back(x); vertices.push_back(0.5f); vertices.push_back(z);
	}
	for (unsigned int i = 0; i < vertices.size() / 3; i++) indices.push_back(i);
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, (unsigned int)vertices.size(), (unsigned int)indices.size());
	meshList.push_back(cilindro);
}

// El cono abierto permite representar el plato de la antena de la sonda.
void CrearCono(int resolucion, float radio)
{
	vector<GLfloat> vertices;
	vector<unsigned int> indices;
	vertices.push_back(0.0f); vertices.push_back(0.5f); vertices.push_back(0.0f);
	float paso = 2.0f * PI / resolucion;
	for (int n = 0; n <= resolucion; n++)
	{
		vertices.push_back(radio * cos(n * paso));
		vertices.push_back(-0.5f);
		vertices.push_back(radio * sin(n * paso));
	}
	for (unsigned int i = 0; i < vertices.size() / 3; i++) indices.push_back(i);
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, (unsigned int)vertices.size(), (unsigned int)indices.size());
	meshList.push_back(cono);
}

void CrearShaders()
{
	// Se conserva el objeto durante toda la ejecucion, igual que en el codigo de clase.
	Shader* shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader);
}

struct Uniformes
{
	GLuint modelo;
	GLuint proyeccion;
	GLuint vista;
	GLuint color;
};

void Aplicar(glm::mat4 matriz, glm::vec3 color, GLuint uniformeModelo, GLuint uniformeColor)
{
	glUniformMatrix4fv(uniformeModelo, 1, GL_FALSE, glm::value_ptr(matriz));
	glUniform3fv(uniformeColor, 1, glm::value_ptr(color));
}

void DibujarCubo(glm::mat4 matriz, glm::vec3 escala, glm::vec3 color, const Uniformes& u)
{
	matriz = glm::scale(matriz, escala);
	Aplicar(matriz, color, u.modelo, u.color);
	meshList[0]->RenderMesh();
}

void DibujarCilindro(glm::mat4 matriz, glm::vec3 escala, glm::vec3 color, const Uniformes& u)
{
	matriz = glm::scale(matriz, escala);
	Aplicar(matriz, color, u.modelo, u.color);
	meshList[1]->RenderMeshGeometry();
}

void DibujarCono(glm::mat4 matriz, glm::vec3 escala, glm::vec3 color, const Uniformes& u)
{
	matriz = glm::scale(matriz, escala);
	Aplicar(matriz, color, u.modelo, u.color);
	meshList[2]->RenderMeshGeometry();
}

void DibujarEsfera(glm::mat4 matriz, glm::vec3 escala, glm::vec3 color, const Uniformes& u)
{
	matriz = glm::scale(matriz, escala);
	Aplicar(matriz, color, u.modelo, u.color);
	esfera.render();
}

// Dibuja una pareja de ruedas. El eje transversal pertenece a la base y es
// compartido, pero cada llanta conserva su propio angulo de giro.
void DibujarParRuedas(glm::mat4 matrizBase, float posicionX, int primerIndice, const Uniformes& u)
{
	const glm::vec3 grisClaro(0.72f, 0.74f, 0.76f);
	const glm::vec3 grisOscuro(0.30f, 0.32f, 0.34f);
	const glm::vec3 negro(0.06f, 0.07f, 0.08f);
	const glm::vec3 naranja(1.0f, 0.55f, 0.05f);

	glm::mat4 eje = glm::translate(matrizBase, glm::vec3(posicionX, -0.45f, 0.0f));
	glm::mat4 barraEje = glm::rotate(eje, glm::radians(90.0f), glm::vec3(1, 0, 0));
	DibujarCilindro(barraEje, glm::vec3(0.18f, 7.6f, 0.18f), grisOscuro, u);

	for (int lado = 0; lado < 2; lado++)
	{
		float signo = lado == 0 ? -1.0f : 1.0f;
		int indiceRueda = primerIndice + lado;

		// Primera parte de la L: sale del eje compartido hacia el costado.
		glm::mat4 esquina = glm::translate(eje, glm::vec3(0.0f, 0.0f, signo * 3.75f));
		glm::mat4 parteHorizontal = glm::translate(eje, glm::vec3(0.0f, 0.0f, signo * 3.35f));
		parteHorizontal = glm::rotate(parteHorizontal, glm::radians(90.0f), glm::vec3(1, 0, 0));
		DibujarCubo(parteHorizontal, glm::vec3(0.38f, 1.15f, 0.38f), grisClaro, u);

		// Segunda parte de la L: baja desde la esquina hasta el centro de la llanta.
		glm::mat4 centroRueda = glm::translate(esquina, glm::vec3(0.0f, -1.55f, signo * 0.75f));
		glm::mat4 parteVertical = glm::translate(esquina, glm::vec3(0.0f, -0.78f, signo * 0.38f));
		parteVertical = glm::rotate(parteVertical, glm::radians(signo * 12.0f), glm::vec3(1, 0, 0));
		DibujarCubo(parteVertical, glm::vec3(0.42f, 1.75f, 0.42f), grisClaro, u);

		glm::mat4 ejeRueda = glm::rotate(centroRueda, glm::radians(90.0f), glm::vec3(1, 0, 0));
		DibujarCilindro(ejeRueda, glm::vec3(0.26f, 0.85f, 0.26f), grisOscuro, u);

		// La rotacion se aplica desde el centro para que esta llanta sea independiente.
		glm::mat4 giro = glm::rotate(centroRueda,
			glm::radians(mainWindow.getGiroRueda(indiceRueda)), glm::vec3(0, 0, 1));
		glm::mat4 llanta = glm::rotate(giro, glm::radians(90.0f), glm::vec3(1, 0, 0));
		DibujarCilindro(llanta, glm::vec3(1.18f, 0.72f, 1.18f), negro, u);

		glm::mat4 rin = glm::translate(giro, glm::vec3(0.0f, 0.0f, signo * 0.42f));
		rin = glm::rotate(rin, glm::radians(90.0f), glm::vec3(1, 0, 0));
		DibujarCilindro(rin, glm::vec3(0.62f, 0.10f, 0.62f), grisClaro, u);

		// La línea naranja basta para seguir visualmente una vuelta completa.
		// Se conserva como hija de la rueda para que gire junto con la llanta y el rin.
		glm::mat4 marca = glm::translate(giro, glm::vec3(0.72f, 0.0f, signo * 0.51f));
		DibujarCubo(marca, glm::vec3(0.52f, 0.22f, 0.14f), naranja, u);
	}
}

void DibujarBrazo(glm::mat4 matrizBase, const Uniformes& u)
{
	const glm::vec3 blanco(0.88f, 0.90f, 0.92f);
	const glm::vec3 azul(0.05f, 0.35f, 0.95f);
	const glm::vec3 amarillo(1.0f, 0.75f, 0.08f);

	glm::mat4 soporte = glm::translate(matrizBase, glm::vec3(-3.1f, 1.15f, 2.4f));
	DibujarCubo(soporte, glm::vec3(1.25f, 1.5f, 1.0f), azul, u);

	// Articulacion 1 mueve el primer segmento y todo lo que depende de el.
	glm::mat4 articulacion1 = glm::translate(soporte, glm::vec3(0.0f, 0.85f, 0.0f));
	articulacion1 = glm::rotate(articulacion1,
		glm::radians(mainWindow.getarticulacion1()), glm::vec3(0, 0, 1));
	DibujarEsfera(articulacion1, glm::vec3(0.48f), azul, u);

	glm::mat4 brazo1 = glm::rotate(articulacion1, glm::radians(-38.0f), glm::vec3(0, 0, 1));
	brazo1 = glm::translate(brazo1, glm::vec3(0.0f, 1.65f, 0.0f));
	DibujarCilindro(brazo1, glm::vec3(0.42f, 3.3f, 0.42f), blanco, u);

	// Articulacion 2 hereda el movimiento anterior y mueve el segundo segmento.
	glm::mat4 articulacion2 = glm::translate(brazo1, glm::vec3(0.0f, 1.65f, 0.0f));
	articulacion2 = glm::rotate(articulacion2,
		glm::radians(mainWindow.getarticulacion2()), glm::vec3(0, 0, 1));
	DibujarEsfera(articulacion2, glm::vec3(0.46f), azul, u);

	glm::mat4 brazo2 = glm::rotate(articulacion2, glm::radians(72.0f), glm::vec3(0, 0, 1));
	brazo2 = glm::translate(brazo2, glm::vec3(0.0f, 1.8f, 0.0f));
	DibujarCilindro(brazo2, glm::vec3(0.38f, 3.6f, 0.38f), blanco, u);

	// Articulacion 3 gira la pinza completa desde la punta del brazo.
	glm::mat4 pinza = glm::translate(brazo2, glm::vec3(0.0f, 1.8f, 0.0f));
	pinza = glm::rotate(pinza,
		glm::radians(mainWindow.getarticulacion3()), glm::vec3(0, 0, 1));
	DibujarEsfera(pinza, glm::vec3(0.40f), azul, u);

	glm::mat4 basePinza = glm::translate(pinza, glm::vec3(0.0f, 0.35f, 0.0f));
	DibujarCubo(basePinza, glm::vec3(1.05f, 0.32f, 0.68f), amarillo, u);
	glm::mat4 mordazaI = glm::translate(pinza, glm::vec3(-0.42f, 0.95f, 0.0f));
	mordazaI = glm::rotate(mordazaI, glm::radians(20.0f), glm::vec3(0, 0, 1));
	DibujarCubo(mordazaI, glm::vec3(0.20f, 1.10f, 0.30f), amarillo, u);
	glm::mat4 mordazaD = glm::translate(pinza, glm::vec3(0.42f, 0.95f, 0.0f));
	mordazaD = glm::rotate(mordazaD, glm::radians(-20.0f), glm::vec3(0, 0, 1));
	DibujarCubo(mordazaD, glm::vec3(0.20f, 1.10f, 0.30f), amarillo, u);
}

void DibujarRover(const Uniformes& u)
{
	// Se recuperaron los colores usados en el ejercicio de clase.
	const glm::vec3 baseColor(0.0f, 1.0f, 0.0f);
	const glm::vec3 cabinaColor(1.0f, 0.0f, 1.0f);
	const glm::vec3 vidrio(0.12f, 0.30f, 0.38f);

	// Raiz del rover: cabina, ruedas y brazo parten de este mismo origen.
	glm::mat4 matrizRover = glm::translate(glm::mat4(1.0f), glm::vec3(-4.6f, 2.4f, -8.0f));
	matrizRover = glm::rotate(matrizRover, glm::radians(mainWindow.getrotay()), glm::vec3(0, 1, 0));
	matrizRover = glm::scale(matrizRover, glm::vec3(0.68f));
	glm::mat4 matrizBase = glm::translate(matrizRover, glm::vec3(0.0f, -0.25f, 0.0f));
	DibujarCubo(matrizBase, glm::vec3(9.2f, 1.5f, 7.0f), baseColor, u);

	// La cabina es hija de la base y se mantiene centrada sobre ella.
	glm::mat4 cabina = glm::translate(matrizBase, glm::vec3(0.8f, 1.75f, 0.0f));
	DibujarCubo(cabina, glm::vec3(6.7f, 2.2f, 5.3f), cabinaColor, u);
	glm::mat4 ventana = glm::translate(cabina, glm::vec3(0.0f, 0.15f, 2.68f));
	DibujarCubo(ventana, glm::vec3(3.4f, 0.65f, 0.10f), vidrio, u);

	// Tres ejes compartidos producen seis ruedas con patas completas en L.
	DibujarParRuedas(matrizBase, -3.7f, 0, u);
	DibujarParRuedas(matrizBase, 0.0f, 2, u);
	DibujarParRuedas(matrizBase, 3.7f, 4, u);
	DibujarBrazo(matrizBase, u);
}

void DibujarSonda(const Uniformes& u)
{
	const glm::vec3 dorado(0.78f, 0.60f, 0.20f);
	const glm::vec3 doradoClaro(0.95f, 0.76f, 0.28f);
	const glm::vec3 gris(0.72f, 0.74f, 0.78f);
	const glm::vec3 oscuro(0.10f, 0.12f, 0.16f);
	const glm::vec3 blanco(0.92f, 0.92f, 0.88f);
	const glm::vec3 azul(0.10f, 0.30f, 0.68f);

	// Cuerpo principal: es la raiz de todas las ramas de la sonda.
	glm::mat4 raiz = glm::translate(glm::mat4(1.0f), glm::vec3(4.4f, 3.4f, -8.0f));
	// La rotación investigada con glm::rotate se aplica a la raíz y mueve toda la sonda con R.
	raiz = glm::rotate(raiz, glm::radians(25.0f + mainWindow.getrotay()), glm::vec3(0, 1, 0));
	raiz = glm::scale(raiz, glm::vec3(0.82f));
	glm::mat4 cuerpo = raiz;
	DibujarCubo(cuerpo, glm::vec3(4.4f, 2.0f, 3.4f), dorado, u);

	// Paneles y caja de instrumentos dan más detalle sin perder la forma principal.
	glm::mat4 panelFrontal = glm::translate(cuerpo, glm::vec3(0.0f, 0.05f, 1.73f));
	DibujarCubo(panelFrontal, glm::vec3(2.7f, 1.15f, 0.12f), oscuro, u);
	glm::mat4 moduloSuperior = glm::translate(cuerpo, glm::vec3(0.0f, 1.02f, 0.0f));
	DibujarCubo(moduloSuperior, glm::vec3(2.5f, 0.38f, 2.2f), gris, u);
	glm::mat4 placaIzquierda = glm::translate(cuerpo, glm::vec3(-2.24f, 0.0f, 0.0f));
	DibujarCubo(placaIzquierda, glm::vec3(0.12f, 1.35f, 2.45f), doradoClaro, u);
	glm::mat4 placaDerecha = glm::translate(cuerpo, glm::vec3(2.24f, 0.0f, 0.0f));
	DibujarCubo(placaDerecha, glm::vec3(0.12f, 1.35f, 2.45f), doradoClaro, u);

	// Antena de alta ganancia inspirada en Voyager.
	glm::mat4 baseAntena = glm::translate(cuerpo, glm::vec3(0.0f, 1.15f, 0.0f));
	DibujarCilindro(baseAntena, glm::vec3(0.28f, 1.0f, 0.28f), gris, u);
	glm::mat4 plato = glm::translate(baseAntena, glm::vec3(0.0f, 1.15f, 0.0f));
	plato = glm::rotate(plato, glm::radians(180.0f), glm::vec3(1, 0, 0));
	DibujarCono(plato, glm::vec3(3.0f, 0.75f, 3.0f), blanco, u);
	glm::mat4 aroPlato = glm::translate(baseAntena, glm::vec3(0.0f, 0.80f, 0.0f));
	DibujarCilindro(aroPlato, glm::vec3(2.35f, 0.10f, 2.35f), gris, u);
	glm::mat4 receptor = glm::translate(baseAntena, glm::vec3(0.0f, 2.25f, 0.0f));
	DibujarEsfera(receptor, glm::vec3(0.34f), gris, u);

	// Tres soportes unen visualmente el receptor con el plato.
	for (int soporte = -1; soporte <= 1; soporte++)
	{
		glm::mat4 varilla = glm::translate(baseAntena, glm::vec3(soporte * 0.72f, 1.55f, -0.35f));
		varilla = glm::rotate(varilla, glm::radians(soporte * -22.0f), glm::vec3(0, 0, 1));
		DibujarCilindro(varilla, glm::vec3(0.07f, 1.15f, 0.07f), gris, u);
	}

	// Rama izquierda: brazo largo de instrumentos cientificos.
	glm::mat4 brazoCiencia = glm::translate(cuerpo, glm::vec3(-2.2f, 0.35f, 0.0f));
	brazoCiencia = glm::rotate(brazoCiencia, glm::radians(-68.0f), glm::vec3(0, 0, 1));
	glm::mat4 tramoCiencia = glm::translate(brazoCiencia, glm::vec3(0.0f, 2.8f, 0.0f));
	DibujarCilindro(tramoCiencia, glm::vec3(0.20f, 5.6f, 0.20f), gris, u);
	glm::mat4 instrumentos = glm::translate(tramoCiencia, glm::vec3(0.0f, 2.9f, 0.0f));
	DibujarCubo(instrumentos, glm::vec3(1.20f, 0.85f, 1.0f), azul, u);
	glm::mat4 camara1 = glm::translate(instrumentos, glm::vec3(-0.35f, 0.55f, 0.0f));
	DibujarCilindro(camara1, glm::vec3(0.22f, 0.85f, 0.22f), oscuro, u);
	glm::mat4 camara2 = glm::translate(instrumentos, glm::vec3(0.35f, 0.55f, 0.0f));
	DibujarCilindro(camara2, glm::vec3(0.22f, 0.85f, 0.22f), oscuro, u);

	// Rama derecha: generadores de energia colocados uno despues de otro.
	glm::mat4 brazoEnergia = glm::translate(cuerpo, glm::vec3(2.2f, -0.10f, 0.0f));
	brazoEnergia = glm::rotate(brazoEnergia, glm::radians(62.0f), glm::vec3(0, 0, 1));
	glm::mat4 tramoEnergia = glm::translate(brazoEnergia, glm::vec3(0.0f, 2.4f, 0.0f));
	DibujarCilindro(tramoEnergia, glm::vec3(0.18f, 4.8f, 0.18f), gris, u);
	for (int i = 0; i < 3; i++)
	{
		glm::mat4 generador = glm::translate(tramoEnergia, glm::vec3(0.0f, 1.7f + i * 0.75f, 0.0f));
		generador = glm::rotate(generador, glm::radians(90.0f), glm::vec3(0, 0, 1));
		DibujarCilindro(generador, glm::vec3(0.42f, 1.15f, 0.42f), oscuro, u);
	}

	// Mastil inferior con magnetometro, heredado directamente del cuerpo.
	glm::mat4 mastil = glm::translate(cuerpo, glm::vec3(0.0f, -2.5f, 0.0f));
	DibujarCilindro(mastil, glm::vec3(0.13f, 3.2f, 0.13f), gris, u);
	glm::mat4 magnetometro = glm::translate(mastil, glm::vec3(0.0f, -1.8f, 0.0f));
	DibujarEsfera(magnetometro, glm::vec3(0.46f), azul, u);
}

int main()
{
	mainWindow = Window(1024, 720);
	if (mainWindow.Initialise() != 0) return 1;

	CrearCubo();
	CrearCilindro(28, 1.0f);
	CrearCono(40, 1.0f);
	CrearShaders();
	esfera.init();
	esfera.load();

	// La misma cámara permite recorrer la escena donde se muestran los dos modelos.
	// Una velocidad amplia hace visible el recorrido con W, A, S y D.
	camera = Camera(glm::vec3(0.0f, 5.0f, 18.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f, -3.0f, 6.0f, 0.25f);

	Uniformes u;
	u.modelo = shaderList[0].getModelLocation();
	u.proyeccion = shaderList[0].getProjectLocation();
	u.vista = shaderList[0].getViewLocation();
	u.color = shaderList[0].getColorLocation();

	while (!mainWindow.getShouldClose())
	{
		GLfloat ahora = (GLfloat)glfwGetTime();
		deltaTime = ahora - lastTime;
		if (deltaTime > 0.05f) deltaTime = 0.05f;
		lastTime = ahora;
		glfwPollEvents();
		mainWindow.actualizarControles(deltaTime);
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.025f, 0.035f, 0.060f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();

		glm::mat4 proyeccion = glm::perspective(glm::radians(55.0f),
			static_cast<float>(mainWindow.getBufferWidth()) / mainWindow.getBufferHeight(), 0.1f, 100.0f);
		glUniformMatrix4fv(u.proyeccion, 1, GL_FALSE, glm::value_ptr(proyeccion));
		glUniformMatrix4fv(u.vista, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// El rover y la sonda se dibujan juntos para comparar sus dos jerarquías.
		DibujarRover(u);
		DibujarSonda(u);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
