#include "Application.h"
#include "Logger.h"
#include <glm/gtc/matrix_transform.hpp> 
#include"PhysicsEngine/Sphere.h"
#include"Timer.h"

#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>

Application* CreateApplication()
{
	AppSpecification spec;
	Application* app = new Application(spec);
	return app;
}

Application::Application(const AppSpecification& appSpec):
                        m_spec(appSpec), 
                        m_camera(std::make_unique<Camera>(TypeCameraView::ThirdPerson, 
                                                            45.0f, 0.1f, 10000.0f))
{
    InitGraphicEnvironment();
    InitShader();
}

Application::~Application()
{
}

void Application::InitGraphicEnvironment()
{    
    // glfw setting 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) 
    {
        Application* instance = static_cast<Application*>(glfwGetWindowUserPointer(window));
        instance->m_camera->ProcessMouseScroll(static_cast<float>(yoffset));
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
    {
        Application* instance = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            if (action == GLFW_PRESS)
            {
                instance->m_mouseHandler.onLeftMouseDown(static_cast<int>(xpos), static_cast<int>(ypos));
            }
            else if (action == GLFW_RELEASE)
            {
                instance->m_mouseHandler.onLeftMouseUp(static_cast<int>(xpos), static_cast<int>(ypos));
            }
        }
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
}

void Application::InitShader()
{
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    Shader shader_objects   = Shader("source/shaders/core_vertex.glsl", "source/shaders/core_fragment.glsl");
    Shader shader_skyBox  = Shader("source/shaders/sky/skybox_vertex.glsl", "source/shaders/sky/skybox_fragment.glsl");
    Shader shader_skyDome = Shader("source/shaders/sky/skydome_vertex.glsl", "source/shaders/sky/skydome_fragment.glsl");
    
    Shader shader_rawTerrain = Shader("source/shaders/terrain/realTerrain_raw.vs", "source/shaders/terrain/realTerrain_raw.frag");
    Shader shader_tessTerrain = Shader("source/shaders/terrain/realTerrain_tess.vs", "source/shaders/terrain/realTerrain_tess.frag", 
                                        nullptr,
                                        "source/shaders/terrain/realTerrain_tess.tcs", "source/shaders/terrain/realTerrain_tess.tes");    

    Shader shader_simulTerrain = Shader("source/shaders/terrain/simulTerrain.vert", 
                                    "source/shaders/terrain/simulTerrain.frag",
                                    nullptr,
                                    "source/shaders/terrain/simulTerrain.tcs",
                                    "source/shaders/terrain/simulTerrain.tes");

    Shader shader_plane = Shader("source/shaders/adv_lighting_vertex.glsl", 
                                    "source/shaders/adv_lighting_fragment.glsl");

    m_shadersManager.setPlaneShader(shader_plane);
    m_shadersManager.setObjectsShader(shader_objects);
    m_shadersManager.setSkyBoxShader(shader_skyBox);
    m_shadersManager.setSkyDomeShader(shader_skyDome);
    m_shadersManager.setRawTerrainShader(shader_rawTerrain);
    m_shadersManager.setTessTerrainShader(shader_tessTerrain);
    m_shadersManager.setSimulTerrainShader(shader_simulTerrain);
}

void Application::ConfigCamera()
{
    if (m_camera->m_typeView == TypeCameraView::FirstPerson)
    {
        //glm::vec3 cam_newPos = m_scene->getTerrain()->ConstrainCameraPosToTerrain(m_camera->GetPosition());
        ///*     m_camera->SetPosition(cam_newPos);*/
        //m_camera->LookAt(glm::vec3(0.0f, 0.0f, -1.0f));
    }
    else if (m_camera->m_typeView == TypeCameraView::ThirdPerson)
    {
          m_camera->LookAtBoundingBox(m_scene->getSceneBounds());
    }
    
}

void Application::Run()
{

    m_running = true;
    
    m_scene =std::make_unique<Scene>(Sky::SkyDome);
    ConfigCamera();
    
    /* FOR HEIGHT FRAMEBUFFER 
    unsigned int width = m_scene->getTerrain()->getWidth();
    unsigned int height = m_scene->getTerrain()->getDepth();

    // STORE HEIGHT MAP
    unsigned int framebuffer;
    unsigned int m_depthTexture;

    // Create framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // Set up texture for the framebuffer
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_depthTexture, 0);

    // Set up draw buffers
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    // Verify that the FBO is correct
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int frameCounter = 0;
    */


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // render loop
    while (!glfwWindowShouldClose(m_window))
    {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_scene->SetGui();

        // per-frame time logic
        float currentFrame = (float)glfwGetTime();
        m_frameTime = currentFrame - m_lastFrameTime;

        DisplayFPS(currentFrame);

        // Reset scene
        if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            m_scene->ResetScene();
        }

        //MoveObjects();

        if (m_camera->m_typeView == TypeCameraView::FirstPerson)
        {
            glm::vec3 cam_newPos = m_scene->getTerrain()->ConstrainCameraPosToTerrain(m_camera->GetPosition());
            m_camera->SetPosition(cam_newPos);
        }
        m_camera->OnUpdate(m_window, m_frameTime);

        m_scene->OnUpdate(m_frameTime);

        m_scene->Render(m_shadersManager,m_camera);

        /*
        //glm::mat4 model_terrain = glm::mat4(1.0f);
        //model_terrain = glm::scale(model_terrain, m_scene->getTerrain()->GetScale());
        //glm::mat4 view_terrain = m_camera->GetViewMatrix();
        //glm::mat4 projection_terrain = m_camera->GetProjectionMatrix();
       
        //static bool isHeightMapLoaded = false;
        //if (!isHeightMapLoaded)
        //{
        //    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //    m_shadersManager.terrain.activate();
        //    GLenum error = glGetError();
        //    if (error != GL_NO_ERROR)
        //    {
        //        printf("OpenGL error after activating shader, code: 0x%x\n", error);
        //    }

        //    m_shadersManager.terrain.setMat_MVP(model_terrain, view_terrain, projection_terrain);

        //    m_scene->getTerrain()->RenderTesselation(m_shadersManager.terrain);

        //    float* data = new float[width * height];
        //    // Bind the framebuffer before reading
        //    glReadBuffer(GL_COLOR_ATTACHMENT1); // Set the read buffer to the height data attachment
        //    glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);
        //    std::vector<float> heightData(data, data + width * height);

        //    m_scene->getTerrain()->storeTerrainHeightData(heightData);
        //    delete[] data;
        //    isHeightMapLoaded = true;
        //    // Unbind the framebuffer after reading
        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //}
        //

        // Function to get height per (x,y) pixel
        //if (m_camera->m_typeView == TypeCameraView::FirstPerson && m_camera->isMoving())
        //{
        //    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //    m_shadersManager.terrain.activate();
        //    m_shadersManager.terrain.setMat_MVP(model_terrain, view_terrain, projection_terrain);
        //    m_scene->getTerrain()->RenderTesselation(m_shadersManager.terrain);
        //    glReadBuffer(GL_COLOR_ATTACHMENT1);
        //    glm::vec2 pos = m_camera->ConvertCameraPosToPixel(m_window);

        //    float height;
        //    glReadPixels(pos.x, pos.y, 1, 1, GL_RED, GL_FLOAT, &height);
        //    Log::info("Height Pos: " + std::to_string(height));
        //    glm::vec3 cam_pos = m_camera->GetPosition();
        //    glm::vec3 cam_newPos(cam_pos.x, height + 2.0f, cam_pos.z);
        //    m_camera->SetPosition(cam_newPos);
        //    // Unbind the framebuffer after reading
        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //}
        //
        //

        //m_shadersManager.terrain.activate();
        //m_shadersManager.terrain.setMat_MVP(model_terrain, view_terrain, projection_terrain);
        //m_scene->getTerrain()->Render(m_shadersManager.terrain);
        */

        
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

void Application::DisplayFPS(float currentFrame)
{
    static int counter = 0;
    
    m_timeStep += m_frameTime;
    m_lastFrameTime = currentFrame;
    counter++; //nbr of frames

    // Update title every second
    if (counter >= 30)
    {
        // Calculate FPS and milliseconds per frame
        float fps = counter / m_timeStep;
        float msPerFrame = m_timeStep / (counter) * 1000.0f;

        // Creates new title
        std::string FPS = std::to_string(static_cast<int>(fps));
        std::string ms = std::to_string((msPerFrame));
        std::string newTitle = "3D Visualizer - " + FPS + "FPS / " + ms + "ms";

        glfwSetWindowTitle(m_window, newTitle.c_str());

        // Reset counters and timer
        m_timeStep = 0.0f;
        counter = 0;
    }
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
bool Application::RayIntersectsBoundingBox(glm::vec2& mousePos,const BoundingBox& bbox, glm::vec3& intersectPoint)
{
    // Convert mouse position to normalized device coordinates (NDC)
    float ndcX = (2.0f * mousePos.x) / m_camera->GetViewPortWidth() - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePos.y) / m_camera->GetViewPortHeight();

    // Construct the near and far points in clip space
    glm::vec4 nearPoint = glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
    glm::vec4 farPoint = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

    // Convert the near and far points to view space
    glm::mat4 inverseProjection = m_camera->GetInverseProjectionMatrix();
    glm::mat4 inverseView = m_camera->GetInverseViewMatrix();

    glm::vec4 nearPointView = inverseView * (inverseProjection * nearPoint);
    nearPointView /= nearPointView.w;

    glm::vec4 farPointView = inverseView * (inverseProjection * farPoint);
    farPointView /= farPointView.w;

    // Create the ray
    Ray ray;
    ray.Origin = glm::vec3(nearPointView);
    ray.Direction = glm::normalize(glm::vec3(farPointView - nearPointView));
    // Print statements for debugging
    bool hit =  RayIntersectsBoundingBox(ray, bbox, intersectPoint);
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
