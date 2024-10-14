#include "Terrain.h"
#include"../resourceManager.h"
#include"../stb_image.h"
#include"../Logger.h"
#include"../PhysicsEngine/RigidBody.h"
#include"../Logger.h"
#include"../Timer.h"

namespace ntn
{
unsigned int nbrPatchesTess = 20;

Terrain::Terrain(TerrainType typeTerrain,glm::vec3 scale):m_typeRealTerrain(typeTerrain), m_scale(scale)
{ 
	m_rigidbody = new RigidBody();

	if (m_typeRealTerrain == TerrainType::Raw)
	{
		InitTerrain();
	}
	else
	{
		InitTerrainTesselation();
	}
	ComputeBoundingBox();
}

Terrain::~Terrain()
{
}

void Terrain::UpdatePhysics(glm::vec3 gravity, float timeStep)
{
	m_rigidbody->UpdatePhysics(gravity, timeStep);
}


void Terrain::Render(Shader& shader)
{
	shader.activate();

	if (m_typeRealTerrain == TerrainType::Raw)
	{
		m_terrain->Render(shader);
	}
	else
	{
		m_terrain->RenderTesselation(shader); 
	}
}


void Terrain::InitTerrain()
{
	// Initialize vertices
	std::string heightMapFilePath = ResourceManager::getInstance().getResourcePath("terrain/heightmap_paris.png");
	std::vector<Vertex> vertices = InitVerticesWithHeightMapFromFile(heightMapFilePath.c_str(), m_width, m_depth);

	// Initialize indices for triangles 
	unsigned int num_indices = (m_width - 1) * (m_depth - 1) * 6;
	std::vector<unsigned int> indices(num_indices, 0);
	int Index = 0;

	for (unsigned int z = 0; z < m_depth - 1; z++)
	{
		for (unsigned int x = 0; x < m_width - 1; x++)
		{
			/*  0 ----- 1 ----- 2
				| \     | \     |
				|  \    |   \   |
				|    \  |     \ |
				3 ----- 4 ----- 5
				| \     |  \    |
				|   \   |   \   |
				|     \ |     \ |
				6 ----- 7 ----- 8 */

			unsigned int IndexBottomLeft = z * m_width + x; //0
			unsigned int IndexTopLeft = (z + 1) * m_width + x; //3
			unsigned int IndexTopRight = (z + 1) * m_width + x + 1; //4
			unsigned int IndexBottomRight = z * m_width + x + 1; //1

			// Add top left triangle
			indices[Index++] = IndexBottomLeft;
			indices[Index++] = IndexTopLeft;
			indices[Index++] = IndexTopRight;

			// Add bottom right triangle
			indices[Index++] = IndexBottomLeft;
			indices[Index++] = IndexTopRight;
			indices[Index++] = IndexBottomRight;
		}
	}

	// Update Normals for vertices
	CalculateNormals(vertices, indices);

	// textures
	std::vector<Texture> textures_terrain;
	
	Texture sand = LoadTerrainTextures("gTextureHeight0", ResourceManager::getInstance().getResourcePath("terrain/rdiffuse.jpg"));
	Texture grass = LoadTerrainTextures("gTextureHeight1", ResourceManager::getInstance().getResourcePath("terrain/sand.jpg"));
	Texture rdiffuse = LoadTerrainTextures("gTextureHeight2", ResourceManager::getInstance().getResourcePath("terrain/snow.jpg"));
	Texture snow = LoadTerrainTextures("gTextureHeight3", ResourceManager::getInstance().getResourcePath("terrain/terrainTexture.jpg"));

	textures_terrain.push_back(sand);
	textures_terrain.push_back(grass);
	textures_terrain.push_back(rdiffuse);
	textures_terrain.push_back(snow);
	
	m_terrain = std::make_unique<Mesh>(vertices, indices, textures_terrain);
}
void Terrain::InitTerrainTesselation()
{
	// Initialize vertices
	std::vector<Vertex> vertices = InitVerticesTessWithHeightMapTexture("source/resources/terrain/heightmap_paris.png", m_width, m_depth);

	// textures
	std::vector<Texture> textures_terrain;

	Texture sand = LoadTerrainTextures("gTextureHeight0", ResourceManager::getInstance().getResourcePath("terrain/rdiffuse.jpg"));
	Texture grass = LoadTerrainTextures("gTextureHeight1", ResourceManager::getInstance().getResourcePath("terrain/sand.jpg"));
	Texture rdiffuse = LoadTerrainTextures("gTextureHeight2", ResourceManager::getInstance().getResourcePath("terrain/snow.jpg"));
	Texture snow = LoadTerrainTextures("gTextureHeight3", ResourceManager::getInstance().getResourcePath("terrain/terrainTexture.jpg"));
	Texture heightMap = LoadTerrainTextures("heightMap", ResourceManager::getInstance().getResourcePath("terrain/heightmap_paris.png"));

	textures_terrain.push_back(sand);
	textures_terrain.push_back(grass);
	textures_terrain.push_back(rdiffuse);
	textures_terrain.push_back(snow);
	textures_terrain.push_back(heightMap);

	m_terrain = std::make_unique<Mesh>(vertices, textures_terrain,1);
}

Texture Terrain::LoadTerrainTextures(std::string name_texture,std::string pathFile_texture)
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

std::vector<Vertex> Terrain::InitVerticesWithHeightMapFromFile(const char* imagePath, unsigned int&width, unsigned int& height)
{
	// Initialize vertices
	std::vector<Vertex> vertices;
	int nChannels; 
	unsigned char* data = stbi_load(imagePath, (int*)(&width), (int*)&height, &nChannels, 0);

	if (!data) 
	{
		Log::error("Can not read Height Texture image");
		return {};
	}
	
	static float yScale = 128.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
	
	if (nChannels == 1 || nChannels == 3 || nChannels == 4) //RGB || RGBA
	{
		for (unsigned int i = 0; i < height; ++i)
		{
			std::vector<float> height_width;
			for (unsigned int j = 0; j < width; ++j)
			{
				// retrieve texel for (i,j) tex coord
				unsigned char* texel = data + (j + width * i) * nChannels;
				// raw height at coordinate
				unsigned char h = texel[0];

				float x = -(int)width / 2.0f + j;
				float z = -(int)height / 2.0f + i;
				float y = (int)h * yScale - yShift;
				//	float y = 1.0f;
				Vertex vertex;
				vertex.Position = glm::vec3(x, y, z);
				//    vertex.Position = glm::vec3(x, 1.0f, z);
				
				//Normals will be computed later
		
				// Add texture coordinates
				float u = (float)j / (width - 1);
				float v = 1.0 - (float)i / (height - 1);
				vertex.TexCoords = glm::vec2(u,v);

				vertices.push_back(vertex);

				m_heightMap.push_back(y);
			}
		}
	}
	stbi_image_free(data);
	return vertices;
}
std::vector<Vertex> Terrain::InitVerticesTessWithHeightMapTexture(const char* heightMapFilePath, 
																	  unsigned int& width, unsigned int& height)
{
	int pwidth, pheight, nbrComponents;
	unsigned char* data = stbi_load(heightMapFilePath, &pwidth, &pheight, &nbrComponents, 0);
	if (data)
	{
		width = pwidth;
		height = pheight;
	}
	else
	{
		Log::error("Can not read heightMap file");
	}
	std::vector<Vertex> vertices;
	
	// Height is loaded from heightmap texture
	// For now, set y=0.f 
	for (unsigned int i = 0; i <= nbrPatchesTess - 1; i++)
	{
		for (unsigned int j = 0; j <= nbrPatchesTess - 1; j++)
		{
			Vertex vertex1;
			float x1 = (-(int)width / 2.0f + (int)width * i / (float)nbrPatchesTess); // v.x
			float y1 = (0.0f); // v.y 
			float z1 = (-(int)height / 2.0f + height * j / (float)nbrPatchesTess); // v.z
			float u1 = (i / (float)nbrPatchesTess); // u
			float v1 = (j / (float)nbrPatchesTess); // v
			vertex1.Position = glm::vec3(x1, y1, z1);
			vertex1.TexCoords = glm::vec2(u1, v1);
			vertices.push_back(vertex1);

			Vertex vertex2;
			float x2 = (-(int)width / 2.0f + width * (i + 1) / (float)nbrPatchesTess); // v.x
			float y2 = (0.0f); // v.y
			float z2 = (-(int)height / 2.0f + height * j / (float)nbrPatchesTess); // v.z
			float u2 = ((i + 1) / (float)nbrPatchesTess); // u
			float v2 = (j / (float)nbrPatchesTess); // v
			vertex2.Position = glm::vec3(x2, y2, z2);
			vertex2.TexCoords = glm::vec2(u2, v2);
			vertices.push_back(vertex2);

			Vertex vertex3;
			float x3 = (-(int)width / 2.0f + width * (i ) / (float)nbrPatchesTess); // v.x
			float y3 = (0.0f); // v.y
			float z3 = (-(int)height / 2.0f + height * (j+1) / (float)nbrPatchesTess); // v.z
			float u3 = (i / (float)nbrPatchesTess); // u
			float v3 = ((j+1) / (float)nbrPatchesTess); // v
			vertex3.Position = glm::vec3(x3, y3, z3);
			vertex3.TexCoords = glm::vec2(u3, v3);
			vertices.push_back(vertex3);

			Vertex vertex4;
			float x4 = (-(int)width / 2.0f + width * (i+1) / (float)nbrPatchesTess); // v.x
			float y4 = (0.0f); // v.y
			float z4 = (-(int)height / 2.0f + height * (j + 1) / (float)nbrPatchesTess); // v.z
			float u4 = ((i+1) / (float)nbrPatchesTess); // u
			float v4 = ((j + 1) / (float)nbrPatchesTess); // v
			vertex4.Position = glm::vec3(x4, y4, z4);
			vertex4.TexCoords = glm::vec2(u4, v4);
			vertices.push_back(vertex4);
		}
	}
	std::cout << "Loaded " << nbrPatchesTess * nbrPatchesTess << " patches of 4 control points each" << std::endl;
	std::cout << "Processing " << nbrPatchesTess * nbrPatchesTess * 4 << " vertices in vertex shader" << std::endl;

	return vertices;
}

void Terrain::CalculateNormals(std::vector<Vertex>& Vertices, std::vector<unsigned int>& Indices)
{
	unsigned int Index = 0;

	// Accumulate each triangle normal into each of the triangle vertices
	for (unsigned int i = 0; i < Indices.size(); i += 3) {
		unsigned int Index0 = Indices[i];
		unsigned int Index1 = Indices[i + 1];
		unsigned int Index2 = Indices[i + 2];
		glm::vec3 v1 = Vertices[Index1].Position - Vertices[Index0].Position;
		glm::vec3 v2 = Vertices[Index2].Position - Vertices[Index0].Position;
		glm::vec3 Normal = glm::cross( v1,v2);
		Normal = glm::normalize(Normal);

		Vertices[Index0].Normal += Normal;
		Vertices[Index1].Normal += Normal;
		Vertices[Index2].Normal += Normal;
	}

	// Normalize all the vertex normals
	for (unsigned int i = 0; i < Vertices.size(); i++) 
	{
		Vertices[i].Normal = glm::normalize(Vertices[i].Normal);
	}
}
void Terrain::SetPosition(const glm::vec3& newPosition)
{
	glm::vec3 pos = GetPosition();
	glm::vec3 deltaPos = newPosition - pos;
	PhysicsObject::SetPosition(newPosition);
	UpdateBoundingBox(deltaPos);
}

void Terrain::ComputeBoundingBox()
{
	if (!m_terrain)
	{
		return;
	}
	m_bbox.Reset();
	// Initialize temporary bounds with the position of the first vertex
	glm::vec3 pos = GetPosition();
	glm::vec3 minBound_temp = m_terrain->vertices[0].Position + pos;
	glm::vec3 maxBound_temp = minBound_temp;

	// Iterate over each vertex within the mesh
	for (const Vertex& vertex : m_terrain->vertices)
	{
		// Apply the model's position to the vertex
		glm::vec3 vertexPosition = vertex.Position + pos;

		// Update the temporary bounds more efficiently
		minBound_temp = glm::min(minBound_temp, vertexPosition);
		maxBound_temp = glm::max(maxBound_temp, vertexPosition);
	}

	// Update the model's bounding box with scale
	minBound_temp *= m_scale;
	maxBound_temp *= m_scale;

	m_bbox.setMinBound(minBound_temp);
	m_bbox.setMaxBound(maxBound_temp);
}

void Terrain::UpdateBoundingBox(glm::vec3 deltaPos)
{
	m_bbox.Move(deltaPos);
}


float Terrain::GetHeightForPos(float x1, float z1)
{
	Timer timer("GetHeightForPos");
	float x = x1;
	float z = z1;

	// Apply the same translation as in the heightmap creation code
	x += ((int)m_width / 2.0f);
	z += ((int)m_depth / 2.0f);

	if (x < 0.0f || x >= m_width - 1 || z < 0.0f || z >= m_depth - 1)
	{
		return 0.0f;
	}

	// Convert (x, z) coordinates to integer indices
	int xIndex = static_cast<int>(x);
	int zIndex = static_cast<int>(z);

	// Calculate the fractional parts for interpolation
	float xFraction = x - xIndex;
	float zFraction = z - zIndex;

	// Perform bilinear interpolation
	float height00 = m_heightMap[zIndex * m_width + xIndex];
	float height01 = m_heightMap[zIndex * m_width + (xIndex + 1)];
	float height10 = m_heightMap[(zIndex + 1) * m_width + xIndex];
	float height11 = m_heightMap[(zIndex + 1) * m_width + (xIndex + 1)];

	float interpolatedHeightTop = glm::mix(height00, height01, xFraction);
	float interpolatedHeightBottom = glm::mix(height10, height11, xFraction);

	return glm::mix(interpolatedHeightTop, interpolatedHeightBottom, zFraction);

//	return GetHeightInterpolated(pos_x, pos_z);
}
float Terrain::GetHeightInterpolated(float x, float z)
{
	Timer timer("GetHeightInterpolated");
	// Ensure x and z are within valid bounds
	if (x < 0.0f || x >= m_width - 1 || z < 0.0f || z >= m_depth - 1)
	{
		return 0.0f;
	}

	// Convert (x, z) coordinates to integer indices
	int xIndex = static_cast<int>(x);
	int zIndex = static_cast<int>(z);

	// Calculate the fractional parts for interpolation
	float xFraction = x - xIndex;
	float zFraction = z - zIndex;

	// Perform bilinear interpolation
	float height00 = m_heightMap[zIndex * m_width + xIndex];
	float height01 = m_heightMap[zIndex * m_width + (xIndex + 1)];
	float height10 = m_heightMap[(zIndex + 1) * m_width + xIndex];
	float height11 = m_heightMap[(zIndex + 1) * m_width + (xIndex + 1)];

	float interpolatedHeightTop = glm::mix(height00, height01, xFraction);
	float interpolatedHeightBottom = glm::mix(height10, height11, xFraction);

	return glm::mix(interpolatedHeightTop, interpolatedHeightBottom, zFraction);
}

glm::vec3 Terrain::ConstrainCameraPosToTerrain(glm::vec3 camPos)
{
	Timer timer("ConstrainCameraPosToTerrain");
	glm::vec3 newCameraPos = camPos;

	// Make sure camera doesn't go outside of the terrain bounds
	// Constrain to the bounding box
	if (camPos.x < m_bbox.GetMinBounds().x)
	{
		newCameraPos.x = m_bbox.GetMinBounds().x + 1.0f;
	}

	if (camPos.z < m_bbox.GetMinBounds().z) 
	{
		newCameraPos.z = m_bbox.GetMinBounds().z + 1.0f;
	}

	if (camPos.x > m_bbox.GetMaxBounds().x) 
	{
		newCameraPos.x = m_bbox.GetMaxBounds().x - 1.0f;
	}

	if (camPos.z > m_bbox.GetMaxBounds().z) {
		newCameraPos.z = m_bbox.GetMaxBounds().z-1.0f;
	}

	newCameraPos.y = GetHeightForPos(newCameraPos.x , newCameraPos.z );

	// Add an offset to simulate walking height
	static float walkingHeightOffset = 10.0f;
	newCameraPos.y += walkingHeightOffset;
	Log::info("x,y,z pos:" + std::to_string(newCameraPos.x) +" " + std::to_string(newCameraPos.y) + " " + std::to_string(newCameraPos.z));

	// Apply smoothed oscillation to simulate walking motion
	//float oscillationFactor = 1.0f;
	//float t = glm::smoothstep(-1.0f, 1.0f, sinf(newCameraPos.x * oscillationFactor) + cosf(newCameraPos.z * oscillationFactor));

	//// Scale and offset the smoothstep result to control amplitude
	//float amplitude = 1/250.0f; // Adjust this value to control the amplitude of oscillation
	//float smoothedOscillation = t * amplitude;
	//// Apply the smoothed oscillation to the y-coordinate
	//newCameraPos.y += smoothedOscillation;
	return newCameraPos;
}
}
