#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;
	// Flechas: orbitar alrededor del modelo desde la clase Camera.
	if(keys[GLFW_KEY_LEFT]||keys[GLFW_KEY_RIGHT]||keys[GLFW_KEY_UP]||keys[GLFW_KEY_DOWN]){
	 float distance=glm::length(position);
	 if(distance>.1f){
	  float angle=glm::degrees(atan2(position.x,position.z));
	  float elevation=glm::degrees(asin(position.y/distance));
	  angle+=(int(keys[GLFW_KEY_RIGHT])-int(keys[GLFW_KEY_LEFT]))*55*deltaTime;
	  elevation+=(int(keys[GLFW_KEY_UP])-int(keys[GLFW_KEY_DOWN]))*55*deltaTime;
	  orbit(angle,glm::clamp(elevation,-85.f,85.f),distance);
	 }
	}
	if(keys[GLFW_KEY_Q])position.y-=velocity;
	if(keys[GLFW_KEY_E])position.y+=velocity;

	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
		
	}

	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
		
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
		
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
	
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}


Camera::~Camera()
{
}

void Camera::orbit(float degrees,float elevation,float distance){
 float a=glm::radians(degrees),e=glm::radians(elevation);
 position=distance*glm::vec3(sin(a)*cos(e),sin(e),cos(a)*cos(e));
 glm::vec3 direction=glm::normalize(-position);
 yaw=glm::degrees(atan2(direction.z,direction.x));pitch=glm::degrees(asin(direction.y));update();
}
void Camera::zoom(float amount){position+=front*amount;}
