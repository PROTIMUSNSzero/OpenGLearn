#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "CustomShader.h"
#include "Camera.h"
#include "Model.h"


using namespace std;
using namespace glm;

const unsigned int SCR_WindowWidth = 1200;
const unsigned int SCR_WindowHeight = 800;

Camera camera(vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WindowWidth / 2.0f;
float lastY = SCR_WindowHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

enum DrawMethodEnum
{
	DrawTriangleOrRetangle = 0,
	DrawWithTextureAndTransform = 1,
	DrawInCoordSystem = 2,
	DrawColor = 3,
	DrawMultiLights = 4,
	DrawModel = 5,
	DrawWithStencilTest = 6,
	DrawWithBlending = 7,
	DrawWithCulling = 8,
};

//#define DRAW_TRIANGLE  //绘制三角形
//#define DRAW_WIREFRAME  //绘制线框
//#define DRAW_IN_ORTHOGRAPHIC

//#define DRAW_LIGHTING
#define DRAW_LIGHT_MAPS

//#define DRAW_MULTILIGHTS
#define DRAW_DIRECTIONAL_LIGHT
//#define DRAW_POINT_LIGHT
//#define DRAW_SPOTLIGHT

#define DRAW_BLENDING

#define DRAW_CULL_FRONT


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	camera.ProcesssMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}

unsigned int loadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else 
	{
		cout << "Texture failed to load at path " << path << endl;
	}

	stbi_image_free(data);
	return textureID;
}

int drawNothin(GLFWwindow* window);
int drawTriangleOrRetangle(GLFWwindow *window);
int drawWithTextureAndTransform(GLFWwindow* window);
int drawInCoordSystem(GLFWwindow* window);
int drawColor(GLFWwindow* window);
int drawMultiLights(GLFWwindow* window);
int drawModel(GLFWwindow* window);
int drawWithStencilTest(GLFWwindow* window);
int drawWithBlending(GLFWwindow* window);
int drawWithCulling(GLFWwindow* window);
int drawFrameBuffer(GLFWwindow* window);

int main()
{
	int drawEnum;
	cin >> drawEnum;
	
	//???????????glfw?????????????????????????
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//???????????
	GLFWwindow* window = glfwCreateWindow(SCR_WindowWidth, SCR_WindowHeight, "LearnOpenGL", 
		NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	//?????????????????????????????
	glfwMakeContextCurrent(window);

	//????????????????????????????????
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}



	switch (drawEnum)
	{
	case (int)DrawTriangleOrRetangle:
		return drawTriangleOrRetangle(window);
	case (int)DrawWithTextureAndTransform:
		return drawWithTextureAndTransform(window);
	case (int)DrawInCoordSystem:
		return drawInCoordSystem(window);
	case (int)DrawColor:
		return drawColor(window);
	case (int)DrawMultiLights:
		return drawMultiLights(window);
	case (int)DrawModel:
		return drawModel(window);
	case (int)DrawWithStencilTest:
		return drawWithStencilTest(window);
	case (int)DrawWithBlending:
		return drawWithBlending(window);
	case (int)DrawWithCulling:
		return drawWithCulling(window);
	default:
		return drawNothin(window);
	}
}



int drawNothin(GLFWwindow* window)
{
	glfwSetWindowShouldClose(window, true);
	glfwTerminate();
	return 0;
}



