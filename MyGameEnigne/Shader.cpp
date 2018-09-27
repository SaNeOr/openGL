#include "Shader.h"
#include <iostream>
#include <fstream>
#include<sstream>
#define GLEW_STATIC			//设置glew使用的是glew32s.lib
#include<GL/glew.h>

using namespace std;


Shader::Shader()
{
}

Shader::Shader(const char * vertexPath, const char * fragmentPath)
{

	ifstream vertexFile;
	ifstream fragmentFile;
	stringstream vertexSStream;
	stringstream fragmentSStream;
	string vertexString;
	string fragmentString;

	vertexFile.exceptions(ifstream::failbit || ifstream::badbit);
	fragmentFile.exceptions(ifstream::failbit || ifstream::badbit);
	try {
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		//badbit/failbit == throw exception; 

		vertexSStream << vertexFile.rdbuf();
		fragmentSStream << fragmentFile.rdbuf();
		vertexFile.close();
		fragmentFile.close();

		vertexString = vertexSStream.str();
		fragmentString = fragmentSStream.str();

		vertexSource = vertexString.c_str();
		fragmentSource = fragmentString.c_str();

		unsigned int vertex, fragment;


		//COUMPILE
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSource, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSource, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		//LINK
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		glDeleteShader(vertex);
		glDeleteShader(fragment);

	}
	catch(ifstream::failure e){
		cout << e.what() << endl;
		
	}
}

Shader& Shader::use()
{
	glUseProgram(ID);
	return *this;
}

void Shader::setUniform4fv(const char * paramNameString, glm::mat4 param)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, paramNameString), 1, GL_FALSE, glm::value_ptr(param));
}

void Shader::setUniform3f(const char * paramNameString, glm::vec3 param)
{
	glUniform3f(glGetUniformLocation(ID, paramNameString), param.x, param.y, param.z);
}

void Shader::setUniform1i(const char * paraNameString, int slot)
{
	glUniform1i(glGetUniformLocation(ID, paraNameString), slot);

}

Shader::~Shader()
{

}

void Shader::checkCompileErrors(unsigned int ID, std::string type)
{
	int success;
	char infoLog[512];

	if (type != "PROGRAM") {
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			cout << "shader compiler error" << infoLog << endl;
		}
	}
	else {
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			cout << "program linking error" << infoLog << endl;
		}
	}
}
