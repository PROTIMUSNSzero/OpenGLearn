#include <stdio.h>
#include <iostream>
#include <random>
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
	DrawWithGammaCorrection = 16,
	DrawShadowMapping = 17,
    DrawOmnidirectionalShadow = 18,
    DrawWithNormalMapping = 19,
    DrawWithParallaxMapping = 20,
    DrawWithHDR = 21,
    DrawBloom = 22,
    DrawWithDeferredRendering = 23,
    DrawSSAO = 24,
};

vector<string> DrawMethodStr
{
    "DrawTriangleOrRetangle     ",
    "DrawWithTextureAndTransform",
    "DrawInCoordSystem          ",
    "DrawColor                  ",
    "DrawMultiLights            ",
    "DrawModel                  ",
    "DrawWithStencilTest        ",
    "DrawWithBlending           ",
    "DrawWithCulling            ",
    "DrawWithFramebuffer        ",
    "DrawWithSkybox             ",
    "DrawWithAdvancedData       ",
    "DrawWithGeometryShader     ",
    "DrawNormal                 ",
    "DrawInstance               ",
    "DrawWithAntiAliasing       ",
    "DrawWithGammaCorrection    ",
    "DrawShadowMapping          ",
    "DrawOmnidirectionalShadow  ",
    "DrawWithNormalMapping      ",
    "DrawWithParallaxMapping    ",
    "DrawWithHDR                ",
    "DrawBloom                  ",
    "DrawWithDeferredRendering  ",
    "DrawSSAO                   ",
};

#define DRAW_TRIANGLE  //����������
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


//���ڵ�1����ʾ�Լ�ÿ�θı䴰�ڴ�Сʱ�������
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //���ã��ӿڣ���Ⱦ���ڵĳߴ磻���������½�λ�ã����ڿ�ߣ�ÿ�θı䴰�ڴ�Сʱ��Ӧ�����ӿڣ�
    //���ڵ�1����ʾʱҲ�����
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

void error_callback(int code, const char *description)
{
    cout << "Error: " << code << " " << description << endl;
}

unsigned int loadTexture(const char* path, bool gammaCorrection = false)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum dataFormat, internalFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
		    internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
		    internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat,
            GL_UNSIGNED_BYTE, data);
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

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
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
int drawWithGammaCorrection(GLFWwindow* window);
int drawShadowMapping(GLFWwindow* window);
int drawOmnidirectionalShadow(GLFWwindow* window);
int drawWithNormalMapping(GLFWwindow* window);
int drawWithParallaxMapping(GLFWwindow* window);
int drawWithHDR(GLFWwindow* window);
int drawBloom(GLFWwindow* window);
int drawWithDeferredRendering(GLFWwindow* window);
int drawSSAO(GLFWwindow* window);

void renderScene(CustomShader &shader, int planeVAO, unsigned int &cubeVAO, unsigned int &cubeVBO);
void renderCube(unsigned int &cubeVAO, unsigned int &cubeVBO);
void renderQuad(unsigned int &quadVAO, unsigned int &quadVBO);
void renderPointLightScene(CustomShader &shader, GLuint &cubeVAO, GLuint &cubeVBO);
void renderPointLightCube(GLuint &cubeVAO, GLuint &cubeVBO);
void renderHDRCube(unsigned int &cubeVAO, unsigned int &cubeVBO);
void renderHDRQuad(unsigned int &quadVAO, unsigned int &quadVBO);
void renderBloomCube(unsigned int &cubeVAO, unsigned int &cubeVBO);
void renderBloomQuad(unsigned int &quadVAO, unsigned int &quadVBO);
void renderDeferredCube(unsigned int &cubeVAO, unsigned int &cubeVBO);
void renderDeferredQuad(unsigned int &quadVAO, unsigned int &quadVBO);
void renderSSAOCube(unsigned int &cubeVAO, unsigned int &cubeVBO);
void renderSSAOQuad(unsigned int &quadVAO, unsigned int &quadVBO);

