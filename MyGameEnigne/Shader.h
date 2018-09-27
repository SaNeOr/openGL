#pragma once


#include <glm.hpp>		//glm(math)
#include <gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include<string>
class Shader
{
public:
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);

	unsigned ID;

	const char* vertexSource;
	const char* fragmentSource;

	enum slot {
		DIFFUSE,
		SPECULAR,
		EMISSION
	};

	Shader& use();

	void setUniform4fv(const char* paramNameString,glm::mat4 param);
	void setUniform3f(const char* paramNameString, glm::vec3 param);
	void setUniform1i(const char* paraNameString, int slot);



	~Shader();

private:
	void checkCompileErrors(unsigned int ID, std::string type);
};

