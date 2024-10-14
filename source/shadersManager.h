#pragma once

#include"shader.h"
#include<unordered_map>

namespace ntn
{
    class ShadersManager
    {
    public:
        Shader& getShader(const std::string& shaderName) 
        {
            // Check if the shader is already loaded
            if (m_listshaders.find(shaderName) == m_listshaders.end()) 
            {
                // If not, load the shader
                m_listshaders[shaderName] = loadShader(shaderName);
            }
            return m_listshaders[shaderName];
        }

    private:
        Shader loadShader(const std::string& shaderName) 
        {
            // Load shaders based on their name
            if (shaderName == "CoreShader")
            {
                return Shader("core_vertex.glsl", "core_fragment.glsl");
            }
            else if (shaderName == "PlaneShader")
            {
                return Shader("adv_lighting_vertex.glsl", "adv_lighting_fragment.glsl");
            }
            else if (shaderName == "SkyBoxShader") 
            {
                return Shader("sky/skybox_vertex.glsl", "sky/skybox_fragment.glsl");
            }
            else if (shaderName == "SkyDomeShader") 
            {
                return Shader("sky/skydome_vertex.glsl", "sky/skydome_fragment.glsl");
            }
            else if (shaderName == "RawTerrainShader") 
            {
                return Shader("terrain/realTerrain_raw.vs", "terrain/realTerrain_raw.frag");
            }
            else if (shaderName == "TessTerrainShader") 
            {
                return Shader("terrain/realTerrain_tess.vs", "terrain/realTerrain_tess.frag",
                    nullptr, "terrain/realTerrain_tess.tcs", "terrain/realTerrain_tess.tes");
            }
            else if (shaderName == "SimulTerrainShader") 
            {
                return Shader("terrain/simulTerrain.vert", "terrain/simulTerrain.frag",
                                nullptr, "terrain/simulTerrain.tcs", "terrain/simulTerrain.tes");
            }

            throw std::runtime_error("Unknown shader: " + shaderName);
        }

    private:
        std::unordered_map<std::string, Shader> m_listshaders;

    };
}
