#include"camera.h"
#include"Logger.h"

namespace ntn
{

Camera::Camera(ViewMode typeView, float verticalFOV, float nearClip, float farClip)
				:typeView(typeView), m_VerticalFOV(verticalFOV), 
				 m_NearClip(nearClip), m_FarClip(farClip)
{
	m_ForwardDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Position = glm::vec3(0.0f, 0.0f, 10.0f);
	recalculateProjection();
	recalculateView();
}

Camera::~Camera()
{
}

void Camera::onResize(int viewport_Width, int viewport_Height)
{ 
	if ((m_ViewportWidth != viewport_Width) || (m_ViewportHeight != viewport_Height))
	{
		m_ViewportWidth = viewport_Width;
		m_ViewportHeight = viewport_Height;
		recalculateProjection();
	}
}

bool Camera::onUpdate(GLFWwindow* window,float deltaTime)
{
	int viewportWidth, viewportHeight;
	glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
	onResize(viewportWidth, viewportHeight);


	// Swith ViewMode 
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (typeView == ViewMode::FirstPerson)
		{
			typeView = ViewMode::ThirdPerson;
		}
		else
		{
			typeView = ViewMode::FirstPerson;
		}
	}
	
	//Wireframe
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		m_isWireFrame = !m_isWireFrame;
		if (m_isWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
	}

	if (m_isMoving)
	{
		recalculateView();
	}
	return m_isMoving;
}

bool Camera::mouseInputEvent(GLFWwindow* window, int button, int action, double xPos, double yPos)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
		glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);
		// Prevents camera from jumping on the first click
		if (m_firstClick)
		{
			glfwSetCursorPos(window, (m_ViewportWidth / 2), (m_ViewportHeight / 2));
			m_LastMousePosition = glm::vec2(m_ViewportWidth / 2, m_ViewportHeight / 2);
			m_firstClick = false;
			return true;
		}
		else
		{
			// Stores the coordinates of the cursor
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			glm::vec2 mousePos(mouseX, mouseY);

			glm::vec2 delta = (mousePos - m_LastMousePosition) * MOUSE_SENSITIVITY;
			m_LastMousePosition = mousePos;

			// Rotation
			if (delta.x != 0.0f || delta.y != 0.0f)
			{
				float pitchDelta = delta.y * ROTATION_SPEED;
				float yawDelta = delta.x * ROTATION_SPEED;

				glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
											 glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
				m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

			}

			m_isMoving = true;
		}
		return true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		m_firstClick = true;
		return true;
	}
	return false;
}

void Camera::scrollInputEvent(double xoffset, double yoffset)
{
	m_VerticalFOV -= yoffset;
	if (m_VerticalFOV < 1.0f)
	{
		m_VerticalFOV = 1.0f;
	}

	if (m_VerticalFOV > 90.0f)
	{
		m_VerticalFOV = 90.0f;
	}

	recalculateProjection();
}

bool Camera::keyboardInputEvent(GLFWwindow* window, int key, int action, float deltaTime)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
		return false;

	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);
	float velocity = TRANSLATION_SPEED * deltaTime;

	switch (typeView)
	{
	case ViewMode::FirstPerson:
		return handleFirstPersonView(window, key, velocity, rightDirection);

	case ViewMode::ThirdPerson:
		return handleThirdPersonView(window, key, velocity, rightDirection, upDirection);

	default:
		return false;
	}
}

bool Camera::handleFirstPersonView(GLFWwindow* window, int key, float velocity, glm::vec3 rightDirection)
{
	static const float smoothFactor = 7.0f;

	if (key == GLFW_KEY_W)
	{
		m_Position.x += m_ForwardDirection.x * velocity / smoothFactor;
		m_Position.z += m_ForwardDirection.z * velocity / smoothFactor;
		m_isMoving = true;
	}
	else if (key == GLFW_KEY_S)
	{
		m_Position.x -= m_ForwardDirection.x * velocity / smoothFactor;
		m_Position.z -= m_ForwardDirection.z * velocity / smoothFactor;
		m_isMoving = true;
	}

	if (key == GLFW_KEY_A)
	{
		m_Position.x -= rightDirection.x * velocity / smoothFactor;
		m_Position.z -= rightDirection.z * velocity / smoothFactor;
		m_isMoving = true;
	}
	else if (key == GLFW_KEY_D)
	{
		m_Position.x += rightDirection.x * velocity / smoothFactor;
		m_Position.z += rightDirection.z * velocity / smoothFactor;
		m_isMoving = true;
	}
	return true;
}

