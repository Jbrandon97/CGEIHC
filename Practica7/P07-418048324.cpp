// Practica 07 - Iluminacion 1 | Campos Medrano John Brandon | 418048324
#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX
#include <Windows.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "CommonValues.h"
#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
Window mainWindow(1280,800);
std::vector<MeshModel*> meshListModel;
MeshModel holocronCodigo;

struct Pieza { const char* archivo; int padre; glm::vec3 pivote; };
const Pieza piezas[] = {
    {"Models/rover_cuerpo.obj", -1, {0.0f,-0.534f,3.3f}},
    {"Models/rover_suspension_izquierda.obj", 0, {-0.5f,1.266f,3.3f}},
    {"Models/rover_suspension_derecha.obj", 0, {-0.5f,-2.334f,3.3f}},
    {"Models/rover_brazo_base.obj", 0, {1.745f,0.566f,4.42f}},
    {"Models/rover_brazo_1.obj", 3, {1.745f,0.566f,5.62f}},
    {"Models/rover_brazo_2.obj", 4, {4.279f,0.566f,8.176f}},
    {"Models/rover_pata_rueda_delantero_izquierdo.obj", 1, {2.145f,2.325f,3.846f}},
    {"Models/rover_pata_rueda_central_izquierdo.obj", 1, {-2.492f,2.325f,3.215f}},
    {"Models/rover_pata_rueda_trasero_izquierdo.obj", 1, {-3.492f,2.325f,3.215f}},
    {"Models/rover_pata_rueda_delantero_derecho.obj", 2, {2.145f,-3.394f,3.846f}},
    {"Models/rover_pata_rueda_central_derecho.obj", 2, {-2.492f,-3.394f,3.215f}},
    {"Models/rover_pata_rueda_trasero_derecho.obj", 2, {-3.492f,-3.394f,3.215f}},
};
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, 1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, 1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);
	MeshModel *obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		
		// back
		8, 10, 9,
		10, 8, 11,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 18, 17,
		18, 16, 19,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	// Cuatro vertices por cara para mantener separados los UV y las normales.
	GLfloat cubo_vertices[] = {
		// Frente: Rebel Alliance; region cuadrada de 128 x 128 pixeles.
		-0.50000000f, -0.50000000f, 0.50000000f, 0.37597656f, 0.99902344f, 0.00000000f, 0.00000000f, 1.00000000f,
		0.50000000f, -0.50000000f, 0.50000000f, 0.37597656f, 0.75097656f, 0.00000000f, 0.00000000f, 1.00000000f,
		0.50000000f, 0.50000000f, 0.50000000f, 0.62402344f, 0.75097656f, 0.00000000f, 0.00000000f, 1.00000000f,
		-0.50000000f, 0.50000000f, 0.50000000f, 0.62402344f, 0.99902344f, 0.00000000f, 0.00000000f, 1.00000000f,
		// Derecha: First Order; region cuadrada de 128 x 128 pixeles.
		0.50000000f, -0.50000000f, 0.50000000f, 0.37597656f, 0.74902344f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 0.37597656f, 0.50097656f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 0.62402344f, 0.50097656f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, 0.50000000f, 0.62402344f, 0.74902344f, 1.00000000f, 0.00000000f, 0.00000000f,
		// Atras: Mandalorian Crest; region cuadrada de 128 x 128 pixeles.
		-0.50000000f, -0.50000000f, -0.50000000f, 0.37597656f, 0.25097656f, 0.00000000f, 0.00000000f, -1.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 0.37597656f, 0.49902344f, 0.00000000f, 0.00000000f, -1.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 0.62402344f, 0.49902344f, 0.00000000f, 0.00000000f, -1.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.62402344f, 0.25097656f, 0.00000000f, 0.00000000f, -1.00000000f,
		// Izquierda: Separatists; region cuadrada de 128 x 128 pixeles.
		-0.50000000f, -0.50000000f, -0.50000000f, 0.37597656f, 0.24902344f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, -0.50000000f, 0.50000000f, 0.37597656f, 0.00097656f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, 0.50000000f, 0.62402344f, 0.00097656f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.62402344f, 0.24902344f, -1.00000000f, 0.00000000f, 0.00000000f,
		// Abajo: Galactic Republic; region cuadrada de 128 x 128 pixeles.
		-0.50000000f, -0.50000000f, 0.50000000f, 0.12597656f, 0.74902344f, 0.00000000f, -1.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, 0.50000000f, 0.37402344f, 0.74902344f, 0.00000000f, -1.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 0.37402344f, 0.50097656f, 0.00000000f, -1.00000000f, 0.00000000f,
		-0.50000000f, -0.50000000f, -0.50000000f, 0.12597656f, 0.50097656f, 0.00000000f, -1.00000000f, 0.00000000f,
		// Arriba: Galactic Empire; region cuadrada de 128 x 128 pixeles.
		-0.50000000f, 0.50000000f, 0.50000000f, 0.87402344f, 0.74902344f, 0.00000000f, 1.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, 0.50000000f, 0.62597656f, 0.74902344f, 0.00000000f, 1.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 0.62597656f, 0.50097656f, 0.00000000f, 1.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.87402344f, 0.50097656f, 0.00000000f, 1.00000000f, 0.00000000f,
	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);

}


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

