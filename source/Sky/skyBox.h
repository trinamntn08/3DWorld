#pragma once

#include"AbstractSky.h"
#include "..\mesh.h"

namespace ntn
{

class Skybox:public AbstractSky
{
public:
    explicit Skybox():AbstractSky() { InitSkyBox(); }
    ~Skybox() {};

    void render(Shader& shader) override ;

    std::vector<Texture> loadTextures(std::vector<std::string> textures_faces);

private:
    void InitSkyBox();
};
}