bool Camera::handleThirdPersonView(GLFWwindow* window, int key, float velocity, 
									glm::vec3 rightDirection, glm::vec3 upDirection)
{
	if (key == GLFW_KEY_W)
	{
		m_Position += m_ForwardDirection * velocity;
		m_isMoving = true;
	}
	else if (key == GLFW_KEY_S)
	{
		m_Position -= m_ForwardDirection * velocity;
		m_isMoving = true;
	}

	if (key == GLFW_KEY_A)
	{
		m_Position -= rightDirection * velocity;
		m_isMoving = true;
	}
	else if (key == GLFW_KEY_D)
	{
		m_Position += rightDirection * velocity;
		m_isMoving = true;
	}

	if (key == GLFW_KEY_Q)
	{
		m_Position -= upDirection * velocity;
		m_isMoving = true;
	}
	else if (key == GLFW_KEY_E)
	{
		m_Position += upDirection * velocity;
		m_isMoving = true;
	}
	return true;
}


void Camera::recalculateProjection()
{
	m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::recalculateView()
{
	m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
	m_InverseView = glm::inverse(m_View);
}

void Camera::lookAtBoundingBox(const BoundingBox& boundingBox) 
{
	// Calculate camera position and target to fit the bounding box
	glm::vec3 boundingBoxCenter = boundingBox.GetCenter();
	float boundingBoxRadius = boundingBox.GetBoundingBoxRadius();
	// Raise the camera position along the y-axis to view the object from above
	m_Position = boundingBoxCenter + glm::vec3(0.0f, 0.0f, 1.5* boundingBoxRadius);
	m_ForwardDirection = glm::normalize(boundingBoxCenter - m_Position);

	recalculateView();
}

void Camera::LookAt(const glm::vec3& target)
{
	// Calculate the new forward direction
	m_ForwardDirection = glm::normalize(target - m_Position);
	recalculateView();
}

void Camera::setPosition(const glm::vec3& pos)
{
	m_Position = pos;
	recalculateView();
}
void printMat4(const glm::mat4& matrix) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}
void printVec4(const glm::vec4& vector) 
{
	std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
}
// Simple 4x4 matrix multiplication function
void matrixVectorMultiply(const float matrix[16], const float vector[4], float result[4]) {
	for (int i = 0; i < 4; ++i) {
		result[i] = 0.0f;
		for (int j = 0; j < 4; ++j) {
			result[i] += matrix[i * 4 + j] * vector[j];
		}
	}
}
void mat4ToArray(const glm::mat4& matrix, float array[16]) {
	// Use glm::value_ptr to get the raw array representation of the matrix
	const float* ptr = glm::value_ptr(matrix);

	// Copy the values to the destination array
	for (int i = 0; i < 16; ++i) {
		array[i] = ptr[i];
	}
}
glm::vec2 Camera::convertCameraPosToPixel(GLFWwindow* window) 
{
	glm::mat4 modelMatrix(1.0f);
	glm::mat4 mvpMatrix = m_Projection * m_View * modelMatrix;

	float matrix_MVP[16];
	mat4ToArray(mvpMatrix, matrix_MVP);
	float pos[4] = { m_Position.x, m_Position.y, m_Position.z, 1.0f };
	float result[4];
	matrixVectorMultiply(matrix_MVP, pos, result);
	glm::vec4 clipSpacePosition(result[0], result[1], result[2], result[3]);

	// Perspective divide to get normalized device coordinates (NDC)
	glm::vec3 ndcPosition = glm::vec3(clipSpacePosition) / clipSpacePosition.w;

	// Map NDC coordinates to pixel coordinates
	glm::vec2 pixelCoordinates;
	int viewportWidth, viewportHeight;
	glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
	pixelCoordinates.x = 0.5f * (ndcPosition.x + 1.0f) * viewportWidth;
	pixelCoordinates.y = 0.5f * (1.0f - ndcPosition.y) * viewportHeight;

	return pixelCoordinates;
}

}

