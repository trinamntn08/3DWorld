#pragma once
#define NOMINMAX
#include<glad/glad.h>
#include<glfw3.h>

#include<memory>
#include<glm/glm.hpp>

#include "shadersManager.h"
#include "inputEvent.h"
#include "timer.h"
#include "camera.h"
#include "Scene.h"

// NOT USED YET
#include "pickingTexture.h" 
#include "mousePicker.h"
#include "traceRay.h"


namespace ntn
{
struct AppSpecification
{
	std::string name = "NTN";
	unsigned int width = 1600;
	unsigned int height = 900;

};

class Application
{
public:

	explicit Application(const AppSpecification& appSpec=AppSpecification());
	~Application();

	void initGraphics();

	void run();
	 
	void processInputEvent();

	void displayFPS();

	// Not used for now
	// Callback function is so slow
	void initInputEvent();

	//TODO for physcial objects interactions
	void MoveObjects();
	bool RayIntersectsBoundingBox(glm::vec2 &mousePos,const BoundingBox& bbox, glm::vec3& intersectPoint);
	bool RayIntersectsBoundingBox(const Ray& ray, const BoundingBox& bbox,glm::vec3& intersectPts);

private:
	AppSpecification m_spec;

	GLFWwindow* m_window;

	bool m_running;

	Timer m_timer;

	ShadersManager m_shadersManager;

	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Scene> m_scene;

	// Mouse
	MouseHandler m_mouseHandler;
	InputEvent m_inputEvent;
};

// Called from entrypoint
Application* CreateApplication();

}