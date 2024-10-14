#include "application.h"
#include "logger.h"
#include "PhysicsEngine/Sphere.h"

#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>

namespace ntn

{
    Application* CreateApplication()
    {
        AppSpecification spec;
        Application* app = new Application(spec);
        return app;
    }

    Application::Application(const AppSpecification& appSpec)
        :m_spec(appSpec),
        m_camera(std::make_unique<Camera>(ViewMode::ThirdPerson, 45.0f, 0.1f, 10000.0f)),
        m_window(nullptr),
        m_running(false),
        m_timer()
    {
        initGraphics();
    }

    Application::~Application()
    {
    }

    void Application::initGraphics()
    {
        // glfw setting 
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window
        m_window = glfwCreateWindow(m_spec.width, m_spec.height, m_spec.name.c_str(), NULL, NULL);
        if (m_window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        // Set the user pointer to the instance of the Application class
        glfwSetWindowUserPointer(m_window, this);

        //Update screen's size
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
            {
                glViewport(0, 0, width, height);
                Application* instance = static_cast<Application*>(glfwGetWindowUserPointer(window));
                instance->m_spec.height = height;
                instance->m_spec.width = width;
            });
 
        // capture mouse
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwMakeContextCurrent(m_window);
        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        // Ensure that textures are flipped vertically when loaded
        stbi_set_flip_vertically_on_load(true);
    }

