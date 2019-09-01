#include <Assets/Model.hpp>

void Model::addMesh(Mesh *m) {
	meshes.push_back(m);
}

void Model::draw(sgl::shader &shader) {
	// TODO: sort before drawing?
	for (auto it = meshes.begin(); it != meshes.end(); ++it) {
		Mesh *mesh = *it;
		
		mesh->draw(shader);
	}
}

