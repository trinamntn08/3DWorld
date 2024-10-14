#include "Plane.h"
#include"../stb_image.h"
#include"../resourceManager.h"

namespace ntn
{
    extern unsigned int LoadTextureFromFile(const std::string& filePath, bool gamma = true);

    Plane::Plane()
    {
        m_shapeID = PLANE;
        m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_normal = glm::vec3(0.0f, 1.0f, 0.0f);
        m_distanceToOrigin = 50.0f;
        m_position = m_normal;
        m_2D = false;
        m_elasticity = 0.7f;
    }

    Plane::Plane(glm::vec3 normal, float distance, bool twoD)
    {
        m_shapeID = PLANE;
        m_normal = normal;
        m_distanceToOrigin = distance;
        m_2D = twoD;
        m_elasticity = 0.7f;
    }


    Plane::~Plane()
    {
    }

    void Plane::UpdatePhysics(glm::vec3 gravity, float timeStep)
    {
    }


    /***************************************************************/
    /***************************************************************/

    PlaneModel::PlaneModel()
    {
        init();
        ComputeBoundingBox();
    }
    void PlaneModel::init()
    {
        std::vector<Vertex> vertices;
        glm::vec3 vertexPositions[4] =
        {
          glm::vec3(100.0f, 10.0f, 100.0f),
          glm::vec3(-100.0f, 10.0f, 100.0f),
          glm::vec3(-100.0f, 10.0f, -100.0f),
          glm::vec3(100.0f, 10.0f, -100.0f),
        };

        glm::vec3 vertexNormals[4] =
        {
          glm::vec3(0.0f, 1.0f, 0.0f),
          glm::vec3(0.0f, 1.0f, 0.0f),
          glm::vec3(0.0f, 1.0f, 0.0f),
          glm::vec3(0.0f, 1.0f, 0.0f),
        };

        glm::vec2 vertexTexCoords[4] =
        {
          glm::vec2(100.0f, 0.0f),
          glm::vec2(0.0f,  0.0f),
          glm::vec2(0.0f, 100.0f),
          glm::vec2(100.0f, 100.0f),
        };

        for (int i = 0; i < 4; i++)
        {
            Vertex vertex;
            vertex.Position = vertexPositions[i];
            vertex.Normal = vertexNormals[i];
            vertex.TexCoords = vertexTexCoords[i];
            vertices.push_back(vertex);
        }

        std::vector<unsigned int> indices =
        {
            0, 1, 2,
            0, 2, 3,
        };

        std::vector<Texture> textures_loaded;

        const std::string plane_texture(ResourceManager::getInstance().getResourcePath("wood/wood.png"));
        Texture texture;
        texture.id = LoadTextureFromFile(plane_texture);
        texture.type = "wood";
        texture.path = plane_texture.c_str();
        textures_loaded.push_back(texture);

        m_plane = new Mesh(vertices, indices, textures_loaded);
    }


    void PlaneModel::Render(Shader& shader_plane)
    {
        shader_plane.activate();
        m_plane->render(shader_plane);
    }

    void PlaneModel::ComputeBoundingBox()
    {
        m_bbox.Reset();

        glm::vec3 pos(0.0f);
        glm::vec3 minBound_temp = m_plane->vertices[0].Position + pos;
        glm::vec3 maxBound_temp = minBound_temp;

        // Iterate over each vertex within the mesh
        for (const Vertex& vertex : m_plane->vertices)
        {
            // Apply the model's position to the vertex
            glm::vec3 vertexPosition = vertex.Position + pos;

            // Update the temporary bounds more efficiently
            minBound_temp = glm::min(minBound_temp, vertexPosition);
            maxBound_temp = glm::max(maxBound_temp, vertexPosition);
        }

        m_bbox.setMinBound(minBound_temp);
        m_bbox.setMaxBound(maxBound_temp);
    }

    void PlaneModel::UpdateBoundingBox(glm::vec3 deltaPos)
    {
        m_bbox.Move(deltaPos);
    }
}
