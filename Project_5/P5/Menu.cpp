#include "Menu.h"


void Menu::Initialize() {

    state.nextScene = -1;
    state.player = new Entity();
}

void Menu::Update(float deltaTime)
{
    if (state.startGame)
    {
        state.nextScene = 1;
    }
}

void Menu::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    if (!state.startGame)
    {
        Util::DrawText(program, fontTextureID, "Two Face Mushroom", 0.5, -0.1, glm::vec3(1.85, -3, 0));
        Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.5, -0.1, glm::vec3(1.25, -4, 0));
    }
}



