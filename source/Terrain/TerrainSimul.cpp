#include "TerrainSimul.h"
#include <glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include"../utils.h"

namespace ntn
{
	TerrainSimul::TerrainSimul(int gl)
	{
		seed = genRandomVec3();

		m_patchTerrainMesh = std::move(InitializePatchTerrainMesh());

		gridLength = gl + (gl + 1); //ensure gridLength is odd
		GenerateTilesGrid(glm::vec2(0.0, 0.0));
		SetTilePositionsBuffer(listTilePositions);

	}

	std::unique_ptr<Mesh> TerrainSimul::InitializePatchTerrainMesh()
	{
		//Vertices
		std::vector<Vertex> vertices;

		cellsCount = 4;
		cellWidth = 10. * 100.0f;
		for (int i = 0; i < cellsCount; i++)
		{
			for (int j = 0; j < cellsCount; j++)
			{
				Vertex vertex;
				//add position
				float x = j * cellWidth / (cellsCount - 1) - cellWidth / 2.0;
				float y = 0.0;
				float z = -i * cellWidth / (cellsCount - 1) + cellWidth / 2.0;

				vertex.Position = glm::vec3(x, y, z);

				//add normal
				float x_n = 0.0;
				float y_n = 1.0;
				float z_n = 0.0;
				vertex.Normal = glm::vec3(x_n, y_n, z_n);

				//add texcoords
				vertex.TexCoords = glm::vec2((float)j / (cellsCount - 1), (float)(cellsCount - i - 1) / (cellsCount - 1));

				// vertex a_Position
			//	vertex.a_Postion = glm::vec2(positionVec[i + j * res]);

				vertices.push_back(vertex);
			}
		}

		//Indices
		const int totalTriangles = (cellsCount - 1) * (cellsCount - 1) * 2;
		std::vector<unsigned int> indices(totalTriangles * 3, 0);

		for (int i = 0; i < totalTriangles; i++)
		{
			int trianglesPerRow = 2 * (cellsCount - 1);
			for (int j = 0; j < trianglesPerRow; j++)
			{
				if (!(i % 2)) //upper triangle
				{
					int k = i * 3;
					int triangleIndex = i % trianglesPerRow;

					int row = i / trianglesPerRow;
					int col = triangleIndex / 2;
					indices[k] = row * cellsCount + col;
					indices[k + 1] = (row + 1) * cellsCount + col;
					indices[k + 2] = row * cellsCount + (col + 1);
				}
				else
				{
					int k = i * 3;
					int triangleIndex = i % trianglesPerRow;

					int row = i / trianglesPerRow;
					int col = triangleIndex / 2;
					indices[k] = row * cellsCount + (col + 1);
					indices[k + 1] = (row + 1) * cellsCount + col;
					indices[k + 2] = (row + 1) * cellsCount + col + 1;
				}
			}
		}
		//Textures
		static std::string path_terrain_textures = "source/resources/terrain/";
		std::vector<Texture> textures_terrain = LoadAllTerrainTextures(path_terrain_textures);

		return std::make_unique<Mesh>(vertices, indices, textures_terrain);
	}

