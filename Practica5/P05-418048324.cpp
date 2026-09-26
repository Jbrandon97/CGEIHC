/*
Practica 5: Optimizacion y carga de modelos
Se muestran al mismo tiempo el Rover, el Holocron y el satelite.
*/

#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <array>
#include <cmath>
#include <string>
#include <vector>

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "Mesh_tn.h"
#include "Model.h"
#include "Shader_m.h"
#include "Skybox.h"
#include "Window.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
Camera camera;
Skybox skybox;

std::vector<MeshModel*> meshListModel;
std::vector<Shader> shaderList;

Model rover;
Model holocronCentro;
Model holocronCristal;
std::array<Model, 8> holocronEsquinas;
Model sateliteCuerpo;
Model satelitePanelIzquierdo;
Model satelitePanelDerecho;
Model sateliteAntena;

std::array<GLfloat, 8> angulosEsquinas = {};
// Cada eje apunta del centro del Holocron hacia su esquina correspondiente.
// Asi la pieza gira en su lugar como un trompo y no atraviesa el cuerpo.
const std::array<glm::vec3, 8> ejesEsquinas = {
    glm::normalize(glm::vec3(-2.61f, -2.58f, -2.65f)),
    glm::normalize(glm::vec3(-2.62f, -2.66f,  2.55f)),
    glm::normalize(glm::vec3(-2.61f,  2.64f, -2.55f)),
    glm::normalize(glm::vec3(-2.61f,  2.56f,  2.66f)),
    glm::normalize(glm::vec3( 2.61f, -2.56f, -2.65f)),
    glm::normalize(glm::vec3( 2.60f, -2.65f,  2.52f)),
    glm::normalize(glm::vec3( 2.62f,  2.66f, -2.53f)),
    glm::normalize(glm::vec3( 2.59f,  2.58f,  2.66f))
};
glm::vec3 posicionSatelite(10.0f, 1.0f, -4.0f);
glm::vec3 rotacionSatelite(0.0f);
GLfloat anguloPanelIzquierdo = 0.0f;
GLfloat anguloPanelDerecho = 0.0f;
GLfloat anguloAntena = 0.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

static const char* vShader = "shaders/shader_m.vert";
static const char* fShader = "shaders/shader_m.frag";


void CrearPiso()
{
    // El piso sirve como referencia para apreciar los movimientos de la escena.
    unsigned int indices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat vertices[] = {
        -1.0f, 0.0f, -1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f
    };

    MeshModel* piso = new MeshModel();
    piso->CreateMeshModel(vertices, indices, 32, 6);
    meshListModel.push_back(piso);
}


void CrearShaders()
{
    Shader* shader = new Shader();
    shader->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader);
}


void CargarModelos()
{
    // Cada archivo se carga por separado para poder aplicar su propia transformacion.
    rover.LoadModel("Models/RoverModificado.obj");

    holocronCentro.LoadModel("Models/Holocron_Centro.obj");
    holocronCristal.LoadModel("Models/Holocron_Cristal.obj");
    for (int i = 0; i < 8; i++)
    {
        std::string ruta = "Models/Holocron_Esquina_0" + std::to_string(i + 1) + ".obj";
        holocronEsquinas[i].LoadModel(ruta);
    }

    sateliteCuerpo.LoadModel("Models/Satelite_Cuerpo.obj");
    satelitePanelIzquierdo.LoadModel("Models/Satelite_Panel_Izquierdo.obj");
    satelitePanelDerecho.LoadModel("Models/Satelite_Panel_Derecho.obj");
    sateliteAntena.LoadModel("Models/Satelite_Antena.obj");
}


void CrearSkybox()
{
    // Las seis imagenes forman el fondo y ayudan a presentar los modelos en el espacio.
    std::vector<std::string> caras = {
        "Textures/Skybox/cupertin-lake-night_rt.tga",
        "Textures/Skybox/cupertin-lake-night_lf.tga",
        "Textures/Skybox/cupertin-lake-night_dn.tga",
        "Textures/Skybox/cupertin-lake-night_up.tga",
        "Textures/Skybox/cupertin-lake-night_bk.tga",
        "Textures/Skybox/cupertin-lake-night_ft.tga"
    };
    skybox = Skybox(caras);
}


void AjustarAngulo(GLfloat& angulo)
{
    while (angulo >= 360.0f) angulo -= 360.0f;
    while (angulo < 0.0f) angulo += 360.0f;
}