//绘制三角形或矩形
int drawTriangleOrRetangle(GLFWwindow *window)
{
#ifdef DRAW_TRIANGLE
	CustomShader myShader("../openGLearn/ShaderSource/Triangle.vs",
	        "../openGLearn/ShaderSource/Triangle.fs");
#else
	CustomShader myShader("../openGLearn/ShaderSource/Rectangle.vs",
	        "../openGLearn/ShaderSource/Rectangle.fs");
#endif


	//三角形顶点
	float vertices[] =
	{	//位置				//颜色
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};
	//矩形顶点
	float rectVertices[] =
	{
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};
	unsigned int indices[] =
	{
		0, 1, 3,
		1, 2, 3,
	};


#ifdef DRAW_TRIANGLE  //绘制三角形

	//????shader???????????????????????????????????
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//??????VAO,????VBO??EBO?????????????????????
	//VBO?????????GL_ARRAY_BUFFER?????????????????????????VBO?????EBO???
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//??????????????????????????????????????
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//??????????????OpenGL?????????????????????????VBO??
	// ????????			 
	//??2??????????????????????????????????
	//??????????????????????????????????????0?????????????3 float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ???????
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

#else  //???????

	//????EBO?????????????????????????????
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//?????????????????VBO??
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

	//??EBO????VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//??1????????????????????????????????????????loation???????????????0??
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//?????????????????????????????????
	glEnableVertexAttribArray(0);

#endif

	//?????EBO??????
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//??????
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//????????????????????????????
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		myShader.use();
		//????????VAO
		//??VAO???????EBO??????????VAO??????????????EBO
		//??????EBO????????????????VAO???????EBO??????????????????
		glBindVertexArray(VAO);

#ifndef DRAW_TRIANGLE
		float timeValue = glfwGetTime();
		float greenValue = sin(timeValue) / 2.0f + 0.5f;
		//???Uniform???????????????
		myShader.setFloat4("ourColor", 0.0f, greenValue, 0.0f, 1.0f);
#endif

#ifdef DRAW_WIREFRAME

		//?????
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#endif

#ifdef DRAW_TRIANGLE

		//?????????????????????
		glDrawArrays(GL_TRIANGLES, 0, 3);

#else

		//???????????????????????2?????????????????2??????????
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

#endif

		//?????????
		glfwPollEvents();
		//???????????
		glfwSwapBuffers(window);
	}

	//???????????
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
#ifndef DRAW_TRIANGLE
	glDeleteBuffers(1, &EBO);
#endif

	//myShader.clean();

	//??????????
	glfwTerminate();

	return 0;
}