    void Application::run()
    {
        m_running = true;

        m_scene = std::make_unique<Scene>(SkyType::SkyDome);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 410");

        while (!glfwWindowShouldClose(m_window))
        {
            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_scene->setGui();

            // per-frame time logic
            m_timer.update();
            displayFPS();

            processInputEvent();

            if (m_camera->typeView == ViewMode::FirstPerson)
            {
                glm::vec3 newPos = m_scene->getTerrain()->ConstrainCameraPosToTerrain(m_camera->getPosition());
                m_camera->setPosition(newPos);
            }

            m_camera->onUpdate(m_window, m_timer.getDeltaTime());
            m_scene->onUpdate(m_timer.getDeltaTime());
            m_scene->render(m_shadersManager, m_camera);


            // Reset scene
            if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                m_scene->resetScene();
            }
            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // glfw: swap buffers and process input events
            // --------------------------------------------
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::processInputEvent()
    {
        // Mouse input processing
        double xPos, yPos;
        glfwGetCursorPos(m_window, &xPos, &yPos);
        for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button)
        {
            int action = glfwGetMouseButton(m_window, button);
            if (action == GLFW_PRESS || action == GLFW_RELEASE)
            {
                m_camera->mouseInputEvent(m_window, button, action, xPos, yPos);
            }
        }

        // Keyboard input processing
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        {
            int action = glfwGetKey(m_window, key);
            if (action == GLFW_PRESS || action == GLFW_RELEASE)
            {
                m_camera->keyboardInputEvent(m_window, key, action, m_timer.getDeltaTime());
            }
        }

        // Keyboard input processing
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        {
            int action = glfwGetKey(m_window, key);
            if (action == GLFW_PRESS || action == GLFW_RELEASE)
            {
                m_camera->keyboardInputEvent(m_window, key, action, m_timer.getDeltaTime());
            }
        }
        // Handle ESC to close the window
        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(m_window, true);
        }
    }

    void Application::displayFPS()
    {
        static int frameCount = 0;
        static float accumulatedTime = 0.0f;

        frameCount++;
        accumulatedTime += m_timer.getDeltaTime();  // Use Timer to get delta time for each frame

        // Update every second
        if (accumulatedTime >= 1.0f)
        {
            float fps = static_cast<float>(frameCount) / accumulatedTime; // Frames per second
            float msPerFrame = (accumulatedTime / frameCount) * 1000.0f;  // Milliseconds per frame

            // Set the window title with FPS and ms/frame information
            std::string title = "3D World - " + std::to_string(static_cast<int>(fps)) + " FPS / " + std::to_string(msPerFrame) + " ms";
            glfwSetWindowTitle(m_window, title.c_str());

            // Reset counters for the next second
            frameCount = 0;
            accumulatedTime = 0.0f;
        }
    }

    /************************NOT USED YET****************************/

    // CALLBACK FUNCTION IS SO SLOW
    void Application::initInputEvent()
    {
        // Set GLFW mouse button callback and forward it to InputEvent handler
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT)
            {
                Application* instance = static_cast<Application*>(glfwGetWindowUserPointer(window));
                double xPos, yPos;
                glfwGetCursorPos(window, &xPos, &yPos);
                instance->m_inputEvent.handleMouseButtonEvent(button, action, xPos, yPos);  // Forward to InputEvent class
            }
            });

        // Set GLFW key callback and forward it to InputEvent handler
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Application* instance = static_cast<Application*>(glfwGetWindowUserPointer(window));
            instance->m_inputEvent.handleKeyEvent(key, action);  // Forward to InputEvent class
            });

        // Set GLFW scroll callback and forward it to InputEvent handler
        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
            Application* instance = static_cast<Application*>(glfwGetWindowUserPointer(window));
            instance->m_inputEvent.handleScrollEvent(xoffset, yoffset);  // Forward to InputEvent class
            });

        // Register callbacks inside InputEvent class
        m_inputEvent.setMouseButtonCallback([this](int button, int action, double xPos, double yPos) {
            if (m_camera)
            {
                m_camera->mouseInputEvent(m_window,button, action, xPos, yPos); 
            }
            });

        m_inputEvent.setKeyCallback([this](int key, int action) {
            if (m_camera)
            {
                m_camera->keyboardInputEvent(m_window, key, action, m_timer.getDeltaTime());
            }

            // Handle ESC to close the window
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(m_window, true);
            }
            });

        m_inputEvent.setScrollCallback([this](double xoffset, double yoffset) {
            if (m_camera)
            {
                m_camera->scrollInputEvent(xoffset, yoffset);  // Forward to Camera's method
            }
            });

    }

    void Application::MoveObjects()
    {
        static bool isObjectGrabbed = false;
        static glm::vec3 grabOffset;
        static PhysicsObject* grabbedPhysicsObject = nullptr; // Track the currently grabbed object
        // Click on objects
        if (m_mouseHandler.leftButton.isLeftPressed)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(m_window, &mouseX, &mouseY);
            glm::vec2 mousePosition((float)mouseX, (float)mouseY);

            for (PhysicsObject* physicsItem : m_scene->AllPhysicsObjects())
            {
                if (BoxModel* item_box = dynamic_cast<BoxModel*>(physicsItem))
                {
                    glm::vec3 intersectPoint;
                    bool hit = RayIntersectsBoundingBox(mousePosition, item_box->GetBoundingBox(), intersectPoint);

                    glm::vec3 clickedPtsOnScene;
                    bool hitScene = RayIntersectsBoundingBox(mousePosition, m_scene->getSceneBounds(), clickedPtsOnScene);

                    if ((hit && hitScene) || isObjectGrabbed)
                    {
                        Log::info("Object Clicked!");
                        Log::info("Intersection Point: " +
                            std::to_string(intersectPoint.x) + " " +
                            std::to_string(intersectPoint.y) + " " +
                            std::to_string(intersectPoint.z));
                        if (!isObjectGrabbed)
                        {
                            isObjectGrabbed = true;
                            //    grabOffset = intersectPoint - item->GetPosition();
                            grabOffset = clickedPtsOnScene - item_box->GetPosition();
                            grabbedPhysicsObject = item_box;
                        }

                        if (isObjectGrabbed && grabbedPhysicsObject == item_box)
                        {
                            Log::info(item_box->GetInfo());


                            const BoundingBox& bbox_item = item_box->GetBoundingBox();

                            // Check for collisions with other objects in the scene
                            /*bool collisionDetected = BoundingBox::CheckCollision(bbox_item, m_scene.getSceneBounds());

                            if (collisionDetected)
                            {
                                Log::info("Object on the scene!!!");
                            }*/

                            glm::vec3 newTarget(clickedPtsOnScene.x - grabOffset.x,
                                clickedPtsOnScene.y - grabOffset.y,
                                clickedPtsOnScene.z - grabOffset.z);

                            // Ensure the object stays above the scene
                            float minY = m_scene->getSceneBounds().GetMaxBounds().y + bbox_item.GetDimensions().y / 2.0f;
                            newTarget.y = std::max(newTarget.y, minY);
                            // Smoothly move the object
                            static float moveSpeed = 0.05f;
                            glm::vec3 newPos = glm::mix(item_box->GetPosition(), newTarget, moveSpeed);

                            item_box->SetPosition(newPos);
                        }
                    }
                }
            }

        }
        else
        {
            // Reset isObjectGrabbed when the left mouse button is released
            isObjectGrabbed = false;
        }
    }

    bool Application::RayIntersectsBoundingBox(glm::vec2& mousePos, const BoundingBox& bbox, glm::vec3& intersectPoint)
    {
        // Convert mouse position to normalized device coordinates (NDC)
        float ndcX = (2.0f * mousePos.x) / m_camera->getViewportWidth() - 1.0f;
        float ndcY = 1.0f - (2.0f * mousePos.y) / m_camera->getViewportHeight();

        // Construct the near and far points in clip space
        glm::vec4 nearPoint = glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
        glm::vec4 farPoint = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

        // Convert the near and far points to view space
        glm::mat4 inverseProjection = m_camera->getInverseProjectionMatrix();
        glm::mat4 inverseView = m_camera->getInverseViewMatrix();

        glm::vec4 nearPointView = inverseView * (inverseProjection * nearPoint);
        nearPointView /= nearPointView.w;

        glm::vec4 farPointView = inverseView * (inverseProjection * farPoint);
        farPointView /= farPointView.w;

        // Create the ray
        Ray ray;
        ray.Origin = glm::vec3(nearPointView);
        ray.Direction = glm::normalize(glm::vec3(farPointView - nearPointView));
        // Print statements for debugging
        bool hit = RayIntersectsBoundingBox(ray, bbox, intersectPoint);
        return hit;
    }

    bool Application::RayIntersectsBoundingBox(const Ray& ray, const BoundingBox& bbox, glm::vec3& intersectPts)
    {
        // Perform ray-box intersection tests
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        // Initialize tmin and tmax to the minimum and maximum possible values
        tmin = (bbox.GetMinBounds().x - ray.Origin.x) / ray.Direction.x;
        tmax = (bbox.GetMaxBounds().x - ray.Origin.x) / ray.Direction.x;
        if (tmin > tmax)
        {
            std::swap(tmin, tmax);
        }

        tymin = (bbox.GetMinBounds().y - ray.Origin.y) / ray.Direction.y;
        tymax = (bbox.GetMaxBounds().y - ray.Origin.y) / ray.Direction.y;
        if (tymin > tymax)
        {
            std::swap(tymin, tymax);
        }

        if (tmin > tymax || tymin > tmax)
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bbox.GetMinBounds().z - ray.Origin.z) / ray.Direction.z;
        tzmax = (bbox.GetMaxBounds().z - ray.Origin.z) / ray.Direction.z;
        if (tzmin > tzmax)
        {
            std::swap(tzmin, tzmax);
        }
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        // Calculate the intersection point
        intersectPts = ray.Origin + tmin * ray.Direction;

        return true;
    }

}
