#include <RenderObject/Billboard.hpp>

Billboard::Billboard(glm::vec2 size)
	: size(size)
{
	setupBillboard();
}

Billboard::~Billboard() {
	destroyBillboard();
}

static float bbCylinder(glm::vec3 cam, glm::vec3 pos, glm::vec3 &outRotAxis) {
	
	glm::vec3 lookAt, objToCamProj, upAux;
	float angleCosine;

	//glPushMatrix();

	// objToCamProj is the vector in world coordinates from the 
	// local origin to the camera projected in the XZ plane
	objToCamProj.x = cam.x - pos.x ;
	objToCamProj.y = 0;
	objToCamProj.z = cam.z - pos.z ;

	// This is the original lookAt vector for the object 
	// in world coordinates
	lookAt.x = 0;
	lookAt.y = 0;
	lookAt.z = 1;


	// normalize both vectors to get the cosine directly afterwards
	objToCamProj = glm::normalize(objToCamProj);

	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the 
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.

	upAux = glm::cross(lookAt, objToCamProj);

	// compute the angle
	angleCosine = glm::dot(lookAt,objToCamProj);

	// perform the rotation. The if statement is used for stability reasons
	// if the lookAt and objToCamProj vectors are too close together then 
	// |angleCosine| could be bigger than 1 due to lack of precision
	//if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
	  //glRotatef(acos(angleCosine)*180/3.14,upAux[0], upAux[1], upAux[2]);	
	
	outRotAxis = upAux;
	return glm::acos(angleCosine);
}

void Billboard::draw(glm::mat4 &uView, glm::mat4 &uProjection, glm::vec3 pos) {
	glm::vec4 campos4 = glm::inverse(uView)[3];
	glm::vec3 campos = glm::vec3(campos4);
	printf("%g, %g, %g\n", campos.x, campos.y, campos.z);
	bbShader.use();
	bbShader["uProjection"] = uProjection;
	bbShader["uView"] = uView;
	
	glm::mat4 uModel = glm::mat4(1.0f);
	//float angle = glm::dot(glm::normalize(campos - pos), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec3 rotAxis;
	float angle = bbCylinder(campos, pos, rotAxis);
	uModel = glm::rotate(uModel, angle, rotAxis);
	uModel = glm::translate(uModel, glm::vec3(0.0f, size.y * 0.5f, 0.0f));
	uModel = glm::scale(uModel, glm::vec3(size.x, size.y, 0.0f));
	bbShader["uModel"] = uModel;
	
	glBindVertexArray(bbVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

/////////////
// PRIVATE //
/////////////

void Billboard::setupBillboard() {
	bbShader.load("res/glsl/2d/billboard_vert.glsl", sgl::shader::VERTEX);
	bbShader.load("res/glsl/2d/billboard_frag.glsl", sgl::shader::FRAGMENT);
	bbShader.compile();
	bbShader.link();
	
	
	glGenVertexArrays(1, &bbVAO);
	glGenBuffers(1, &bbVBO);
	glGenBuffers(1, &bbEBO);
	
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f	
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 1, 3
	};
	
	glBindVertexArray(bbVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bbVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bbEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Billboard::destroyBillboard() {
	glDeleteVertexArrays(1, &bbVAO);
	glDeleteBuffers(1, &bbVBO);
	glDeleteBuffers(1, &bbEBO);
}