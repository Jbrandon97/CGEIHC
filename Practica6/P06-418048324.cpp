#define STB_IMAGE_IMPLEMENTATION
#include "CommonValues.h"
#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <filesystem>
#define NOMINMAX
#include <Windows.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"

Window mainWindow(1200, 760);
MeshModel holocronCodigo;

void CrearHolocron()
{
    // Posiciones del holocron y caras triangulares.
    const glm::vec3 posiciones[] = {
        {-2.51727390f, -2.57909489f, 0.00211500f},
        {0.04964900f, 2.44012809f, -2.41721702f},
        {0.05115200f, 2.39978099f, 2.54504299f},
        {-2.64678597f, -0.13904700f, 2.49375701f},
        {2.59300303f, -0.08522200f, 2.50575495f},
        {-0.14069600f, -2.60193396f, 2.55856109f},
        {2.53494406f, -0.06114600f, -2.58028007f},
        {0.08143200f, -2.51320410f, -2.42600489f},
        {-2.45397997f, -0.06338800f, -2.47745109f},
        {2.63220096f, -2.55389190f, 0.00722600f},
        {2.52120590f, 2.44824100f, 0.06338800f},
        {-2.45137191f, 2.42175889f, 0.07753800f},
    };
    const unsigned int caras[][3] = {
        {6, 10, 9},
        {4, 9, 10},
        {1, 10, 6},
        {7, 6, 9},
        {7, 9, 0},
        {5, 9, 4},
        {5, 3, 0},
        {5, 0, 9},
        {2, 4, 10},
        {2, 3, 5},
        {2, 5, 4},
        {8, 7, 0},
        {8, 0, 3},
        {8, 1, 6},
        {8, 6, 7},
        {11, 10, 1},
        {11, 2, 10},
        {11, 3, 2},
        {11, 8, 3},
        {11, 1, 8},
    };
    // UV separadas para colocar cada logo.
    const glm::vec2 uv[][3] = {
        {{0.33174530f, 0.20564157f}, {0.32989603f, 0.44604111f}, {0.09485056f, 0.21438508f}},
        {{0.09775078f, 0.44604111f}, {0.09485056f, 0.21438508f}, {0.32989603f, 0.44604111f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
        {{0.32412478f, 0.77763426f}, {0.09774586f, 0.79121858f}, {0.32351565f, 0.54880810f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
        {{0.09897243f, 0.54880816f}, {0.32351565f, 0.54880810f}, {0.09774586f, 0.79121858f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
        {{0.39250201f, 0.44528785f}, {0.39250195f, 0.21333596f}, {0.61234576f, 0.22198316f}},
        {{0.39250201f, 0.44528785f}, {0.61234576f, 0.22198316f}, {0.61486435f, 0.45461822f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
        {{0.38586646f, 0.55319494f}, {0.60980153f, 0.55451530f}, {0.60935754f, 0.77572656f}},
        {{0.91366947f, 0.78622055f}, {0.67836517f, 0.78622055f}, {0.67929488f, 0.55166721f}},
        {{0.91366947f, 0.78622055f}, {0.67929488f, 0.55166721f}, {0.91002017f, 0.55194902f}},
        {{0.68004692f, 0.21773033f}, {0.90936482f, 0.44894144f}, {0.68004692f, 0.44907010f}},
        {{0.68004692f, 0.21773033f}, {0.91019362f, 0.21962450f}, {0.90936482f, 0.44894144f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
        {{0.38586646f, 0.77912468f}, {0.38586646f, 0.55319494f}, {0.60935754f, 0.77572656f}},
        {{0.02f, 0.02f}, {0.02f, 0.02f}, {0.02f, 0.02f}},
    };
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < 20; ++i)
    {
        glm::vec3 a = posiciones[caras[i][0]];
        glm::vec3 b = posiciones[caras[i][1]];
        glm::vec3 c = posiciones[caras[i][2]];
        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        for (unsigned int j = 0; j < 3; ++j)
        {
            glm::vec3 v = posiciones[caras[i][j]];
            vertices.insert(vertices.end(), {v.x, v.y, v.z, uv[i][j].x,
                uv[i][j].y, normal.x, normal.y, normal.z});
            indices.push_back(static_cast<unsigned int>(indices.size()));
        }
    }
    holocronCodigo.CreateMeshModel(vertices.data(), indices.data(),
        static_cast<unsigned int>(vertices.size()), static_cast<unsigned int>(indices.size()));
}

int main()
{
    // Los recursos se buscan junto al ejecutable.
    wchar_t ruta[MAX_PATH];
    GetModuleFileNameW(nullptr, ruta, MAX_PATH);
    std::filesystem::current_path(std::filesystem::path(ruta).parent_path());
    if (mainWindow.Initialise() != 0) return 1;
    glfwSwapInterval(1);
    printf("OpenGL: %s | GPU: %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));
    while (glGetError() != GL_NO_ERROR) {}
    CrearHolocron();
    Shader shader;
    shader.CreateFromFiles("shaders/shader_texture.vert", "shaders/shader_texture.frag");
    shader.UseShader();
    GLint programa = 0, enlazado = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programa);
    glGetProgramiv(programa, GL_LINK_STATUS, &enlazado);
    if (!enlazado) return 2;

    // Carga la textura del holocron.
    Texture holocronTexture("Textures/holocron_textura.png");
    if (!holocronTexture.LoadTextureA()) return 3;
    Model holocronBlender, avion;
    if (!holocronBlender.LoadModel("Models/holocron_blender.obj")) return 4;
    if (!avion.LoadModel("Models/ripslinger.obj")) return 5;
    printf("1: Holocron por codigo | 2: Blender | 3: Avion | 4: Alas | 0: Todos\n");
    printf("Flechas: girar vista | W/S: acercar/alejar | R: restablecer | Esc: salir\n");
    fflush(stdout);
    int vista = 0;
    float angulo = 0.0f, elevacion = 15.0f, distancia = 25.0f;
    double anterior = glfwGetTime();
    while (!mainWindow.getShouldClose())
    {
        double ahora = glfwGetTime();
        float dt = std::min(static_cast<float>(ahora - anterior), 0.05f);
        anterior = ahora;
        glfwPollEvents();
        bool* teclas = mainWindow.getsKeys();
        int elegida = mainWindow.consumeVista();
        if (elegida >= 0)
        {
            vista = elegida; angulo = vista == 0 || vista == 4 ? 0.0f : 28.0f;
            elevacion = vista == 4 ? 65.0f : (vista == 3 ? 20.0f : 18.0f);
            distancia = vista == 0 ? 25.0f : (vista >= 3 ? 10.0f : 9.0f);
        }
        if (teclas[GLFW_KEY_LEFT]) angulo -= 65.0f * dt;
        if (teclas[GLFW_KEY_RIGHT]) angulo += 65.0f * dt;
        if (teclas[GLFW_KEY_UP]) elevacion += 50.0f * dt;
        if (teclas[GLFW_KEY_DOWN]) elevacion -= 50.0f * dt;
        if (teclas[GLFW_KEY_W]) distancia -= 8.0f * dt;
        if (teclas[GLFW_KEY_S]) distancia += 8.0f * dt;
        if (teclas[GLFW_KEY_R]) { angulo = vista == 0 || vista == 4 ? 0.0f : 28.0f; elevacion = vista == 4 ? 65.0f : 20.0f; distancia = vista == 0 ? 25.0f : (vista >= 3 ? 10.0f : 9.0f); }
        elevacion = std::clamp(elevacion, -80.0f, 80.0f);
        distancia = std::clamp(distancia, 5.0f, 45.0f);
        const char* nombres[] = {"Todos", "1 Holocron por codigo", "2 Holocron Blender", "3 Avion Ripslinger", "4 Alas del avion"};
        char titulo[240];
        snprintf(titulo, sizeof(titulo), "P06 - %s | 0-4: vistas | Flechas: girar | W/S: zoom | R: inicio | Esc: salir", nombres[vista]);
        glfwSetWindowTitle(glfwGetCurrentContext(), titulo);
        int ancho, alto;
        glfwGetFramebufferSize(glfwGetCurrentContext(), &ancho, &alto);
        if (!ancho || !alto) { glfwWaitEvents(); continue; }
        glViewport(0, 0, ancho, alto);
        glClearColor(0.12f, 0.15f, 0.19f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.UseShader();
        float a = glm::radians(angulo), e = glm::radians(elevacion);
        glm::vec3 ojo(distancia * cos(e) * sin(a), distancia * sin(e), distancia * cos(e) * cos(a));
        glm::mat4 view = glm::lookAt(ojo, glm::vec3(0), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(ancho) / alto, 0.1f, 200.0f);
        glUniformMatrix4fv(shader.GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(shader.GetViewLocation(), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(shader.getColorLocation(), 1, 1, 1);
        if (vista == 0 || vista == 1)
        {
            glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(vista == 0 ? -7.0f : 0.0f, 0, 0));
            glUniformMatrix4fv(shader.GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
            holocronTexture.UseTexture();
            holocronCodigo.RenderMeshModel();
        }
        if (vista == 0 || vista == 2)
        {
            glm::mat4 model = glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0));
            glUniformMatrix4fv(shader.GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
            holocronBlender.RenderModel();
        }
        if (vista == 0 || vista >= 3)
        {
            // Dibuja el avion con su textura y UV corregidas.
            glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(vista == 0 ? 7.5f : 0.0f, 0, 0));
            model = glm::scale(model, glm::vec3(0.065f));
            model = glm::translate(model, glm::vec3(0, -14, 23));
            glUniformMatrix4fv(shader.GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
            avion.RenderModel();
        }
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) { printf("Error OpenGL: %u\n", error); return 6; }
        mainWindow.swapBuffers();
    }
    avion.ClearModel();
    holocronBlender.ClearModel();
    holocronCodigo.ClearMeshModel();
    holocronTexture.ClearTexture();
    shader.ClearShader();
    printf("Cierre correcto.\n");
    return 0;
}
