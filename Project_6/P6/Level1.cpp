#include "Level1.h"

#define LEVEL1_target_COUNT 2

void Level1::Initialize() {

    state.nextScene = -1;

    state.mouse = new Entity();
    GLuint mouseTextureID = Util::LoadTexture("dot.png");
    state.mouse->entityType = MOUSE;
    state.mouse->movement = glm::vec3(0);
    state.mouse->position = glm::vec3(0);
    state.mouse->textureID = mouseTextureID;

    srand(time(NULL));

    state.target = new Entity[LEVEL1_target_COUNT];
    GLuint targetTextureID = Util::LoadTexture("mushroom.png");

    state.target[0].entityType = TARGET;
    state.target[0].textureID = targetTextureID;
    state.target[0].position = glm::vec3(state.target[0].spawn_x, state.target[0].spawn_y, 0);
    state.target[0].speed = 1;
    state.target[0].aiType = AILINEAR;
    state.target[0].aiState = LINEAR;

    state.target[1].entityType = TARGET;
    state.target[1].textureID = targetTextureID;
    state.target[1].position = glm::vec3(state.target[1].spawn_x, state.target[1].spawn_y, 0);
    //state.target[1].position = glm::vec3(0);

    state.time_limit = 60;
}

void Level1::Update(float deltaTime) 
{
    //if (!state.mouse->pass && !state.mouse->fail)
    //{
    //    state.target[1].isActive = true;
    //}

    //if (state.click)
    //{
    //    state.click = false;
    //    state.mouse->Update(deltaTime, state.mouse, state.target, LEVEL1_target_COUNT);
    //    state.target[0].respawn = true;
    //}

    state.mouse->Update(deltaTime, state.mouse, state.target, LEVEL1_target_COUNT);

    if (state.target[0].respawn)
    {
        state.target[0].respawn = false;
        state.target[0].position = glm::vec3(state.target[0].spawn_x, state.target[0].spawn_y, 0);
    }
	
    
    for (int i = 0; i < LEVEL1_target_COUNT; i++) 
    {
        state.target[i].Update(deltaTime, state.mouse, state.target, LEVEL1_target_COUNT);
    }
}

void Level1::Render(ShaderProgram* program) 
{
	state.mouse->Render(program);
    for (int i = 0; i < LEVEL1_target_COUNT; i++)
    {
        state.target[i].Render(program);
    }
    //after target renders, start time
}

