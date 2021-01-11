
#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;
using namespace glm;

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	vec3 Position;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), 
		float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), 
		MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
		float pitch) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
		MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = vec3(posX, posY, posZ);
		WorldUp = vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	mat4 GetViewMatrix()
	{
		//观察矩阵，从世界空间转换至观察空间
		//第1参数：相机在世界空间的位置；第2参数：相机观察（指向）的目标位置；第3参数：世界空间的上向量
		return lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			Position += Front * velocity;
		}
		if (direction == BACKWARD)
		{
			Position -= Front * velocity;
		}
		if (direction == LEFT)
		{
			Position -= Right * velocity;
		}
		if (direction == RIGHT)
		{
			Position += Right * velocity;
		}
		if (direction == UP)
		{
			Position += Up * velocity;
		}
		if (direction == DOWN)
		{
			Position -= Up * velocity;
		}
	}

	void ProcesssMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= MouseSensitivity;
		yOffset *= MouseSensitivity;

		Yaw += xOffset;
		Pitch += yOffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
			{
				Pitch = 89.0f;
			}
			if (Pitch < -89.0f)
			{
				Pitch = -89.0f;
			}
		}

		updateCameraVectors();
	}

	void ProcessMouseScroll(float yOffset)
	{
		Zoom -= (float) yOffset;
		if (Zoom < 1.0f)
		{
			Zoom = 1.0f;
		}
		if (Zoom > 90.0f)
		{
			Zoom = 90.0f;
		}
	}

private:

	//通过偏移角度计算相机的各方向的向量
	void updateCameraVectors()
	{
		vec3 front;
		front.x = cos(radians(Yaw)) * cos(radians(Pitch));
		front.y = sin(radians(Pitch));
		front.z = sin(radians(Yaw)) * cos(radians(Pitch));
		Front = normalize(front);
		Right = normalize(cross(Front, WorldUp));
		Up = normalize(cross(Right, Front));
	}
};

#endif

