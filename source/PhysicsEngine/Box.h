#pragma once

#include"PhysicsObject.h"
#include"..\Model.h"
#include"..\BoundingBox.h"

#include<memory>

namespace ntn
{
	class Box : public PhysicsObject
	{
	public:
		Box() = default;
		Box(glm::vec3 position, glm::vec3 velocity, float mass, glm::vec3 size, glm::vec4 color = glm::vec4(1.0f), bool twoD = false);
		Box(glm::vec3 position, float angle, float speed, float mass, glm::vec3 size, glm::vec4 color, bool twoD = false);
		void Copy(const Box& other);
		virtual ~Box();

		virtual void UpdatePhysics(glm::vec3 gravity, float timeStep);

		glm::vec3 GetSize() { return m_size; }
		glm::vec4 getColor() { return m_color; }

		bool checkCollision(PhysicsObject* other);
		float distPointToBox(glm::vec3 point);


	protected:
		glm::vec3 m_size;
		glm::vec4 m_color;
	};

	class BoxModel : public Box
	{
	public:
		BoxModel() = default;

		BoxModel(const std::string& pathToModel, const glm::vec3& position = glm::vec3(0.0f),
			glm::vec3 velocity = glm::vec3(1.0f, 0.0f, 0.0f),
			float mass = 100.f, glm::vec3 size = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));

		BoxModel(BoxModel& other);

		void Copy(const BoxModel& other);

		virtual ~BoxModel() {};

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

		const BoundingBox& GetBoundingBox() const { return m_bbox; };

	private:
		std::unique_ptr<Model> m_model = nullptr;
		void LoadModel(const std::string& pathToModel);
		glm::vec3 m_scale = glm::vec3(1.0f);
		BoundingBox m_bbox;

	};
}