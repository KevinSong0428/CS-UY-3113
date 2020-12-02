#include "Menu.h"


void Menu::Initialize() {

    state.nextScene = -1;
    state.mouse = new Entity();
    state.time_limit = 0;
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
        Util::DrawText(program, fontTextureID, "Shoot the Targets!", 0.75, -0.1, glm::vec3(0.5, -3, 0));
        Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.75, -0.1, glm::vec3(-0.2, -4, 0));
    }
}



