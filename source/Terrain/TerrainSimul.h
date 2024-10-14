#pragma once
#include <glm/glm.hpp>
#include<vector>
#include"../Camera.h"
#include"../model.h"

namespace ntn
{
	struct TerrainParameters
	{
		int octaves;
		float frequency;
		float dispFactor;
		float grassCoverage;
		float tessMultiplier;
		bool drawFog;
		float fogFalloff;
		float power;
		glm::vec3 rockColor;

		TerrainParameters() : octaves(13),
			frequency(0.015f),
			dispFactor(20.0f),
			grassCoverage(0.55f),
			tessMultiplier(1.0f),
			drawFog(true),
			fogFalloff(1.5f),
			power(3.0f),
			rockColor(glm::vec4(120, 105, 75, 255) * 1.5f / 255.f)
		{}
	};


	enum tPosition {
		C, N, S, E, W, SE, SW, NE, NW, totTiles
	};

	class TerrainSimul
	{
	public:

		TerrainSimul(int gl = 100);
		~TerrainSimul() {};

		// Function to initialize per-patch terrain mesh
		std::unique_ptr<Mesh> InitializePatchTerrainMesh();

		std::vector<Texture> LoadAllTerrainTextures(std::string path_terrain_textures);
		Texture LoadTerrainTextures(std::string name_texture, std::string pathFile_texture);

		void Render(Shader shader_terrain2, const std::unique_ptr<Camera>& camera);
		void SetGui();

		void GenerateTilesGrid(glm::vec2 offset);
		void updateTilesPositions();
		void SetTilePositionsBuffer(std::vector<glm::vec2>& pos);

		glm::vec2 position, eps;
		float up = 0.0;

		//	bool inTile(Camera camera, glm::vec2 pos);


		//	Water * waterPtr;

		inline void setOctaves(int oct) { if (oct > 0) { m_terrainParams.octaves = oct; } }

		inline void setFreq(float freq) { if (freq > 0.0f) { m_terrainParams.frequency = freq; } }

		inline void setDispFactor(float disp) { if (disp > 0.0f) { m_terrainParams.dispFactor = disp; } }

		void setScale(float scale);
		inline float getScale() const { return scaleFactor; }

		inline int getOctaves() const { return m_terrainParams.octaves; }
		inline float getFreq() const { return m_terrainParams.frequency; }
		inline float getDispFactor() const { return m_terrainParams.dispFactor; }
		inline void setGrassCoverage(float gc) { m_terrainParams.grassCoverage = gc; }
		inline float getGrassCoverage() const { return m_terrainParams.grassCoverage; }
		inline void setTessMultiplier(float tm) { if (tm > 0.0) m_terrainParams.tessMultiplier = tm; }
		inline float getTessMultiplier() const { return m_terrainParams.tessMultiplier; }

		void deleteBuffer();

		void drawVertices(int nInstances);

		inline void setPos(int row, int col, glm::vec2 pos) { listTilePositions[col + row * gridLength] = pos; }

		inline glm::vec2 getPos(int row, int col) { return listTilePositions[col + row * gridLength]; }


		bool getWhichTileCameraIs(glm::vec2& result);

		void getColRow(int i, int& col, int& row);

		void reset();

	private:

		TerrainParameters m_terrainParams;
		std::unique_ptr<Mesh> m_patchTerrainMesh = nullptr;
		std::vector<glm::vec2> listTilePositions;

		int cellsCount; //nbr of cells per patch
		float cellWidth;

		int gridLength;

		float scaleFactor;

		glm::vec3 seed;

		unsigned int posBuffer = 0;

		glm::mat4 modelMatrix;



	};

}