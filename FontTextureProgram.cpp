#include "FontTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load< FontTextureProgram > font_texture_program(LoadTagEarly);

// Structured based on ColorTextureProgram.cpp
// Shaders source - https://learnopengl.com/In-Practice/Text-Rendering
FontTextureProgram::FontTextureProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330 core\n"
		"layout (location = 0) in vec4 vertex;\n"
		"out vec2 TexCoords;\n"
		"uniform mat4 projection;\n"
		"void main() {\n"
		//"	gl_Position = OBJECT_TO_CLIP * Position;\n"
		" gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"  // TODO - uncomment this for line below!!!
		//" gl_Position = vec4(gl_VertexID % 2, gl_VertexID / 2, 0, 1);\n"
		"	TexCoords = vertex.zw;\n"
		"}\n"
	,
		//fragment shader:
		"#version 330 core\n"
		"in vec2 TexCoords;\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"uniform vec3 textColor;\n"
		"void main() {\n"
		"	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
		" color = vec4(textColor, 1.0) * sampled;\n"
		//" color = vec4(1,0,1,1);\n" // Swap this for line above to make all fragments pink
		"}\n"
	);
	//As you can see above, adjacent strings in C/C++ are concatenated.
	// this is very useful for writing long shader programs inline.

	//look up the locations of vertex attributes:
	/*Position_vec4 = glGetAttribLocation(program, "Position");
	Color_vec4 = glGetAttribLocation(program, "Color");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");*/

	//look up the locations of uniforms:
	projection = glGetUniformLocation(program, "projection");
	GLuint TEX_sampler2D = glGetUniformLocation(program, "text");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now

	// Set up the vertex array object and vertex buffer object
	// Source - https://learnopengl.com/In-Practice/Text-Rendering
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

FontTextureProgram::~FontTextureProgram() {
	glDeleteProgram(program);
	program = 0;
}

