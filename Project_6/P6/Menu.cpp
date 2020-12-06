#include "Menu.h"


void Menu::Initialize() {

    state.nextScene = -1;
}

void Menu::Update(float deltaTime)
{
    if (state.startGame)
    {
        state.startGame = false;
        state.nextScene = 1;
    }
}

void Menu::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    if (!state.startGame)
    {
        if (state.level < 2)
        {
            Util::DrawText(program, fontTextureID, "Shoot the Targets!", 0.75, -0.1, glm::vec3(-5.5, 1.5, 0));
            //Util::DrawText(program, fontTextureID, "Shoot to Score! Reach the Goal Before Time's Up!", 0.75, -0.1, glm::vec3(-15, 0.5, 0));
            Util::DrawText(program, fontTextureID, "Shoot to score! Reach the goal within a minute!", 0.75, -0.1, glm::vec3(-15, 0.5, 0));
            Util::DrawText(program, fontTextureID, "Pass all three levels to win!", 0.75, -0.1, glm::vec3(-9.5, -0.5, 0));
            Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.75, -0.1, glm::vec3(-6, -1.5, 0));
        }
        else if (state.level == 4)
        {            
            Util::DrawText(program, fontTextureID, "You Have Reached the Final Level", 0.75, -0.1, glm::vec3(-9.75, 4.5, 0));
            Util::DrawText(program, fontTextureID, "Level " + std::to_string(state.level), 0.75, -0.1, glm::vec3(-2, 0.5, 0));
            Util::DrawText(program, fontTextureID, "Press Enter to Advance", 0.75, -0.1, glm::vec3(-6.25, -0.5, 0));
        }
        else if (state.level > 1)
        {
            Util::DrawText(program, fontTextureID, "Level " + std::to_string(state.level) , 0.75, -0.1, glm::vec3(-2, 0.5, 0));
            Util::DrawText(program, fontTextureID, "Press Enter to Advance", 0.75, -0.1, glm::vec3(-6.25, -0.5, 0));
        }
        
    }
}