void ActualizarControles(bool* teclas, GLfloat tiempo)
{
    // Se evita un salto grande si la ventana se detiene por un momento.
    if (tiempo > 0.05f) tiempo = 0.05f;

    const GLfloat velocidadEsquina = 55.0f;
    const GLfloat sentido = (teclas[GLFW_KEY_LEFT_SHIFT] || teclas[GLFW_KEY_RIGHT_SHIFT]) ? -1.0f : 1.0f;

    // Los numeros controlan las ocho esquinas; Shift cambia el sentido del giro.
    for (int i = 0; i < 8; i++)
    {
        if (teclas[GLFW_KEY_1 + i])
        {
            angulosEsquinas[i] += sentido * velocidadEsquina * tiempo;
            AjustarAngulo(angulosEsquinas[i]);
        }
    }

    const GLfloat velocidadMovimiento = 3.5f * tiempo;
    if (teclas[GLFW_KEY_LEFT]) posicionSatelite.x -= velocidadMovimiento;
    if (teclas[GLFW_KEY_RIGHT]) posicionSatelite.x += velocidadMovimiento;
    if (teclas[GLFW_KEY_PAGE_UP]) posicionSatelite.y += velocidadMovimiento;
    if (teclas[GLFW_KEY_PAGE_DOWN]) posicionSatelite.y -= velocidadMovimiento;
    if (teclas[GLFW_KEY_UP]) posicionSatelite.z -= velocidadMovimiento;
    if (teclas[GLFW_KEY_DOWN]) posicionSatelite.z += velocidadMovimiento;

    // Estos tres pares permiten orientar el satelite completo en sus tres ejes.
    const GLfloat velocidadRotacion = 48.0f * tiempo;
    if (teclas[GLFW_KEY_I]) rotacionSatelite.x += velocidadRotacion;
    if (teclas[GLFW_KEY_K]) rotacionSatelite.x -= velocidadRotacion;
    if (teclas[GLFW_KEY_J]) rotacionSatelite.y += velocidadRotacion;
    if (teclas[GLFW_KEY_L]) rotacionSatelite.y -= velocidadRotacion;
    if (teclas[GLFW_KEY_U]) rotacionSatelite.z += velocidadRotacion;
    if (teclas[GLFW_KEY_O]) rotacionSatelite.z -= velocidadRotacion;

    // Los paneles y la antena se mueven desde el punto donde se unen al cuerpo.
    const GLfloat velocidadPieza = 45.0f * tiempo;
    if (teclas[GLFW_KEY_Z]) anguloPanelIzquierdo += velocidadPieza;
    if (teclas[GLFW_KEY_X]) anguloPanelIzquierdo -= velocidadPieza;
    if (teclas[GLFW_KEY_C]) anguloPanelDerecho += velocidadPieza;
    if (teclas[GLFW_KEY_V]) anguloPanelDerecho -= velocidadPieza;
    if (teclas[GLFW_KEY_B]) anguloAntena += velocidadPieza;
    if (teclas[GLFW_KEY_N]) anguloAntena -= velocidadPieza;

    anguloPanelIzquierdo = glm::clamp(anguloPanelIzquierdo, -90.0f, 90.0f);
    anguloPanelDerecho = glm::clamp(anguloPanelDerecho, -90.0f, 90.0f);
    anguloAntena = glm::clamp(anguloAntena, -70.0f, 70.0f);
}


void DibujarModelo(Model& modelo, const glm::mat4& matriz, const glm::vec3& color,
    GLuint uniformeModelo, GLuint uniformeColor)
{
    glUniformMatrix4fv(uniformeModelo, 1, GL_FALSE, glm::value_ptr(matriz));
    glUniform3fv(uniformeColor, 1, glm::value_ptr(color));
    modelo.RenderModel();
}