//绘制纹理,基础变换
int drawWithTextureAndTransform(GLFWwindow* window)
{
	CustomShader myShader("../openGLearn/ShaderSource/Texture.vs",
	        "../openGLearn/ShaderSource/Texture.fs");

	float vertices[] =
	{
		//顶点位置         //纹理坐标（范围为0-1）
		0.5f, 0.5f, 0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
	};
	unsigned int indices[] =
	{
		0, 1, 3,
		1, 2, 3,
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//创建、绑定纹理对象
	unsigned int texture1, texture2;
	//第1参数：生成纹理对象的数量
	glGenTextures(1, &texture1); 
	glBindTexture(GL_TEXTURE_2D, texture1);
	//设置纹理环绕方式（超出范围的纹理图像设置），包括S（缩放）、T（变换）
	//环绕方式：REPEAT：重复图像（默认）；MIRRORED_REPEAT：镜像重复；CLAMP_TO_EDGE：边界拉伸；
	//CLAMP_TO_BORDER：指定超出部分的显示颜色（需添加指定的颜色值参数）
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//设置纹理过滤方式，包括放大和缩小
	//过滤方式：NEAREST：选取邻近点像素（默认）；LINEAR：线性（计算邻近插值，平滑过渡）；
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//加载图片纹理
	int width, height, nrChannels;
	//翻转纹理的y轴，即上下颠倒（需在加载图像前调用）
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("../openGLearn/Res/Texture/container.jpg",
	        &width, &height,&nrChannels, 0);
	if (data)
	{
		//生成纹理（需纹理对象和加载的纹理数据）；
		//第2参数：为纹理指定多级渐远纹理的级别；第3参数：纹理存储格式；第4、5参数：纹理宽高；
		//第7参数：源图格式；第8参数：源图数据类型；第9参数：图像数据；
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//生成多级渐远纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	//生成纹理后释放图像内存
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("../openGLearn/Res/Texture/awesomeface.png", &width, &height,
	        &nrChannels,0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);

	myShader.use();
	//设置着色器里每个采样器的对应纹理单元
	glUniform1i(glGetUniformLocation(myShader.ID, "texture1"), 0); //手动设置
	myShader.setInt("texture2", 1); //使用着色器类设置

	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//激活纹理单元，之后绑定纹理，纹理会自动赋值到着色器的纹理采样器
		//GL_TEXTURE0默认激活，若只有1个纹理单元则无需调用该方法
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//矩阵变换
		mat4 transform = mat4(1.0f);
		//平移
		transform = translate(transform, vec3(0.5f, -0.5f, 0.0f));
		//旋转 第2参数：旋转量；第2参数：旋转轴
		transform = rotate(transform, (float)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f));

		myShader.use();

		//传递uniform变换矩阵 
		//2参数：矩阵数量；3参数：是否转置（glm矩阵默认为列布局，无需转置）；数据用value_ptr方法转变
		unsigned int transformLoc = glGetUniformLocation(myShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(transform));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();

	return 0;
}

//坐标空间下绘制，相机移动
int drawInCoordSystem(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//开启深度测试
	glEnable(GL_DEPTH_TEST);

	CustomShader myShader("../openGLearn/ShaderSource/CoordSystem.vs",
	        "../openGLearn/ShaderSource/CoordSystem.fs");

	//立方体顶点
	float vertices[] =
	{	//位置				  //纹理坐标
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};

	vec3 cubePositions[] =
	{
		vec3(0.0f,  0.0f,  0.0f),
		vec3(2.0f,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3(2.4f, -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3(1.3f, -2.0f, -2.5f),
		vec3(1.5f,  2.0f, -2.5f),
		vec3(1.5f,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f),
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture1;
	
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("../openGLearn/Res/Texture/container.jpg",
	        &width,&height,&nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);

	myShader.use();
	myShader.setInt("texture1", 0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		//开启深度缓冲后每次渲染迭代前需清除深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, texture1);

		myShader.use();

		mat4 view = camera.GetViewMatrix();
		myShader.setMat4("view", view);

		mat4 projection = mat4(1.0f);
#ifdef DRAW_IN_ORTHOGRAPHIC
		//创建正交矩阵，使用正交投影
		//第1-4参数：平截头体的左、右、底、顶坐标；第5、6参数：近、远平面距离；
		projection = ortho(-8.0f, 8.0f, -6.0f, 6.0f, 0.1f, 20.0f);
#else
		//创建透视矩阵，使用透视投影
		//第1参数:FOV角度；第2参数：宽高比；第3、4参数：近、远平面距离；
		projection = perspective(radians(camera.Zoom),
		        (float)SCR_WindowWidth / SCR_WindowHeight,
			0.1f, 100.0f);
#endif
		myShader.setMat4("projection", projection);
		

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			mat4 model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			float angle = 20.0f * (i + 1) * currentFrame;
			model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
			myShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

int drawColor(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

#ifdef DRAW_LIGHTING
	CustomShader cubeShader("../openGLearn/ShaderSource/Lighting.vs",
	        "../openGLearn/ShaderSource/Lighting.fs");
	CustomShader lightShader("../openGLearn/ShaderSource/Color.vs",
	        "../openGLearn/ShaderSource/LightColor.fs");
#else
#ifdef DRAW_LIGHT_MAPS
	CustomShader cubeShader("../openGLearn/ShaderSource/LightMaps.vs",
	        "../openGLearn/ShaderSource/LightMaps.fs");
	CustomShader lightShader("../openGLearn/ShaderSource/Color.vs",
	        "../openGLearn/ShaderSource/LightColor.fs");
#else
	CustomShader cubeShader("../openGLearn/ShaderSource/Color.vs",
	        "../openGLearn/ShaderSource/Color.fs");
	CustomShader lightShader("../openGLearn/ShaderSource/Color.vs",
	        "../openGLearn/ShaderSource/LightColor.fs");
#endif
#endif

	//光照位置
	vec3 lightPos(1.2f, 1.0f, 2.0f);

	//立方体顶点
	float vertices[] =
	{	//位置				  //法线				   //纹理坐标，仅用于光照贴图
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
	glEnableVertexAttribArray(2);

#ifdef DRAW_LIGHT_MAPS
	unsigned int diffuseMap = loadTexture("../openGLearn/Res/Texture/Container2.png");
	unsigned int specularMap = loadTexture("../openGLearn/Res/Texture/Container2_specular.png");

	cubeShader.use();
	cubeShader.setInt("material.diffuse", 0);
	cubeShader.setInt("material.specular", 1);
#endif


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeShader.use();

#if (defined DRAW_LIGHTING) || (defined DRAW_LIGHT_MAPS)
		cubeShader.setFloat3("material.specular", 0.5f, 0.5f, 0.5f);
		cubeShader.setFloat("material.shininess", 64.0f);

		cubeShader.setVec3("light.position", lightPos);
		cubeShader.setVec3("viewPos", camera.Position);
#endif

#ifdef DRAW_LIGHTING
		cubeShader.setFloat3("material.ambient", 0.1f, 0.2f, 0.2f);
		cubeShader.setFloat3("material.diffuse", 0.5f, 0.3f, 0.3f);

		vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);
		vec3 diffuseColor = lightColor * vec3(0.5f);
		vec3 ambientColor = lightColor * vec3(0.1f);

		cubeShader.setVec3("light.ambient", ambientColor);
		cubeShader.setVec3("light.diffuse", diffuseColor);
		cubeShader.setVec3("light.specular", vec3(1.0f));
#else
#ifdef DRAW_LIGHT_MAPS
		cubeShader.setFloat3("light.ambient", 0.2f, 0.2f, 0.2f);
		cubeShader.setFloat3("light.diffuse", 0.5f, 0.5f, 0.5f);
		cubeShader.setVec3("light.specular", vec3(1.0f));
#else
		cubeShader.setFloat3("lightColor", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat3("objectColor", 0.5f, 0.3f, 0.1f);
#endif
#endif

		mat4 view = camera.GetViewMatrix();
		cubeShader.setMat4("view", view);

		mat4 projection = mat4(1.0f);
		projection = perspective(radians(camera.Zoom),
		        (float)SCR_WindowWidth / SCR_WindowHeight,
			0.1f, 100.0f);
		cubeShader.setMat4("projection", projection);

		mat4 model = mat4(1.0f);
		cubeShader.setMat4("model", model);

#ifdef DRAW_LIGHT_MAPS
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
#endif

		glBindVertexArray(VAO);
		//绘制cube物体
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//设置光照物体的变换
		lightShader.use();

		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		model = mat4(1.0f);
		model = translate(model, lightPos);
		model = scale(model, vec3(0.2f));
		lightShader.setMat4("model", model);

		//绘制光照物体
		glDrawArrays(GL_TRIANGLES, 0, 36);
	

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

int drawMultiLights(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

#ifdef DRAW_MULTILIGHTS
	CustomShader cubeShader("../openGLearn/ShaderSource/LightMaps.vs",
	        "../openGLearn/ShaderSource/Multilights.fs");
#else
#ifdef DRAW_DIRECTIONAL_LIGHT
	CustomShader cubeShader("../openGLearn/ShaderSource/LightMaps.vs",
	        "../openGLearn/ShaderSource/DirectionalLight.fs");
#else
#ifdef DRAW_POINT_LIGHT
	CustomShader cubeShader("../openGLearn/ShaderSource/LightMaps.vs",
	        "../openGLearn/ShaderSource/PointLight.fs");
#else
#ifdef DRAW_SPOTLIGHT
	CustomShader cubeShader("../openGLearn/ShaderSource/LightMaps.vs",
	        "../openGLearn/ShaderSource/Spotlight.fs");
#endif
#endif
#endif 
#endif

	//点光源位置
	vec3 lightPos(1.2f, 1.0f, 2.0f);
	//平行光源方向
	vec3 lightDirection(-1.2f, -10.0f, -2.0f);

	//立方体顶点
	float vertices[] =
	{	//位置				  //法线				   //纹理坐标
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	};

	vec3 cubePositions[] = 
	{
		vec3( 0.0f,  0.0f,  0.0f),
		vec3( 2.0f,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3( 2.4f, -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3( 1.3f, -2.0f, -2.5f),
		vec3( 1.5f,  2.0f, -2.5f),
		vec3( 1.5f,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};

	vec3 pointLightPosition[] =
	{
		vec3(0.7f,  0.2f,  2.0f),
		vec3(2.3f, -3.3f, -4.0f),
		vec3(-4.0f,  2.0f, -12.0f),
		vec3(0.0f,  0.0f, -3.0f),
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int diffuseMap = loadTexture("../openGLearn/Res/Texture/Container2.png");
	unsigned int specularMap = loadTexture("../openGLearn/Res/Texture/Container2_specular.png");

	cubeShader.use();
	cubeShader.setInt("material.diffuse", 0);
	cubeShader.setInt("material.specular", 1);


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeShader.use();

		cubeShader.setFloat("material.shininess", 64.0f);

		cubeShader.setVec3("viewPos", camera.Position);

#ifdef DRAW_MULTILIGHTS
		cubeShader.setFloat3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		cubeShader.setFloat3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		cubeShader.setFloat3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		cubeShader.setFloat3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		cubeShader.setVec3("pointLights[0].position", pointLightPosition[0]);
		cubeShader.setFloat3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		cubeShader.setFloat3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShader.setFloat3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat("pointLights[0].constant", 1.0f);
		cubeShader.setFloat("pointLights[0].linear", 0.09f);
		cubeShader.setFloat("pointLights[0].quadratic", 0.032f);

		cubeShader.setVec3("pointLights[1].position", pointLightPosition[1]);
		cubeShader.setFloat3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		cubeShader.setFloat3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShader.setFloat3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat("pointLights[1].constant", 1.0f);
		cubeShader.setFloat("pointLights[1].linear", 0.09f);
		cubeShader.setFloat("pointLights[1].quadratic", 0.032f);

		cubeShader.setVec3("pointLights[2].position", pointLightPosition[2]);
		cubeShader.setFloat3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		cubeShader.setFloat3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShader.setFloat3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat("pointLights[2].constant", 1.0f);
		cubeShader.setFloat("pointLights[2].linear", 0.09f);
		cubeShader.setFloat("pointLights[2].quadratic", 0.032f);

		cubeShader.setVec3("pointLights[3].position", pointLightPosition[3]);
		cubeShader.setFloat3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		cubeShader.setFloat3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShader.setFloat3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat("pointLights[3].constant", 1.0f);
		cubeShader.setFloat("pointLights[3].linear", 0.09f);
		cubeShader.setFloat("pointLights[3].quadratic", 0.032f);

		cubeShader.setVec3("spotlight.position", camera.Position);
		cubeShader.setVec3("spotlight.direction", camera.Front);
		cubeShader.setFloat3("spotlight.ambient", 0.0f, 0.0f, 0.0f);
		cubeShader.setFloat3("spotlight.diffuse", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat3("spotlight.specular", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat("spotlight.constant", 1.0f);
		cubeShader.setFloat("spotlight.linear", 0.09f);
		cubeShader.setFloat("spotlight.quadratic", 0.032f);
		cubeShader.setFloat("spotlight.cutOff", cos(radians(12.5f)));
		cubeShader.setFloat("spotlight.outerCutOff", cos(radians(15.0f)));
#else
		cubeShader.setFloat3("material.specular", 0.5f, 0.5f, 0.5f);

		cubeShader.setFloat3("light.ambient", 0.2f, 0.2f, 0.2f);
		cubeShader.setFloat3("light.diffuse", 0.5f, 0.5f, 0.5f);
		cubeShader.setVec3("light.specular", vec3(1.0f));
#endif


#ifdef DRAW_DIRECTIONAL_LIGHT
		cubeShader.setVec3("light.direction", lightDirection);
#endif
#ifdef DRAW_POINT_LIGHT
		cubeShader.setVec3("light.position", lightPos);
		cubeShader.setFloat("light.constant", 1.0f);
		cubeShader.setFloat("light.linear", 0.09f);
		cubeShader.setFloat("light.quadratic", 0.032f);
#endif
#ifdef DRAW_SPOTLIGHT
		cubeShader.setVec3("light.position", camera.Position);
		cubeShader.setVec3("light.direction", camera.Front);
		cubeShader.setFloat("light.cutOff", cos(radians(12.5f)));
		cubeShader.setFloat("light.outerCutOff", cos(radians(17.5f)));
#endif

		mat4 view = camera.GetViewMatrix();
		cubeShader.setMat4("view", view);

		mat4 projection = mat4(1.0f);
		projection = perspective(radians(camera.Zoom),
		        (float)SCR_WindowWidth / SCR_WindowHeight,
			0.1f, 200.0f);
		cubeShader.setMat4("projection", projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(VAO);

		mat4 model = mat4(1.0f);
		for (int i = 0; i < 10; i++)
		{
			model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			model = rotate(model, radians(20.0f * (i + 1) * currentFrame),
			        vec3(1.0f, 0.3f, 0.5f));
			cubeShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

int drawModel(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);
	//深度测试 通过方法，默认为LESS，即深度小于模板参数时通过测试，否则丢弃
	glDepthFunc(GL_LESS);
	//设置深度掩码以禁用深度缓冲的写入
	//glDepthMask(GL_FALSE);

	CustomShader myShader("../openGLearn/ShaderSource/Model.vs",
	        "../openGLearn/ShaderSource/Model.fs");

	Model myModel("../openGLearn/Res/Model/nanosuit/nanosuit.obj");

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.use();

		mat4 projection = perspective(radians(camera.Zoom),
			(float)SCR_WindowWidth / (float)SCR_WindowHeight, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		myShader.setMat4("projection", projection);
		myShader.setMat4("view", view);
		mat4 model = mat4(1.0f);
		model = translate(model, vec3(0.0f));
		model = scale(model, vec3(1.0f));
		myShader.setMat4("model", model);
		myModel.Draw(myShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

//模板测试
int drawWithStencilTest(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	//测试完成后执行的操作。此处表示通过模板测试后模板值改为1
	//3个参数分别表示模板测试失败、模板测试成功但深度测试失败、模板和深度测试均成功时的操作
	//GL_KEEP：保留模板值；GL_REPLACE：修改模板值为模板测试（StencilFunc）的比较值ref；
	//GL_ZERO：模板值改为0；
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	CustomShader cubeShader("../openGLearn/ShaderSource/StencilTest.vs",
	        "../openGLearn/ShaderSource/StencilTest.fs");
	CustomShader singleColorShader("../openGLearn/ShaderSource/Model.vs",
	        "../openGLearn/ShaderSource/StencilSingleColor.fs");

	float cubeVertices[] =
	{
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	float planeVertices[] =
	{
		 5.0f, -0.5f,  5.0f,  2.0f,  0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f,  0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f,  2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f,  0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f,  2.0f,
		 5.0f, -0.5f,  5.0f,  2.0f,  2.0f,
	};

	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int cubeTexture = loadTexture("../openGLearn/Res/Texture/container.jpg");
	unsigned int floorTexture = loadTexture("../openGLearn/Res/Texture/awesomeface.png");

	cubeShader.use();
	cubeShader.setInt("texture1", 0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		singleColorShader.use();
		mat4 model = mat4(1.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WindowWidth
			/ (float)SCR_WindowHeight, 0.1f, 100.0f);
		singleColorShader.setMat4("view", view);
		singleColorShader.setMat4("projection", projection);

		cubeShader.use();
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);

		//绘制平面，需关闭模板写入
		//Mask参数：0x00表示关闭写入，0xFF表示打开写入
		glStencilMask(0x00);
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		cubeShader.setMat4("model", mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//开启模板测试和写入，将绘制区域的片段模板值全更新为1
		//Func参数：比较操作Op；测试及写入值ref；比较掩码mask
		//比较方法为：(ref & mask) Op (模板缓冲值stencil & mask)
		//mask通常为0xFF，不影响比较
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
	
		glStencilMask(0xFF);

		//绘制方块并更新模板缓冲
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = translate(model, vec3(-1.0f, 0.0f, -1.0f));
		cubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = mat4(1.0f);
		model = translate(model, vec3(2.0f, 0.0f, 0.0f));
		cubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//关闭模板写入，筛选方块放大后的边框片段进行绘制，关闭深度测试，保证边框不会被其他物体遮挡。
		//之前渲染的立方体区域模板值为1，丢弃；立方体放大后的边框位置为默认值0，通过测试，绘制纯色
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		singleColorShader.use();
		float scaleAmt = 1.1f;

		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = mat4(1.0f);
		model = translate(model, vec3(-1.0f, 0.0f, -1.0f));
		model = scale(model, vec3(scaleAmt, scaleAmt, scaleAmt));
		singleColorShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = mat4(1.0f);
		model = translate(model, vec3(2.0f, 0.0f, 0.0f));
		model = scale(model, vec3(scaleAmt, scaleAmt, scaleAmt));
		singleColorShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		//绘制完成后打开深度测试
		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}

//半透明 混合
int drawWithBlending(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);

#ifdef DRAW_BLENDING
	glEnable(GL_BLEND);
	//设置源颜色和目标颜色因子
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CustomShader myShader("../openGLearn/ShaderSource/CoordSystem.vs",
	        "../openGLearn/ShaderSource/CoordSystem.fs");
#else
	CustomShader myShader("../openGLearn/ShaderSource/CoordSystem.vs",
	        "../openGLearn/ShaderSource/Grass.fs");
#endif

	float cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float planeVertices[] =
	{
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	float transparentVertices[] =
	{
							 //纹理坐标y轴翻转（文本上下翻转）
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

#ifdef DRAW_BLENDING
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
#endif

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

#ifdef DRAW_BLENDING
	unsigned int cubeTexture = loadTexture("../openGLearn/Res/Texture/container2.png");
#endif
	unsigned int floorTexture = loadTexture("../openGLearn/Res/Texture/floor.jpg");

#ifdef DRAW_BLENDING
	unsigned int grassTexture = loadTexture("../openGLearn/Res/Texture/window.png");
#else
	unsigned int grassTexture = loadTexture("../openGLearn/Res/Texture/grass.png");
#endif
	//采样材质边缘时，顶部透明部分与相邻的（GL_REPEAT方式下产生的）底部非透明部分插值会产生有色边缘，
	//需将材质设为GL_CLAMP_TO_EDGE以消除边界外部分的非透明图像
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	vector<vec3> transparent
	{
		vec3(-0.6f, 0.0f, -0.1f),
		vec3(0.0f, 0.0f, 0.7f),
		vec3(-0.3f, 0.0f, -2.3f),
		vec3(0.6f, 0.0f, -2.6f),
	};

	myShader.use();
	myShader.setInt("texture1", 0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

#ifdef DRAW_BLENDING
		//半透明物体按距离排序，由远及近渲染
		map<float, vec3> sorted;
		for (unsigned int i = 0; i < transparent.size(); i++)
		{
			float distance = length(camera.Position - transparent[i]);
			sorted[distance] = transparent[i];
		}
#endif

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.use();
		mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WindowWidth
			/ (float)SCR_WindowHeight, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 model = mat4(1.0f);
		myShader.setMat4("projection", projection);
		myShader.setMat4("view", view);
		myShader.setMat4("model", model);

#ifdef DRAW_BLENDING
		//先绘制不透明物体，最后绘制半透明物体
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::translate(model, glm::vec3(-0.4f, 0.0f, -1.0f));
		myShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.6f, 0.0f, 0.0f));
		myShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
#endif

		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(transparentVAO);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
#ifdef DRAW_BLENDING
		for (map<float, vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = mat4(1.0f);
			model = translate(model, it->second);
			myShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
#else
		for (unsigned int i = 0; i < transparent.size(); i++)
		{
			model = mat4(1.0f);
			model = translate(model, transparent[i]);
			myShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
#endif

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}

int drawWithCulling(GLFWwindow* window)
{
	glEnable(GL_CULL_FACE);
#ifdef DRAW_CULL_FRONT
	//选择剔除的面；GL_FRONT：剔除正向面；GL_BACK：剔除背向面
	glCullFace(GL_FRONT);
#else 
	glCullFace(GL_BACK);
#endif
	//CCW：逆时针代表正向面；CW：顺时针代表正向面
	glFrontFace(GL_CCW);


	CustomShader myShader("../openGLearn/ShaderSource/CoordSystem.vs",
	        "../openGLearn/ShaderSource/CoordSystem.fs");

	//逆时针定义的顶点，包括正面和背面
	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left       
	};

	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	unsigned int cubeTexture = loadTexture("../openGLearn/Res/Texture/container2.png");

	myShader.use();
	myShader.setInt("texture1", 0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.use();
		mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WindowWidth
			/ (float)SCR_WindowHeight, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 model = mat4(1.0f);
		myShader.setMat4("projection", projection);
		myShader.setMat4("view", view);

		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = translate(model, glm::vec3(-0.4f, 0.0f, -1.0f));
		model = rotate(model, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		myShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

	return 0;
}

int drawframeBuffer(GLFWwindow *window)
{
    glEnable(GL_DEPTH_TEST);

    CustomShader shader1("../openGLearn/ShaderSource/Framebuffer.vs",
            "../openGLearn/ShaderSource/Framebuffer.fs");
    CustomShader shader2("../openGLearn/ShaderSource/Framebuffer.vs",
            "../openGLearn/ShaderSource/Framebuffer.fs");

    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    float quadVertices[] = {
        // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    unsigned int cubeTexture = loadTexture("../openGLearn/Res/Texture/container.jpg");
    unsigned int floorTexture = loadTexture("../openGLearn/Res/Texture/floor.jpg");

    shader1.use();
    shader1.setInt("screenTexture", 0);

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGB,
       GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WindowWidth, SCR_WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE))
    {
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not compelete!" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader1.use();
        mat4 model = mat4(1.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 projection = perspective(radians(camera.Zoom),
                (float)SCR_WindowWidth / (float)SCR_WindowHeight, 0.1f, 100.0f);
        shader1.setMat4("view", view);
        shader1.setMat4("projection", projection);

        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = translate(model, vec3(-1.0f, 0.0f, -1.0f));
        shader1.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = mat4(1.0f);
        model = translate(model, vec3(2.0f, 0.0f, 0.0f));
        shader1.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader1.setMat4("model", mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader2.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();
    return 0;

}

//绘制天空盒
int drawSkybox(GLFWwindow *window)
{


	return 0;
}
