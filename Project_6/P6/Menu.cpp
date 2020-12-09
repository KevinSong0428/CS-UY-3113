#include "Menu.h"


void Menu::Initialize() 
{
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
            //Util::DrawText(program, fontTextureID, "Shoot the Targets!", 1, -0.25, glm::vec3(-6.5, 5.5, 0));
            Util::DrawText(program, fontTextureID, "Shoot the Aliens!", 1, -0.25, glm::vec3(-6.4, 5.5, 0));
            Util::DrawText(program, fontTextureID, "Click to Shoot! Reach the goal within a minute!", 0.75, -0.1, glm::vec3(-14.75, 0.5, 0));
            Util::DrawText(program, fontTextureID, "Pass all five levels to win!", 0.75, -0.1, glm::vec3(-9.5, -0.5, 0));
            Util::DrawText(program, fontTextureID, "As you progress, the speed will increase and disappear faster!", 0.75, -0.25, glm::vec3(-15, -1.5, 0));
            Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.75, -0.1, glm::vec3(-6, -5.5, 0));
        }
        else if (state.level == 5)
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



