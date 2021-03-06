#include <Assets/AssetManager.hpp>

AssetManager::~AssetManager() {
	
	int freedTextures = 0;
	for (auto iter : textures) {
		++freedTextures;
		iter.second->destroy();
		delete iter.second;
		iter.second = nullptr;
	}
	
	int freedTiledTextures = 0;
	for (auto iter : tiledtextures) {
		++freedTiledTextures;
		// no `iter.second->destroy();` since we destroyed it already as Texture*
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

	int freedAudios = 0;
	for (auto iter : audios) {
		++freedAudios;
		delete iter.second;
		iter.second = nullptr;
	}
	
	#if CFG_DEBUG
		printf("[LOG] AssetManager: freed %d textures.\n", freedTextures);
		printf("[LOG] AssetManager: freed %d tiledtextures.\n", freedTiledTextures);
		printf("[LOG] AssetManager: freed %d models.\n", freedModels);
		printf("[LOG] AssetManager: freed %d meshes.\n", freedMeshes);
		printf("[LOG] AssetManager: freed %d audios.\n", freedAudios);
	#endif
}

// texture loading

bool AssetManager::preloadTexture(std::string path, Texture::Flags flags,
		Texture::WrapMode wrap_s, Texture::WrapMode wrap_t)
{
	bool loaded = this->loadTexture(path, flags);
	if (!loaded) return false;
	
	getTexture(path)->setWrapMode(wrap_s, wrap_t);
	
	return true;
}

bool AssetManager::loadTexture(std::string path, Texture::Flags flags) {
	auto texture = textures.find(path);
	
	// texture already loaded
	// todo: reload from path
	// todo: check if this texture is already loaded with the same flags,
	//       if not: create new Texture*
	if (texture != textures.end()) {
		#if CFG_DEBUG
			printf("[LOG] AssetManager: texture already loaded... skipping '%s'\n", (path).c_str());
		#endif
		return false;
	}
	
	Texture *tex = new Texture(flags);
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
		bool is_loaded = loadTexture(path, Texture::Flags::NONE);
		if (!is_loaded) {
			#if CFG_DEBUG
				printf("[WARN] AssetManager: getTexture(path) could not load:\n  path = \"%s\"!\n", path.c_str());
			#endif
			return nullptr;
		}
	}
	
	return textures.at(path);
}

TiledTexture *AssetManager::getTiledTexture(std::string path,
		unsigned int tiles_w, unsigned int tiles_h, int tile_x, int tile_y)
{
	auto tile = tiledtextures.find(std::make_tuple(path, tiles_w, tiles_h, tile_x, tile_y));
	if (tile == tiledtextures.end()) {
		bool is_loaded = loadTiledTexture(path, tiles_w, tiles_h, tile_x, tile_y, Texture::Flags::NONE);
		if (!is_loaded) {
			#if CFG_DEBUG
				printf("[WARN] AssetManager: getTiledTexture(path) could not load:\n  path = \"%s\"!\n", path.c_str());
			#endif
			return nullptr;
		}
	}
	
	return tiledtextures.at(std::make_tuple(path, tiles_w, tiles_h, tile_x, tile_y));
}

bool AssetManager::loadTiledTexture(std::string path,
		unsigned int tiles_w, unsigned int tiles_h, int tile_x, int tile_y,
		Texture::Flags flags)
{
	Texture *texture = getTexture(path);
	if (texture == nullptr) {
		#if CFG_DEBUG
			printf("[WARN] AssetManager: loadTiledTexture(path) failed\n");
		#endif
		return false;
	}
	
	TiledTexture *tt = new TiledTexture(tiles_w, tiles_h, flags);
	if (!tt->loadTexture(*texture)) {
		#if CFG_DEBUG
			printf("[WARN] AssetManager: loadTiledTexture(path) TiledTexture::loadTexture(const Texture &copy) failed!\n");
		#endif
		return false;
	}
	tt->setTile(tile_x, tile_y);
	
	tiledtextures.insert({std::make_tuple(path, tiles_w, tiles_h, tile_x, tile_y), tt});
	return true;
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

// audio loading

bool AssetManager::loadAudio(std::string path) {
	// TODO: check for filetype and decide which loader to use
	return loadAudioWAV(path);
}

sgl::audio *AssetManager::getAudio(std::string path) {
	auto audio = audios.find(path);
	if (audio == audios.end()) {
		bool is_loaded = loadAudio(path);
		if (!is_loaded) {
			#if CFG_DEBUG
				printf("[WARN] AssetManager: getAudio(path) could not load:\n  path = \"%s\"!\n", path.c_str());
			#endif
			return nullptr;
		}
	}
	
	return audios.at(path);
}

bool AssetManager::loadAudioWAV(std::string path) {
	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 totalPCMFrameCount;
	short *pSampleData = drwav_open_file_and_read_pcm_frames_s16(path.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);
	if (pSampleData == nullptr) {
		std::cout << "[ERR] DRWAV: Could not read file '" << path << "'." << std::endl;
		return false;
	}
	
	// TODO: add support for 8 bit audio files.
	sgl::audio::format audio_fmt;
	switch (channels) {
		case 1:
			audio_fmt = sgl::audio::format::mono16;
			break;
		case 2:
			audio_fmt = sgl::audio::format::stereo16;
			break;
	};

	sgl::audio *audio = new sgl::audio;
	audio->load(audio_fmt, sampleRate, totalPCMFrameCount * channels * sizeof(short), pSampleData);
	drwav_free(pSampleData, nullptr);

	audios[path] = audio;
	return true;
}