	std::vector<Texture> TerrainSimul::LoadAllTerrainTextures(std::string path_terrain_textures)
	{
		std::vector<Texture> textures_terrain;
		Texture sand = LoadTerrainTextures("sand", path_terrain_textures + "sand.jpg");
		Texture grass = LoadTerrainTextures("grass", path_terrain_textures + "grass.jpg");
		Texture rdiffuse = LoadTerrainTextures("rock", path_terrain_textures + "rdiffuse.jpg");
		Texture snow = LoadTerrainTextures("snow", path_terrain_textures + "snow.jpg");
		Texture rnormal = LoadTerrainTextures("rockNormal", path_terrain_textures + "rnormal.jpg");
		Texture terrain = LoadTerrainTextures("grass1", path_terrain_textures + "terrainTexture.jpg");

		textures_terrain.push_back(sand);
		textures_terrain.push_back(grass);
		textures_terrain.push_back(rdiffuse);
		textures_terrain.push_back(snow);
		textures_terrain.push_back(rnormal);
		textures_terrain.push_back(terrain);
		return textures_terrain;
	}
	Texture TerrainSimul::LoadTerrainTextures(std::string name_texture, std::string pathFile_texture)
	{
		Texture texture_loaded;
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(pathFile_texture.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
			{
				format = GL_RED;
			}
			else if (nrComponents == 3)
			{
				format = GL_RGB;
			}
			else if (nrComponents == 4)
				format = GL_RGBA;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << pathFile_texture << std::endl;
			stbi_image_free(data);
		}

		texture_loaded.id = textureID;
		texture_loaded.type = name_texture;
		texture_loaded.path = pathFile_texture.c_str();

		return texture_loaded;
	}


	void TerrainSimul::GenerateTilesGrid(glm::vec2 offset)
	{
		listTilePositions.resize(gridLength * gridLength);

		glm::vec2 cellWidth_x = glm::vec2(1, 0) * cellWidth;
		glm::vec2 cellWidth_y = glm::vec2(0, 1) * cellWidth;

		for (int indexRow = 0; indexRow < gridLength; indexRow++)
		{
			for (int indexCol = 0; indexCol < gridLength; indexCol++)
			{
				glm::vec2 pos = (float)(indexCol - gridLength / 2) * glm::vec2(cellWidth_x) + (float)(indexRow - gridLength / 2) * glm::vec2(cellWidth_y);
				setPos(indexRow, indexCol, pos + offset);
			}
		}
	}

	void TerrainSimul::SetTilePositionsBuffer(std::vector<glm::vec2>& pos)
	{
		if (posBuffer)
		{
			this->deleteBuffer();
		}

		// vertex Buffer Object
		glGenBuffers(1, &posBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
		glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec2), &pos[0], GL_STATIC_DRAW);

		glBindVertexArray(m_patchTerrainMesh->VAO);
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

