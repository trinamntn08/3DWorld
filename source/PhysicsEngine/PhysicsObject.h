#pragma once
#include "glm/glm.hpp"

namespace ntn
{
	enum ShapeType
	{
		JOINT = -1,
		PLANE = 0,
		SPHERE = 1,
		BOX = 2,
		SHAPE_COUNT = 3
	};

	class RigidBody;

	class PhysicsObject
	{
	protected:
		PhysicsObject() {};
		PhysicsObject(ShapeType shapeID) : m_shapeID(shapeID) {}
		void Copy(const PhysicsObject& other);
	public:
		virtual ~PhysicsObject()
		{
			if (m_rigidbody != nullptr)
			{
				delete m_rigidbody;
			}
		}
		virtual void UpdatePhysics(glm::vec3 gravity, float timeStep) = 0;

		int getShapeID() { return m_shapeID; }

		virtual glm::vec3 GetPosition();
		virtual glm::vec3 GetStartPosition();
		virtual glm::vec3 GetVelocity();
		virtual glm::vec3 GetStartVelocity();
		virtual glm::vec3 GetRotation();
		virtual float GetMass();


		virtual void SetPosition(glm::vec3 position);
		virtual void SetVelocity(glm::vec3 velocity);
		virtual void SetRotation(glm::vec3 rotation);
		virtual void SetMass(float mass);

		void SetOriginalPosition(glm::vec3 position);
		void SetCurrentPosAsOriginalPos();

		RigidBody* Rigidbody() { return m_rigidbody; }
		bool Switch2DState() { m_2D = !m_2D; return m_2D; }

		virtual void ResetPosition();
		virtual void ResetVelocity();

	protected:
		ShapeType m_shapeID = ShapeType::BOX;
		RigidBody* m_rigidbody = nullptr;
		bool m_2D = false;
	};
}
