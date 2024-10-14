#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "RigidBody.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"

#include <string>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <glm\gtx\range.hpp>
#include <glm\gtc\type_ptr.hpp> // used for gl::vec to [] in gui functions

namespace ntn
{

typedef bool(*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFuncs[] = 
{
	PhysicsScene::planeToPlane, PhysicsScene::planeToSphere, PhysicsScene::planeToBox,
	PhysicsScene::sphereToPlane, PhysicsScene::sphereToSphere, PhysicsScene::sphereToBox,
	PhysicsScene::boxToPlane, PhysicsScene::boxToSphere, PhysicsScene::boxToBox
};

PhysicsScene::PhysicsScene()
{
	m_applyForce = false;
}

PhysicsScene::~PhysicsScene()
{
	for (auto object : m_allObjects)
	{
		delete object;
	}
}

void PhysicsScene::addObject(PhysicsObject* object)
{
	m_allObjects.push_back(object);
}

void PhysicsScene::removeObject(PhysicsObject* object)
{
	auto objItr = std::find(m_allObjects.begin(), m_allObjects.end(), object);

	if (objItr != m_allObjects.end())
	{
		m_allObjects.erase(objItr);
	}
}

void PhysicsScene::resetScene()
{
	for (auto object : m_allObjects) {
		object->ResetPosition();
		object->ResetVelocity();
	}

	if (m_applyForce)
	{
		m_applyForce = false;
	}
}

void PhysicsScene::clearScene()
{
	m_properties.gravity = false;
	m_properties.collisions = false;
	m_allObjects.clear();
}

void PhysicsScene::Update(float deltaTime)
{
	// update physics at fixed time step
	static float timer = 0;
	timer += deltaTime;
	if (timer >= m_timeStep) 
	{
		timer -= m_timeStep;
		// update all
		for (auto object : m_allObjects)
		{
			if (object != nullptr)
			{
				object->UpdatePhysics(m_properties.gravity ? m_gravity : glm::vec3(0.0f), deltaTime);
			}
		}
		// check for collisions
		if (m_properties.collisions)
		{
			checkCollisions();
		}
	}
}

void PhysicsScene::checkCollisions()
{
	int nbrObjects = numberOfObjects();

	// check for collisions with this object
	for (int outer = 0; outer < nbrObjects - 1; outer++)
	{
		for (int inner = outer +1; inner < nbrObjects; inner++)
		{
			PhysicsObject* objA = m_allObjects[outer];
			PhysicsObject* objB = m_allObjects[inner];
			int shapeIdA = objA->getShapeID();
			int shapeIdB = objB->getShapeID();
			// skip checking collisions for joints
			if (shapeIdA < 0 || shapeIdB < 0) 
			{
				continue;
			}
			// function pointers
			int functionID = (shapeIdA * SHAPE_COUNT) + shapeIdB;
			fn collisionFuncPtr = collisionFuncs[functionID];
			if (collisionFuncPtr != nullptr) 
			{
				collisionFuncPtr(objA, objB);
			}
		}
	}
}
/*********************************************************************************************************
* Plane to Object collisions
**********************************************************************************************************/
bool PhysicsScene::planeToPlane(PhysicsObject* a_planeA, PhysicsObject* a_planeB)
{
	Plane* planeA = dynamic_cast<Plane*>(a_planeA);
	Plane* planeB = dynamic_cast<Plane*>(a_planeB);
	// check if objects aren't null before testing
	if (planeA != nullptr && planeB != nullptr) {
		// check plane for collision
		// if the normals are pointing in the same direction
		//	- just check distance between and distance fom origin
		// if the normal are at an angle
		//	- could do some fancy trig or aabb stuff
	}
	return false;
}

bool PhysicsScene::planeToSphere(PhysicsObject* plane, PhysicsObject* sphere)
{
	return sphereToPlane(sphere, plane);
}

bool PhysicsScene::planeToBox(PhysicsObject* plane, PhysicsObject* box)
{
	return boxToPlane(box, plane);
}
/*********************************************************************************************************
* Sphere to Object collsions
**********************************************************************************************************/
bool PhysicsScene::sphereToSphere(PhysicsObject * a_sphereA, PhysicsObject * a_sphereB)
{
	Sphere* sphereA = dynamic_cast<Sphere*>(a_sphereA);
	Sphere* sphereB = dynamic_cast<Sphere*>(a_sphereB);

	if (sphereA != nullptr && sphereB != nullptr) 
	{
		// check sphere for collision
		float distance = glm::distance(sphereA->GetPosition(), sphereB->GetPosition());
		float totalRadius = sphereA->GetRadius() + sphereB->GetRadius();
		// compare distance between centers to combined radius
		if (distance < totalRadius) {
			// cahce bool values
			bool kinematicA = sphereA->Rigidbody()->m_data.isKinematic;
			bool kinematicB = sphereB->Rigidbody()->m_data.isKinematic;
			bool onGroundA = sphereA->Rigidbody()->m_data.onGround;
			bool onGroundB = sphereB->Rigidbody()->m_data.onGround;
			// check either is kinematic
			if (!kinematicA || !kinematicB) {
				// get the normal of the gap between objects
				glm::vec3 collisionNormal = glm::normalize(sphereB->GetPosition() - sphereA->GetPosition());
				// if both spheres are not on the ground
				if (!onGroundA && !onGroundB) {
					// calculate force vector
					glm::vec3 relativeVelocity = sphereA->GetVelocity() - sphereB->GetVelocity();
					glm::vec3 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
					glm::vec3 forceVector =	collisionVector * 1.0f / (1.0f / sphereA->Rigidbody()->m_data.mass + 1.0f / sphereB->Rigidbody()->m_data.mass);

					// combine elasticity
					float combinedElasticity = (sphereA->Rigidbody()->m_data.elasticity +
												sphereB->Rigidbody()->m_data.elasticity / 2.0f);
					// use Newton's third law to apply collision forces to colliding bodies 
					sphereA->Rigidbody()->applyForceToAnotherBody(sphereB->Rigidbody(), forceVector + (forceVector*combinedElasticity));
					
					// apply torque
					glm::vec3 centerPoint = sphereA->GetPosition() - sphereB->GetPosition();
					glm::vec3 torqueLever = glm::normalize(glm::vec3(centerPoint.y, -centerPoint.x, 0.0f));

					float torque = glm::dot(torqueLever, relativeVelocity) * 1.0f / (1.0f / sphereA->Rigidbody()->m_data.mass + 1.0f / sphereB->Rigidbody()->m_data.mass);
					
					sphereA->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f,-torque));
					sphereB->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, torque));

					// move out spheres out of collision 
					glm::vec3 separationVector = collisionNormal * distance * 0.5f;
					sphereA->SetPosition(sphereA->GetPosition() - separationVector);
					sphereB->SetPosition(sphereB->GetPosition() + separationVector);
				}
				// if one sphere is on the ground treat collsion as plane collision
				if (onGroundA || onGroundB) 
				{
					// determine moving sphere
					Sphere * sphere = (onGroundA ? sphereB : sphereA);
					Sphere * sphereGround = (onGroundA ? sphereA : sphereB);
					// calculate force vector
					glm::vec3 forceVector = -1 * sphere->Rigidbody()->m_data.mass * collisionNormal * (glm::dot(collisionNormal, sphere->GetVelocity()));
					// apply force
					sphere->Rigidbody()->applyForce(forceVector * 2.0f);
					// apply torque
					glm::vec3 torqueLever = glm::normalize(glm::vec3(collisionNormal.y, -collisionNormal.x, 0.0f));

					float torque =	glm::dot(torqueLever, sphere->GetVelocity()) * -1.0f /(1.0f / sphereA->Rigidbody()->m_data.mass);
					sphere->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, torque));