glm::mat4 MatrizSatelite()
{
    glm::mat4 matriz(1.0f);
    matriz = glm::translate(matriz, posicionSatelite);
    matriz = glm::rotate(matriz, rotacionSatelite.x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    matriz = glm::rotate(matriz, rotacionSatelite.y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    matriz = glm::rotate(matriz, rotacionSatelite.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    matriz = glm::scale(matriz, glm::vec3(0.22f));
    return matriz;
}


glm::mat4 RotarDesdeUnion(const glm::mat4& padre, const glm::vec3& unionPieza, GLfloat angulo)
{
    // La pieza se lleva temporalmente a su union, gira y regresa a su lugar.
    glm::mat4 matriz = glm::translate(padre, unionPieza);
    matriz = glm::rotate(matriz, angulo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    matriz = glm::translate(matriz, -unionPieza);
    return matriz;
}


void MostrarControles()
{
    printf("Controles de la Practica 5\n");
    printf("WASD y mouse: camara\n");
    printf("1 a 8: girar cada esquina del Holocron\n");
    printf("Shift + 1 a 8: girar la esquina en sentido contrario\n");
    printf("Flechas y Page Up/Page Down: mover el satelite en X, Y y Z\n");
    printf("I/K, J/L, U/O: rotar el satelite en X, Y y Z\n");
    printf("Z/X: panel izquierdo, C/V: panel derecho, B/N: antena\n");
    printf("Escape: cerrar\n");
}


int main()
{
    mainWindow = Window(1366, 768);
    if (mainWindow.Initialise() != 0) return 1;

    CrearPiso();
    CrearShaders();
    CargarModelos();
    CrearSkybox();
    MostrarControles();

    camera = Camera(
        glm::vec3(0.0f, 5.5f, 30.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f,
        -8.0f,
        5.0f,
        0.12f
    );

    glm::mat4 projection = glm::perspective(
        45.0f * toRadians,
        static_cast<GLfloat>(mainWindow.getBufferWidth()) / mainWindow.getBufferHeight(),
        0.1f,
        1000.0f
    );

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = static_cast<GLfloat>(glfwGetTime());
        deltaTime = now - lastTime;
        lastTime = now;

        glfwPollEvents();
        bool* teclas = mainWindow.getsKeys();
        camera.keyControl(teclas, deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        ActualizarControles(teclas, deltaTime);

        glClearColor(0.01f, 0.015f, 0.03f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

        shaderList[0].UseShader();
        GLuint uniformeModelo = shaderList[0].GetModelLocation();
        GLuint uniformeProyeccion = shaderList[0].GetProjectionLocation();
        GLuint uniformeVista = shaderList[0].GetViewLocation();
        GLuint uniformeColor = shaderList[0].getColorLocation();

        glUniformMatrix4fv(uniformeProyeccion, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformeVista, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        // Se dibuja un piso oscuro para distinguir la altura de cada modelo.
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.2f, -3.0f));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 20.0f));
        glUniformMatrix4fv(uniformeModelo, 1, GL_FALSE, glm::value_ptr(model));
        glm::vec3 colorPiso(0.08f, 0.10f, 0.14f);
        glUniform3fv(uniformeColor, 1, glm::value_ptr(colorPiso));
        meshListModel[0]->RenderMeshModel();

        // El Rover se mantiene como referencia del trabajo anterior.
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-11.0f, -2.2f, -3.0f));
        model = glm::rotate(model, -18.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.35f));
        DibujarModelo(rover, model, glm::vec3(0.76f, 0.80f, 0.84f), uniformeModelo, uniformeColor);

        // El centro del Holocron es el padre comun de su cristal y de sus esquinas.
        glm::mat4 holocronBase(1.0f);
        holocronBase = glm::translate(holocronBase, glm::vec3(0.0f, 1.0f, -3.0f));
        holocronBase = glm::rotate(holocronBase, -12.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        holocronBase = glm::rotate(holocronBase, 28.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        holocronBase = glm::scale(holocronBase, glm::vec3(0.42f));

        DibujarModelo(holocronCentro, holocronBase, glm::vec3(0.06f, 0.20f, 0.58f), uniformeModelo, uniformeColor);
        DibujarModelo(holocronCristal, holocronBase, glm::vec3(0.12f, 0.72f, 0.95f), uniformeModelo, uniformeColor);
        for (int i = 0; i < 8; i++)
        {
            // El eje diagonal pasa por el centro y por la esquina, por eso no cambia de lugar.
            glm::mat4 matrizEsquina = glm::rotate(
                holocronBase,
                angulosEsquinas[i] * toRadians,
                ejesEsquinas[i]
            );
            DibujarModelo(
                holocronEsquinas[i],
                matrizEsquina,
                glm::vec3(0.94f, 0.55f, 0.08f),
                uniformeModelo,
                uniformeColor
            );
        }

        // El cuerpo transmite su movimiento a todas las piezas del satelite.
        glm::mat4 sateliteBase = MatrizSatelite();
        DibujarModelo(sateliteCuerpo, sateliteBase, glm::vec3(0.68f, 0.72f, 0.78f), uniformeModelo, uniformeColor);

        glm::mat4 panelIzquierdo = RotarDesdeUnion(
            sateliteBase,
            glm::vec3(-2.0f, 0.0f, -0.72f),
            anguloPanelIzquierdo
        );
        glm::mat4 panelDerecho = RotarDesdeUnion(
            sateliteBase,
            glm::vec3(2.0f, 0.0f, -0.72f),
            anguloPanelDerecho
        );
        glm::mat4 antena = RotarDesdeUnion(
            sateliteBase,
            glm::vec3(-2.30f, 0.0f, -0.15f),
            anguloAntena
        );

        DibujarModelo(satelitePanelIzquierdo, panelIzquierdo, glm::vec3(0.04f, 0.16f, 0.48f), uniformeModelo, uniformeColor);
        DibujarModelo(satelitePanelDerecho, panelDerecho, glm::vec3(0.04f, 0.16f, 0.48f), uniformeModelo, uniformeColor);
        DibujarModelo(sateliteAntena, antena, glm::vec3(0.95f, 0.58f, 0.10f), uniformeModelo, uniformeColor);

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
