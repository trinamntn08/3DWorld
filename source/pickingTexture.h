#pragma once
#include<iostream>

struct PixelInfo {
    unsigned int ObjectID = 0; // object associated with
    unsigned int PrimID = 0; // triangle associated with 
    unsigned int DrawID = 0; // for rendering


    void Print()
    {
        std::cout << "Object " << ObjectID << ", " << PrimID << ", " << DrawID << std::endl;
    }
};

class PickingTexture
{
public:
    PickingTexture() {}

    ~PickingTexture();

    void Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void EnableWriting();

    void DisableWriting();

    PixelInfo ReadPixel(unsigned int x, unsigned int y);

private:
    unsigned int m_fbo = 0;
    unsigned int m_pickingTexture = 0;
    unsigned int m_depthTexture = 0;
};
