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
	//shader����ID
	unsigned int ID;

	//��������ȡ��������ɫ��
	CustomShader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		//���ļ�·����ȡ����/Ƭ����ɫ��
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		//��֤ifstream������׳��쳣
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try
		{
			//���ļ�
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			stringstream vShaderStream, fShaderStream;
			//��ȡ�ļ��Ļ������ݵ���������
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//�ر��ļ�������
			vShaderFile.close();
			fShaderFile.close();
			//ת����������string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e)
		{
			cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//������ɫ��
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		//��������shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		//���shaderԴ�벢����
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		//����Ƭ��shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		//����shader����������ָ��shader�����ӳ���
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
		//���Ӻ�ɾ��shader
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	//ʹ��/����shader����
	void use()
	{
		glUseProgram(ID);
	}

	//���shader����
	void clean()
	{
		glDeleteProgram(ID);
	}

	//Uniform���ߺ�������ѯUniform����λ�ò���ֵ
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
		//4����4��floatֵ
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
		//����4x4����ֵ
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(value));
	}
};

#endif