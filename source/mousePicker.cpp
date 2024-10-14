#include"mousePicker.h"
#include<glfw3.h>

namespace ntn
{

MousePicker::MousePicker(Camera camera):m_camera(camera)
{
}

void MousePicker::update()
{
    glm::mat4 viewMatrix = m_camera.getViewMatrix();
    m_currentRay = calculateMouseRay();
}
glm::vec3 MousePicker::calculateMouseRay()
{
    glm::vec2 normalizedCoords = ViewportToNDC(m_mouse.x, m_mouse.y);
    glm::vec4 clipCoords(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
    glm::vec4 viewCoords = NDCToViewCoords(clipCoords);
    glm::vec3 worldRay = ViewToWorldCoords(viewCoords);
    return worldRay;
}
glm::vec2 MousePicker::ViewportToNDC(float mouseX, float mouseY)
{
    float x = (2.0f * mouseX) / static_cast<float>(m_camera.getViewportWidth()) - 1.0f;
    float y = (2.0f * mouseY) / static_cast<float>(m_camera.getViewportHeight()) - 1.0f;
    return glm::vec2(x, y);
}
glm::vec4 MousePicker::NDCToViewCoords(glm::vec4 clipCoords)
{
    glm::mat4 invertedProjection = m_camera.getInverseProjectionMatrix();
    glm::vec4 viewCoords = invertedProjection * clipCoords;
    return glm::vec4(viewCoords.x, viewCoords.y, -1.0f, 0.0f);
}
glm::vec3 MousePicker::ViewToWorldCoords(glm::vec4 viewCoords)
{
    glm::mat4 invertedView = m_camera.getInverseViewMatrix();
    glm::vec4 rayWorld = invertedView * viewCoords;
    glm::vec3 mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
    mouseRay = glm::normalize(mouseRay);
    return mouseRay;
}

glm::vec3 MousePicker::GetCurrentRay()
{
    return m_currentRay;
}

glm::vec3 MousePicker::getPointOnRay(glm::vec3 ray, float distance) 
{
    glm::vec3 camPos = m_camera.getPosition();
    glm::vec3 start(camPos.x, camPos.y, camPos.z);
    glm::vec3 scaledRay(ray.x * distance, ray.y * distance, ray.z * distance);
    return start + scaledRay;
}

glm::vec3 MousePicker::binarySearch(int count, float start, float finish, glm::vec3 ray) 
{
   /* float half = start + ((finish - start) / 2.0f);
    if (count >= RECURSION_COUNT) {
        glm::vec3 endPoint = getPointOnRay(ray, half);
        Terrain terrain = getTerrain(endPoint.x, endPoint.z);
        if (terrain != nullptr) {
            return endPoint;
        }
        else {
            return glm::vec3(0.0f);
        }
    }
    if (intersectionInRange(start, half, ray)) {
        return binarySearch(count + 1, start, half, ray);
    }
    else {
        return binarySearch(count + 1, half, finish, ray);
    }*/
    return glm::vec3();
}

bool MousePicker::intersectionInRange(float start, float finish, glm::vec3 ray) 
{
    glm::vec3 startPoint = getPointOnRay(ray, start);
    glm::vec3 endPoint = getPointOnRay(ray, finish);
    if (!isUnderGround(startPoint) && isUnderGround(endPoint)) {
        return true;
    }
    else {
        return false;
    }
}

bool MousePicker::isUnderGround(glm::vec3 testPoint) 
{
    /*Terrain terrain = getTerrain(testPoint.x, testPoint.z);
    float height = 0.0f;
    if (terrain != nullptr) {
        height = terrain->getHeightOfTerrain(testPoint.x, testPoint.z);
    }
    if (testPoint.y < height) {
        return true;
    }
    else {
        return false;
    }*/
    return true;
}

//Terrain* MousePicker::getTerrain(float worldX, float worldZ) 
//{
//    // Implement the logic to get the terrain at the specified coordinates.
//    // You may need to adjust the return type and logic based on your actual implementation.
//    return nullptr;
//}

void MousePicker::setMousePosition(double mouseX, double mouseY)
{
    m_mouse.x = mouseX;
    m_mouse.y = mouseY;
}

bool MousePicker::rayIntersectsBoundingBox(const BoundingBox& box) 
{
    return true;
}
}