#include "sgl/sgl_shader.hpp"

sgl::shader::shader(std::string fname_vert, std::string fname_frag)
{
	
	this->load(fname_vert, sgl::shader::VERTEX);
	this->load(fname_frag, sgl::shader::FRAGMENT);
	this->compile(sgl::shader::VERTEX);
	this->compile(sgl::shader::FRAGMENT);
	this->link();
}

sgl::shader::shader()
{
	for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i)
		this->stage_of_type[i] = sgl::shader::STAGE_EMPTY;
}

sgl::shader::~shader()
{
	this->unload();
}

void sgl::shader::unload()
{
	glDeleteProgram(this->program);

	for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i) {
		this->stage_of_type[i] = sgl::shader::STAGE_EMPTY;
		this->shaders_src[i] = nullptr;
		this->shaders[i] = 0;
		this->filenames[i].clear();
	}
	this->program = 0;
}

void sgl::shader::reload()
{
	for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i) {
		if (!this->filenames[i].empty())
			this->load(this->filenames[i], (sgl::shader::type)i);
	}
	this->compile();
	this->link();
}

void sgl::shader::use()
{
	glUseProgram(this->program);
}

std::string sgl::shader::load_file(std::string fname)
{
	std::ifstream infile(fname);
	if (!infile.is_open())
		return "";
	
	std::string content{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
	return content;
}

bool sgl::shader::load(std::string fname, sgl::shader::type type)
{
	std::string content = sgl::shader::load_file(fname);
	
	bool loaded = this->load_from_memory(content, type);
	if (loaded)
		filenames[type] = fname;
	
	return loaded;
}

bool sgl::shader::load_from_memory(std::string ssrc, sgl::shader::type type)
{
	if (ssrc.size() == 0 || type == sgl::shader::WILDCARD)
		return false;
	
	const size_t src_len = ssrc.size();
	this->shaders_src[type] = new char[src_len + 1];
	strncpy(this->shaders_src[type], ssrc.c_str(), src_len);
	this->shaders_src[type][src_len] = 0;
	
	this->stage_of_type[type] = sgl::shader::STAGE_LOADED;

	return true;
}

bool sgl::shader::compile(sgl::shader::type type)
{
	/* special case WILDCARD: compile all loaded shaders, return true only if all compiled correctly! */
	if (type == sgl::shader::WILDCARD) {
		for (int i = 0; i < sgl::shader::MAX_TYPES; ++i) {
			if (this->stage_of_type[i] == sgl::shader::STAGE_LOADED) {
				bool ok = this->compile((sgl::shader::type)i);
				if (!ok) return false;
			}
		}

		return true;
	}

	if (this->stage_of_type[type] != sgl::shader::STAGE_LOADED) {
		std::cout << "Cannot compile " << sgl::shader::type_to_name[type] << " shader. Is it loaded?" << std::endl;
		return false;
	}

	this->shaders[type] = glCreateShader(sgl::shader::type_to_gl_shader[type]);
	glShaderSource(this->shaders[type], 1, (const GLchar* const*)&(this->shaders_src[type]), NULL);
	glCompileShader(this->shaders[type]);
	
	/* error message */
	const size_t info_log_len = 1024;
	int success;
	char info_log[info_log_len];
	
	glGetShaderiv(this->shaders[type], GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(this->shaders[type], info_log_len, NULL, info_log);
		
		std::cout << "Failed compiling " << sgl::shader::type_to_name[type] << " shader:" << std::endl;
		std::cout << info_log << std::endl;
	} else {
		this->stage_of_type[type] = sgl::shader::STAGE_COMPILED;
	}
	
	return success;
}

bool sgl::shader::link()
{
	this->program = glCreateProgram();
	for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i) {
		if (this->stage_of_type[i] == sgl::shader::STAGE_COMPILED)
			glAttachShader(this->program, this->shaders[i]);
	}
	glLinkProgram(this->program);
	
	/* error message */
	const size_t info_log_len = 1024;
	int success;
	char info_log[info_log_len];
	
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->program, info_log_len, NULL, info_log);
		
		std::cout << "Failed linking shaders:" << std::endl;
		std::cout << info_log << std::endl;
		
		return false;
	} else {
		for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i) {
			if (this->stage_of_type[i] == sgl::shader::STAGE_COMPILED) {
				this->stage_of_type[i] = sgl::shader::STAGE_LINKED;
			}
		}
	}
	
	/* free resources */
	for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i) {
		if (this->stage_of_type[i] == sgl::shader::STAGE_LINKED) {
			glDeleteShader(this->shaders[i]);
			delete[] this->shaders_src[i];
		}
	}

	return true;
}

sgl::shader::operator GLuint() const
{
	return this->program;
}

sgl::shader_uniform sgl::shader::operator[](const char *uniform_name)
{
	return sgl::shader_uniform(this->program, this->uniform_location(uniform_name));
}

sgl::shader_uniform sgl::shader::operator[](GLint location)
{
	return sgl::shader_uniform(this->program, location);
}

sgl::shader_uniform sgl::shader::uniform(const char *uniform_name)
{
	return sgl::shader_uniform(this->program, this->uniform_location(uniform_name));
}

sgl::shader_uniform sgl::shader::uniform(GLint location)
{
	return sgl::shader_uniform(this->program, location);
}

GLint sgl::shader::uniform_location(std::string uniform_name) const
{
	return this->uniform_location(uniform_name.c_str());
}

GLint sgl::shader::uniform_location(const char *uniform_name) const
{
	return glGetUniformLocation(this->program, uniform_name);
}