int main()
{
	for(int i = 0; i < DrawMethodStr.size(); i++)
    {
        if (i % 2 == 0)
        {
            cout << endl;
        }
	    cout << i << " :  " << DrawMethodStr[i] << "   ";
    }
    
    int drawEnum;
    cin >> drawEnum;

//	if(drawEnum == (int)DrawWithAntiAliasing)
//    {
//	    //���ö��ز�������Ĳ����㣬�ڴ�������֮ǰ����
//	    glfwWindowHint(GLFW_SAMPLES, 4);
//	    //�������ز�����Ĭ�������ã�
//	    glEnable(GL_MULTISAMPLE);
//    }
    
    //��ʼ��������GLFW
    //��ʼ��glfw�⣬ʹ��glfw����ǰ�����ʼ��
    glfwInit();
    //����glfw����1������ѡ���2������ѡ��ֵ
    //openGL�汾��3.3���豣֤�汾��3.3�����
    //�������汾��Ϊ3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //���ôΰ汾��Ϊ3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //ʹ�ú���ģʽ
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //�������
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
    //�������ڶ���
    GLFWwindow *window = glfwCreateWindow(SCR_WindowWidth, SCR_WindowHeight, "LearnOpenGL",
       NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        //���ٴ��ڣ�����������Դ����������Ϊδ��ʼ��״̬�������³�ʼ����glfwInit�����ܵ���glfw����
        glfwTerminate();
        return -1;
    }
    //��������������Ϊ��ǰ�̵߳���������
    glfwMakeContextCurrent(window);
    
    //���ڳߴ�ı�Ļص�
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //��ʼ��glad������opengl����֮ǰ�����ʼ��������Ϊ����ϵͳ��ص�opengl����ָ���ַ�ĺ���
    //glfwGetProcAddress���ݱ����ϵͳ��������ȷ�ĺ���
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    
    //����ص�
    glfwSetErrorCallback(error_callback);
    
    //glfw ������ȡ����
    const char* description;
    int code = glfwGetError(&description);
    if(description)
    {
        cout << "Error: " << code << " " << description << endl;
    }
    
    //glad ������ȡ����
    cout << glGetError() << endl;
    
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
    case (int)DrawWithGammaCorrection:
        return drawWithGammaCorrection(window);
    case (int)DrawShadowMapping:
        return drawShadowMapping(window);
    case (int)DrawOmnidirectionalShadow:
        return drawOmnidirectionalShadow(window);
    case (int) DrawWithNormalMapping:
        drawWithNormalMapping(window);
    case (int)DrawWithParallaxMapping:
        drawWithParallaxMapping(window);
    case (int)DrawWithHDR:
        drawWithHDR(window);
    case (int)DrawBloom:
        drawBloom(window);
    case (int)DrawWithDeferredRendering:
        drawWithDeferredRendering(window);
    case (int)DrawSSAO:
        drawSSAO(window);
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
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
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
	
	unsigned int VBO, VAO;
	//����ָ�������Ķ���������󲢷��ض�Ӧ������id����2������
	glGenVertexArrays(1, &VAO);
	//����ָ�������Ķ��㻺����󲢷��ض�Ӧ������id����2������
	glGenBuffers(1, &VBO);
	//��vao������vbo��ebo������glVertexAttribPointer��glEnableVertexAttribArray
	//���ö��ᴢ���ڵ�ǰ�󶨵�vao��
	glBindVertexArray(VAO);
	//��vbo/ebo��ָ���Ļ�����Ŀ�꣨��1�������У�ÿ����������ֻ�ܰ�1���������
	//��������Ŀ�꣺GL_ARRAY_BUFFER����vbo��GL_ELEMENT_ARRAY_BUFFER����ebo��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//�����ڴ沢Ϊ��ǰ�󶨵�vbo/ebo��������
	//��4����ָ���Կ���ι��������Ա�֤����д��
	//GL_STATIC_DRAW�����ݲ���ı䣻 GL_DYNAMIC_DRAW�����ݻ�ı�ܶࣻ
	// GL_STREAM_DRAW������ÿ�λ���ʱ����ı�
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//������������
	//��1��������������λ��ֵ����Ӧshader��layout(location = 0)������2�������������Գߴ磻
	//��3�������������ͣ���4�����������Ƿ񱻱�׼������5�����������������Ķ�������֮��ļ������
	//��6������������ʼλ�õ�ƫ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//���ö������ԣ�����Ϊ��������λ��ֵ��layout(location = 0)��
	//��ʹ��glDisableVertexAttribArray�رն������ԣ�Ĭ�Ϲرգ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

#else  //���ƾ���
	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

#endif
	
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//glfw�Ƿ�Ҫ���˳�
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//���������ɫ�����Ҫ���õ���ɫ���ڵ���glClear��glClearBuffer����ɫ���������Ϊָ������ɫ
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//�����ǰ֡��ָ������
		//GL_COLOR_BUFFER����ɫ���壨������ɫ��glClearColor�����ã��� GL_DEPTH_BUFFER����Ȼ��壻
		//GL_STENCIL_BUFFER��ģ�建�壻
		glClear(GL_COLOR_BUFFER_BIT);

		myShader.use();
		glBindVertexArray(VAO);

#ifndef DRAW_TRIANGLE
		float timeValue = glfwGetTime();
		float greenValue = sin(timeValue) / 2.0f + 0.5f;
		//����shader����ֵ
		myShader.setFloat4("ourColor", 0.0f, greenValue, 0.0f, 1.0f);
#endif

#ifdef DRAW_WIREFRAME

		//?????
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#endif

#ifdef DRAW_TRIANGLE

		//���ƶ��㣬����֮ǰ�輤���Ӧ��ɫ�������ö������ݣ���VBO����ͨ��VAO��Ӱ󶨣���
		//��1������ָ����ȾͼԪ������  GL_POINTS, GL_LINE_STRIP, GL_TRIANGLES ...
		//��2������ָ�������������������3������ָ����Ⱦ�����������
		glDrawArrays(GL_TRIANGLES, 0, 3);

#else

		//???????????????????????2?????????????????2??????????
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

#endif

		//����¼����������û����룩�����´���״̬�������ö�Ӧ�ص�
		glfwPollEvents();
        //�������ڵ�ǰ����ɫ���壬�������Ʋ���ʾ��˫���壬�󻺳������ɺ��л�Ϊǰ���岢��ʾ����Ļ�ϣ�
		glfwSwapBuffers(window);
	}

	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
#ifndef DRAW_TRIANGLE
	glDeleteBuffers(1, &EBO);
#endif

	//myShader.clean();
	
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
		model = translate(model, vec3(-0.4f, 0.0f, -1.0f));
		myShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = mat4(1.0f);
		model = translate(model, vec3(0.6f, 0.0f, 0.0f));
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
		model = translate(model, vec3(-0.4f, 0.0f, -1.0f));
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer,
        0);

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

        //�����ǰ��֡���������
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

    mat4 projection = perspective(45.0f,
        (float)SCR_WindowWidth / (float)SCR_WindowHeight,0.1f, 100.0f);
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

