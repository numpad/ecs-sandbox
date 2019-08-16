#include "sgl_shader_uniform.hpp"

void sgl::shader_uniform::use_program()
{
	/* save current shader program */
	glGetIntegerv(GL_CURRENT_PROGRAM, &this->saved_program);
	glUseProgram(this->shader_program);
}

void sgl::shader_uniform::restore_previous_program()
{
	glUseProgram((GLuint)this->saved_program); /* restore last used shader program */
}

sgl::shader_uniform::shader_uniform(const GLuint program, const GLint location)
	: shader_program(program),
	  uniform_location(location)
{
}

sgl::shader_uniform::operator int() const
{
	return this->uniform_location;
}

void sgl::shader_uniform::operator=(const GLboolean &b)
{
	this->use_program();
	glUniform1i(this->uniform_location, b);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const GLint &i)
{
	this->use_program();
	glUniform1i(this->uniform_location, i);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const GLuint &u)
{
	this->use_program();
	glUniform1ui(this->uniform_location, u);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const GLfloat &v)
{
	this->use_program();
	glUniform1f(this->uniform_location, v);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const glm::vec2 &vec2)
{
	this->use_program();
	glUniform2f(this->uniform_location, vec2.x, vec2.y);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const glm::vec3 &vec3)
{
	this->use_program();
	glUniform3f(this->uniform_location, vec3.x, vec3.y, vec3.z);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const glm::vec4 &vec4)
{
	this->use_program();
	glUniform4f(this->uniform_location, vec4.x, vec4.y, vec4.z, vec4.w);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const glm::mat2 &mat2)
{
	this->use_program();
	glUniformMatrix2fv(this->uniform_location, 1, GL_FALSE, &mat2[0][0]);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const glm::mat3 &mat3)
{
	this->use_program();
	glUniformMatrix3fv(this->uniform_location, 1, GL_FALSE, &mat3[0][0]);
	this->restore_previous_program();
}

void sgl::shader_uniform::operator=(const glm::mat4 &mat4)
{
	this->use_program();
	glUniformMatrix4fv(this->uniform_location, 1, GL_FALSE, &mat4[0][0]);
	this->restore_previous_program();
}

