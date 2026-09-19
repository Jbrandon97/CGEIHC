#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = true;
	rotax = rotay = rotaz = 0.0f;
	articulacion1 = articulacion2 = articulacion3 = 0.0f;
	articulacion4 = articulacion5 = articulacion6 = 0.0f;
	for (int i = 0; i < 6; i++) giroRuedas[i] = 0.0f;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	rotax = 0.0f;
	rotay = 0.0f;
	rotaz = 0.0f;
	articulacion1 = 0.0f;
	articulacion2 = 0.0f;
	articulacion3 = 0.0f;
	articulacion4 = 0.0f;
	articulacion5 = 0.0f;
	articulacion6 = 0.0f;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = true;
	for (int i = 0; i < 6; i++) giroRuedas[i] = 0.0f;
	
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicializaci�n de GLFW
	if (!glfwInit())
	{
		printf("Fall� inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "P04 - Rover y sonda espacial", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tama�o de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();
	// El cursor queda capturado para poder girar la cámara sin detenerse en los bordes.
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fall� inicializaci�n de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se est� usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

GLfloat Window::getGiroRueda(int indice) const
{
	if (indice < 0 || indice >= 6) return 0.0f;
	return giroRuedas[indice];
}

void Window::actualizarControles(GLfloat deltaTime)
{
	// Se limita un salto ocasional de tiempo para que los movimientos permanezcan suaves.
	if (deltaTime > 0.05f) deltaTime = 0.05f;

	// Mantener R presionada permite completar vueltas continuas de todo el conjunto.
	if (keys[GLFW_KEY_R])
	{
		rotay += 60.0f * deltaTime;
		while (rotay >= 360.0f) rotay -= 360.0f;
	}

	// Cada tecla numérica hace girar de forma continua solamente su rueda.
	for (int indice = 0; indice < 6; indice++)
	{
		if (keys[GLFW_KEY_1 + indice])
		{
			giroRuedas[indice] += 180.0f * deltaTime;
			while (giroRuedas[indice] >= 360.0f) giroRuedas[indice] -= 360.0f;
		}
	}
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		theWindow->rotax += 10.0;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		theWindow->rotaz += 10.0;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		theWindow->articulacion1 += 10.0;
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		theWindow->articulacion2 += 10.0;
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		theWindow->articulacion3 += 10.0;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		theWindow->articulacion4 += 10.0;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		theWindow->articulacion5 += 10.0;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		theWindow->articulacion6 += 10.0;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		const char* key_name = glfwGetKeyName(GLFW_KEY_D, 0);
		//printf("se presiono la tecla: %s\n",key_name);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = static_cast<GLfloat>(xPos);
		theWindow->lastY = static_cast<GLfloat>(yPos);
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = static_cast<GLfloat>(xPos) - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - static_cast<GLfloat>(yPos);

	theWindow->lastX = static_cast<GLfloat>(xPos);
	theWindow->lastY = static_cast<GLfloat>(yPos);
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
