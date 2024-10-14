#pragma once
#include "glm/glm.hpp"

namespace ntn
{
	struct RigidBodyData
	{
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.00000001f);
		glm::vec3 angularVelocity;

		glm::vec3 startPosition = glm::vec3(0.0f);
		glm::vec3 startVelocity = glm::vec3(0.0f);

		glm::mat4 rotationTransform = glm::mat4(1);

		float scale = 1.0f;
		float mass = 1.0f;

		bool onGround = false;
		bool isStatic = false; // immovable

		bool isKinematic = false;

		bool rotationLock = true;

		// friction
		float linearDrag = 1.0f;	// 1.0f equals no drag
		float angularDrag = 0.45f;	// 1.0f equals no drag

		float elasticity = 0.1f;	// lower numbers are force absorbant

	};


	class RigidBody
	{
	public:
		RigidBody() {};
		RigidBody(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, float mass);
		RigidBody(glm::vec3 position, float angle, float speed, glm::vec3 rotation, float mass);

		~RigidBody();

		void UpdatePhysics(glm::vec3 gravity, float timeStep);

		// force application
		void applyForce(glm::vec3 force);
		void applyForceToAnotherBody(RigidBody* otherBody, glm::vec3 force);
		void applyTorque(glm::vec3 force);
		void applyTorqueToAnotherBody(RigidBody* otherBody, glm::vec3 force);

		// position prediction, only on x,y axis
		glm::vec3 predictPosition(float deltatime, float angle, float speed, glm::vec3 gravity);
		glm::vec3 predictPosition(float deltatime, glm::vec3 velocity, glm::vec3 gravity);
		glm::vec3 predictPosition(float deltatime, glm::vec3 gravity);

		RigidBodyData m_data;
	};
}

