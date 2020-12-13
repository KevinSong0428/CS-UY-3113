#include "Level1.h"


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

int negOne_to_one()
{
    int var = (rand() % 3) - 1;
    return var;
}

void rand_movement(Entity* other)
{
    int x = negOne_to_one();
    int y = negOne_to_one();
    //fix speed if going diagonal
    if (x != 0 &&
        y != 0)
    {
        x *= 2 / sqrt(2);
        y *= 2 / sqrt(2);
    }
    other->movement = glm::vec3(x, y, 0);
}

void rand_AI(Entity* other)
{
    int var = rand() % 4;
    if (var == 0 || var == 1) other->aiType = AILINEAR;
    else if (var == 2) other->aiType = AICIRCLE1;
    else other->aiType = AICIRCLE2;
}

GLuint targetTexture1ID, targetTexture2ID;

void Level1::Initialize() {

    state.nextScene = -1;

    srand(time(NULL));

    state.target = new Entity();

    targetTexture1ID = Util::LoadTexture("UFO.png");
    targetTexture2ID = Util::LoadTexture("UFO1.png");

    state.target->textureID = targetTexture1ID;
    state.target->entityType = TARGET;
    state.target->position = glm::vec3(spawn_x(), spawn_y(), 0);
    state.target->speed = 0.55;
    state.target->aiState = LINEAR;
    state.target->aiType = AILINEAR;
    state.target->respawn = false;
    rand_movement(state.target);
}

void Level1::Update(float deltaTime) 
{

    state.goal = state.level * 5 + 10;

    if (state.target->respawn &&
        !state.gameFailed &&
        !state.gameSuccess)
    {
        state.target->respawn = false;
        if (state.target->isActive) state.target->isActive = false;
        rand_AI(state.target);
        state.target->position = glm::vec3(spawn_x(), spawn_y(), 0);
        state.target->isActive = true;
        rand_movement(state.target);
        state.spawnTime = state.time;

        if (state.target->aiType == AILINEAR)
        {
            state.target->textureID = targetTexture1ID;
        }
        else
        {
            int var = 10 - abs(state.target->position.y);
            state.target->roamRand = (rand() % var) - var;
            state.target->textureID = targetTexture2ID;
        }
    }

    if (state.level == 1)
    {
        state.target->width = 3;
        state.target->height = 3;
        state.durationTime = 2;
    }
    else if (state.level == 2)
    {
        state.target->width = 2;
        state.target->height = 2;
        state.target->speed = 0.65;
        state.durationTime = 0.7;
    }
    else if (state.level == 3)
    {
        state.target->speed = 0.75;
        state.target->width = 1.5;
        state.target->height = 1.5;
        state.durationTime = 0.75;
    }
    else if (state.level == 4)
    {
        state.target->speed = 0.85;
        state.target->width = 1;
        state.target->height = 1;
        state.goal += 5;
        if (state.target->aiType == AILINEAR) state.durationTime = 1.0;
        else state.durationTime = 0.75;
    }
    else if (state.level == 5)
    {
        state.target->speed = 0.9;
        state.goal += 5;
        if (state.target->aiType == AILINEAR) state.durationTime = 1.0;
        else state.durationTime = 0.75;
    }

    state.target->Update(deltaTime, state.target);

    if (prev_level != state.level)
    {
        state.nextScene = 0;
        prev_level = state.level;
        state.spawnTime = 61.0;
    }

}

void Level1::Render(ShaderProgram* program) 
{
    state.target->Render(program);
}

