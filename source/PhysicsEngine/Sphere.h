#pragma once

#include"..\Model.h"
#include"..\boundingBox.h"
#include"PhysicsObject.h"

#include<memory>

namespace ntn
{
class Sphere :public PhysicsObject
{
public:
	Sphere(glm::vec3 position, glm::vec3 velocity, float mass, 
			float radius, glm::vec4 color=glm::vec4(1.0f), bool twoD = false);
	Sphere(glm::vec3 position, float angle, float speed, float mass, 
			float radius, glm::vec4 color, bool twoD = false);
	virtual ~Sphere() {};

	void UpdatePhysics(glm::vec3 gravity, float timeStep);
	
	float GetRadius() { return m_radius; }
	void  SetRadius(float radius) { m_radius = radius; }

	glm::vec4 GetColor() { return m_color; }
private:
	float m_radius=1.0f;
	glm::vec4 m_color=glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};


class SphereModel : public Sphere
{
public:
	SphereModel() = default;

	SphereModel(const std::string& pathToModel, const glm::vec3& position = glm::vec3(0.0f),
		glm::vec3 velocity = glm::vec3(1.0f, 0.0f, 0.0f),
		float mass = 10.f, const glm::vec3& scale = glm::vec3(1.0f),
		float radius= 10.f);

	//Ball(Model* model, const glm::vec3& position = glm::vec3(0.0f),
	//const glm::vec3& rotation = glm::vec3(0.0f),
	//const glm::vec3& scale = glm::vec3(1.0f));

	virtual ~SphereModel() {};

	void UpdatePhysics(glm::vec3 gravity, float timeStep);

	std::string GetInfo();
	
	// Communicate with RigidBody
	void SetPosition(const glm::vec3& newPosition);
//	glm::vec3 GetPosition() override;

//	void SetRotation(const glm::vec3& newRotation);
//	inline glm::vec3 GetRotation() const { return m_rotation; }

	inline void SetScale(const glm::vec3& newScale) { m_scale = newScale; }
	inline glm::vec3 GetScale() const { return m_scale; }
	
	void Translation(const glm::vec3&& deltaPos = glm::vec3(0.0f));

	inline void SetModel(Model* model) { m_model.reset(model); }
	inline const std::unique_ptr<Model>& GetModel() const { return m_model; }

	void Render(Shader& shader);

	void ComputeBoundingBox();
	void UpdateBoundingBox(glm::vec3 deltaPos);

	BoundingBox GetBoundingBox() { return m_bbox; };

private:
	std::unique_ptr<Model> m_model = nullptr;
	void LoadModel(const std::string& pathToModel);
	glm::vec3 m_scale = glm::vec3(1.0f);
	BoundingBox m_bbox;

};

}