					// move out of collision
					glm::vec3 separationVector = collisionNormal * distance * 0.5f;
					sphere->SetPosition(sphere->GetPosition() - separationVector);
					// stop other sphere from being on ground
					sphereGround->Rigidbody()->m_data.onGround = false;
				}
			}
			else 
			{
				// object colliding yes, stop objects
				sphereA->SetVelocity(glm::vec3(0.0f));
				sphereB->SetVelocity(glm::vec3(0.0f));
				if (onGroundA || onGroundB) 
				{
					sphereA->Rigidbody()->m_data.onGround = true;
					sphereB->Rigidbody()->m_data.onGround = true;
				}
			}
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphereToPlane(PhysicsObject * a_sphere, PhysicsObject * a_plane)
{
	Sphere* sphere = dynamic_cast<Sphere*>(a_sphere);
	Plane* plane = dynamic_cast<Plane*>(a_plane);

	if (sphere != nullptr && plane != nullptr) {
		bool kinematic = sphere->Rigidbody()->m_data.isKinematic;
		glm::vec3 planeNorm = plane->getNormal();
		float planeDO = plane->getDistance();
		// magnitude of sphere vector, plane normal
		float mag = (dot(sphere->GetPosition(), planeNorm));

		// if planeNorm is below 0 magnitude will be negative
		if (mag < 0)
		{
			planeNorm *= -1;
			mag *= -1;
		}

		float collision = mag - sphere->GetRadius();
		// collision check
		if (collision < 0.0f) {
			if (!kinematic) {
				// calculate force vector
				glm::vec3 forceVector = -1 * sphere->Rigidbody()->m_data.mass * planeNorm * (glm::dot(planeNorm, sphere->GetVelocity()));
				// combine elasticity
				float combinedElasticity = (sphere->Rigidbody()->m_data.elasticity +
											plane->getElasticity() / 2.0f);
				// only bounce if not resting on the ground
				if (!sphere->Rigidbody()->m_data.onGround) 
				{
					sphere->Rigidbody()->applyForce(forceVector + (forceVector*combinedElasticity));
					// apply torque
					glm::vec3 centerPoint = sphere->GetPosition() - planeNorm;
					glm::vec3 torqueLever = glm::normalize(glm::vec3(centerPoint.y, -centerPoint.x, 0.0f));

					float torque = glm::dot(torqueLever, sphere->GetVelocity()) * 1.0f / (1.0f / sphere->Rigidbody()->m_data.mass);

					sphere->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, -torque));

					// move out of collision
					glm::vec3 separationVector = planeNorm * collision * 0.5f;
					sphere->SetPosition(sphere->GetPosition() - separationVector);
				}
			}
			else 
			{
				// object colliding, stop object
				sphere->SetVelocity(glm::vec3(0.0f));
				sphere->Rigidbody()->m_data.onGround = true;
			}
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphereToBox(PhysicsObject * a_sphere, PhysicsObject * a_box)
{
	return boxToSphere(a_box, a_sphere);
}
/*********************************************************************************************************
* Box to Object collsions
**********************************************************************************************************/
bool PhysicsScene::boxToSphere(PhysicsObject * a_box, PhysicsObject * a_sphere)
{
	Box* box = dynamic_cast<Box*>(a_box);
	Sphere * sphere = dynamic_cast<Sphere*>(a_sphere);

	if (box != nullptr && sphere != nullptr) 
	{
		// collision check
		if (box->checkCollision(sphere)) 
		{
			// cache some bools for later use
			bool kinematicA = box->Rigidbody()->m_data.isKinematic;
			bool kinematicB = sphere->Rigidbody()->m_data.isKinematic;
			bool onGroundA = box->Rigidbody()->m_data.onGround;
			bool onGroundB = sphere->Rigidbody()->m_data.onGround;
			// check either is kinematic
			if (!kinematicA || !kinematicB) 
			{
				glm::vec3 centerDist = sphere->GetPosition() - box->GetPosition();
				glm::vec3 boxesMaxSize = glm::vec3(box->GetSize() + sphere->GetRadius());
				glm::vec3 collisionNormal = glm::normalize(centerDist);
				glm::vec3 overlap = abs(centerDist - boxesMaxSize);
				// if both boxs are not on the ground
				if (!onGroundA && !onGroundB) {
					// calculate force vector
					glm::vec3 relativeVelocity = box->GetVelocity() - sphere->GetVelocity();
					glm::vec3 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
					glm::vec3 forceVector = collisionVector * 1.0f / (1.0f / box->Rigidbody()->m_data.mass + 1.0f / sphere->Rigidbody()->m_data.mass);
					// combine elasticity
					float combinedElasticity = (box->Rigidbody()->m_data.elasticity +
												sphere->Rigidbody()->m_data.elasticity / 2.0f);
					// use Newton's third law to apply collision forces to colliding bodies 
					box->Rigidbody()->applyForceToAnotherBody(sphere->Rigidbody(), forceVector + (forceVector*combinedElasticity));

					// apply torque
					float torque = glm::dot(collisionVector, relativeVelocity) * 1.0f / (1.0f / box->Rigidbody()->m_data.mass + 1.0f / sphere->Rigidbody()->m_data.mass);

					box->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, -torque));
					sphere->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, torque));

					// move out boxs out of collision 
					glm::vec3 separationVector = collisionNormal * overlap * 0.5f;
					box->SetPosition(box->GetPosition() - separationVector);
					sphere->SetPosition(sphere->GetPosition() + separationVector);
				}
				// if one box is on the ground treat collision as plane collision
				if (onGroundA || onGroundB) 
				{
					// determine moving box
					PhysicsObject* obj = (onGroundA ? dynamic_cast<PhysicsObject*>(sphere) : dynamic_cast<PhysicsObject*>(box));
					PhysicsObject* objGround = (onGroundA ? dynamic_cast<PhysicsObject*>(box) : dynamic_cast<PhysicsObject*>(sphere));
					// calculate force vector
					glm::vec3 forceVector = -1 * obj->Rigidbody()->m_data.mass * collisionNormal * (glm::dot(collisionNormal, obj->GetVelocity()));
					// apply force
					obj->Rigidbody()->applyForce(forceVector * 2.0f);
					// move out of collision
					glm::vec3 separationVector = collisionNormal * overlap * 0.5f;
					obj->SetPosition(obj->GetPosition() - separationVector);
					// stop other box from being on ground
					objGround->Rigidbody()->m_data.onGround = objGround->Rigidbody()->m_data.isStatic ? true : false;
				}
			}
			else 
			{
				// object colliding yes, stop objects
				box->SetVelocity(glm::vec3(0.0f));
				sphere->SetVelocity(glm::vec3(0.0f));
				if (onGroundA || onGroundB) {
					box->Rigidbody()->m_data.onGround = true;
					sphere->Rigidbody()->m_data.onGround = true;
				}
			}
			return true;
		}
	}
	return false;
}