//GammaУ��
int drawWithGammaCorrection(GLFWwindow* window)
{
    bool gammaEnabled = false;
    bool gammaKeyPressed = false;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CustomShader myShader("../openGLearn/ShaderSource/GammaCorrection.vs",
                          "../openGLearn/ShaderSource/GammaCorrection.fs");
    float planeVertices[] =
    {
         // positions           // normals          // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f,
    };

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int texture = loadTexture("../openGLearn/Res/Texture/wood.png",
            false);
    unsigned int gammaTexture = loadTexture("../openGLearn/Res/Texture/wood.png",
            true);

    myShader.use();
    myShader.setInt("floorTexture", 0);
    vec3 lightPositions[] =
    {
        vec3(-3.0f, 0.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f),
        vec3(3.0f, 0.0f, 0.0f),
    };
    vec3 lightColors[] =
    {
        vec3(0.25f),
        vec3(0.50f),
        vec3(0.75f),
        vec3(1.00f),
    };
  
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
    
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gammaKeyPressed)
        {
            gammaEnabled = !gammaEnabled;
            cout << (gammaEnabled ? "Gamma enabled" : "Gamma disabled") << endl;
            gammaKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
        {
            gammaKeyPressed = false;
        }
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        myShader.use();
        mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WindowWidth
            / (float)SCR_WindowHeight, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        glUniform3fv(glGetUniformLocation(myShader.ID, "lightPositions"), 4, &lightPositions[0][0]);
        glUniform3fv(glGetUniformLocation(myShader.ID, "lightColors"), 4, &lightColors[0][0]);
        myShader.setVec3("viewPos", camera.Position);
        myShader.setInt("gamma", gammaEnabled);
        
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gammaEnabled ? gammaTexture : texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    
    glfwTerminate();
    return 0;
}

//��ʾ��Ӱ��ͼ���ݣ�������,����������Ⱦ����
//#define DEBUG_SHADOW_MAP

//�����޳��Խ����Ӱʧ��
#define CULL_FRONT_FACE_TO_SOLVE_ACNE

//��Ӱӳ��
int drawShadowMapping(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);
    CustomShader myShader("../openGLearn/ShaderSource/ShadowMapping.vs",
        "../openGLearn/ShaderSource/ShadowMapping.fs");
    CustomShader depthShader("../openGLearn/ShaderSource/ShadowMappingDepth.vs",
        "../openGLearn/ShaderSource/ShadowMappingDepth.fs");
    CustomShader debugShader("../openGLearn/ShaderSource/ShadowMappingDebug.vs",
        "../openGLearn/ShaderSource/ShadowMappingDebug.fs");
    
    float planeVertices[] =
    {
        // positions            // normals         // texcoords
         15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    
         15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
         15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
    };
    
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    
    unsigned int woodTex = loadTexture("../openGLearn/Res/Texture/wood.png");
    
    //��Ӱ��ͼ�ߴ�С�������Ӱ��ݡ�ʧ��
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    //��Ӱ��ͼ�����������Ϣ
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //���û��Ʒ�ʽ�ͱ߿���ɫ��ʹ�ó��������Ե��������ʾ��Ӱ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    //֡����ֻ�������������Ϣ��������Ⱦ����ɫ������
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    unsigned int cubeVAO = 0, cubeVBO = 0;
    unsigned int quadVAO = 0, quadVBO = 0;
    
    myShader.use();
    myShader.setInt("diffuseTexture", 0);
    myShader.setInt("shadowMap", 1);
    debugShader.use();
    debugShader.setInt("depthMap", 0);
    
    vec3 lightPos(-2.0f, 4.0f, -1.0f);
    
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mat4 lightProjection, lightView;
        mat4 lightSpaceMatrix;
        float near_plane = -10.0f, far_plane = 10.0f;
        //�����ռ������ڴ洢ƽ�й�Դ����Ӱ��Ϣ��͸��ͶӰ�����ڴ洢���Դ�������Ϣ
        lightProjection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane,
            far_plane);
        lightView = lookAt(lightPos, vec3(1.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        
#ifdef CULL_FRONT_FACE_TO_SOLVE_ACNE
        glCullFace(GL_FRONT);
#endif
        //1. ���Զ���֡�������Թ�Դ�ӽ���Ⱦ1�γ�����������Ӱ��ͼ�����Ϣ���������
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTex);
        renderScene(depthShader, planeVAO, cubeVAO, cubeVBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifdef CULL_FRONT_FACE_TO_SOLVE_ACNE
        glCullFace(GL_BACK);
#endif
        
        glViewport(0, 0, SCR_WindowWidth, SCR_WindowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //2. ��Ĭ��֡������������ӽ�������Ӱ��ͼ��Ⱦ1�γ��������
#ifndef DEBUG_SHADOW_MAP
        myShader.use();
        mat4 projection = perspective(radians(camera.Zoom),
            (float)SCR_WindowWidth / (float)SCR_WindowHeight, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);

        myShader.setVec3("viewPos", camera.Position);
        myShader.setVec3("lightPos", lightPos);
        myShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(myShader, planeVAO, cubeVAO, cubeVBO);
#else
        //�����Ӱ��ͼ��������
        debugShader.use();
        debugShader.setFloat("near_plane", near_plane);
        debugShader.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad(quadVAO, quadVBO);
#endif
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    
    glfwTerminate();
    return 0;
}

//��Ӱӳ�� - ��Ⱦ����
void renderScene(CustomShader &shader, int planeVAO, unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    glBindVertexArray(planeVAO);
    mat4 model = mat4(1.0f);
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, 1.5f, 0.0f));
    model = scale(model, vec3(0.5f));
    shader.setMat4("model", model);
    renderCube(cubeVAO, cubeVBO);
    model = mat4(1.0f);
    model = translate(model, vec3(2.0f, 0.0f, 1.0f));
    model = scale(model, vec3(0.5f));
    shader.setMat4("model", model);
    renderCube(cubeVAO, cubeVBO);
    model = mat4(1.0f);
    model = translate(model, vec3(-1.f, 0.0f, 2.0f));
    model = rotate(model, radians(60.0f),
        normalize(vec3(1.0f, 0.0f, 1.0f)));
    model = scale(model, vec3(0.25f));
    shader.setMat4("model", model);
    renderCube(cubeVAO, cubeVBO);
}

