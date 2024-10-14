#pragma once

#define NOMINMAX
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glfw3.h>
#include<vector>
#include"boundingBox.h"

namespace ntn
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	enum class ViewMode
	{
		FirstPerson = 0,
		ThirdPerson = 1
	};

	// Default camera values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float TRANSLATION_SPEED = 250.0f;
	const float ROTATION_SPEED = 0.8f;
	const float ZOOM = 45.0f;
	const float MOUSE_SENSITIVITY = 0.002f;

	class Camera
	{
	public:
		ViewMode typeView = ViewMode::ThirdPerson;

		Camera(ViewMode typeView = ViewMode::ThirdPerson,
				float verticalFOV = 45.0f, float nearClip = 0.1f, float farClip = 100.f);
		~Camera();

		bool onUpdate(GLFWwindow* window, float deltaTime = 0.01);
		void onResize(int viewportWidth, int viewportHeight);

		// Projection and View Matrices
		const glm::mat4& getProjectionMatrix() const { return m_Projection; }
		const glm::mat4& getInverseProjectionMatrix() const { return m_InverseProjection; }
		const glm::mat4& getViewMatrix() const { return m_View; }
		const glm::mat4& getInverseViewMatrix() const { return m_InverseView; }

		// Position and Direction
		const glm::vec3& getPosition() const { return m_Position; }
		const glm::vec3& getDirection() const { return m_ForwardDirection; }
		
		// Viewport dimensions
		int getViewportWidth() { return m_ViewportWidth; };
		int getViewportHeight() { return m_ViewportHeight; };

		float getRotationSpeed() { return ROTATION_SPEED; };
		void scrollInputEvent(double xoffset, double yoffset);

		// Camera View and Position Control
		void lookAtBoundingBox(const BoundingBox& boundingBox);
		void LookAt(const glm::vec3& target);
		void setPosition(const glm::vec3& pos);

		inline bool isMoving() { return m_isMoving; }

		glm::vec2 convertCameraPosToPixel(GLFWwindow* window);

		//InputEvent
		bool mouseInputEvent(GLFWwindow* window, int button, int action, double xPos, double yPos);
		bool keyboardInputEvent(GLFWwindow* window, int key, int action, float deltaTime);
		bool handleFirstPersonView(GLFWwindow* window, int key, float velocity, glm::vec3 rightDirection);
		bool handleThirdPersonView(GLFWwindow* window, int key, float velocity, 
									glm::vec3 rightDirection, glm::vec3 upDirection);


	private:

		void recalculateProjection();
		void recalculateView();

	private:
		glm::mat4 m_Projection{ 1.0f };
		glm::mat4 m_View{ 1.0f };
		glm::mat4 m_InverseProjection{ 1.0f };
		glm::mat4 m_InverseView{ 1.0f };

		float m_VerticalFOV = 45.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 100.0f;

		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, -1.0f };

		//Mouse
		bool m_firstClick = true;
		glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

		int m_ViewportWidth = 800, m_ViewportHeight = 600;

		bool m_isWireFrame = false;

		bool m_isMoving = false;

	};
}