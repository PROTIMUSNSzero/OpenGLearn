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
	DrawWithFramebuffer = 9,
	DrawWithSkybox = 10,
	DrawWithAdvancedData = 11,
	DrawWithGeometryShader = 12,
	DrawNormal = 13,
	DrawInstance = 14,
	DrawWithAntiAliasing = 15,
};

//#define DRAW_TRIANGLE  //����������
//#define DRAW_WIREFRAME  //�����߿�
//#define DRAW_IN_ORTHOGRAPHIC

//#define DRAW_LIGHTING
#define DRAW_LIGHT_MAPS

//#define DRAW_MULTILIGHTS
#define DRAW_DIRECTIONAL_LIGHT
//#define DRAW_POINT_LIGHT
//#define DRAW_SPOTLIGHT

#define DRAW_BLENDING

#define DRAW_CULL_FRONT

const int DRAW_FRAMEBUFFER_TYPE = 4;

#define DRAW_CUBEMAP_LIGHTING


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

unsigned int loadCubemap(vector<const char*> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    //����Ŀ��˳���� �� �� �� �� ǰ ö��ֵ����
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i], &width, &height, &nrComponents, 0);
        if(data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
                GL_UNSIGNED_BYTE,data);
            stbi_image_free(data);
        }
        else
        {
            cout << "Cubemap texture failed to load at path: " << faces[i] << endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //GL_TEXTURE_WRAP_R��zά�ȣ�
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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
int drawWithFramebuffer(GLFWwindow* window);
int drawSkybox(GLFWwindow* window);
int drawWithAdvancedData(GLFWwindow* window);
int drawWithGeometryShader(GLFWwindow* window);
int drawNormal(GLFWwindow* window);
int drawInstance(GLFWwindow* window);
int drawWithAntiAliasing(GLFWwindow* window);

int main()
{
	int drawEnum;
	cin >> drawEnum;

//	if(drawEnum == (int)DrawWithAntiAliasing)
//    {
//	    //���ö��ز�������Ĳ����㣬�ڴ�������֮ǰ����
//	    glfwWindowHint(GLFW_SAMPLES, 4);
//	    //�������ز�����Ĭ�������ã�
//	    glEnable(GL_MULTISAMPLE);
//    }

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
    case (int)DrawWithFramebuffer:
        return drawWithFramebuffer(window);
    case (int)DrawWithSkybox:
        return drawSkybox(window);
    case (int)DrawWithAdvancedData:
        return drawWithAdvancedData(window);
    case (int)DrawWithGeometryShader:
        return drawWithGeometryShader(window);
    case (int)DrawNormal:
        return drawNormal(window);
    case (int)DrawInstance:
        return drawInstance(window);
    case (int)DrawWithAntiAliasing:
        return drawWithAntiAliasing(window);
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

//���������λ����
int drawTriangleOrRetangle(GLFWwindow *window)
{
#ifdef DRAW_TRIANGLE
	CustomShader myShader("../openGLearn/ShaderSource/Triangle.vs",
	        "../openGLearn/ShaderSource/Triangle.fs");
#else
	CustomShader myShader("../openGLearn/ShaderSource/Rectangle.vs",
	        "../openGLearn/ShaderSource/Rectangle.fs");
#endif


	//�����ζ���
	float vertices[] =
	{	//λ��				//��ɫ
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};
	//���ζ���
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


#ifdef DRAW_TRIANGLE  //����������

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

//��������,�����任
int drawWithTextureAndTransform(GLFWwindow* window)
{
	CustomShader myShader("../openGLearn/ShaderSource/Texture.vs",
	        "../openGLearn/ShaderSource/Texture.fs");

	float vertices[] =
	{
		//����λ��         //�������꣨��ΧΪ0-1��
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

	//���������������
	unsigned int texture1, texture2;
	//��1����������������������
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//���������Ʒ�ʽ��������Χ������ͼ�����ã�������S�����ţ���T���任��
	//���Ʒ�ʽ��REPEAT���ظ�ͼ��Ĭ�ϣ���MIRRORED_REPEAT�������ظ���CLAMP_TO_EDGE���߽����죻
	//CLAMP_TO_BORDER��ָ���������ֵ���ʾ��ɫ�������ָ������ɫֵ������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//����������˷�ʽ�������Ŵ����С
	//���˷�ʽ��NEAREST��ѡȡ�ڽ������أ�Ĭ�ϣ���LINEAR�����ԣ������ڽ���ֵ��ƽ�����ɣ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//����ͼƬ����
	int width, height, nrChannels;
	//��ת�����y�ᣬ�����µߵ������ڼ���ͼ��ǰ���ã�
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("../openGLearn/Res/Texture/container.jpg",
	        &width, &height,&nrChannels, 0);
	if (data)
	{
		//�����������������ͼ��ص��������ݣ���
		//��2������Ϊ����ָ���༶��Զ����ļ��𣻵�3����������洢��ʽ����4��5�����������ߣ�
		//��7������Դͼ��ʽ����8������Դͼ�������ͣ���9������ͼ�����ݣ�
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//���ɶ༶��Զ����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	//����������ͷ�ͼ���ڴ�
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
	//������ɫ����ÿ���������Ķ�Ӧ����Ԫ
	glUniform1i(glGetUniformLocation(myShader.ID, "texture1"), 0); //�ֶ�����
	myShader.setInt("texture2", 1); //ʹ����ɫ��������

	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//��������Ԫ��֮�������������Զ���ֵ����ɫ�������������
		//GL_TEXTURE0Ĭ�ϼ����ֻ��1������Ԫ��������ø÷���
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//����任
		mat4 transform = mat4(1.0f);
		//ƽ��
		transform = translate(transform, vec3(0.5f, -0.5f, 0.0f));
		//��ת ��2��������ת������2��������ת��
		transform = rotate(transform, (float)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f));

		myShader.use();

		//����uniform�任����
		//2����������������3�������Ƿ�ת�ã�glm����Ĭ��Ϊ�в��֣�����ת�ã���������value_ptr����ת��
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

//����ռ��»��ƣ�����ƶ�
int drawInCoordSystem(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

	CustomShader myShader("../openGLearn/ShaderSource/CoordSystem.vs",
	        "../openGLearn/ShaderSource/CoordSystem.fs");

	//�����嶥��
	float vertices[] =
	{	//λ��				  //��������
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
		//������Ȼ����ÿ����Ⱦ����ǰ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, texture1);

		myShader.use();

		mat4 view = camera.GetViewMatrix();
		myShader.setMat4("view", view);

		mat4 projection = mat4(1.0f);
#ifdef DRAW_IN_ORTHOGRAPHIC
		//������������ʹ������ͶӰ
		//��1-4������ƽ��ͷ������ҡ��ס������ꣻ��5��6����������Զƽ����룻
		projection = ortho(-8.0f, 8.0f, -6.0f, 6.0f, 0.1f, 20.0f);
#else
		//����͸�Ӿ���ʹ��͸��ͶӰ
		//��1����:FOV�Ƕȣ���2��������߱ȣ���3��4����������Զƽ����룻
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

	//����λ��
	vec3 lightPos(1.2f, 1.0f, 2.0f);

	//�����嶥��
	float vertices[] =
	{	//λ��				  //����				   //�������꣬�����ڹ�����ͼ
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
		//����cube����
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//���ù�������ı任
		lightShader.use();

		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		model = mat4(1.0f);
		model = translate(model, lightPos);
		model = scale(model, vec3(0.2f));
		lightShader.setMat4("model", model);

		//���ƹ�������
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

	//���Դλ��
	vec3 lightPos(1.2f, 1.0f, 2.0f);
	//ƽ�й�Դ����
	vec3 lightDirection(-1.2f, -10.0f, -2.0f);

	//�����嶥��
	float vertices[] =
	{	//λ��				  //����				   //��������
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

	stbi_set_flip_vertically_on_load(false);

	glEnable(GL_DEPTH_TEST);
	//��Ȳ��� ͨ��������Ĭ��ΪLESS�������С��ģ�����ʱͨ�����ԣ�������
	glDepthFunc(GL_LESS);
	//������������Խ�����Ȼ����д��
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

//ģ�����
int drawWithStencilTest(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	//������ɺ�ִ�еĲ������˴���ʾͨ��ģ����Ժ�ģ��ֵ��Ϊ1
	//3�������ֱ��ʾģ�����ʧ�ܡ�ģ����Գɹ�����Ȳ���ʧ�ܡ�ģ�����Ȳ��Ծ��ɹ�ʱ�Ĳ���
	//GL_KEEP������ģ��ֵ��GL_REPLACE���޸�ģ��ֵΪģ����ԣ�StencilFunc���ıȽ�ֵref��
	//GL_ZERO��ģ��ֵ��Ϊ0��
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

		//����ƽ�棬��ر�ģ��д��
		//Mask������0x00��ʾ�ر�д�룬0xFF��ʾ��д��
		glStencilMask(0x00);
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		cubeShader.setMat4("model", mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//����ģ����Ժ�д�룬�����������Ƭ��ģ��ֵȫ����Ϊ1
		//Func�������Ƚϲ���Op�����Լ�д��ֵref���Ƚ�����mask
		//�ȽϷ���Ϊ��(ref & mask) Op (ģ�建��ֵstencil & mask)
		//maskͨ��Ϊ0xFF����Ӱ��Ƚ�
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		glStencilMask(0xFF);

		//���Ʒ��鲢����ģ�建��
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

		//�ر�ģ��д�룬ɸѡ����Ŵ��ı߿�Ƭ�ν��л��ƣ��ر���Ȳ��ԣ���֤�߿򲻻ᱻ���������ڵ���
		//֮ǰ��Ⱦ������������ģ��ֵΪ1��������������Ŵ��ı߿�λ��ΪĬ��ֵ0��ͨ�����ԣ����ƴ�ɫ
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
		//������ɺ����Ȳ���
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

//��͸�� ���
int drawWithBlending(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);

#ifdef DRAW_BLENDING
	glEnable(GL_BLEND);
	//����Դ��ɫ��Ŀ����ɫ����
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
							 //��������y�ᷭת���ı����·�ת��
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
	//�������ʱ�Եʱ������͸�����������ڵģ�GL_REPEAT��ʽ�²����ģ��ײ���͸�����ֲ�ֵ�������ɫ��Ե��
	//�轫������ΪGL_CLAMP_TO_EDGE�������߽��ⲿ�ֵķ�͸��ͼ��
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
		//��͸�����尴����������Զ������Ⱦ
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
		//�Ȼ��Ʋ�͸�����壬�����ư�͸������
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

//���޳�
int drawWithCulling(GLFWwindow* window)
{
	glEnable(GL_CULL_FACE);
#ifdef DRAW_CULL_FRONT
	//ѡ���޳����棻GL_FRONT���޳������棻GL_BACK���޳�������
	glCullFace(GL_FRONT);
#else
	glCullFace(GL_BACK);
#endif
	//CCW����ʱ����������棻CW��˳ʱ�����������
	glFrontFace(GL_CCW);


	CustomShader myShader("../openGLearn/ShaderSource/CoordSystem.vs",
	        "../openGLearn/ShaderSource/CoordSystem.fs");

	//��ʱ�붨��Ķ��㣬��������ͱ���
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
}

//֡����
int drawWithFramebuffer(GLFWwindow *window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    CustomShader shader1("../openGLearn/ShaderSource/StencilTest.vs",
            "../openGLearn/ShaderSource/StencilTest.fs");
    CustomShader shader2("../openGLearn/ShaderSource/Framebuffer.vs",
            "../openGLearn/ShaderSource/Framebuffer.fs");

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
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    float quadVertices[] =
    {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
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
    shader1.setInt("texture1", 0);

    shader2.use();
    shader2.setInt("screenTexture", 0);
    shader2.setInt("drawType", DRAW_FRAMEBUFFER_TYPE);

    unsigned int framebuffer;
    //��������֡�������
    glGenFramebuffers(1, &framebuffer);
    //��֮�����ж�ȡ��/��д��֡����Ĳ�������Ӱ�쵽��ǰ�󶨵�֡����
    //��1��������Ŀ�ꡣGL_READ_FRAMEBUFFER�����ڶ�ȡ������GL_DRAW_FRAMEBUFFER��������Ⱦ�������д�������
    //GL_FRAMEBUFFER�����ڶ���д����
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //���������������Ϊ���������֡������
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGB,
       GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //��ֹ������Ļͼ���Եʱ��������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //֡������������������Ϊ���������������Ϊ���/ģ��/��ɫ����������֡����Ŀ���У�
    //��2����:�������͡�GL_COLOR_ATTACHMENT0����ɫ������GL_DEPTH_ATTACHMENT0����Ȼ��帽����
    //GL_STENCIL_ATTACHMENT��ģ�建�帽����GL_DEPTH_STENCIL_ATTACHMENT����ȼ�ģ�建�帽����
    //�ɽ���Ⱥ�ģ�建����ӵ�1�������������У������32λ����24λ�����Ϣ��8λģ����Ϣ
    //��3��������������;��5�������༶��Զ������
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    //������Ⱦ���������Ϊ������ӵ�֡�����У��˴�������Ȳ��Ժ�ģ����ԣ�
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    //����Ⱦ�������ʹ֮�����Ⱦ��������ܹ�Ӱ�쵱ǰ����Ⱦ�������
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //Ϊ��Ⱦ��������ͼ������ڴ�ȣ�����ֵ��
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WindowWidth, SCR_WindowHeight);
    //������Ⱦ������󵽰󶨵�֡����Ŀ����
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    //��鵱ǰ�󶨵�֡��������Ƿ�����
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    }
    //��Ĭ��֡�������֡����Ŀ���У���ΪGL_FRAMEBUFFER��
    //������Ĭ��֡���壬�������Ⱦ��������֡��������У����ڴ�����ʾ��������Ⱦ��
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //��Ⱦ��ָ��֡����
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader1.use();
        mat4 model = mat4(1.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 projection = perspective(radians(camera.Zoom),
            (float) SCR_WindowWidth / (float) SCR_WindowHeight, 0.1f, 100.0f);
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

        //��Ⱦ��Ĭ��֡����
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //�ر���Ȳ��ԣ���ֹ��Ƭ���޳�
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader2.use();
        glBindVertexArray(quadVAO);
        //��֮ǰ��Ⱦ�����������Ϊ��ǰ֡�������ʾ����
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

//������պ�(�������������ӻ�)
int drawSkybox(GLFWwindow *window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    CustomShader shader1("../openGLearn/ShaderSource/Cubemap.vs",
            "../openGLearn/ShaderSource/Cubemap.fs");
    CustomShader shader2("../openGLearn/ShaderSource/Skybox.vs",
            "../openGLearn/ShaderSource/Skybox.fs");

    CustomShader normalShader("../openGLearn/ShaderSource/Normal.vs",
                              "../openGLearn/ShaderSource/Normal.fs",
                              "../openGLearn/ShaderSource/Normal.gs");

    float cubeVertices[] = {
         //position           //texCoords   //normal
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    };
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    unsigned int normalVAO, normalVBO;
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);
    glBindVertexArray(normalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    vector<const char*> faces
    {
        "../openGLearn/Res/Texture/skybox_right.jpg",
        "../openGLearn/Res/Texture/skybox_left.jpg",
        "../openGLearn/Res/Texture/skybox_top.jpg",
        "../openGLearn/Res/Texture/skybox_bottom.jpg",
        "../openGLearn/Res/Texture/skybox_front.jpg",
        "../openGLearn/Res/Texture/skybox_back.jpg",
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    unsigned int cubeTexture = loadTexture("../openGLearn/Res/Texture/container.jpg");

    shader1.use();
    shader1.setInt("skybox", 0);
    shader1.setInt("texture1", 1);
    shader2.use();
    shader2.setInt("skybox", 0);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader1.use();
        mat4 model = mat4(1.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 projection = perspective(radians(camera.Zoom),
                (float)SCR_WindowWidth / SCR_WindowHeight,0.1f, 100.0f);
        shader1.setMat4("model", model);
        shader1.setMat4("view", view);
        shader1.setMat4("projection", projection);
        shader1.setVec3("cameraPos", camera.Position);
        shader1.setInt("windowWidth", SCR_WindowWidth);
#ifdef DRAW_CUBEMAP_LIGHTING
        shader1.setBool("drawLighting", true);
#else
        shader1.setBool("drawLighting", false);
#endif

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glBindVertexArray(normalVAO);
        normalShader.use();
        normalShader.setMat4("view", view);
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        //���ģ������(glClear)��ֵΪ1��Ϊ��֤���ֵͬΪ1��xyww������պ�������Ⱦ����������Ȳ���ΪLEQUAL
        glDepthFunc(GL_LEQUAL);
        shader2.use();
        //ֻȡ3x3���֣��Ƴ�λ��Ч����ֻ������תЧ��
        view = mat4(mat3(camera.GetViewMatrix()));
        shader2.setMat4("view", view);
        shader2.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
	return 0;
}

//�߼�GLSL����
int drawWithAdvancedData(GLFWwindow* window)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CustomShader shader1("../openGLearn/ShaderSource/AdvancedData.vs",
        "../openGLearn/ShaderSource/AdvancedData.fs");
    CustomShader shader2("../openGLearn/ShaderSource/AdvancedData.vs",
        "../openGLearn/ShaderSource/AdvancedData.fs");
    CustomShader shader3("../openGLearn/ShaderSource/AdvancedData.vs",
         "../openGLearn/ShaderSource/AdvancedData.fs");
    CustomShader shader4("../openGLearn/ShaderSource/AdvancedData.vs",
         "../openGLearn/ShaderSource/AdvancedData.fs");

    float cubeVertices[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //����shader������uniform�����������֮���glUniformBlockBinding��ͬʹ��
    unsigned int uniformBlockInd1 = glGetUniformBlockIndex(shader1.ID, "Matrices");
    unsigned int uniformBlockInd2 = glGetUniformBlockIndex(shader2.ID, "Matrices");
    unsigned int uniformBlockInd3 = glGetUniformBlockIndex(shader3.ID, "Matrices");
    unsigned int uniformBlockInd4 = glGetUniformBlockIndex(shader4.ID, "Matrices");
    //��shader�����е�uniform��󶨵�uniform�����ָ���󶨵��У�֮���ٽ��������������ͬһ�󶨵��
    //shader��uniform������uniform����������һ��
    //��3����Ϊ�󶨵�
    glUniformBlockBinding(shader1.ID, uniformBlockInd1, 0);
    glUniformBlockBinding(shader2.ID, uniformBlockInd2, 0);
    glUniformBlockBinding(shader3.ID, uniformBlockInd3, 0);
    glUniformBlockBinding(shader4.ID, uniformBlockInd4, 0);

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    //���ٻ����ڴ�ռ䣬����ֵ��NULL��
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //��uniform����������ָ���İ󶨵��У�����ͬglBindBufferBase��
    //�ɽ����������������ͬ�󶨵��У���Ӧshader�в�ͬ��uniform�顣
    //��2����Ϊ�󶨵㣻��3����Ϊ������󣻵�4����Ϊ���������ʼλ��ƫ��������5����Ϊ���ݳ��ȣ�
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(mat4));

    mat4 projection = perspective(45.0f, (float)SCR_WindowWidth / (float)SCR_WindowHeight,
        0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    //���ò��ֻ������ݡ���2����Ϊ����ƫ��������3����Ϊ��׼��������
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(projection));

    //��uniform�������ݶ�Ӧ��uniform��İ󶨵���shader�ڲ����壨��ʹ��glGetUniformBlockBinding��
    unsigned int uboAlpha;
    glGenBuffers(1, &uboAlpha);
    glBindBuffer(GL_UNIFORM_BUFFER, uboAlpha);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboAlpha, 0, sizeof(float));
    float alpha = 0.5f;
    glBindBuffer(GL_UNIFORM_BUFFER, uboAlpha);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &alpha);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    while(!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindVertexArray(cubeVAO);
        mat4 model = mat4(1.0f);
        vec4 color;

        shader1.use();
        model = mat4(1.0f);
        model = translate(model, vec3(-0.75f, 0.75f, 0.0f));
        shader1.setMat4("model", model);
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        shader1.setVec4("color", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader2.use();
        model = mat4(1.0f);
        model = translate(model, vec3(0.75f, 0.75f, 0.0f));
        shader2.setMat4("model", model);
        color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        shader1.setVec4("color", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader3.use();
        model = mat4(1.0f);
        model = translate(model, vec3(-0.75f, -0.75f, 0.0f));
        shader3.setMat4("model", model);
        color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
        shader1.setVec4("color", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader4.use();
        model = mat4(1.0f);
        model = translate(model, vec3(0.75f, -0.75f, 0.0f));
        shader4.setMat4("model", model);
        color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
        shader1.setVec4("color", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glfwTerminate();
    return 0;
}

//����shader
int drawWithGeometryShader(GLFWwindow* window) {
    glEnable(GL_DEPTH_TEST);

    CustomShader shader1("../openGLearn/ShaderSource/Geometry.vs",
                         "../openGLearn/ShaderSource/Geometry.fs",
                         "../openGLearn/ShaderSource/Geometry.gs");

    float points[] =
    {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // bottom-left
    };

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader1.use();
        glBindVertexArray(VAO);
        //��ȾͼԪ����Ϊ������
        glDrawArrays(GL_POINTS, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

//���������ӻ�
int drawNormal(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    CustomShader normalShader("../openGLearn/ShaderSource/Normal.vs",
            "../openGLearn/ShaderSource/Normal.fs",
            "../openGLearn/ShaderSource/Normal.gs");
    CustomShader shader1("../openGLearn/ShaderSource/Model.vs",
            "../openGLearn/ShaderSource/Model.fs");

    Model nanoSuit("../openGLearn/Res/Model/nanosuit/nanosuit.obj");

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 projection = perspective(radians(camera.Zoom),
                (float)SCR_WindowWidth / float(SCR_WindowHeight),1.0f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 model = mat4(1.0f);
        shader1.use();
        shader1.setMat4("projection", projection);
        shader1.setMat4("view", view);
        shader1.setMat4("model", model);
        nanoSuit.Draw(shader1);

        //���Ʒ�����
        normalShader.use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);
        normalShader.setMat4("model", model);
        nanoSuit.Draw(normalShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//ʵ��������
int drawInstance(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    CustomShader asteroidShader("../openGLearn/ShaderSource/Instance.vs",
        "../openGLearn/ShaderSource/Model.fs");
    CustomShader planetShader("../openGLearn/ShaderSource/Model.vs",
        "../openGLearn/ShaderSource/Model.fs");

    Model asteroid("../openGLearn/Res/Model/rock/rock.obj");
    Model planet("../openGLearn/Res/Model/planet/planet.obj");

    unsigned int amount = 100000;

    //ʵ��������洢����ʵ����ģ�͵�����任����
    mat4* modelMatrices;
    modelMatrices = new mat4[amount];
    srand(glfwGetTime());
    float radius = 40.0f;
    float offset = 5.0f;
    for(unsigned int i = 0; i < amount; i++)
    {
        mat4 model = mat4(1.0f);
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.02f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = translate(model, vec3(x, y, z));

        float scaleAmt = (rand() % 20) / 500.0f + 0.05f;
        model = scale(model, vec3(scaleAmt));

        float rotAngle = (rand() % 360);
        model = rotate(model, rotAngle, vec3(0.4f, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for(unsigned int i = 0; i < asteroid.meshes.size(); i++)
    {
        unsigned int VAO = asteroid.meshes[i].VAO;
        glBindVertexArray(VAO);
        //�洢mat4ʵ�����ݣ���Ϊ4��vec4���ݴ洢 ��layout (location = 3) in mat4 ...��
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(2 * sizeof(vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(3 * sizeof(vec4)));

        //ָ������ʵ��������������1�����ݵ�Ƶ�ʣ�0��ÿ������shader���������£�1����Ⱦÿ��ʵ��ʱ���¶������ԣ�
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 projection = perspective(radians(camera.Zoom),
            (float)SCR_WindowWidth / (float)SCR_WindowHeight, 0.1f, 1000.0f);
        mat4 view = camera.GetViewMatrix();
        view = translate(view, vec3(0.0f, -2.0f, -50.0f));
        asteroidShader.use();
        asteroidShader.setMat4("projection", projection);
        asteroidShader.setMat4("view", view);
        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);

        mat4 model = mat4(1.0f);
        model = translate(model, vec3(0.0f, -6.0f, -0.0f));
        model = scale(model, vec3(8.0f));
        planetShader.setMat4("model", model);
        planet.Draw(planetShader);

        asteroidShader.use();
        asteroidShader.setInt("texture_diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, asteroid.textures_loaded[0].id);
        for(unsigned int i = 0; i < asteroid.meshes.size(); i++)
        {
            glBindVertexArray(asteroid.meshes[i].VAO);
            //ʵ������Ⱦ
            glDrawElementsInstanced(GL_TRIANGLES, asteroid.meshes[i].indices.size(), GL_UNSIGNED_INT,
                0, amount);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//���ز��������
int drawWithAntiAliasing(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    CustomShader shader1("../openGLearn/ShaderSource/Color.vs",
            "../openGLearn/ShaderSource/LightColor.fs");
    CustomShader screenShader("../openGLearn/ShaderSource/Framebuffer.vs",
            "../openGLearn/ShaderSource/Framebuffer.fs");

    float vertices[] =
    {	//λ��
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };
    float quadVertices[] = {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int textureColorBufferMultiSampled;
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WindowWidth, SCR_WindowHeight,
        GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
        textureColorBufferMultiSampled, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,
        SCR_WindowWidth, SCR_WindowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"  << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGB,
        GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);
    screenShader.setInt("drawType", 0);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        shader1.use();
        mat4 projection = perspective(radians(camera.Zoom),
            (float)SCR_WindowHeight / (float)SCR_WindowHeight, 0.1f, 1000.0f);
        shader1.setMat4("projection", projection);
        shader1.setMat4("view", camera.GetViewMatrix());
        shader1.setMat4("model", mat4(1.0f));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, SCR_WindowWidth, SCR_WindowHeight, 0, 0, SCR_WindowWidth,
            SCR_WindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, screenTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

