#include "Menu.h"

GLuint newFontTextureID;

void Menu::Initialize() 
{
    state.nextScene = -1;
    newFontTextureID = Util::LoadTexture("font1.png");
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
    
    if (!state.startGame)
    {
        if (state.level < 2)
        {
            Util::DrawText(program, newFontTextureID, "Shoot the Aliens!", 1, -0.25, glm::vec3(-6.4, 5.5, 0));
            Util::DrawText(program, newFontTextureID, "Click to Shoot! Reach the goal within a minute!", 0.75, -0.1, glm::vec3(-14.75, 0.5, 0));
            Util::DrawText(program, newFontTextureID, "Pass all five levels to win!", 0.75, -0.1, glm::vec3(-9, -0.5, 0));
            Util::DrawText(program, newFontTextureID, "As you progress, the speed will increase and disappear faster!", 0.75, -0.25, glm::vec3(-15, -1.5, 0));
            Util::DrawText(program, newFontTextureID, "Press Enter to Start", 0.75, -0.1, glm::vec3(-6, -5.5, 0));
        }
        else if (state.level == 5)
        {            
            Util::DrawText(program, newFontTextureID, "You Have Reached the Final Level", 0.75, -0.1, glm::vec3(-9.75, 4.5, 0));
            Util::DrawText(program, newFontTextureID, "Level " + std::to_string(state.level), 0.75, -0.1, glm::vec3(-2, 0.5, 0));
            Util::DrawText(program, newFontTextureID, "Press Enter to Advance", 0.75, -0.1, glm::vec3(-6.25, -0.5, 0));

        }
        else
        {
            Util::DrawText(program, newFontTextureID, "Level " + std::to_string(state.level) , 0.75, -0.1, glm::vec3(-2, 0.5, 0));
            Util::DrawText(program, newFontTextureID, "Press Enter to Advance", 0.75, -0.1, glm::vec3(-6.25, -0.5, 0));
        }
        
    }
}



