#include "Game.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

int main()
{
    Game game;
    while (!game.GetWindow()->IsOver())
    {
        // Game loop.
        game.HandleInput();
        game.Update();
        game.Render();
        game.RestartClock();
    }
    return 0;
}