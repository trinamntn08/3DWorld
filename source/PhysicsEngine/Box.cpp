#define NOMINMAX
#include "Box.h"
#include "RigidBody.h"
#include "Sphere.h"
#include "Plane.h"

#include <stdlib.h>
#include <iostream>
#include "glm\glm.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtx\transform.hpp"
namespace ntn
{
    Box::Box(glm::vec3 position, glm::vec3 velocity, float mass, glm::vec3 size, glm::vec4 color, bool twoD)
    {
        m_shapeID = BOX;
        m_size = size;
        m_color = color;
        m_rigidbody = new RigidBody(position, velocity, glm::vec3(0.0f, 0.0f, 0.0000001f), mass);
        m_2D = twoD;
    }

    Box::Box(glm::vec3 position, float angle, float speed, float mass, glm::vec3 size, glm::vec4 color, bool twoD)
    {
        m_shapeID = BOX;
        m_size = size;
        m_color = color;
        m_rigidbody = new RigidBody(position, angle, speed, glm::vec3(0.0f, 0.0f, 0.00000001f), mass);
        m_2D = twoD;
    }
    void Box::Copy(const Box& other)
    {
        // Copy member variables from the other object
        PhysicsObject::Copy(other);  // Assuming PhysicsObject has a copy function or copy constructor

        m_size = other.m_size;
        m_color = other.m_color;
    }
    Box::~Box()
    {
        delete m_rigidbody;
    }

    void Box::UpdatePhysics(glm::vec3 gravity, float timeStep)
    {
        m_rigidbody->UpdatePhysics(gravity, timeStep);
    }

    bool Box::checkCollision(PhysicsObject* other)
    {
        if (other->getShapeID() == BOX)
        {
            Box* otherBox = dynamic_cast<Box*>(other);
            glm::vec3 thisPos = this->GetPosition();
            glm::vec3 otherPos = otherBox->GetPosition();
            glm::vec3 otherSize = otherBox->GetSize();

            // perform checks
            if (abs(thisPos.x - otherPos.x) > (m_size.x + otherSize.x)) return false;
            if (abs(thisPos.y - otherPos.y) > (m_size.y + otherSize.y)) return false;
            if (abs(thisPos.z - otherPos.z) > (m_size.z + otherSize.z)) return false;
            return true;
        }

        if (other->getShapeID() == SPHERE)
        {
            Sphere* sphere = dynamic_cast<Sphere*>(other);
            float radius = sphere->GetRadius();
            // get the squared distance bewtween the center point and the AABB
            float distance = distPointToBox(sphere->GetPosition());

            return distance > radius ? false : true;
        }

        return false;
    }

    float Box::distPointToBox(glm::vec3 point)
    {
        glm::vec3 center = this->GetPosition();
        float distance = 0.0f;

        glm::vec3 min(center.x - m_size.x, center.y - m_size.y, center.z - m_size.z);
        glm::vec3 max(center.x + m_size.x, center.y + m_size.y, center.z + m_size.z);

        for (int i = 0; i < 3; i++)
        {
            float p = point[i];
            if (p < min[i])
            {
                distance += std::pow((min[i] - p), 2);
            }
            if (p > max[i])
            {
                distance += std::pow((p - max[i]), 2);
            }
        }
        return std::sqrt(distance);
    }

    /////////////////////////////////////////////////////////////////
    /////////////////////////CUBE//////////////////////////
    /////////////////////////////////////////////////////////////////

    BoxModel::BoxModel(const std::string& pathToModel, const glm::vec3& position,
        glm::vec3 velocity, float mass, glm::vec3 size, const glm::vec3& scale) :
        Box(position, velocity, mass, size)
    {
        LoadModel(pathToModel);
        ComputeBoundingBox();
        m_size = m_bbox.GetDimensions() / 2.0f;
    }

    BoxModel::BoxModel(BoxModel& other) :Box(other)
    {
        // Copy the Model (assuming Model has a copy constructor or supports cloning)
        if (other.m_model) {
            m_model = std::make_unique<Model>(*other.m_model);
        }
        else {
            m_model = nullptr;
        }
        ComputeBoundingBox();
        m_size = m_bbox.GetDimensions() / 2.0f;
    }

    void BoxModel::Copy(const BoxModel& other)
    {
        // Copy the base class part
        Box::Copy(other);
        if (other.m_model) {
            m_model = std::make_unique<Model>(*other.m_model);
        }
        else {
            m_model = nullptr;
        }
    }
    void BoxModel::LoadModel(const std::string& pathToModel)
    {
        m_model = std::make_unique<Model>(pathToModel);
    }
    void BoxModel::Render(Shader& shader)
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

    void BoxModel::UpdatePhysics(glm::vec3 gravity, float timeStep)
    {
        m_rigidbody->UpdatePhysics(gravity, timeStep);
        auto velocity = GetVelocity();
        UpdateBoundingBox(velocity * timeStep);
    }

    void BoxModel::ComputeBoundingBox()
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

    void BoxModel::UpdateBoundingBox(glm::vec3 deltaPos)
    {
        m_bbox.Move(deltaPos);
    }

    void BoxModel::SetPosition(const glm::vec3& newPosition)
    {
        glm::vec3 pos = GetPosition();
        glm::vec3 deltaPos = newPosition - pos;
        Box::SetPosition(newPosition);
        UpdateBoundingBox(deltaPos);
    }
    void BoxModel::Translation(const glm::vec3&& deltaPos)
    {
        glm::vec3 newPos = GetPosition() + deltaPos;
        Box::SetPosition(newPos);
        UpdateBoundingBox(deltaPos);
    }

    std::string BoxModel::GetInfo()
    {
        auto position = GetPosition();
        auto rotation = GetRotation();
        std::stringstream ss;
        ss << "BoxModel Position: " << std::to_string(position.x) << " " <<
            std::to_string(position.y) << " " <<
            std::to_string(position.z) << "\n ";

        ss << "Rotation: " << std::to_string(rotation.x) << " " <<
            std::to_string(rotation.y) << " " <<
            std::to_string(rotation.z);
        return ss.str();
    }
}