//��Ӱӳ�� - ��Ⱦ������
void renderCube(unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    if(cubeVAO == 0)
    {
        float vertices[] =
        {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
    
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

//��Ӱӳ�� - ��Ⱦ��Ƭ����ʾ��ͼ����
void renderQuad(unsigned int &quadVAO, unsigned int &quadVBO)
{
    if (quadVAO == 0)
    {
        float quadVertices[] =
        {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
      
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            (void *) (3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

//���Դ��Ӱ
int drawOmnidirectionalShadow(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    CustomShader myShader("../openGLearn/ShaderSource/PointLightShadow.vs",
        "../openGLearn/ShaderSource/PointLightShadow.fs");
    CustomShader depthShader("../openGLearn/ShaderSource/PointLightShadowDepth.vs",
        "../openGLearn/ShaderSource/PointLightShadowDepth.fs",
        "../openGLearn/ShaderSource/PointLightShadowDepth.gs");
    
    unsigned int woodTex = loadTexture("../openGLearn/Res/Texture/wood.png");
    
    bool shadows = true;
    bool shadowsKeyPressed = false;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    //������������ͼ�Դ洢��Ӱ���
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for(int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
            SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    unsigned int cubeVAO = 0, cubeVBO = 0;
    
    myShader.use();
    myShader.setInt("diffuseTexture", 0);
    myShader.setInt("depthMap", 1);
    
    vec3 lightPos(0.0f, 0.0f, 0.0f);
    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
    
        //������Ӱ
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !shadowsKeyPressed)
        {
            shadows = !shadows;
            shadowsKeyPressed = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
        {
            shadowsKeyPressed = false;
        }
        
        //�����ƶ�
        lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;
        
        float near = 1.0f, far = 25.0f;
        //��Ӱ��ͼ��Ⱦ��6�����������ϣ������壩��fovΪ90�ȣ������ͬ���Ա�֤���ڵ����ڱ�Ե����
        mat4 shadowProj = perspective(radians(90.0f),
            (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT, near, far);
        vector<mat4> shadowTransforms;
        //����6�����ת�����󣬰�����ͼ�ռ�任�������Դ�ռ䣨͸��ͶӰ���任����
        shadowTransforms.push_back(shadowProj * lookAt(lightPos,
            lightPos + vec3( 1.0,  0.0,  0.0), vec3(0.0, -1.0,  0.0)));
        shadowTransforms.push_back(shadowProj * lookAt(lightPos,
            lightPos + vec3(-1.0,  0.0,  0.0), vec3(0.0, -1.0,  0.0)));
        shadowTransforms.push_back(shadowProj * lookAt(lightPos,
            lightPos + vec3( 0.0,  1.0,  0.0), vec3(0.0,  0.0,  1.0)));
        shadowTransforms.push_back(shadowProj * lookAt(lightPos,
            lightPos + vec3( 0.0, -1.0,  0.0), vec3(0.0,  0.0, -1.0)));
        shadowTransforms.push_back(shadowProj * lookAt(lightPos,
            lightPos + vec3( 0.0,  0.0,  1.0), vec3(0.0, -1.0,  0.0)));
        shadowTransforms.push_back(shadowProj * lookAt(lightPos,
            lightPos + vec3( 0.0,  0.0, -1.0), vec3(0.0, -1.0,  0.0)));
    
        //�ⲿִ��1����Ⱦ���ã��ڲ�����������ͼִ��6����Ⱦ����������
        //��Ⱦ������������Ӱ���
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        for(unsigned int i = 0; i < 6; i++)
        {
            depthShader.setMat4("shadowMatrices[" + to_string(i) + "]", shadowTransforms[i]);
        }
        depthShader.setFloat("far_plane", far);
        depthShader.setVec3("lightPos", lightPos);
        renderPointLightScene(depthShader, cubeVAO, cubeVBO);
        
        //��Ⱦ��������
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WindowWidth, SCR_WindowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        myShader.use();
        
        mat4 projection, view;
        mat4 lightSpaceMatrix;
        projection = perspective(radians(camera.Zoom),
            (float)SCR_WindowWidth / (float)SCR_WindowHeight, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        myShader.setVec3("lightPos", lightPos);
        myShader.setVec3("viewPos", camera.Position);
        myShader.setInt("shadows", shadows);
        myShader.setFloat("far_plane", far);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        renderPointLightScene(myShader, cubeVAO, cubeVBO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

//���Դ��Ӱ-��Ⱦ����
void renderPointLightScene(CustomShader &shader, unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    mat4 model = mat4(1.0f);
    model = scale(model, vec3(8.0f));
    shader.setMat4("model", model);
    glDisable(GL_CULL_FACE);
    shader.setInt("reverse_normals", 1);
    renderPointLightCube(cubeVAO, cubeVBO);
    shader.setInt("reverse_normals", 0);
    glEnable(GL_CULL_FACE);
    
    model = mat4(1.0f);
    model = translate(model, vec3(3.0f, -3.5f, 0.0f));
    model = scale(model, vec3(0.5f));
    shader.setMat4("model", model);
    renderPointLightCube(cubeVAO, cubeVBO);
    model = mat4(1.0f);
    model = translate(model, vec3(2.0f, 3.0f, 1.0f));
    model = scale(model, vec3(0.75f));
    shader.setMat4("model", model);
    renderPointLightCube(cubeVAO, cubeVBO);
    model = mat4(1.0f);
    model = translate(model, vec3(-3.0f, -1.0f, 0.0f));
    model = scale(model, vec3(0.5f));
    shader.setMat4("model", model);
    renderPointLightCube(cubeVAO, cubeVBO);
    model = mat4(1.0f);
    model = translate(model, vec3(-1.5f, 1.0f, 1.5f));
    model = scale(model, vec3(0.5f));
    shader.setMat4("model", model);
    renderPointLightCube(cubeVAO, cubeVBO);
    model = mat4(1.0f);
    model = translate(model, vec3(-1.5f, 2.0f, -3.0f));
    model = rotate(model, radians(60.0f),
        normalize(vec3(1.0f, 0.0f, 1.0f)));
    model = scale(model, vec3(0.75f));
    shader.setMat4("model", model);
    renderPointLightCube(cubeVAO, cubeVBO);
}

//���Դ��Ӱ-��Ⱦ������
void renderPointLightCube(unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
       
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
            8 * sizeof(float), (void *) (6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

//����ӳ��,�޷���Ⱦ��߶�ԭ����ڵ���Ч��
int drawWithNormalMapping(GLFWwindow* window)
{
    glEnable(GL_DEPTH_TEST);
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    CustomShader myShader("../openGLearn/ShaderSource/NormalMapping.vs",
                          "../openGLearn/ShaderSource/NormalMapping.fs");
    
    unsigned int diffuseMap = loadTexture("../openGLearn/Res/Texture/brickwall.jpg");
    unsigned int normalMap = loadTexture("../openGLearn/Res/Texture/brickwall_normal.jpg");
    
    myShader.use();
    myShader.setInt("diffuseMap", 0);
    myShader.setInt("normalMap", 1);
    
    vec3 lightPos(0.5f, 1.0f, 0.3f);
    
    
    //quad data
    unsigned int quadVAO, quadVBO;
    vec3 pos1(-1.0f, 1.0f, 0.0f);
    vec3 pos2(-1.0f, -1.0f, 0.0f);
    vec3 pos3(1.0f, -1.0f, 0.0f);
    vec3 pos4(1.0f, 1.0f, 0.0f);
    // texture coordinates
    vec2 uv1(0.0f, 1.0f);
    vec2 uv2(0.0f, 0.0f);
    vec2 uv3(1.0f, 0.0f);
    vec2 uv4(1.0f, 1.0f);
    // normal vector
    vec3 nm(0.0f, 0.0f, 1.0f);
    
    // calculate tangent/bitangent vectors of both triangles
    vec3 tangent1, bitangent1;
    vec3 tangent2, bitangent2;
    // triangle 1
    vec3 edge1 = pos2 - pos1;
    vec3 edge2 = pos3 - pos1;
    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;
    
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    
    // triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;
    
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    
    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    
    float quadVertices[] =
    {
        // positions            // normal         // texcoords  // tangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z,
        // bitangent
        bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z,
        bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z,
        bitangent1.x, bitangent1.y, bitangent1.z,
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z,
        bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z,
        bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z,
        bitangent2.x, bitangent2.y, bitangent2.z
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    //end quad data
    
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mat4 projection = perspective(radians(camera.Zoom),
            (float)SCR_WindowWidth/ (float)SCR_WindowHeight, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        myShader.use();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        mat4 model = mat4(1.0f);
//        model = rotate(model, radians((float)glfwGetTime() * -10.0f),
//            normalize(vec3(1.0f, 0.0f, 1.0f)));
        myShader.setMat4("model", model);
        myShader.setVec3("viewPos", camera.Position);
        myShader.setVec3("lightPos", lightPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

//�Ӳ�ӳ�䣬������Ⱦ��߶�ԭ����ڵ���Ч��
int drawWithParallaxMapping(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);
    
    float heightScale = 0.1f;
    bool parallax_mapping = true;
    
    CustomShader myShader("../openGLearn/ShaderSource/ParallaxMapping.vs",
                          "../openGLearn/ShaderSource/ParallaxMapping.fs");
    
    unsigned int diffuseMap = loadTexture("../openGLearn/Res/Texture/bricks2.jpg");
    unsigned int normalMap = loadTexture("../openGLearn/Res/Texture/bricks2_normal.jpg");
    //�Ӳ���ͼ
    unsigned int heightMap = loadTexture("../openGLearn/Res/Texture/bricks2_disp.jpg");
    
    myShader.use();
    myShader.setInt("diffuseMap", 0);
    myShader.setInt("normalMap", 1);
    myShader.setInt("depthMap", 2);
    
    vec3 lightPos(0.5f, 1.0f, 0.3f);
    
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    
    //position
    vec3 pos1(-1.0, 1.0, 0.0);
    vec3 pos2(-1.0, -1.0, 0.0);
    vec3 pos3(1.0, -1.0, 0.0);
    vec3 pos4(1.0, 1.0, 0.0);
    // texture coordinates
    vec2 uv1(0.0, 1.0);
    vec2 uv2(0.0, 0.0);
    vec2 uv3(1.0, 0.0);
    vec2 uv4(1.0, 1.0);
    // normal vector
    vec3 nm(0.0, 0.0, 1.0);
    
    // calculate tangent/bitangent vectors of both triangles
    vec3 tangent1, bitangent1;
    vec3 tangent2, bitangent2;
    // - triangle 1
    vec3 edge1 = pos2 - pos1;
    vec3 edge2 = pos3 - pos1;
    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;
    
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = normalize(tangent1);
    
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = normalize(bitangent1);
    
    // - triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;
    
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = normalize(tangent2);
    
    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = normalize(bitangent2);
    
    float quadVertices[] =
    {
        // Positions            // normal         // TexCoords  // Tangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y,
        // Bitangent
        tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y,
        tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y,
        tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y,
        tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y,
        tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y,
        tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
        (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
        (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
        (void *) (8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
        (void *) (11 * sizeof(float)));
    
    
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            parallax_mapping = false;
        }
        else
        {
            parallax_mapping = true;
        }
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        myShader.use();
        mat4 view = camera.GetViewMatrix();
        mat4 projection = perspective(radians(camera.Zoom),
            (float)SCR_WindowWidth / (float)SCR_WindowHeight,0.1f, 100.0f);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);
        mat4 model = mat4(1.0f);
        model = rotate(model, radians((float)glfwGetTime() * 0),
            normalize(vec3(1.0, 0.0, 1.0)));
        myShader.setMat4("model", model);
        myShader.setVec3("viewPos", camera.Position);
        myShader.setVec3("lightPos", lightPos);
        myShader.setFloat("heightScale", heightScale);
        myShader.setBool("enableParallax", parallax_mapping);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, heightMap);
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

//HDR
int drawWithHDR(GLFWwindow* window)
{
    bool hdr = true;
    bool hdrKeyPressed = false;
    float exposure = 1.0f;
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);
    
    CustomShader myShader("../openGLearn/ShaderSource/HDRScene.vs",
                          "../openGLearn/ShaderSource/HDRScene.fs");
    CustomShader hdrShader("../openGLearn/ShaderSource/HDR.vs",
                           "../openGLearn/ShaderSource/HDR.fs");
    
    unsigned int woodTexture = loadTexture("../openGLearn/Res/Texture/wood.png",
        true);
    // note that we're loading the texture as an SRGB texture
    
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    //��ɫ����ʹ�ø���֡�����ʽ��GL_RGB16F��GL_RGBA_16F��GL_RGB32F��GL_RGBA32F�����ɴ洢����0.0��1.0��Χ��ֵ
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
        GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WindowWidth, SCR_WindowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // lighting info
    // positions
    std::vector<vec3> lightPositions;
    lightPositions.push_back(vec3(0.0f, 0.0f, 49.5f)); // back light
    lightPositions.push_back(vec3(-1.4f, -1.9f, 9.0f));
    lightPositions.push_back(vec3(0.0f, -1.8f, 4.0f));
    lightPositions.push_back(vec3(0.8f, -1.7f, 6.0f));
    // colors
    std::vector<vec3> lightColors;
    lightColors.push_back(vec3(200.0f, 200.0f, 200.0f));
    lightColors.push_back(vec3(0.1f, 0.0f, 0.0f));
    lightColors.push_back(vec3(0.0f, 0.0f, 0.2f));
    lightColors.push_back(vec3(0.0f, 0.1f, 0.0f));
    
    myShader.use();
    myShader.setInt("diffuseTexture", 0);
    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);
    
    unsigned int quadVAO = 0, cubeVAO = 0;
    unsigned int quadVBO, cubeVBO;
    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            hdr = false;
        }
        else
        {
            hdr = true;
        }
    
        //�ع��
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            if (exposure > 0.0f)
                exposure -= 0.001f;
            else
                exposure = 0.0f;
            
            cout << "exposure: " << exposure << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            exposure += 0.001f;
    
            cout << "exposure: " << exposure << endl;
        }
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //��Ⱦ����������֡�������Ա�����ɫ����[0.0��1.0]�Ĳ���
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4 projection = perspective(radians(camera.Zoom),
            (GLfloat) SCR_WindowWidth / (GLfloat) SCR_WindowHeight, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        myShader.use();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            myShader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            myShader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
        }
        myShader.setVec3("viewPos", camera.Position);
        
        mat4 model = mat4(1.0f);
        model = translate(model, vec3(0.0f, 0.0f, 25.0));
        model = scale(model, vec3(2.5f, 2.5f, 27.5f));
        myShader.setMat4("model", model);
        myShader.setInt("inverse_normals", true);
        renderHDRCube(cubeVAO, cubeVBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        //������ɫ�����������Ⱦ��Ĭ��֡�����У���ɫֵ����[0.0��1.0]��Χ��
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        hdrShader.setInt("hdr", hdr);
        hdrShader.setFloat("exposure", exposure);
        renderHDRQuad(quadVAO, quadVBO);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

//hdr
void renderHDRCube(unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    if (cubeVAO == 0)
    {
        float vertices[] =
        {
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void *)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderHDRQuad(unsigned int &quadVAO, unsigned int &quadVBO)
{
    if (quadVAO == 0)
    {
        float quadVertices[] =
        {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            (void *) (3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

//������Ⱦ
int drawBloom(GLFWwindow* window)
{
    bool bloom = true;
    float exposure = 1.0f;
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);
    
    CustomShader myShader("../openGLearn/ShaderSource/Bloom.vs",
                          "../openGLearn/ShaderSource/Bloom.fs");
    CustomShader lightShader("../openGLearn/ShaderSource/Bloom.vs",
                          "../openGLearn/ShaderSource/LightBox.fs");
    CustomShader blurShader("../openGLearn/ShaderSource/Blur.vs",
                          "../openGLearn/ShaderSource/Blur.fs");
    CustomShader finalBloomShader("../openGLearn/ShaderSource/Blur.vs",
                          "../openGLearn/ShaderSource/FinalBloom.fs");
    
    unsigned int woodTexture = loadTexture("../openGLearn/Res/Texture/wood.png",
        true);
    unsigned int containerTexture = loadTexture("../openGLearn/Res/Texture/container2.png",
        true);
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for(unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
            GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
            colorBuffers[i], 0);
    }
    
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WindowWidth, SCR_WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    
    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "Framebuffer not completed!" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for(unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
            GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            pingpongColorbuffers[i], 0);
        if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            cout << "Framebuffer not completed!" << endl;
        }
    }
    
    vector<vec3> lightPositions;
    lightPositions.push_back(vec3( 0.0f, 0.5f,  1.5f));
    lightPositions.push_back(vec3(-4.0f, 0.5f, -3.0f));
    lightPositions.push_back(vec3( 3.0f, 0.5f,  1.0f));
    lightPositions.push_back(vec3(-0.8f, 2.4f, -1.0f));
    
    vector<vec3> lightColors;
    lightColors.push_back(vec3( 5.0f, 5.0f,  5.0f));
    lightColors.push_back(vec3(10.0f, 0.0f,  0.0f));
    lightColors.push_back(vec3( 0.0f, 0.0f, 15.0f));
    lightColors.push_back(vec3( 0.0f, 5.0f,  0.0f));
    
    myShader.use();
    myShader.setInt("diffuseTexture", 0);
    blurShader.use();
    blurShader.setInt("image", 0);
    finalBloomShader.use();
    finalBloomShader.setInt("scene", 0);
    finalBloomShader.setInt("bloomBlur", 1);
    
    unsigned int cubeVAO = 0, cubeVBO = 0;
    unsigned int quadVAO = 0, quadVBO = 0;
    
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            if (exposure > 0.0f)
                exposure -= 0.001f;
            else
                exposure = 0.0f;
            
            cout << "exposure: " << exposure << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            exposure += 0.001f;
    
            cout << "exposure: " << exposure << endl;
        }
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mat4 projection = perspective(radians(camera.Zoom),
            (float)SCR_WindowWidth / (float)SCR_WindowHeight, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 model = mat4(1.0f);
        myShader.use();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        
        for(unsigned int i = 0; i < lightPositions.size(); i++)
        {
            myShader.setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
            myShader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
        }
        myShader.setVec3("viewPos", camera.Position);
        
        model = mat4(1.0f);
        model = translate(model, vec3(0.0f, -1.0f, 0.0f));
        model = scale(model, vec3(12.5f, 0.5f, 12.5f));
        myShader.setMat4("model", model);
        renderBloomCube(cubeVAO, cubeVBO);
        
        glBindTexture(GL_TEXTURE_2D, containerTexture);
        
        model = mat4(1.0f);
        model = translate(model, vec3(0.5f));
        myShader.setMat4("model", model);
        renderBloomCube(cubeVAO, cubeVBO);
        model = mat4(1.0f);
        model = translate(model, vec3(2.0f, 0.0f, 1.0));
        model = scale(model, vec3(0.5f));
        myShader.setMat4("model", model);
        renderBloomCube(cubeVAO, cubeVBO);
        model = mat4(1.0f);
        model = translate(model, vec3(-1.0f, -1.0f, 2.0));
        model = rotate(model, radians(60.0f),
            normalize(vec3(1.0, 0.0, 1.0)));
        myShader.setMat4("model", model);
        renderBloomCube(cubeVAO, cubeVBO);
        model = mat4(1.0f);
        model = translate(model, vec3(0.0f, 2.7f, 4.0));
        model = rotate(model, radians(23.0f),
            normalize(vec3(1.0, 0.0, 1.0)));
        model = scale(model, vec3(1.25));
        myShader.setMat4("model", model);
        renderBloomCube(cubeVAO, cubeVBO);
        model = mat4(1.0f);
        model = translate(model, vec3(-2.0f, 1.0f, -3.0));
        model = rotate(model, radians(124.0f),
            normalize(vec3(1.0, 0.0, 1.0)));
        myShader.setMat4("model", model);
        renderBloomCube(cubeVAO, cubeVBO);
        model = mat4(1.0f);
        model = translate(model, vec3(-3.0f, 0.0f, 0.0));
        model = scale(model, vec3(0.5f));
        myShader.setMat4("model", model);
        renderBloomCube(cubeVAO, cubeVBO);
        
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        
        for(unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = mat4(1.0f);
            model = translate(model, vec3(lightPositions[i]));
            model = scale(model, vec3(0.25f));
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", lightColors[i]);
            renderBloomCube(cubeVAO, cubeVBO);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use();
        for(unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1]
            : pingpongColorbuffers[!horizontal]);
            renderBloomQuad(quadVAO, quadVBO);
            horizontal = !horizontal;
            if(first_iteration)
            {
                first_iteration = false;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        finalBloomShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        finalBloomShader.setInt("bloom", bloom);
        finalBloomShader.setFloat("exposure", exposure);
        renderBloomQuad(quadVAO, quadVBO);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

void renderBloomCube(unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    if (cubeVAO == 0)
    {
        float vertices[] =
        {
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
        };
        
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void *) (6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderBloomQuad(unsigned int &quadVAO, unsigned int &quadVBO)
{
    if (quadVAO == 0)
    {
        float quadVertices[] =
        {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            (void *) (3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int drawWithDeferredRendering(GLFWwindow * window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    stbi_set_flip_vertically_on_load_thread(true);

    glEnable(GL_DEPTH_TEST);

    CustomShader geometryPassShader("../openGLearn/ShaderSource/GBuffer.vs",
        "../openGLearn/ShaderSource/GBuffer.fs");
    CustomShader lightingPassShader("../openGLearn/ShaderSource/Blur.vs",
        "../openGLearn/ShaderSource/DeferredShader.fs");
    CustomShader lightBoxShader("../openGLearn/ShaderSource/Color.vs",
         "../openGLearn/ShaderSource/DeferredLightBox.fs");

    Model nanosuit("../openGLearn/Res/Model/nanosuit/nanosuit.obj");

    vector<vec3> objectPositions;
    objectPositions.push_back(vec3(-3.0,  -0.5, -3.0));
    objectPositions.push_back(vec3( 0.0,  -0.5, -3.0));
    objectPositions.push_back(vec3( 3.0,  -0.5, -3.0));
    objectPositions.push_back(vec3(-3.0,  -0.5,  0.0));
    objectPositions.push_back(vec3( 0.0,  -0.5,  0.0));
    objectPositions.push_back(vec3( 3.0,  -0.5,  0.0));
    objectPositions.push_back(vec3(-3.0,  -0.5,  3.0));
    objectPositions.push_back(vec3( 0.0,  -0.5,  3.0));
    objectPositions.push_back(vec3( 3.0,  -0.5,  3.0));
    
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;
    
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
        GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
        GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WindowWidth, SCR_WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const unsigned int NR_LIGHTS = 32;
    vector<vec3> lightPositions;
    vector<vec3> lightColors;
    srand(13);
    for(unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        lightColors.push_back(vec3(rColor, gColor, bColor));
    }

    lightingPassShader.use();
    lightingPassShader.setInt("gPosition", 0);
    lightingPassShader.setInt("gNormal", 1);
    lightingPassShader.setInt("gAlbedoSpec", 2);

    unsigned int cubeVAO = 0, cubeVBO = 0;
    unsigned int quadVAO = 0, quadVBO = 0;

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4 projection = perspective(radians(camera.Zoom),
            (float) SCR_WindowWidth / (float) SCR_WindowHeight, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 model = mat4(1.0f);
        geometryPassShader.use();
        geometryPassShader.setMat4("projection", projection);
        geometryPassShader.setMat4("view", view);
        for(unsigned int i = 0; i < objectPositions.size(); i++)
        {
            model = mat4(1.0f);
            model = translate(model, objectPositions[i]);
            model = scale(model, vec3(0.25f));
            geometryPassShader.setMat4("model", model);
            nanosuit.Draw(geometryPassShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingPassShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        for(unsigned int i = 0; i < lightPositions.size(); i++)
        {
            lightingPassShader.setVec3("lights[" + to_string(i) + "].Position",
                lightPositions[i]);
            lightingPassShader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
            const float constant = 1.0;
            const float linear = 0.7;
            const float quadratic = 1.8;
            lightingPassShader.setFloat("lights[" + to_string(i) + "].Linear", linear);
            lightingPassShader.setFloat("lights[" + to_string(i) + "].Quadratic", quadratic);
            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g),
                lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic
                * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            lightingPassShader.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
        }
        lightingPassShader.setVec3("viewPos", camera.Position);
        renderDeferredQuad(quadVAO, quadVBO);

        glBindFramebuffer(GL_READ_FRAMEBUFFER,  gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0, 0, SCR_WindowWidth, SCR_WindowHeight, 0, 0, SCR_WindowWidth,
            SCR_WindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightBoxShader.use();
        lightBoxShader.setMat4("projection", projection);
        lightBoxShader.setMat4("view", view);
        for(unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = mat4(1.0f);
            model = translate(model, lightPositions[i]);
            model = scale(model, vec3(0.125f));
            lightBoxShader.setMat4("model", model);
            lightBoxShader.setVec3("lightColor", lightColors[i]);
            renderDeferredCube(cubeVAO, cubeVBO);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void renderDeferredCube(unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    if(cubeVAO == 0)
    {
        float vertices[] =
        {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderDeferredQuad(unsigned int &quadVAO, unsigned int &quadVBO)
{
    if (quadVAO == 0)
    {
        float quadVertices[] =
        {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
            GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int drawSSAO(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    CustomShader geometryShader("../openGLearn/ShaderSource/SSAOGBuffer.vs",
    "../openGLearn/ShaderSource/SSAOGBuffer.fs");
    CustomShader lightingShader("../openGLearn/ShaderSource/Blur.vs",
        "../openGLearn/ShaderSource/SSAOLighting.fs");
    CustomShader ssaoShader("../openGLearn/ShaderSource/Blur.vs",
        "../openGLearn/ShaderSource/SSAO.fs");
    CustomShader blurShader("../openGLearn/ShaderSource/Blur.vs",
        "../openGLearn/ShaderSource/SSAOBlur.fs");

    Model nanosuit("../openGLearn/Res/Model/nanosuit/nanosuit.obj");

    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedo;
    
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
        GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
        GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WindowWidth, SCR_WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RED, GL_FLOAT,
        NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WindowWidth, SCR_WindowHeight, 0, GL_RED, GL_FLOAT,
        NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur,
        0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // generate sample kernel
    // generates random floats between 0.0 and 1.0
    uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    default_random_engine generator;
    vector<vec3> ssaoKernel;
    
    for (unsigned int i = 0; i < 64; ++i)
    {
        vec3 sample(randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;
        
        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    
    vector<vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        // rotate around z-axis (in tangent space)
        vec3 noise(randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,0.0f);
        ssaoNoise.push_back(noise);
    }
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    vec3 lightPos = vec3(2.0, 4.0, -2.0);
    vec3 lightColor = vec3(0.2, 0.2, 0.7);

    lightingShader.use();
    lightingShader.setInt("gPosition", 0);
    lightingShader.setInt("gNormal", 1);
    lightingShader.setInt("gAlbedo", 2);
    lightingShader.setInt("ssao", 3);
    ssaoShader.use();
    ssaoShader.setInt("gPosition", 0);
    ssaoShader.setInt("gNormal", 1);
    ssaoShader.setInt("texNoise", 2);
    blurShader.use();
    blurShader.setInt("ssaoInput", 0);

    unsigned int cubeVAO = 0, cubeVBO = 0;
    unsigned int quadVAO = 0, quadVBO = 0;

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4 projection = perspective(radians(camera.Zoom),
            (float) SCR_WindowWidth / (float) SCR_WindowHeight, 0.1f, 50.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 model = mat4(1.0f);
        geometryShader.use();
        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);
        // room cube
        model = mat4(1.0f);
        model = translate(model, vec3(0.0, 7.0f, -7.5f));
        model = scale(model, vec3(7.5f, 7.5f, 15.0f));
        geometryShader.setMat4("model", model);
        // invert normals as we're inside the cube
        geometryShader.setInt("invertedNormals", 1);
        renderSSAOCube(cubeVAO, cubeVBO);
        geometryShader.setInt("invertedNormals", 0);
        // backpack model on the floor
        model = mat4(1.0f);
        model = translate(model, vec3(0.0f, 0.5f, 0.0));
        model = rotate(model, radians(-90.0f), vec3(1.0, 0.0, 0.0));
        model = scale(model, vec3(1.0f));
        geometryShader.setMat4("model", model);
        nanosuit.Draw(geometryShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoShader.use();
        for(unsigned int i = 0; i < 64; i++)
        {
            ssaoShader.setVec3("samples[" + to_string(i) + "]", ssaoKernel[i]);
        }
        ssaoShader.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        renderSSAOQuad(quadVAO, quadVBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        blurShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderSSAOQuad(quadVAO, quadVBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingShader.use();
        vec3 lightPosView = vec3(camera.GetViewMatrix() * vec4(lightPos, 1.0));
        lightingShader.setVec3("light.Position", lightPosView);
        lightingShader.setVec3("light.Color", lightColor);
        // Update attenuation parameters
        const float linear = 0.09;
        const float quadratic = 0.032;
        lightingShader.setFloat("light.Linear", linear);
        lightingShader.setFloat("light.Quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        // add extra SSAO texture to lighting pass
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        renderSSAOQuad(quadVAO, quadVBO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void renderSSAOCube(unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    if (cubeVAO == 0)
    {
        float vertices[] =
        {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderSSAOQuad(unsigned int &quadVAO, unsigned int &quadVBO)
{
    if (quadVAO == 0)
    {
        float quadVertices[] =
        {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
            GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



