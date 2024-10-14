#pragma once
#include <vector>
#include "glm\glm.hpp"

namespace ntn
{

struct PhysicsProperties
{
	bool gravity = false;
	bool collisions = false;
	bool collisionResponse = false;
};

class PhysicsObject;

class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene();

	void Update(float deltaTime);

	void addObject(PhysicsObject* object);
	void removeObject(PhysicsObject* object);

	std::vector<PhysicsObject*> getAllObjects() { return m_allObjects; };
	int numberOfObjects() { return m_allObjects.size(); }

	void resetScene();
	void clearScene();

	void setGravity(const glm::vec3 gravity) { m_gravity = gravity; }
	glm::vec3 getGravity() const { return m_gravity; }

	void setTimeStep(const float timeStep) { m_timeStep = timeStep; }
	float setTimeStep() const { return m_timeStep; }

	/**************     COLLISIONS  ****************/
	void checkCollisions();
	// plane
	static bool planeToPlane(PhysicsObject* planeA, PhysicsObject* planeB);
	static bool planeToSphere(PhysicsObject* plane, PhysicsObject* sphere);
	static bool planeToBox(PhysicsObject*  plane, PhysicsObject* box);
	// sphere
	static bool sphereToSphere(PhysicsObject* sphereA, PhysicsObject* sphereB);
	static bool sphereToPlane(PhysicsObject* sphere, PhysicsObject* plane);
	static bool sphereToBox(PhysicsObject* sphere, PhysicsObject* box);
	// box
	static bool boxToSphere(PhysicsObject* box, PhysicsObject* sphere);
	static bool boxToPlane(PhysicsObject* box, PhysicsObject* plane);
	static bool boxToBox(PhysicsObject * boxA, PhysicsObject* boxB);
	/************************************************/

	// scene properties
	PhysicsProperties m_properties;

protected:
	glm::vec3 m_gravity=glm::vec3(0.f);
	float m_timeStep = 0.f;
	std::vector<PhysicsObject*> m_allObjects;

	bool m_applyForce;

};
}
