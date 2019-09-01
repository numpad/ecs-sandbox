#pragma once

#include <vector>
#include <util/sgl_shader.hpp>
#include <Assets/Mesh.hpp>

class Model {
public:
	
	Model() = default;
	Model(const Model &copy) = delete;
	~Model() = default;
	
	void addMesh(Mesh *m);

	void draw(sgl::shader &shader);
	
private:
	std::vector<Mesh *> meshes;
	
};
