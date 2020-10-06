#pragma once

#include "GL.hpp"
#include "Load.hpp"

// Structured based on ColorTextureProgram.cpp
//Shader program
struct FontTextureProgram {
	FontTextureProgram();
	~FontTextureProgram();

	GLuint program = 0;

	//Attribute (per-vertex variable) locations:
	//GLuint Position_vec4 = -1U;
	GLuint Color_vec4 = -1U;
	//GLuint TexCoord_vec2 = -1U;

	//Uniform (per-invocation variable) locations:
	GLuint projection = -1U;

	//Textures:
	//TEXTURE0 - texture that is accessed by TexCoord

	GLuint VAO, VBO;
};

extern Load< FontTextureProgram > font_texture_program;
