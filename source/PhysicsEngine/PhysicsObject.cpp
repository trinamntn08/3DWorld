#include "PhysicsObject.h"
#include "RigidBody.h"


namespace ntn
{
void PhysicsObject::Copy(const PhysicsObject& other)
{
	// Copy member variables
	m_shapeID = other.m_shapeID;
	m_2D = other.m_2D;

	// Copy RigidBody if it exists
	if (other.m_rigidbody != nullptr) {
		m_rigidbody = new RigidBody(*other.m_rigidbody);
	}
	else {
		m_rigidbody = nullptr;
	}
}
glm::vec3 PhysicsObject::GetPosition()
{
	return m_rigidbody != nullptr ? m_rigidbody->m_data.position : glm::vec3(0.0f);
}
glm::vec3 PhysicsObject::GetStartPosition()
{
	return m_rigidbody != nullptr ? m_rigidbody->m_data.startPosition : glm::vec3(0.0f);
}
glm::vec3 PhysicsObject::GetVelocity()
{
	return m_rigidbody != nullptr ? m_rigidbody->m_data.velocity : glm::vec3(0.0f);
}
glm::vec3 PhysicsObject::GetStartVelocity()
{
	return m_rigidbody != nullptr ? m_rigidbody->m_data.startVelocity : glm::vec3(0.0f);
}
glm::vec3 PhysicsObject::GetRotation()
{
	return m_rigidbody != nullptr ? m_rigidbody->m_data.rotation : glm::vec3(0.0f);
}
float PhysicsObject::GetMass()
{
	return m_rigidbody != nullptr ? m_rigidbody->m_data.mass : 0.0f;
}

void PhysicsObject::SetPosition(glm::vec3 position)
{
	if (m_rigidbody != nullptr) 
	{
		m_rigidbody->m_data.position = position;
	}
}

void PhysicsObject::SetVelocity(glm::vec3 velocity)
{
	if (m_rigidbody != nullptr) 
	{
		m_rigidbody->m_data.velocity = velocity;
	}
}
void PhysicsObject::SetRotation(glm::vec3 rotation)
{
	if (m_rigidbody != nullptr)
	{
		m_rigidbody->m_data.rotation = rotation;
	}
}
void PhysicsObject::SetMass(float mass)
{
	if (m_rigidbody != nullptr)
	{
		m_rigidbody->m_data.mass = mass;
	}
}

void PhysicsObject::SetOriginalPosition(glm::vec3 position)
{
	if (m_rigidbody != nullptr)
	{
		m_rigidbody->m_data.startPosition = position;
	}
}
void PhysicsObject::SetCurrentPosAsOriginalPos()
{
	if (m_rigidbody != nullptr)
	{
		m_rigidbody->m_data.startPosition = m_rigidbody->m_data.position;
	}
}
void PhysicsObject::ResetPosition()
{
	if (m_rigidbody != nullptr) 
	{
		m_rigidbody->m_data.position = m_rigidbody->m_data.startPosition;
	}
}

void PhysicsObject::ResetVelocity()
{
	if (m_rigidbody != nullptr) 
	{
		m_rigidbody->m_data.velocity = m_rigidbody->m_data.startVelocity;
	}
}
}
