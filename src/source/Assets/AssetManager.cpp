#include <Assets/AssetManager.hpp>

AssetManager::~AssetManager() {
	
	int freedTextures = 0;
	for (auto iter : textures) {
		++freedTextures;
		delete iter.second;
		iter.second = nullptr;
	}
	
	int freedModels = 0;
	for (auto iter : models) {
		++freedModels;
		delete iter.second;
		iter.second = nullptr;
	}
	
	int freedMeshes = 0;
	for (auto m : meshes) {
		++freedMeshes;
		delete m;
		m = nullptr;
	}
	
	#if CFG_DEBUG
		printf("[LOG] AssetManager: freed %d textures.\n", freedTextures);
		printf("[LOG] AssetManager: freed %d models.\n", freedModels);
		printf("[LOG] AssetManager: freed %d meshes.\n", freedMeshes);
	#endif
}

// texture loading

bool AssetManager::loadTexture(std::string path) {
	auto texture = textures.find(path);
	
	// texture already loaded
	// todo: reload from path
	if (texture != textures.end()) {
		#if CFG_DEBUG
			printf("[LOG] AssetManager: texture already loaded... skipping '%s'\n", (path).c_str());
		#endif
		return false;
	}
	
	Texture *tex = new Texture();
	bool is_loaded = tex->loadImage(path);
	
	if (is_loaded) {
		#if CFG_DEBUG
			printf("[LOG] AssetManager: loaded texture '%s'\n", (path).c_str());
		#endif
			
		textures.insert({path, tex});
		return true;
	} else {
		#if CFG_DEBUG
			printf("[ERR] AssetManager: failed loading texture '%s'\n", (path).c_str());
		#endif
	}
	
	return false;
}

Texture *AssetManager::getTexture(std::string path) {
	auto texture = textures.find(path);
	if (texture == textures.end()) {
		bool is_loaded = loadTexture(path);
		if (!is_loaded) return nullptr;
	}
	
	return textures.at(path);
}

// model loading

static glm::vec2 vertexToVec2(aiVector3D vertex) {
	return glm::vec2(vertex.x, vertex.y);
}

static glm::vec3 vertexToVec3(aiVector3D vertex) {
	return glm::vec3(vertex.x, vertex.y, vertex.z);
}

std::vector<Texture *> AssetManager::convertAndCollectMaterialTextures(
	aiMaterial *mat, aiTextureType type, Texture::UsageType usage)
{
	
	std::vector<Texture *> textures;
	for (size_t i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString pathAssimp;
		mat->GetTexture(type, i, &pathAssimp);
		std::string path = std::string(pathAssimp.C_Str() + 8);
		for (char &c : path)
			if (c == '\\') c = '/';
		
		Texture *texture = this->getTexture("res/" + path);
		texture->setUsageType(usage);
		texture->setBorderColor(0.3f, 1.0f, 0.3f);
		texture->setWrapMode(Texture::WrapMode::BORDER);
		textures.push_back(texture);
	}
	
	return textures;
}

Mesh *AssetManager::convertAndCollectMesh(aiMesh *mesh, const aiScene *scene) {
	
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture *> textures;
	
	// vertices
	for (size_t i = 0; i < mesh->mNumVertices; ++i) {
		Vertex v;
		v.position = vertexToVec3(mesh->mVertices[i]);
		v.normal = vertexToVec3(mesh->mNormals[i]);
		// check if contains uv coords at all
		if (mesh->mTextureCoords[0]) {
			v.texcoords = vertexToVec2(mesh->mTextureCoords[0][i]);
		} else {
			v.texcoords = glm::vec2(0.0f);
		}
		vertices.push_back(v);
	}
	
	// indices
	for (size_t i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
	
	// textures
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse
		std::vector<Texture *> diffuseTextures =
			convertAndCollectMaterialTextures(material, aiTextureType_DIFFUSE,
				Texture::UsageType::DIFFUSE);
		
		textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
		// specular
		std::vector<Texture *> specularTextures =
			convertAndCollectMaterialTextures(material, aiTextureType_SPECULAR,
				Texture::UsageType::SPECULAR);
		
		textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
		// normal
		std::vector<Texture *> normalTextures =
			convertAndCollectMaterialTextures(material, aiTextureType_NORMALS,
				Texture::UsageType::NORMAL);
		
		textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());
		
	}
	
	
	return this->collectMesh(new Mesh(vertices, indices, textures));
}

void AssetManager::collectMeshes(Model &model, aiNode *node, const aiScene *scene) {
	// process & convert meshes
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		model.addMesh(convertAndCollectMesh(mesh, scene));
	}
	// and same for its children
	for (size_t i = 0; i < node->mNumChildren; ++i) {
		collectMeshes(model, node->mChildren[i], scene);
	}
	
}

Mesh *AssetManager::collectMesh(Mesh *m) {
	meshes.push_back(m);
	return m;
}

bool AssetManager::loadModel(std::string path) {
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path,
		aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_RemoveRedundantMaterials
		);
	
	// load assimp model representation
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("[ERR] AssetManager: %s\n", import.GetErrorString());
		return false;
	}
	
	Model *model = new Model();
	collectMeshes(*model, scene->mRootNode, scene);
	models.insert({path, model});
	return true;
}

Model *AssetManager::getModel(std::string path) {
	auto model = models.find(path);
	if (model == models.end()) {
		bool is_loaded = loadModel(path);
		if (!is_loaded) {
			#if CFG_DEBUG
				printf("[ERR] AssetManager: getModel() failed loading '%s'\n",
					path.c_str());
			#endif
			return nullptr;
		}
	}
	
	return models.at(path);
}