		glVertexAttribDivisor(7, 1);
		glBindVertexArray(0);
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "setPositionsArray: OpenGL error: " << err << std::endl;
		}
	}

	void TerrainSimul::deleteBuffer() {
		glDeleteBuffers(1, &posBuffer);
		posBuffer = 0;
	}
	void TerrainSimul::Render(Shader shader, const std::unique_ptr<Camera>& camera)
	{
		glEnable(GL_CLIP_DISTANCE0);
		shader.activate();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 projection = camera->getProjectionMatrix();

		shader.setVec3("camPos", camera->getPosition());
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setFloat("gDispFactor", m_terrainParams.dispFactor);

		//	float waterHeight = (waterPtr ? waterPtr.getModelMatrix()[3][1] : 100.0);
		float waterHeight = 20.0f;
		glm::vec4 clipPlane(0.0, 1.0, 0.0, -waterHeight);
		shader.setVec4("clipPlane", clipPlane * up);
		glm::vec3 lightColor(255, 255, 230);
		lightColor /= 255.0;
		shader.setVec3("u_LightColor", lightColor);


		glm::vec3 lightPos(-0.408, 0.408, 0.816);
		shader.setVec3("u_LightPosition", lightPos);
		shader.setVec3("u_ViewPosition", camera->getPosition());
		glm::vec3 fogColor(0.5, 0.6, 0.7);
		shader.setVec3("fogColor", fogColor);
		//	m_terrainParams.rockColor = glm::vec4(120, 105, 75, 255) * 1.5f / 255.f;
		shader.setVec3("rockColor", m_terrainParams.rockColor);
		shader.setVec3("seed", seed);

		shader.setInt("octaves", m_terrainParams.octaves);
		shader.setFloat("freq", m_terrainParams.frequency);
		shader.setFloat("u_grassCoverage", m_terrainParams.grassCoverage);
		shader.setFloat("waterHeight", waterHeight);
		shader.setFloat("tessMultiplier", m_terrainParams.tessMultiplier);
		shader.setFloat("fogFalloff", m_terrainParams.fogFalloff * 1.e-6);
		shader.setFloat("power", m_terrainParams.power);

		shader.setBool("normals", true);
		shader.setBool("drawFog", m_terrainParams.drawFog);


		int nbrInstances = listTilePositions.size();
		m_patchTerrainMesh->RenderTerrain(shader, cellsCount, nbrInstances);
		glDisable(GL_CLIP_DISTANCE0);
	}
	void TerrainSimul::SetGui()
	{
		ImGui::Begin("Terrain controls: ");
		ImGui::SliderInt("Octaves", &m_terrainParams.octaves, 1, 20);
		ImGui::SliderFloat("Frequency", &m_terrainParams.frequency, 0.0f, 0.05f);
		ImGui::SliderFloat("Displacement factor", &m_terrainParams.dispFactor, 0.0f, std::pow(32.f * 32.f * 32.f, 1 / m_terrainParams.power));
		ImGui::SliderFloat("Grass coverage", &m_terrainParams.grassCoverage, 0.0f, 1.f);
		ImGui::SliderFloat("Tessellation multiplier", &m_terrainParams.tessMultiplier, 0.1f, 5.f);
		ImGui::SliderFloat("Fog fall-off", &m_terrainParams.fogFalloff, 0.0f, 10.);
		ImGui::SliderFloat("Power", &m_terrainParams.power, 0.0f, 10.);
		ImGui::ColorEdit3("Rock color", (float*)&m_terrainParams.rockColor[0]); // Edit 3 floats representing a color
		ImGui::End();
	}
	bool TerrainSimul::getWhichTileCameraIs(glm::vec2& result) {

		//for (glm::vec2 p : positionVec) {
		//	if (inTile(*(scene->cam), p)) {
		//		//std::cout << "You're in Tile: " << p.x << ", " << p.y << std::endl;
		//		result = p;
		//		return true;
		//	}
		//}
		return false;
	}

	void TerrainSimul::getColRow(int i, int& col, int& row)
	{
		col = (i) % gridLength;

		row = (i - col) / gridLength;
	}

	/*
	void TerrainSimul::initializePlaneVAO(const int res, const int width, GLuint* planeVAO, GLuint* planeVBO, GLuint* planeEBO)
	{
		const int nPoints = res * res;
		const int size = nPoints * 3 + nPoints * 3 + nPoints * 2;
		float* vertices = new float[size];
		for (int i = 0; i < res; i++) {
			for (int j = 0; j < res; j++) {
				//add position
				float x = j * (float)width / (res - 1) - width / 2.0;
				float y = 0.0;
				float z = -i * (float)width / (res - 1) + width / 2.0;

				vertices[(i + j * res) * 8] = x; //8 = 3 + 3 + 2, float per point
				vertices[(i + j * res) * 8 + 1] = y;
				vertices[(i + j * res) * 8 + 2] = z;

				//add normal
				float x_n = 0.0;
				float y_n = 1.0;
				float z_n = 0.0;

				vertices[(i + j * res) * 8 + 3] = x_n;
				vertices[(i + j * res) * 8 + 4] = y_n;
				vertices[(i + j * res) * 8 + 5] = z_n;

				//add texcoords
				vertices[(i + j * res) * 8 + 6] = (float)j / (res - 1);
				vertices[(i + j * res) * 8 + 7] = (float)(res - i - 1) / (res - 1);
			}
		}

		const int nTris = (res - 1) * (res - 1) * 2;
		int* trisIndices = new int[nTris * 3];

		for (int i = 0; i < nTris; i++) {
			int trisPerRow = 2 * (res - 1);
			for (int j = 0; j < trisPerRow; j++) {
				if (!(i % 2)) { //upper triangle
					int k = i * 3;
					int triIndex = i % trisPerRow;

					int row = i / trisPerRow;
					int col = triIndex / 2;
					trisIndices[k] = row * res + col;
					trisIndices[k + 1] = ++row * res + col;
					trisIndices[k + 2] = --row * res + ++col;
				}
				else {
					int k = i * 3;
					int triIndex = i % trisPerRow;

					int row = i / trisPerRow;
					int col = triIndex / 2;
					trisIndices[k] = row * res + ++col;
					trisIndices[k + 1] = ++row * res + --col;
					trisIndices[k + 2] = row * res + ++col;
				}
			}
		}
		glGenVertexArrays(1, planeVAO);
		glGenBuffers(1, planeVBO);
		glGenBuffers(1, planeEBO);

		glBindVertexArray(*planeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, *planeVBO);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *planeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTris * 3 * sizeof(unsigned int), trisIndices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);

		delete[] vertices;
	}
	*/

	/*
	void TerrainSimul::draw()
	{
		sceneElements* se = drawableObject::scene;
		drawFog = !se->wireframe;

		if (up != 0.0f)
		{
			glEnable(GL_CLIP_DISTANCE0);
		}
		glm::mat4 gWorld = modelMatrix;
		glm::mat4 gVP = se->projMatrix * se->cam->GetViewMatrix();

		shad->use();
		shad->setVec3("gEyeWorldPos", se->cam->Position);
		shad->setMat4("gWorld", gWorld);
		shad->setMat4("gVP", gVP);
		shad->setFloat("gDispFactor", dispFactor);

		float waterHeight = (waterPtr ? waterPtr->getModelMatrix()[3][1] : 100.0);
		glm::vec4 clipPlane(0.0, 1.0, 0.0, -waterHeight);
		shad->setVec4("clipPlane", clipPlane*up);
		shad->setVec3("u_LightColor", se->lightColor);
		shad->setVec3("u_LightPosition", se->lightPos);
		shad->setVec3("u_ViewPosition", se->cam->Position);
		shad->setVec3("fogColor", se->fogColor);
		shad->setVec3("rockColor", rockColor);
		shad->setVec3("seed", se->seed);

		shad->setInt("octaves", octaves);
		shad->setFloat("freq", frequency);
		shad->setFloat("u_grassCoverage", grassCoverage);
		shad->setFloat("waterHeight", waterHeight);
		shad->setFloat("tessMultiplier", tessMultiplier);
		shad->setFloat("fogFalloff", fogFalloff*1.e-6);
		shad->setFloat("power", power);

		shad->setBool("normals", true);
		shad->setBool("drawFog", TerrainSimul::drawFog);


		// set textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		shad->setInt("sand", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		shad->setInt("grass", 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		shad->setInt("rock", 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		shad->setInt("snow", 4);

		shad->setSampler2D("grass1", textures[5], 5);

		shad->setSampler2D("rockNormal", textures[4], 6);

		int nIstances = positionVec.size();

		drawVertices(nIstances);


		glDisable(GL_CLIP_DISTANCE0);
		up = 0.0;
	}
	*/

	/*
	void TerrainSimul::setGui()
	{
		ImGui::Begin("TerrainSimul controls: ");
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "TerrainSimul Controls");
		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Clouds PostProc + God Rays", this->getPostProcPointer());
		ImGui::SliderInt("Octaves", &octaves, 1, 20);
		ImGui::SliderFloat("Frequency",&frequency, 0.0f, 0.05f);
		ImGui::SliderFloat("Displacement factor", &dispFactor, 0.0f, std::pow(32.f*32.f*32.f, 1/power));
		ImGui::SliderFloat("Grass coverage", &grassCoverage, 0.0f, 1.f);
		ImGui::SliderFloat("Tessellation multiplier", &tessMultiplier, 0.1f, 5.f);
		ImGui::SliderFloat("Fog fall-off", &fogFalloff, 0.0f, 10.);
		ImGui::SliderFloat("Power", &power, 0.0f, 10.);

		//glm::vec3 * cloudBottomColor = this->getCloudColorBottomPtr();
		//ImGui::ColorEdit3("Cloud color", (float*)cloudBottomColor); // Edit 3 floats representing a color

		//ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sky controls");
		ImGui::ColorEdit3("Rock color", (float*)&rockColor[0]); // Edit 3 floats representing a color
		//ImGui::ColorEdit3("Sky bottom color", (float*)this->getSkyBottomColorPtr()); // Edit 3 floats representing a color
		ImGui::End();
	}
	*/

	void TerrainSimul::drawVertices(int nInstances) {
		//glBindVertexArray(planeVAO);
		////shader.use();
		//shad->use();
		//glDrawElementsInstanced(GL_PATCHES, (res-1)*(res-1)*2*3, GL_UNSIGNED_INT, 0, nInstances);
		//glBindVertexArray(0);
	}

	void TerrainSimul::setScale(float scale)
	{
		glm::mat4 id;
		glm::mat4 scaleMatrix = glm::scale(id, glm::vec3(scale, 0.0, scale));
		glm::mat4 positionMatrix = glm::translate(id, glm::vec3(position.x * scale / this->scaleFactor, 0.0, position.y * scale / this->scaleFactor));
		modelMatrix = positionMatrix * scaleMatrix;
		scaleFactor = scale;
	}
	//
	//bool TerrainSimul::inTile(const Camera camera, glm::vec2 pos) {
	//	float camX = camera.Position.x;
	//	float camY = camera.Position.z;
	//
	//	float x = pos.x;
	//	float y = pos.y;
	//
	//	bool inX = false;
	//	if ((camX <= x + 1.0 * cellWidth/2.0f) && (camX >= x - 1.0 * cellWidth/2.0f)) { inX = true; }
	//	bool inY = false;
	//	if ((camY <= y + 1.0 * cellWidth/2.0f) && (camY >= y - 1.0 * cellWidth/2.0f)) { inY = true; }
	//
	//	bool result = inX && inY;
	//
	//	if (result) {
	//
	//		//std::cout << y << " :y" << std::endl;
	//		//std::cout << y << " :y" << std::endl;
	//
	//		//std::cout << y + scaleFactor * cellWidth / 2.0f << ": y + scalefactor" << std::endl;
	//		//std::cout << y - scaleFactor * cellWidth / 2.0f << ": y - scalefactor" << std::endl;
	//	}
	//
	//	return result;
	//
	//}


	void TerrainSimul::updateTilesPositions() {
		/*sceneElements* se = drawableObject::scene;
		glm::vec2 camPosition(se->cam->Position.x, se->cam->Position.z);
		int whichTile = -1;
		int howManyTiles = 0;

		glm::vec2 currentTile;
		if (getWhichTileCameraIs(currentTile)) {
			glm::vec2 center = getPos(gridLength / 2, gridLength / 2);
			for (glm::vec2& p : positionVec) {
				p += currentTile - center;
			}
			setPositionsArray(positionVec);

			if (waterPtr) {
				glm::vec2 center = getPos(gridLength / 2, gridLength / 2);
				waterPtr->setPosition(center, 1.0*gridLength, waterPtr->getHeight());
			}
		}*/
	}


	void TerrainSimul::reset()
	{
		int octaves = this->getOctaves();
		float freq = this->getFreq();
		float grassCoverage = this->getGrassCoverage();
		float dispFactor = this->getDispFactor();
		float tessMultiplier = this->getTessMultiplier();

		setOctaves(octaves);
		setFreq(freq);
		setGrassCoverage(grassCoverage);
		setDispFactor(dispFactor);
		setTessMultiplier(tessMultiplier);
	}
}