bool PhysicsScene::boxToPlane(PhysicsObject* a_box, PhysicsObject* a_plane)
{
	Box* box = dynamic_cast<Box*>(a_box);
	Plane* plane = dynamic_cast<Plane*>(a_plane);

	if (box != nullptr && plane != nullptr) 
	{
		glm::vec3 planeNormal = plane->getNormal();
		glm::vec3 center = box->GetPosition();
		glm::vec3 extents(box->GetSize());
		// magnitude of box center and plane vectors
		float mag = dot(planeNormal, center);
		// projection interval radius of box onto the plane
		float radius = extents.x * abs(planeNormal.x) + extents.y * abs(planeNormal.y) + extents.z * abs(planeNormal.z);

		// if planeNorm is below 0 magnitude will be negative
		if (mag < 0)
		{
			planeNormal *= -1;
			mag *= -1;
		}

		float collision = mag - radius;

		// collision check
		if (collision <= 0.0f) 
		{
			// cache some data
			bool kinematic = box->Rigidbody()->m_data.isKinematic;
			if (!kinematic) {
				// calculate force vector
				glm::vec3 forceVector = -1 * box->Rigidbody()->m_data.mass * planeNormal * (glm::dot(planeNormal, box->GetVelocity()));
				// combine elasticity
				float combinedElasticity = (box->Rigidbody()->m_data.elasticity +
											plane->getElasticity() / 2.0f);
				// only bounce if not resting on the ground
				if (!box->Rigidbody()->m_data.onGround) {
					// apply force
					box->Rigidbody()->applyForce(forceVector + (forceVector*combinedElasticity));

					// apply torque
					glm::vec3 centerPoint = box->GetPosition() - planeNormal;
					glm::vec3 torqueLever = glm::normalize(glm::vec3(centerPoint.y, -centerPoint.x, 0.0f));
					float torque = glm::dot(torqueLever, box->GetVelocity()) * 1.0f / (1.0f / box->Rigidbody()->m_data.mass);
					box->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, -torque));

					// move out of collision
					glm::vec3 separationVector = planeNormal * collision * 0.5f;
					box->SetPosition(box->GetPosition() - separationVector);
				}
			}
			else {
				// object colliding, stop object
				box->SetVelocity(glm::vec3(0.0f));
				box->Rigidbody()->m_data.onGround = true;
			}
			return true;
		}
	}
	return false;
}

