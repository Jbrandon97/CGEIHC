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
	//InicializaciÃƒÂ¯Ã‚Â¿Ã‚Â½n de GLFW
	if (!glfwInit())
	{
		printf("FallÃƒÂ¯Ã‚Â¿Ã‚Â½ inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "P07 - Iluminacion 1", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaÃƒÂ¯Ã‚Â¿Ã‚Â½o de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();
	// El cursor queda capturado para poder girar la cÃƒÆ’Ã‚Â¡mara sin detenerse en los bordes.
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("FallÃƒÂ¯Ã‚Â¿Ã‚Â½ inicializaciÃƒÂ¯Ã‚Â¿Ã‚Â½n de GLEW");
		if (mainWindow) glfwDestroyWindow(mainWindow);
	mainWindow = nullptr;
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se estÃƒÂ¯Ã‚Â¿Ã‚Â½ usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

GLfloat Window::getGiroRueda(int indice) const
{
	if (indice < 0 || indice >= 6) return 0.0f;
	return giroRuedas[indice];
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
        glfwSetWindowShouldClose(window, GL_TRUE);

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

	// Al recuperar el foco, Windows puede informar un salto grande del cursor.
	// Se ignora ese ÃƒÆ’Ã‚Âºnico salto para evitar que la cÃƒÆ’Ã‚Â¡mara cambie bruscamente de direcciÃƒÆ’Ã‚Â³n.
	if (theWindow->xChange > 100.0f || theWindow->xChange < -100.0f ||
		theWindow->yChange > 100.0f || theWindow->yChange < -100.0f)
	{
		theWindow->xChange = 0.0f;
		theWindow->yChange = 0.0f;
	}

	theWindow->lastX = static_cast<GLfloat>(xPos);
	theWindow->lastY = static_cast<GLfloat>(yPos);
}


Window::~Window()
{
	if (mainWindow) glfwDestroyWindow(mainWindow);
	mainWindow = nullptr;
	glfwTerminate();

}
