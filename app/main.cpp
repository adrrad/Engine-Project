#include <iostream>
#include "renderer/Renderer.hpp"

using namespace std;

int main()
{
    Rendering::Renderer renderer;
    renderer.RenderLoop();
    return 0;
}