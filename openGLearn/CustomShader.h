#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class CustomShader
{
public:
	//shader程序ID
	unsigned int ID;

	//构造器读取并构建着色器
	CustomShader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		//从文件路径获取顶点/片段着色器
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		//保证ifstream对象可抛出异常
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try
		{
			//打开文件
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			stringstream vShaderStream, fShaderStream;
			//读取文件的缓冲内容到数据流中
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//关闭文件处理器
			vShaderFile.close();
			fShaderFile.close();
			//转换数据流到string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e)
		{
			cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//编译着色器
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		//创建顶点shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		//添加shader源码并编译
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		//创建片段shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		//创建shader程序对象并添加指定shader，链接程序
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		//链接后删除shader
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	//使用/激活shader程序
	void use()
	{
		glUseProgram(ID);
	}

	//清除shader程序
	void clean()
	{
		glDeleteProgram(ID);
	}

	//Uniform工具函数，查询Uniform变量位置并赋值
	void setBool(const string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat4(const string& name, float value0, float value1, float value2, float value3)
		const
	{
		//4代表4个float值
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2, value3);
	}
	void setFloat3(const string& name, float value0, float value1, float value2)
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2);
	}
	void setVec3(const string& name, vec3 value)
	{
		setFloat3(name, value.x, value.y, value.z);
	}
	void setVec4(const string& name, vec4 value)
	{
		setFloat4(name, value.x, value.y, value.z, value.w);
	}
	void setMat4(const string& name, mat4 value)
	{
		//设置4x4矩阵值
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(value));
	}
};

#endif