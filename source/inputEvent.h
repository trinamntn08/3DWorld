#pragma once
#include <functional>

class InputEvent
{
public:
    // Define callback types
    using MouseButtonCallback = std::function<void(int button, int action, double xPos, double yPos)>;
    using KeyCallback = std::function<void(int key, int action)>;
    using ScrollCallback = std::function<void(double xoffset, double yoffset)>;

    void setMouseButtonCallback(MouseButtonCallback callback) { m_mouseButtonCallback = callback; }
    void setKeyCallback(KeyCallback callback) { m_keyCallback = callback; }
    void setScrollCallback(ScrollCallback callback) { m_scrollCallback = callback; }

    void handleMouseButtonEvent(int button, int action, double xPos, double yPos)
    {
        if (m_mouseButtonCallback)
        {
            m_mouseButtonCallback(button, action, xPos, yPos);
        }
    }

    void handleKeyEvent(int key, int action)
    {
        if (m_keyCallback)
        {
            m_keyCallback(key, action);
        }
    }

    void handleScrollEvent(double xOffset, double yOffset)
    {
        if (m_scrollCallback)
        {
            m_scrollCallback(xOffset, yOffset);
        }
    }

private:
    MouseButtonCallback m_mouseButtonCallback;
    KeyCallback m_keyCallback;
    ScrollCallback m_scrollCallback;
};
