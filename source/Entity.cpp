#include"Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/vector_query.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/projection.hpp>

namespace ntn
{
    Entity::Entity() : m_position(0.0f, 0.0f, 0.0f),
        m_rotation(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f),
        m_model(nullptr)
    {
        ComputeBoundingBox();
    }

    Entity::Entity(const std::string& pathToModel, const glm::vec3& position,
        const glm::vec3& rotation, const glm::vec3& scale,
        glm::vec3 velocity, float mass,
        glm::vec3 torque, float rotationAngle, float angularVel) :
        m_position(position), m_rotation(rotation),
        m_scale(scale), m_velocity(velocity),
        m_mass(mass), m_torque(torque),
        m_rotationAngle(rotationAngle),
        m_angularVel(angularVel)
    {
        m_model = new Model(pathToModel);
        ComputeBoundingBox();
    }

    Entity::Entity(Model* model, const glm::vec3& position,
        const glm::vec3& rotation, const glm::vec3& scale) :
        m_model(model), m_position(position),
        m_rotation(rotation), m_scale(scale)
    {
        ComputeBoundingBox();
    }

    void Entity::Render(Shader& shader)
    {
        // Update the model matrix based on the position
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        //Apply transtion 
        modelMatrix = glm::translate(modelMatrix, m_position);

        // Apply rotation
        modelMatrix = glm::rotate(modelMatrix, m_rotation.z, glm::vec3(0, 0, 1));
        modelMatrix = glm::rotate(modelMatrix, m_rotation.y, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, m_rotation.x, glm::vec3(1, 0, 0));

        //   modelMatrix = glm::rotate(modelMatrix, m_rotationAngle, m_torque);

           // Apply scaling 
        modelMatrix = glm::scale(modelMatrix, m_scale);

        shader.setMat4("model", modelMatrix);
        m_model->Render(shader);
    }
    void Entity::SetPosition(const glm::vec3& newPosition)
    {
        glm::vec3 deltaPos = newPosition - m_position;
        m_position = newPosition;
        UpdateBoundingBox(deltaPos);
    }
    void Entity::Translation(const glm::vec3&& deltaPos)
    {
        m_position += deltaPos;
        UpdateBoundingBox(deltaPos);
    }
    void Entity::Rotate(Shader& shader, float angle, const glm::vec3& rotateAxis)
    {
        // Create a rotation matrix
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, rotateAxis);
        model = rotation * model;
        shader.setMat4("model", model);
    }
    void Entity::RotateOverTime(float deltaTime, const glm::vec3& rotateAxis)
    {
        // Accumulate the elapsed time
        static float accumulatedTime = 0.0f;
        accumulatedTime += deltaTime;

        static float rotationSpeed = 30.0f;
        // Calculate the rotation angle based on time and speed
        float rotationAngle = glm::radians(rotationSpeed * accumulatedTime);
        m_rotation = rotationAngle * rotateAxis;
    }

    void Entity::OnMove(float deltaTime)
    {
        glm::vec3 newPos = m_position + m_velocity * deltaTime;
        m_rotationAngle += m_angularVel * deltaTime;
        SetPosition(newPos);
    }

    void Entity::ComputeBoundingBox()
    {
        if (m_model->meshes.empty())
        {
            return;
        }
        m_modelBounds.Reset();
        // Initialize temporary bounds with the position of the first vertex
        glm::vec3 minBound_temp = m_model->meshes[0].vertices[0].Position + m_position;
        glm::vec3 maxBound_temp = minBound_temp;

        // Iterate over each mesh within the model
        for (const Mesh& mesh : m_model->meshes)
        {
            // Iterate over each vertex within the mesh
            for (const Vertex& vertex : mesh.vertices)
            {
                // Apply the model's position to the vertex
                glm::vec3 vertexPosition = vertex.Position + m_position;

                // Update the temporary bounds more efficiently
                minBound_temp = glm::min(minBound_temp, vertexPosition);
                maxBound_temp = glm::max(maxBound_temp, vertexPosition);
            }
        }

        // Update the model's bounding box with scale
        minBound_temp *= m_scale;
        maxBound_temp *= m_scale;

        m_modelBounds.setMinBound(minBound_temp);
        m_modelBounds.setMaxBound(maxBound_temp);
    }
    void Entity::UpdateBoundingBox(glm::vec3 deltaPos)
    {
        m_modelBounds.Move(deltaPos);
    }
    const BoundingBox& Entity::GetBoundingBox() const
    {
        return m_modelBounds;
    }

    void Entity::performCollision(Entity& obj1, Entity& obj2)
    {
        glm::vec3 centersVector = (obj1.m_position - obj2.m_position);
        centersVector = glm::normalize(centersVector);

        glm::vec3 v1proj = glm::proj(obj1.m_velocity, centersVector);
        glm::vec3 v2proj = glm::proj(obj2.m_velocity, centersVector);

        float v1n = -1.0f * glm::length(v1proj);
        float v2n = glm::length(v2proj);

        float v1n_final = (v1n * (obj1.m_mass - obj2.m_mass) + 2 * (obj2.m_mass) * v2n) / (obj1.m_mass + obj2.m_mass);
        float v2n_final = (v2n * (obj2.m_mass - obj1.m_mass) + 2 * (obj1.m_mass) * v1n) / (obj1.m_mass + obj2.m_mass);

        glm::vec3 Ff1_dir = glm::normalize(-(obj1.m_velocity - v1proj));
        glm::vec3 Ff2_dir = glm::normalize(-(obj2.m_velocity - v2proj));

        obj1.m_velocity = (obj1.m_velocity - v1proj) + (v1n_final * glm::normalize(centersVector));
        obj2.m_velocity = (obj2.m_velocity - v2proj) + (v2n_final * glm::normalize(centersVector));

        //Ff = kfc * (m * (deltav)/(deltat))
        float kfc_div_deltat = 0.1f;
        float Ff_norm = obj1.m_mass * (v1n_final - v1n) * kfc_div_deltat * 2;//equals to obj2.mass * (v2n_final - v2n) * kfc_div_deltat

        glm::vec3 Ff1 = Ff1_dir * Ff_norm;
        glm::vec3 Ff2 = Ff2_dir * Ff_norm;

        glm::vec3 torque1 = glm::cross((-centersVector), Ff1);
        glm::vec3 torque2 = glm::cross((centersVector), Ff2);

        obj1.m_torque += torque1;
        obj2.m_torque += torque2;

        float alpha1 = glm::length(obj1.m_torque) / (0.4 * obj1.m_mass * (0.93) * (0.93));//I = 2/5 * m * r^2
        float alpha2 = glm::length(obj2.m_torque) / (0.4 * obj2.m_mass * (0.93) * (0.93));//I = 2/5 * m * r^2

        obj1.m_angularVel = alpha1;
        obj2.m_angularVel = alpha2;
    }
    /////////////////////////////////////////////////////////////


    PhysicsEntity::PhysicsEntity() : m_position(0.0f, 0.0f, 0.0f),
        m_rotation(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f),
        m_model(nullptr)
    {
        ComputeBoundingBox();
    }

    PhysicsEntity::PhysicsEntity(const std::string& pathToModel, const glm::vec3& position,
        const glm::vec3& rotation, const glm::vec3& scale,
        glm::vec3 velocity, float mass,
        glm::vec3 torque, float rotationAngle, float angularVel) :
        m_position(position), m_rotation(rotation),
        m_scale(scale), m_velocity(velocity),
        m_mass(mass), m_torque(torque),
        m_rotationAngle(rotationAngle),
        m_angularVel(angularVel)
    {
        m_model = new Model(pathToModel);
        ComputeBoundingBox();
    }

    PhysicsEntity::PhysicsEntity(Model* model, const glm::vec3& position,
        const glm::vec3& rotation, const glm::vec3& scale) :
        m_model(model), m_position(position),
        m_rotation(rotation), m_scale(scale)
    {
        ComputeBoundingBox();
    }

    void PhysicsEntity::Render(Shader& shader)
    {
        // Update the model matrix based on the position
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        //Apply transtion 
        modelMatrix = glm::translate(modelMatrix, m_position);

        // Apply rotation
        modelMatrix = glm::rotate(modelMatrix, m_rotation.z, glm::vec3(0, 0, 1));
        modelMatrix = glm::rotate(modelMatrix, m_rotation.y, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, m_rotation.x, glm::vec3(1, 0, 0));

        //   modelMatrix = glm::rotate(modelMatrix, m_rotationAngle, m_torque);

           // Apply scaling 
        modelMatrix = glm::scale(modelMatrix, m_scale);

        shader.setMat4("model", modelMatrix);
        m_model->Render(shader);
    }
    void PhysicsEntity::Rotate(Shader& shader, float angle, const glm::vec3& rotateAxis)
    {
        // Create a rotation matrix
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, rotateAxis);
        model = rotation * model;
        shader.setMat4("model", model);
    }
    void PhysicsEntity::RotateOverTime(float deltaTime, const glm::vec3& rotateAxis)
    {
        // Accumulate the elapsed time
        static float accumulatedTime = 0.0f;
        accumulatedTime += deltaTime;

        static float rotationSpeed = 30.0f;
        // Calculate the rotation angle based on time and speed
        float rotationAngle = glm::radians(rotationSpeed * accumulatedTime);
        m_rotation = rotationAngle * rotateAxis;
    }

    void PhysicsEntity::OnMove(float deltaTime)
    {
        glm::vec3 newPos = m_position + m_velocity * deltaTime;
        m_rotationAngle += m_angularVel * deltaTime;
        SetPosition(newPos);
    }

    void PhysicsEntity::ComputeBoundingBox()
    {
        if (m_model->meshes.empty())
        {
            return;
        }
        m_modelBounds.Reset();
        // Initialize temporary bounds with the position of the first vertex
        glm::vec3 minBound_temp = m_model->meshes[0].vertices[0].Position + m_position;
        glm::vec3 maxBound_temp = minBound_temp;

        // Iterate over each mesh within the model
        for (const Mesh& mesh : m_model->meshes)
        {
            // Iterate over each vertex within the mesh
            for (const Vertex& vertex : mesh.vertices)
            {
                // Apply the model's position to the vertex
                glm::vec3 vertexPosition = vertex.Position + m_position;

                // Update the temporary bounds more efficiently
                minBound_temp = glm::min(minBound_temp, vertexPosition);
                maxBound_temp = glm::max(maxBound_temp, vertexPosition);
            }
        }

        // Update the model's bounding box with scale
        minBound_temp *= m_scale;
        maxBound_temp *= m_scale;

        m_modelBounds.setMinBound(minBound_temp);
        m_modelBounds.setMaxBound(maxBound_temp);
    }
    void PhysicsEntity::UpdateBoundingBox(glm::vec3 deltaPos)
    {
        m_modelBounds.Move(deltaPos);
    }
    const BoundingBox& PhysicsEntity::GetBoundingBox() const
    {
        return m_modelBounds;
    }

    void PhysicsEntity::performCollision(PhysicsEntity& obj1, PhysicsEntity& obj2)
    {
        glm::vec3 centersVector = (obj1.m_position - obj2.m_position);
        centersVector = glm::normalize(centersVector);

        glm::vec3 v1proj = glm::proj(obj1.m_velocity, centersVector);
        glm::vec3 v2proj = glm::proj(obj2.m_velocity, centersVector);

        float v1n = -1.0f * glm::length(v1proj);
        float v2n = glm::length(v2proj);

        float v1n_final = (v1n * (obj1.m_mass - obj2.m_mass) + 2 * (obj2.m_mass) * v2n) / (obj1.m_mass + obj2.m_mass);
        float v2n_final = (v2n * (obj2.m_mass - obj1.m_mass) + 2 * (obj1.m_mass) * v1n) / (obj1.m_mass + obj2.m_mass);

        glm::vec3 Ff1_dir = glm::normalize(-(obj1.m_velocity - v1proj));
        glm::vec3 Ff2_dir = glm::normalize(-(obj2.m_velocity - v2proj));

        obj1.m_velocity = (obj1.m_velocity - v1proj) + (v1n_final * glm::normalize(centersVector));
        obj2.m_velocity = (obj2.m_velocity - v2proj) + (v2n_final * glm::normalize(centersVector));

        //Ff = kfc * (m * (deltav)/(deltat))
        float kfc_div_deltat = 0.1f;
        float Ff_norm = obj1.m_mass * (v1n_final - v1n) * kfc_div_deltat * 2;//equals to obj2.mass * (v2n_final - v2n) * kfc_div_deltat

        glm::vec3 Ff1 = Ff1_dir * Ff_norm;
        glm::vec3 Ff2 = Ff2_dir * Ff_norm;

        glm::vec3 torque1 = glm::cross((-centersVector), Ff1);
        glm::vec3 torque2 = glm::cross((centersVector), Ff2);

        obj1.m_torque += torque1;
        obj2.m_torque += torque2;

        float alpha1 = glm::length(obj1.m_torque) / (0.4 * obj1.m_mass * (0.93) * (0.93));//I = 2/5 * m * r^2
        float alpha2 = glm::length(obj2.m_torque) / (0.4 * obj2.m_mass * (0.93) * (0.93));//I = 2/5 * m * r^2

        obj1.m_angularVel = alpha1;
        obj2.m_angularVel = alpha2;
    }
}