#pragma once

#include"shader.h"
#include <glm/glm.hpp>

namespace ntn
{
    class Mesh;
    class Model;

    enum class BBoxType
    {
        Box = 0,
        Sphere = 1
    };


    class BoundingBox
    {
    public:
        BoundingBox();

        void Reset();

        glm::vec3 GetCenter() const;

        glm::vec3 GetDimensions() const;
        float GetBoundingBoxRadius() const;
        void Render(Shader& shader);
        void ExpandToInclude(const BoundingBox& bbox);

        void UpdateScale(float scale);

        void Move(const glm::vec3& movePos);


        void setMinBound(glm::vec3& minBound) { m_minBounds = minBound; };
        void setMaxBound(glm::vec3& maxBound) { m_maxBounds = maxBound; };

        const glm::vec3 GetMinBounds() const { return m_minBounds; };
        const glm::vec3 GetMaxBounds() const { return m_maxBounds; };

        Mesh toMesh();

        bool CheckCollision(const BoundingBox& other);

        static bool CheckCollision(const BoundingBox& box1, const BoundingBox& box2);

    private:
        glm::vec3 m_minBounds; //The vector is closest to the origin (0,0,0)
        glm::vec3 m_maxBounds; //The vector is farthest  to the origin (0,0,0)
    };
}