struct Estado {
    glm::vec3 rover{-7.0f,0.10f,0.0f}, avion{5.0f,5.5f,0.0f};
    float patas[6]={}; int pata=0;
    float giroRover=0, giroAvion=0, base=0, brazo1=0, brazo2=0, suspension=0;
};
glm::mat4 TR(glm::vec3 p,float giro) {
    return glm::rotate(glm::translate(glm::mat4(1),p),glm::radians(giro),glm::vec3(0,1,0));
}
void Mover(Estado& e,const bool* k,float dt) {
    for(int j=0;j<6;j++)if(k[GLFW_KEY_4+j])e.pata=j;
    e.patas[e.pata]=std::clamp(e.patas[e.pata]+(float(k[GLFW_KEY_B])-float(k[GLFW_KEY_V]))*12*dt,-12.0f,12.0f);
    e.giroRover += (float(k[GLFW_KEY_J])-float(k[GLFW_KEY_L]))*45*dt;
    e.giroAvion += (float(k[GLFW_KEY_Z])-float(k[GLFW_KEY_X]))*35*dt;
    e.rover += glm::vec3(TR({},e.giroRover)*glm::vec4(1,0,0,0))*(float(k[GLFW_KEY_I])-float(k[GLFW_KEY_K]))*3.0f*dt;
    e.avion += glm::vec3(TR({},e.giroAvion)*glm::vec4(0,0,1,0))*(float(k[GLFW_KEY_UP])-float(k[GLFW_KEY_DOWN]))*3.0f*dt;
    e.avion.y=std::clamp(e.avion.y+(float(k[GLFW_KEY_PAGE_UP])-float(k[GLFW_KEY_PAGE_DOWN]))*3*dt,2.5f,14.0f);
    e.base=std::clamp(e.base+(float(k[GLFW_KEY_T])-float(k[GLFW_KEY_G]))*30*dt,-70.0f,70.0f);
    e.brazo1=std::clamp(e.brazo1+(float(k[GLFW_KEY_Y])-float(k[GLFW_KEY_H]))*25*dt,-35.0f,35.0f);
    e.brazo2=std::clamp(e.brazo2+(float(k[GLFW_KEY_U])-float(k[GLFW_KEY_N]))*25*dt,-35.0f,35.0f);
    e.suspension=std::clamp(e.suspension+(float(k[GLFW_KEY_O])-float(k[GLFW_KEY_P]))*12*dt,-8.0f,8.0f);
}
struct Faro { glm::vec3 posicion,direccion; };
Faro AsociarLuz(SpotLight& luz,const glm::mat4& padre,glm::vec3 offset,glm::vec3 direccion) {
    // w=1 aplica traslacion; w=0 transforma solamente la direccion.
    Faro f{glm::vec3(padre*glm::vec4(offset,1)),glm::normalize(glm::vec3(padre*glm::vec4(direccion,0)))};
    luz.SetFlash(f.posicion,f.direccion);
    return f;
}
void Capturar(const std::filesystem::path& file,int w,int h) {
    std::vector<unsigned char> pixels(w*h*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);glReadBuffer(GL_BACK);glFinish();
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::ofstream out(file,std::ios::binary);out<<"P6\n"<<w<<" "<<h<<"\n255\n";
    for(int y=h-1;y>=0;--y)out.write(reinterpret_cast<char*>(pixels.data()+y*w*3),w*3);
}
int main(int argc,char** argv) {
    const bool prueba=argc>1 && std::string(argv[1])=="--pruebas";
    std::filesystem::path evidencia=argc>2?std::filesystem::absolute(argv[2]):std::filesystem::path("evidencias");
    wchar_t ruta[MAX_PATH];GetModuleFileNameW(nullptr,ruta,MAX_PATH);
    std::filesystem::current_path(std::filesystem::path(ruta).parent_path());
    if(mainWindow.Initialise()!=0)return 1;
    glfwSwapInterval(prueba?0:1);
    printf("OpenGL %s | GPU %s\n",glGetString(GL_VERSION),glGetString(GL_RENDERER));
    while(glGetError()!=GL_NO_ERROR){}
    CreateObjects();CrearDado();CrearHolocron();
    Shader shader;shader.CreateFromFiles("shaders/shader_light.vert","shaders/shader_light.frag");
    shader.UseShader();GLint id=0,ok=0;glGetIntegerv(GL_CURRENT_PROGRAM,&id);glGetProgramiv(id,GL_LINK_STATUS,&ok);if(!ok)return 2;
    Texture piso("Textures/piso.tga"),agave("Textures/Agave.tga"),dado("Textures/textura_cubo_starwars.png"),holo("Textures/holocron_textura.png"),faroTex("Textures/faro.jpg");
    if(!piso.LoadTextureA()||!agave.LoadTextureA()||!dado.LoadTextureA()||!holo.LoadTextureA()||!faroTex.LoadTexture())return 3;
    Model avion,rover[12];if(!avion.LoadModel("Models/ripslinger.obj"))return 4;
    for(int i=0;i<12;i++)if(!rover[i].LoadModel(piezas[i].archivo))return 4;
    std::vector<std::string> faces;
    for(const char* f:{"rt","lf","up","dn","bk","ft"}) {
        std::string name=std::string("Textures/Skybox/cupertin-lake_")+f+".tga";
        if(!std::filesystem::exists(name))return 5;faces.push_back(name);
    }
    Skybox sky(faces);
    // Disco del faro con la fotografia proporcionada.
    MeshModel lente;std::vector<GLfloat> lv={0,0,0,.5f,.5f,0,0,1};std::vector<unsigned> li;
    for(unsigned i=0;i<=48;i++){float a=i*6.2831853f/48;lv.insert(lv.end(),{cosf(a),sinf(a),0,(cosf(a)+1)/2,(sinf(a)+1)/2,0,0,1});if(i)li.insert(li.end(),{0,i,i+1});}
    lente.CreateMeshModel(lv.data(),li.data(),unsigned(lv.size()),unsigned(li.size()));
    DirectionalLight sol(1,.98f,.92f,.22f,.58f,-.5f,-1,-.3f);
    SpotLight luces[3];bool encendidas[3]={true,true,true},prev[1024]={};
    Material mate(.2f,16),brillo(.7f,64);
    Camera cam(glm::vec3(22,16,29),glm::vec3(0,1,0),-125,-22,8,.2f);
    Estado e;int vista=0,frame=0;double ultimo=glfwGetTime();
    std::ofstream log;
    if(prueba){std::filesystem::create_directories(evidencia);log.open(evidencia/"pruebas.csv");log<<"caso,luz,px,py,pz,dx,dy,dz\n";}
    const char* casos[]={"01_escena","02_dado_holocron","03_rover","04_rover_avanza","05_rover_retrocede","06_avion_inferior","07_avion_sube","08_avion_baja","09_avion_frontal","10_avion_avanza","11_avion_retrocede","12_rover_articulado","13_rover_gira","14_avion_gira","15_pata_1","16_pata_2","17_pata_3","18_pata_4","19_pata_5","20_pata_6"};
    while(!mainWindow.getShouldClose()) {
        double now=glfwGetTime();float dt=std::min(float(now-ultimo),.05f);ultimo=now;glfwPollEvents();bool* k=mainWindow.getsKeys();
        if(!prueba){
            Mover(e,k,dt);cam.keyControl(k,dt);
            if(glfwGetMouseButton(glfwGetCurrentContext(),GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS)cam.mouseControl(mainWindow.getXChange(),mainWindow.getYChange());
            else {mainWindow.getXChange();mainWindow.getYChange();}
            for(int j=0;j<4;j++)if(k[GLFW_KEY_0+j])vista=j;
            for(int j=0;j<3;j++)if(k[GLFW_KEY_F1+j]&&!prev[GLFW_KEY_F1+j])encendidas[j]=!encendidas[j];
            if(k[GLFW_KEY_R])e=Estado();
            std::copy(k,k+1024,prev);
        }else{
            e=Estado();vista=0;
            bool sim[1024]={};
            if(frame==1)vista=1;
            if(frame>=2&&frame<=4)vista=2;
            if(frame==3)sim[GLFW_KEY_I]=true;
            if(frame==4)sim[GLFW_KEY_K]=true;
            if(frame>=5&&frame<=10)vista=3;
            if(frame==6)sim[GLFW_KEY_PAGE_UP]=true;
            if(frame==7)sim[GLFW_KEY_PAGE_DOWN]=true;
            if(frame==9)sim[GLFW_KEY_UP]=true;
            if(frame==10)sim[GLFW_KEY_DOWN]=true;
            if(frame==11){vista=2;sim[GLFW_KEY_T]=sim[GLFW_KEY_Y]=sim[GLFW_KEY_U]=sim[GLFW_KEY_O]=true;}
            if(frame==12){vista=2;sim[GLFW_KEY_J]=true;}
            if(frame==13){vista=3;sim[GLFW_KEY_Z]=true;}
            if(frame>=14){vista=2;sim[GLFW_KEY_4+frame-14]=true;sim[GLFW_KEY_B]=true;}
            Mover(e,sim,1.0f);
        }
        int w,h;glfwGetFramebufferSize(glfwGetCurrentContext(),&w,&h);if(!w||!h){glfwWaitEvents();continue;}glViewport(0,0,w,h);
        glm::mat4 r=TR(e.rover,e.giroRover),a=TR(e.avion,e.giroAvion);
        if(prueba){encendidas[0]=(frame<5||frame>10);encendidas[1]=(frame<2||(frame>=5&&frame<=7)||frame==13);encendidas[2]=(frame<2||(frame>=8&&frame<=10)||frame==13);}
        for(int j=0;j<3;j++)luces[j]=SpotLight(j==1?.65f:1.0f,j==0?.87f:1.0f,j==0?.65f:1.0f,0,encendidas[j]?2.8f:0,0,0,0,0,-1,0,1,.045f,.012f,j==1?32.0f:24.0f);
        Faro f[3]={AsociarLuz(luces[0],r,{1.32f,1.35f,.2f},{1,-.22f,0}),AsociarLuz(luces[1],a,{0,-.9f,.3f},{0,-1,0}),AsociarLuz(luces[2],a,{0,-.2f,3.15f},{0,-.65f,1})};
        glm::vec3 eye=cam.getCameraPosition(),target(0,2,0);
        glm::mat4 view=cam.calculateViewMatrix();
        if(prueba||vista){
            if(vista==0){eye={22,16,29};target={0,2,0};}
            if(vista==1){eye={-2,7,18};target={-2,1,9};}
            if(vista==2){eye={6,9,16};target={-5,1,0};}
            if(vista==3){eye={18,15,27};target={5,2,5};}
            view=glm::lookAt(eye,target,glm::vec3(0,1,0));
        }
        glm::mat4 proj=glm::perspective(glm::radians(45.0f),float(w)/h,.1f,250.0f);
        glClearColor(.06f,.07f,.09f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);sky.DrawSkybox(view,proj);shader.UseShader();
        glUniformMatrix4fv(shader.GetProjectionLocation(),1,GL_FALSE,glm::value_ptr(proj));glUniformMatrix4fv(shader.GetViewLocation(),1,GL_FALSE,glm::value_ptr(view));
        glUniform3fv(shader.GetEyePositionLocation(),1,glm::value_ptr(eye));glUniform3f(shader.getColorLocation(),1,1,1);
        glUniform1i(glGetUniformLocation(id,"alphaCutout"),0);
        shader.SetDirectionalLight(&sol);shader.SetPointLights(nullptr,0);shader.SetSpotLights(luces,3);
        auto model=[&](const glm::mat4& m){glUniformMatrix4fv(shader.GetModelLocation(),1,GL_FALSE,glm::value_ptr(m));};
        mate.UseMaterial(shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        model(glm::scale(glm::mat4(1),glm::vec3(4,1,4)));piso.UseTexture();meshListModel[2]->RenderMeshModel();
        model(glm::scale(glm::rotate(glm::translate(glm::mat4(1),glm::vec3(-5,1,9)),glm::radians(-90.0f),glm::vec3(0,0,1)),glm::vec3(2)));
        dado.UseTexture();meshListModel[4]->RenderMeshModel();
        model(glm::scale(glm::translate(glm::mat4(1),glm::vec3(0,1.56f,9)),glm::vec3(.6f)));holo.UseTexture();holocronCodigo.RenderMeshModel();
        brillo.UseMaterial(shader.GetSpecularIntensityLocation(),shader.GetShininessLocation());
        model(glm::translate(glm::scale(a,glm::vec3(.065f)),glm::vec3(0,-14,23)));avion.RenderModel();
        // Conversion de Blender Z arriba a OpenGL Y arriba, compartida por todas las piezas.
        glm::mat4 raiz=glm::scale(glm::rotate(r,glm::radians(-90.0f),glm::vec3(1,0,0)),glm::vec3(.36f));
        glm::mat4 mundial[12];
        for(int i=0;i<12;i++){
            int padre=piezas[i].padre;
            glm::vec3 local=piezas[i].pivote-(padre<0?glm::vec3(0):piezas[padre].pivote);
            glm::mat4 t=glm::translate(glm::mat4(1),local);
            if(i==3)t=glm::rotate(t,glm::radians(e.base),glm::vec3(0,0,1));
            if(i==4||i==5)t=glm::rotate(t,glm::radians(i==4?e.brazo1:e.brazo2),glm::vec3(0,1,0));
            if(i==1||i==2)t=glm::rotate(t,glm::radians(e.suspension*(i==1?1:-1)),glm::vec3(0,1,0));
            if(i>=6)t=glm::rotate(t,glm::radians(e.patas[i-6]),glm::vec3(0,1,0));
            mundial[i]=(padre<0?raiz:mundial[padre])*t;model(mundial[i]);rover[i].RenderModel();
        }
        // Faro visible: usa la misma matriz y offset que su spotlight.
        faroTex.UseTexture();
        model(glm::scale(glm::rotate(glm::translate(r,glm::vec3(1.32f,1.35f,.2f)),glm::radians(90.0f),glm::vec3(0,1,0)),glm::vec3(.25f)));lente.RenderMeshModel();
        model(glm::scale(glm::rotate(glm::translate(a,glm::vec3(0,-.2f,3.15f)),glm::radians(0.0f),glm::vec3(0,1,0)),glm::vec3(.22f)));lente.RenderMeshModel();
        model(glm::scale(glm::rotate(glm::translate(a,glm::vec3(0,-.9f,.3f)),glm::radians(90.0f),glm::vec3(1,0,0)),glm::vec3(.22f)));lente.RenderMeshModel();
        model(glm::scale(glm::translate(glm::mat4(1),glm::vec3(-10,1.5f,7)),glm::vec3(3)));
        glUniform1i(glGetUniformLocation(id,"alphaCutout"),1);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);agave.UseTexture();meshListModel[3]->RenderMeshModel();glDisable(GL_BLEND);
        GLenum error=glGetError();if(error!=GL_NO_ERROR){printf("Error OpenGL %u\n",error);return 6;}
        if(prueba){
            Capturar(evidencia/(std::string(casos[frame])+".ppm"),w,h);
            for(int j=0;j<3;j++)log<<casos[frame]<<","<<j<<","<<f[j].posicion.x<<","<<f[j].posicion.y<<","<<f[j].posicion.z<<","<<f[j].direccion.x<<","<<f[j].direccion.y<<","<<f[j].direccion.z<<"\n";
            printf("PRUEBA %s OK\n",casos[frame]);
        }
        glfwSetWindowTitle(glfwGetCurrentContext(),"P07 Iluminacion 1 | 0-3 vistas | I/K rover | Flechas y RePag/AvPag avion | F1-F3 luces | R reiniciar");
        mainWindow.swapBuffers();if(prueba&&++frame==20)break;
    }
    for(auto& m:rover)m.ClearModel();avion.ClearModel();for(auto m:meshListModel)delete m;holocronCodigo.ClearMeshModel();lente.ClearMeshModel();
    printf("CIERRE CORRECTO, sin errores OpenGL.\n");return 0;
}
