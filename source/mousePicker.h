#pragma once

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include"camera.h"

namespace ntn
{

static const int RECURSION_COUNT = 200;
static const float RAY_RANGE = 600.0f;

struct MouseButton
{
    bool isLeftPressed = false;
    int x=0;
    int y=0;
};

class MouseHandler
{
public:
    MouseHandler():leftButton() {};
    MouseButton leftButton;

    void updateMousePosition(int x, int y)
    {
        leftButton.x = x;
        leftButton.y = y;
    }

    void setLeftButtonState(bool isPressed)
    {
        leftButton.isLeftPressed = isPressed;
    }

    void onLeftMouseDown(int x, int y)
    {
        setLeftButtonState(true);
        updateMousePosition(x, y);
    }

    void onLeftMouseUp(int x, int y)
    {
        setLeftButtonState(false);
        updateMousePosition(x, y);
    }


    void onMouseMove(int x, int y)
    {
        if (leftButton.isLeftPressed)
        {
           updateMousePosition(x, y);
        }
    }
};

class MousePicker 
{
public:
    MousePicker(Camera camera);
    void update();

    glm::vec3 GetCurrentRay();
    glm::vec2 ViewportToNDC(float mouseX, float mouseY);
    glm::vec4 NDCToViewCoords(glm::vec4 clipCoords);
    glm::vec3 ViewToWorldCoords(glm::vec4 eyeCoords);

    glm::vec3 calculateMouseRay();
    glm::vec3 getPointOnRay(glm::vec3 ray, float distance);
    glm::vec3 binarySearch(int count, float start, float finish, glm::vec3 ray);
    bool intersectionInRange(float start, float finish, glm::vec3 ray);

    bool isUnderGround(glm::vec3 testPoint);

    void setMousePosition(double mouseX, double mouseY);
    bool rayIntersectsBoundingBox(const BoundingBox& box);

private:
    glm::vec3 m_currentRay=glm::vec3(0.0f);
    Camera m_camera;
    MouseButton m_mouse;
};

}