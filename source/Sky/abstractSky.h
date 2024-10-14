#pragma once
#include <memory>
#include "../mesh.h"

namespace ntn
{

class AbstractSky
{
public:
    AbstractSky(): m_sky(std::make_unique<Mesh>()) {};

    virtual ~AbstractSky() {};

    virtual void render(Shader& shader) = 0;
    virtual std::vector<Texture> loadTextures(std::vector<std::string> textures) =0;

protected:
    std::unique_ptr<Mesh> m_sky ;
};
}