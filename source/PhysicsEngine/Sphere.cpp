#include "Sphere.h"
#include "RigidBody.h"
namespace ntn
{

    Sphere::Sphere(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 color, bool twoD)
    {
        m_shapeID = SPHERE;
        m_radius = radius;
        m_color = color;
        m_rigidbody = new RigidBody(position, velocity, glm::vec3(0.0f, 0.0f, 0.01f), mass);
        m_2D = twoD;
    }

    Sphere::Sphere(glm::vec3 position, float angle, float speed, float mass, float radius, glm::vec4 color, bool twoD)
    {
        m_shapeID = SPHERE;
        m_radius = radius;
        m_color = color;
        m_rigidbody = new RigidBody(position, angle, speed, glm::vec3(0.0f, 0.0f, 0.01f), mass);
        m_2D = twoD;
    }


    void Sphere::UpdatePhysics(glm::vec3 gravity, float timeStep)
    {
        m_rigidbody->UpdatePhysics(gravity, timeStep);
    }

    /////////////////////////////////////////////////////////////////

    SphereModel::SphereModel(const std::string& pathToModel, const glm::vec3& position,
        glm::vec3 velocity, float mass,
        const glm::vec3& scale, float radius) :Sphere(position, velocity, mass, radius)
    {
        LoadModel(pathToModel);
        ComputeBoundingBox();
        auto bbox_radius = m_bbox.GetBoundingBoxRadius() / 2.0f;
        SetRadius(bbox_radius);
    }

    void SphereModel::LoadModel(const std::string& pathToModel)
    {
        m_model = std::make_unique<Model>(pathToModel);
    }
    void SphereModel::Render(Shader& shader)
    {
        if (!m_model)
            return;
        // Update the model matrix based on the position
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        //Apply transtion 
        glm::vec3 pos = GetPosition();
        modelMatrix = glm::translate(modelMatrix, pos);

        // Apply rotation
        glm::vec3 rot = GetRotation();
        modelMatrix = glm::rotate(modelMatrix, rot.z, glm::vec3(0, 0, 1));
        modelMatrix = glm::rotate(modelMatrix, rot.y, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, rot.x, glm::vec3(1, 0, 0));

        // Apply scaling 
        modelMatrix = glm::scale(modelMatrix, m_scale);

        shader.setMat4("model", modelMatrix);
        m_model->Render(shader);
    }

    void SphereModel::UpdatePhysics(glm::vec3 gravity, float timeStep)
    {
        m_rigidbody->UpdatePhysics(gravity, timeStep);
        auto velocity = GetVelocity();
        UpdateBoundingBox(velocity * timeStep);
    }

    void SphereModel::ComputeBoundingBox()
    {
        if (m_model->meshes.empty())
        {
            return;
        }
        m_bbox.Reset();
        // Initialize temporary bounds with the position of the first vertex
        glm::vec3 pos = GetPosition();
        glm::vec3 minBound_temp = m_model->meshes[0].vertices[0].Position + pos;
        glm::vec3 maxBound_temp = minBound_temp;

        // Iterate over each mesh within the model
        for (const Mesh& mesh : m_model->meshes)
        {
            // Iterate over each vertex within the mesh
            for (const Vertex& vertex : mesh.vertices)
            {
                // Apply the model's position to the vertex
                glm::vec3 vertexPosition = vertex.Position + pos;

                // Update the temporary bounds more efficiently
                minBound_temp = glm::min(minBound_temp, vertexPosition);
                maxBound_temp = glm::max(maxBound_temp, vertexPosition);
            }
        }

        // Update the model's bounding box with scale
        minBound_temp *= m_scale;
        maxBound_temp *= m_scale;

        m_bbox.setMinBound(minBound_temp);
        m_bbox.setMaxBound(maxBound_temp);
    }

    void SphereModel::UpdateBoundingBox(glm::vec3 deltaPos)
    {
        m_bbox.Move(deltaPos);
    }

    void SphereModel::SetPosition(const glm::vec3& newPosition)
    {
        glm::vec3 pos = GetPosition();
        glm::vec3 deltaPos = newPosition - pos;
        Sphere::SetPosition(newPosition);
        UpdateBoundingBox(deltaPos);
    }

    void SphereModel::Translation(const glm::vec3&& deltaPos)
    {
        glm::vec3 newPos = GetPosition() + deltaPos;
        Sphere::SetPosition(newPos);
        UpdateBoundingBox(deltaPos);
    }

    std::string SphereModel::GetInfo()
    {
        auto position = GetPosition();
        auto rotation = GetRotation();
        std::stringstream ss;
        ss << "SphereModel Position: " << std::to_string(position.x) << " " <<
            std::to_string(position.y) << " " <<
            std::to_string(position.z) << "\n ";

        ss << "Rotation: " << std::to_string(rotation.x) << " " <<
            std::to_string(rotation.y) << " " <<
            std::to_string(rotation.z);
        return ss.str();
    }
}
