#pragma once
#include <iostream>
#include"model.h"
#include"boundingBox.h"
#include"PhysicsEngine/PhysicsObject.h"

namespace ntn
{
    class Entity
    {
    public:
        Entity();

        Entity(const std::string& pathToModel, const glm::vec3& position = glm::vec3(0.0f),
            const glm::vec3& rotation = glm::vec3(0.0f),
            const glm::vec3& scale = glm::vec3(1.0f),
            glm::vec3 velocity = glm::vec3(1.0f, 0.0f, 0.0f),
            float mass = 10.f, glm::vec3 torque = glm::vec3(0, 0, 0),
            float rotationAngle = 0.0f, float angularVel = 1.0f);

        Entity(Model* model, const glm::vec3& position = glm::vec3(0.0f),
            const glm::vec3& rotation = glm::vec3(0.0f),
            const glm::vec3& scale = glm::vec3(1.0f));

        ~Entity() { if (m_model) delete m_model; };

        void SetPosition(const glm::vec3& newPosition);
        inline glm::vec3 GetPosition() const { return m_position; }

        inline void SetRotation(const glm::vec3& newRotation) { m_rotation = newRotation; }
        inline glm::vec3 GetRotation() const { return m_rotation; }

        inline void SetScale(const glm::vec3& newScale) { m_scale = newScale; }
        inline glm::vec3 GetScale() const { return m_scale; }

        void Translation(const glm::vec3&& deltaPos = glm::vec3(0.0f));

        inline void SetModel(Model* model) { m_model = model; }
        inline Model* GetModel() const { return m_model; }

        void Render(Shader& shader);

        // Bounding box
        void ComputeBoundingBox(); // initialization
        void UpdateBoundingBox(glm::vec3 deltaPos); // update when event changed
        const BoundingBox& GetBoundingBox() const;

        // Manipulation
        void Rotate(Shader& shader, float angle, const glm::vec3& rotateAxis);
        void RotateOverTime(float deltaTime, const glm::vec3& rotateAxis);
        void OnMove(float deltaTime);

        std::string getInfo()
        {
            std::stringstream ss;
            ss << "Object Position: " << std::to_string(m_position.x) << " " <<
                std::to_string(m_position.y) << " " <<
                std::to_string(m_position.z) << "\n ";

            ss << "Rotation: " << std::to_string(m_rotation.x) << " " <<
                std::to_string(m_rotation.y) << " " <<
                std::to_string(m_rotation.z);
            return ss.str();
        }


        static void performCollision(Entity& obj1, Entity& obj2);

    private:
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_scale;
        float m_mass = 1.0f;

        glm::vec3 m_velocity = glm::vec3(1.0f, 0.0f, 0.0f);
        float m_rotationAngle = 0;
        glm::vec3 m_torque = glm::vec3(0, 0, 0); // do^ xoay

        float m_angularVel = 0.0f;

        BoundingBox m_modelBounds;

        Model* m_model = nullptr;
    };


    class PhysicsEntity : public PhysicsObject
    {
    public:
        PhysicsEntity();

        PhysicsEntity(const std::string& pathToModel, const glm::vec3& position = glm::vec3(0.0f),
            const glm::vec3& rotation = glm::vec3(0.0f),
            const glm::vec3& scale = glm::vec3(1.0f),
            glm::vec3 velocity = glm::vec3(1.0f, 0.0f, 0.0f),
            float mass = 10.f, glm::vec3 torque = glm::vec3(0, 0, 0),
            float rotationAngle = 0.0f, float angularVel = 1.0f);

        PhysicsEntity(Model* model, const glm::vec3& position = glm::vec3(0.0f),
            const glm::vec3& rotation = glm::vec3(0.0f),
            const glm::vec3& scale = glm::vec3(1.0f));

        inline void SetPosition(const glm::vec3& newPosition)
        {
            glm::vec3 deltaPos = newPosition - m_position;
            m_position = newPosition;
            UpdateBoundingBox(deltaPos);
        }
        inline glm::vec3 GetPosition() const { return m_position; }
        inline void Translation(const glm::vec3&& deltaPos = glm::vec3(0.0f))
        {
            m_position += deltaPos;
            UpdateBoundingBox(deltaPos);
        }
        inline void SetRotation(const glm::vec3& newRotation) { m_rotation = newRotation; }
        inline glm::vec3 GetRotation() const { return m_rotation; }

        inline void SetScale(const glm::vec3& newScale) { m_scale = newScale; }
        inline glm::vec3 GetScale() const { return m_scale; }

        inline void SetModel(Model* model) { m_model = model; }
        inline Model* GetModel() const { return m_model; }

        void Render(Shader& shader);

        // Bounding box
        void ComputeBoundingBox(); // initialization
        void UpdateBoundingBox(glm::vec3 deltaPos); // update when event changed
        const BoundingBox& GetBoundingBox() const;

        // Manipulation
        void Rotate(Shader& shader, float angle, const glm::vec3& rotateAxis);
        void RotateOverTime(float deltaTime, const glm::vec3& rotateAxis);
        void OnMove(float deltaTime);

        std::string getInfo()
        {
            std::stringstream ss;
            ss << "Object Position: " << std::to_string(m_position.x) << " " <<
                std::to_string(m_position.y) << " " <<
                std::to_string(m_position.z) << "\n ";

            ss << "Rotation: " << std::to_string(m_rotation.x) << " " <<
                std::to_string(m_rotation.y) << " " <<
                std::to_string(m_rotation.z);
            return ss.str();
        }


        static void performCollision(PhysicsEntity& obj1, PhysicsEntity& obj2);

    private:
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_scale;
        float m_mass = 1.0f;

        glm::vec3 m_velocity = glm::vec3(1.0f, 0.0f, 0.0f);
        float m_rotationAngle = 0;
        glm::vec3 m_torque = glm::vec3(0, 0, 0); // do^ xoay

        float m_angularVel = 0.0f;

        BoundingBox m_modelBounds;

        Model* m_model = nullptr;
    };
}