bool PhysicsScene::boxToBox(PhysicsObject* a_boxA, PhysicsObject* a_boxB)
{
	Box* boxA = dynamic_cast<Box*>(a_boxA);
	Box* boxB = dynamic_cast<Box*>(a_boxB);

	if (boxA != nullptr && boxB != nullptr) 
	{
		// collision check
		if (boxA->checkCollision(boxB)) 
		{
			// cache some bools for later use
			bool kinematicA = boxA->Rigidbody()->m_data.isKinematic;
			bool kinematicB = boxB->Rigidbody()->m_data.isKinematic;
			bool onGroundA = boxA->Rigidbody()->m_data.onGround;
			bool onGroundB = boxB->Rigidbody()->m_data.onGround;
			// check either is kinematic
			if (!kinematicA || !kinematicB) {
				glm::vec3 centerDist = boxB->GetPosition() - boxA->GetPosition();
				glm::vec3 boxesMaxSize = glm::vec3(boxA->GetSize() + boxB->GetSize());
				glm::vec3 collisionNormal = glm::normalize(centerDist);
				glm::vec3 overlap = abs(centerDist - boxesMaxSize);
				// if both boxs are not on the ground
				if (!onGroundA && !onGroundB) 
				{
					// calculate force vector
					glm::vec3 relativeVelocity = boxA->GetVelocity() - boxB->GetVelocity();
					glm::vec3 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
					glm::vec3 forceVector = collisionVector * 1.0f / (1.0f / boxA->Rigidbody()->m_data.mass + 1.0f / boxB->Rigidbody()->m_data.mass);
					// combine elasticity
					float combinedElasticity = (boxA->Rigidbody()->m_data.elasticity +
												boxB->Rigidbody()->m_data.elasticity / 2.0f);
					// use Newton's third law to apply collision forces to colliding bodies 
					boxA->Rigidbody()->applyForceToAnotherBody(boxB->Rigidbody(), forceVector + (forceVector*combinedElasticity));

					// apply torque
					float torque = glm::dot(collisionVector, relativeVelocity) * 1.0f / (1.0f / boxA->Rigidbody()->m_data.mass + 1.0f / boxB->Rigidbody()->m_data.mass);

					boxA->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, -torque));
					boxB->Rigidbody()->applyTorque(glm::vec3(0.0f, 0.0f, torque));

					// move out boxs out of collision 
					glm::vec3 separationVector = collisionNormal * overlap * 0.5f;
					boxA->SetPosition(boxA->GetPosition() - separationVector);
					boxB->SetPosition(boxB->GetPosition() + separationVector);
				}
				// if one box is on the ground treat collsion as plane collision
				if (onGroundA || onGroundB) 
				{
					// determine moving box
					Box* box = (onGroundA ? boxB : boxA);
					Box* boxGround = (onGroundA ? boxA : boxB);
					// calculate force vector
					glm::vec3 forceVector = -1 * box->Rigidbody()->m_data.mass * collisionNormal * (glm::dot(collisionNormal, box->GetVelocity()));
					// apply force
					box->Rigidbody()->applyForce(forceVector * 2.0f);
					// move out of collision
					glm::vec3 separationVector = collisionNormal * overlap * 0.5f;
					box->SetPosition(box->GetPosition() - separationVector);
					// stop other box from being on ground
					boxGround->Rigidbody()->m_data.onGround = false;
				}
			}
			else 
			{
				// object colliding yes, stop objects
				boxA->SetVelocity(glm::vec3(0.0f));
				boxB->SetVelocity(glm::vec3(0.0f));
				if (onGroundA || onGroundB) {
					boxA->Rigidbody()->m_data.onGround = true;
					boxB->Rigidbody()->m_data.onGround = true;
				}
			}
			return true;
		}
	}
	return false;
}
}

