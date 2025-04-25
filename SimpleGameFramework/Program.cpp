#include "Program.h"
#include <iostream>
#include <fstream>
#include "Common.h"

sgf::SimpleProgram::SimpleProgram()
{

}

sgf::SimpleProgram::~SimpleProgram()
{

}

void sgf::SimpleProgram::LoadFromFile(const char* vertexShader, const char* fragmentShader)
{
	unsigned int vertexShaderUnit = LoadShader(vertexShader,SHADER_VERTEX);
	unsigned int fragmentShaderUnit = LoadShader(fragmentShader,SHADER_FRAGMENT);
	
	mProgram = glCreateProgram();
	GL_CALL(glAttachShader(mProgram, vertexShaderUnit));
	GL_CALL(glAttachShader(mProgram, fragmentShaderUnit));

	GLint success = 0;
	GLchar errorLog[1024] = { 0 };
	GL_CALL(glLinkProgram(mProgram));

	glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
	if (success == 0) {
		glGetProgramInfoLog(mProgram, sizeof(errorLog), NULL, errorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", errorLog);
		exit(1);
	}

	glDeleteShader(vertexShaderUnit);
	glDeleteShader(fragmentShaderUnit);

}

unsigned int sgf::SimpleProgram::LoadShader(const char* path,ShaderType type)
{
	std::ifstream shaderFile;
	shaderFile.open(path);
	shaderFile.seekg(0,std::ios_base::end);
	int shaderSize = shaderFile.tellg();
	shaderFile.seekg(0, std::ios_base::beg);
	char* shader = new char[shaderSize+1];
	memset(shader,0, shaderSize + 1);
	shaderFile.read(shader, shaderSize);
	shaderFile.close();

	unsigned int shaderType;
	if(type == SHADER_VERTEX)
		shaderType = GL_VERTEX_SHADER;
	else
		shaderType = GL_FRAGMENT_SHADER;

	unsigned int shaderUnit = glCreateShader(shaderType);

	GL_CALL(glShaderSource(shaderUnit, 1, &shader, NULL));

	GL_CALL(glCompileShader(shaderUnit));
	int success;
	glGetShaderiv(shaderUnit, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shaderUnit, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", shaderUnit, InfoLog);
		return -1;
	}

	SGF_ASSERT(shader);
	delete[] shader;
	return shaderUnit;
}

void sgf::SimpleProgram::Use()
{
	glUseProgram(mProgram);
}


void sgf::Vertex::InformationInit()
{
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(float)));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(float)));
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(10 * sizeof(float)));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
}
