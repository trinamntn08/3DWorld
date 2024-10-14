#include "RigidBody.h"
#include <string>


#define MIN_LINEAR_THRESHOLD 0.05f
#define MIN_ROTATION_THRESHOLD 0.05f

namespace ntn
{

	RigidBody::RigidBody(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, float mass)
	{
		m_data.position = position;
		m_data.startPosition = position;
		m_data.velocity = velocity;
		m_data.startVelocity = velocity;
		m_data.rotation = rotation;
		m_data.mass = mass;
	}

	RigidBody::RigidBody(glm::vec3 position, float angle, float speed, glm::vec3 rotation, float mass)
	{
		m_data.position = position;
		m_data.startPosition = position;
		m_data.velocity = glm::vec3(speed * cos(angle), speed * sin(angle), 0.0f);
		m_data.startVelocity = m_data.velocity;
		m_data.rotation = rotation;
		m_data.mass = mass;
	}

	RigidBody::~RigidBody()
	{
	}

	void RigidBody::UpdatePhysics(glm::vec3 gravity, float timeStep)
	{
		if (m_data.isStatic)
		{
			return;
		}
		// adjust gravity based on ground state
		glm::vec3 totalGravity(m_data.onGround ? glm::vec3(0.0f) : gravity);
		applyForce(totalGravity * m_data.mass * timeStep);

		// apply drag
		if (!m_data.isKinematic && !m_data.rotationLock)
		{
			m_data.velocity *= m_data.linearDrag;
			m_data.angularVelocity *= m_data.angularDrag;
		}

		// update position
		m_data.position += m_data.velocity * timeStep;

		// update rotation
		if (!m_data.rotationLock)
		{
			if (m_data.rotation.z > 360.0f || m_data.rotation.z < -360.0f)
			{
				m_data.rotation.z = 0.0f;
			}
			else
			{
				m_data.rotation += m_data.angularVelocity * timeStep;
			}
		}

		// adjust velocity's to keep them in check
		if (length(m_data.velocity) < MIN_LINEAR_THRESHOLD)
		{
			if (length(m_data.velocity) < length(gravity) * m_data.linearDrag * timeStep) {
				m_data.velocity = glm::vec3(0.0f);
			}
		}

		if (length(m_data.angularVelocity) < MIN_ROTATION_THRESHOLD)
		{
			if (length(m_data.velocity) < length(m_data.angularVelocity)) {
				m_data.angularVelocity = glm::vec3(0.0f);
			}
		}
	}

	void RigidBody::applyForce(glm::vec3 force)
	{
		m_data.velocity += force / m_data.mass;
	}

	void RigidBody::applyForceToAnotherBody(RigidBody* otherBody, glm::vec3 force)
	{
		if (!otherBody->m_data.onGround)
		{
			otherBody->applyForce(force);
		}
		if (!m_data.onGround)
		{
			applyForce(-force);
		}
	}

	void RigidBody::applyTorque(glm::vec3 force)
	{
		m_data.angularVelocity += force / m_data.mass;
	}

	void RigidBody::applyTorqueToAnotherBody(RigidBody* otherBody, glm::vec3 force)
	{
		if (!otherBody->m_data.onGround) {
			otherBody->applyTorque(force);
		}
		if (!m_data.onGround) {
			applyTorque(-force);
		}
	}

	glm::vec3 RigidBody::predictPosition(float deltatime, float angle, float speed, glm::vec3 gravity)
	{
		glm::vec3 result(0.0f);
		glm::vec3 velocity(speed * cos(angle), speed * sin(angle), 0.0f); // z-axis =0.0f, only move on x,y axis
		result = predictPosition(deltatime, velocity, gravity);
		return result;
	}

	glm::vec3 RigidBody::predictPosition(float deltatime, glm::vec3 velocity, glm::vec3 gravity)
	{
		glm::vec3 result(0.0f);

		result.x = (m_data.startPosition.x + (velocity.x * deltatime));
		result.y = (m_data.startPosition.y + (velocity.y * deltatime)) + ((gravity.y * 0.5) * (deltatime * deltatime));

		return result;
	}

	glm::vec3 RigidBody::predictPosition(float deltatime, glm::vec3 gravity)
	{
		glm::vec3 result(0.0f);

		result.x = (m_data.startPosition.x + (m_data.startVelocity.x * deltatime));
		result.y = (m_data.startPosition.y + (m_data.startVelocity.y * deltatime)) + ((gravity.y * 0.5) * (deltatime * deltatime));

		return result;
	}
}
