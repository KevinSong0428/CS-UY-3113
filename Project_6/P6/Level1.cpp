#include "Level1.h"

#define LEVEL1_target_COUNT 1

int prev_level = 1;

int spawn_x()
{
    int x = (rand() % 27) - 13; //to get random value ranging from -13 to 13
    return x;
}
int spawn_y()
{
    int y = (rand() % 13) - 6;  //to get random value ranging from -6 to 6
    return y;
}

int zero_to_one()
{
    int var = (rand() % 3) - 1;
    return var;
}

void rand_movement(Entity* other)
{
    other->movement = glm::vec3(zero_to_one(), zero_to_one(), 0);
}

void rand_AI(Entity* other)
{
    int var = rand() % 2;
    if (var == 0) other->aiType = AILINEAR;
    else other->aiType = AIROAM;
}

void Level1::Initialize() {

    state.nextScene = -1;

    srand(time(NULL));

    state.target = new Entity[LEVEL1_target_COUNT];

    GLuint targetTextureID = Util::LoadTexture("UFO.png");

    state.target[0].textureID = targetTextureID;
    state.target[0].entityType = TARGET;
    state.target[0].position = glm::vec3(spawn_x(), spawn_y(), 0);
    state.target[0].speed = 2;
    state.target[0].aiState = LINEAR;
    state.target[0].respawn = false;
    rand_movement(&state.target[0]);
    rand_AI(&state.target[0]);
}

void Level1::Update(float deltaTime) 
{

    state.goal = state.level * 5 + 10;


    if (state.target[0].respawn &&
        !state.gameFailed &&
        !state.gameSuccess)
    {
        state.target[0].respawn = false;
        if (state.target[0].isActive) state.target[0].isActive = false;
        rand_AI(&state.target[0]);
        state.target[0].position = glm::vec3(spawn_x(), spawn_y(), 0);
        state.target[0].isActive = true;
        rand_movement(&state.target[0]);
        state.spawnTime = state.time;

        //HERE IS THE LAGGY CODE COMMENTED OUT

        //if (state.target[0].aiType == LINEAR)
        //{
        //    GLuint targetTextureID = Util::LoadTexture("UFO.png");
        //    state.target[0].textureID = targetTextureID;
        //}
        //else
        //{
        //    GLuint targetTextureID = Util::LoadTexture("UFO1.png");
        //    state.target[0].textureID = targetTextureID;
        //}
    }

    if (state.level == 1)
    {
        state.target[0].width = 3;
        state.target[0].height = 3;
        state.durationTime = 3;
    }
    else if (state.level == 2)
    {
        state.target[0].width = 3;
        state.target[0].height = 3;
        state.target[0].speed = 4;
        state.durationTime = 0.5;
    }
    else if (state.level == 3)
    {
        //GLuint targetTextureID = Util::LoadTexture(".png");
        //state.target[0].textureID = targetTextureID;
        state.target[0].speed = 5.5;
        state.target[0].width = 1.5;
        state.target[0].height = 1.5;
        state.durationTime = 1;
    }
    else if (state.level == 4)
    {
        //GLuint targetTextureID = Util::LoadTexture("UFO1.png");
        //state.target[0].textureID = targetTextureID;
        state.target[0].speed = 6.5;
        state.target[0].width = 1;
        state.target[0].height = 1;
        state.goal += 5;
        state.durationTime = 1;
    }
    else
    {
        state.target[0].speed = 8;
        state.goal += 5;
        state.durationTime = 1.5;
        state.durationTime = 1;
    }

    state.target[0].Update(deltaTime, state.target, LEVEL1_target_COUNT);

    if (prev_level != state.level)
    {
        state.nextScene = 0;
        prev_level = state.level;
        state.spawnTime = 61.0;
    }

}

void Level1::Render(ShaderProgram* program) 
{
    state.target[0].Render(program);
}

