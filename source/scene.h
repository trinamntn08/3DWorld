#pragma once

#include<memory>

#include"camera.h"
#include"shadersManager.h"

#include"PhysicsEngine/Box.h"
#include"PhysicsEngine/Plane.h"
#include"Terrain/Terrain.h"
#include"Terrain/TerrainSimul.h"
#include"Sky/AbstractSky.h"

namespace ntn
{

    struct PhysicsProperties
    {
        bool gravity;
        bool collisions;
        bool collisionResponse ;

        PhysicsProperties(bool gravity_ = true, bool collisions_ = true, bool collisionResponse = true) :
                            gravity(gravity_), collisions(collisions_), collisionResponse(collisionResponse) {};
    };
    enum class SkyType
    {
        SkyBox = 0,
        SkyDome = 1
    };

    class Scene
    {
    public:
        Scene(SkyType typeSkye = SkyType::SkyDome);
        ~Scene();

        // Scene Management
        void setGui();
        void loadScene();
        void resetScene(); //reset objects's positions
        void clearScene(); //delete objects

        void onUpdate(float deltaTime);

        SkyType typeSky() { return m_typeSky; };
        void updateSky(SkyType& skyType);

        void updateTerrain(TerrainType terrainType);

        // Getters
        inline std::unique_ptr <Terrain>& getTerrain() { return m_terrain; };
        inline std::unique_ptr <TerrainSimul>& getTerrain2() { return m_terrainSimul; };
        inline std::unique_ptr <AbstractSky>& getSky() { return m_sky; };
        inline std::unique_ptr <PlaneModel>& getPlane() { return m_plane; }

        // Scene Properties
        void CalculateSceneBounds();
        const BoundingBox& getSceneBounds() const { return m_sceneBounds; }

        void InitializeCubes(const std::string& filePath);
        void InitializeBalls(const std::string& filePath);


        // PHYSICS OBJECTS
        inline std::vector<PhysicsObject*> AllPhysicsObjects() { return m_allPhysicsObjects; };
        void UpdateAllObjectsToFitScene();
        void UpdatePhysicsObjectToFitScene(PhysicsObject& object);
        inline int numberOfObjects() { return m_allPhysicsObjects.size(); };

        // Rendering Methods
        void render(ShadersManager& shadersManager, const std::unique_ptr<Camera>& camera);
        void renderSkyBox(Shader& shader_skyBox, const std::unique_ptr<Camera>& camera);
        void RenderSkyDome(Shader& shader_skydome, const std::unique_ptr<Camera>& camera);
        void RenderPlane(Shader& shader_plane, const std::unique_ptr<Camera>& camera);
        void RenderTerrain(Shader& shader_terrain, const std::unique_ptr<Camera>& camera);
        void RenderTerrain2(Shader& shader_terrain2, const std::unique_ptr<Camera>& camera);
        void RenderPhysicsObjects(Shader& shader, const std::unique_ptr<Camera>& camera, bool isRender_BBoxes = false);

        /**************     COLLISIONS  ****************/
        void checkCollisions();

        void setGravity(const glm::vec3 gravity) { m_gravity = gravity; }
        glm::vec3 getGravity() const { return m_gravity; }
        // plane
        static bool planeToPlane(PhysicsObject* planeA, PhysicsObject* planeB);
        static bool planeToSphere(PhysicsObject* plane, PhysicsObject* sphere);
        static bool planeToBox(PhysicsObject* plane, PhysicsObject* box);
        // sphere
        static bool sphereToSphere(PhysicsObject* sphereA, PhysicsObject* sphereB);
        static bool sphereToPlane(PhysicsObject* sphere, PhysicsObject* plane);
        static bool sphereToBox(PhysicsObject* sphere, PhysicsObject* box);
        // box
        static bool boxToSphere(PhysicsObject* box, PhysicsObject* sphere);
        static bool boxToPlane(PhysicsObject* box, PhysicsObject* plane);
        static bool boxToBox(PhysicsObject* boxA, PhysicsObject* boxB);


    private:
        SkyType m_typeSky = SkyType::SkyBox;

        std::unique_ptr<AbstractSky> m_sky = nullptr;
        std::unique_ptr<Terrain> m_terrain = nullptr;
        std::unique_ptr<TerrainSimul> m_terrainSimul = nullptr;

        std::unique_ptr <PlaneModel> m_plane = nullptr;
        std::vector<PhysicsObject*> m_allPhysicsObjects;
        std::vector<BoxModel*> m_cubes;

        BoundingBox m_sceneBounds;

        glm::vec3 m_gravity = glm::vec3(0.f, -2.0f, 0.0f);
        PhysicsProperties m_properties = PhysicsProperties(true, true, true);

    };
}