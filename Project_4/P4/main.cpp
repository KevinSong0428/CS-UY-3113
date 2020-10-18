#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.h"
#include <vector>

//this means there are 3 platforms
//go to initialization code --> will initialize platform
#define PLATFORM_COUNT 17
#define ENEMY_COUNT 3
//#define AI_COUNT x


struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameSuccess = false;
bool gameFailed = false;
bool gameOver = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
GLuint fontTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    gameOver = true;
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++)
    {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
         offset + (-0.5f * size), 0.5f * size,
         offset + (-0.5f * size), -0.5f * size,
         offset + (0.5f * size), 0.5f * size,
         offset + (0.5f * size), -0.5f * size,
         offset + (0.5f * size), 0.5f * size,
         offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
        u, v,
        u, v + height,
        u + width, v,
        u + width, v + height,
        u + width, v,
        u, v + height,
            });

    } // end of for loop

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() 
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects
    state.player = new Entity();
    state.platforms = new Entity[PLATFORM_COUNT];
    state.enemies = new Entity[ENEMY_COUNT];


    // Initialize Player
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-2, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("george_0.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.player->height = 0.8f;
    state.player->width = 0.8f;
    state.player->jumpPower = 5.0f;

    //make new platform and initialize
    
    
    GLuint platformTextureID = LoadTexture("platformPack_tile042.png");
    fontTextureID = LoadTexture("font1.png");

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
    }

    //left floor 0 1 2 3
    for (int i = 0; i < 4; i++)
    {        
        state.platforms[i].position = glm::vec3(-4.5 + i, -3.25, 0);
    }

    //right floor 4 5 6 7 8
    for (int i = 0; i < 5; i++)
    {
        state.platforms[i + 4].position = glm::vec3(i + 0.5, -3.25, 0);
    }

    //left stairs 9 10
    state.platforms[9].position = glm::vec3(-3.5, -2.25, 0);
    state.platforms[10].position = glm::vec3(-4.5, -1.25, 0);

    //top left platform 13 14 15
    for (int i = 0; i < 3; i++)
    {
        state.platforms[i + 11].position = glm::vec3(-2.25 + i, -0.25, 0);
    }

    //top right platform 16 17 18
    for (int i = 0; i < 3; i++)
    {
        state.platforms[i + 14].position = glm::vec3(2 + i, 0.75, 0);
    }

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].Update(0, NULL, NULL, 0);
    }

    
    GLuint enemyTextureID = LoadTexture("mushroom.png");
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].speed = 1;
        state.enemies[i].isActive = true;
    }

    state.enemies[0].position = glm::vec3(2.5, -2.25, 0);
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

    state.enemies[1].position = glm::vec3(-2, 0.75, 0);
    state.enemies[1].aiType = PATROLLER;
    state.enemies[1].aiState = WALKING;

    state.enemies[2].position = glm::vec3(2.5, 1.75, 0);
    state.enemies[2].aiType = ATTACKER;
    state.enemies[2].aiState = WALKING;
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].jumpPower = 2.0f;


}

void ProcessInput()
{
    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;


            case SDLK_SPACE:
                if (state.player->collidedBottom)
                {
                    state.player->jump = true;
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (!gameOver)
    {
        if (keys[SDL_SCANCODE_LEFT] && state.player->position.x > -4.65) {
            state.player->movement.x = -1.0f;
            state.player->animIndices = state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT] && state.player->position.x < 4.65) {
            state.player->movement.x = 1.0f;
            state.player->animIndices = state.player->animRight;
        }


        if (glm::length(state.player->movement) > 1.0f) {
            state.player->movement = glm::normalize(state.player->movement);
        }
    }

}

//60 fps
#define FIXED_TIMESTEP 0.0166666f

float lastTicks = 0;
float accumulator = 0.0f;

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        //state.player->Update(FIXED_TIMESTEP, state.platforms, state.platforms, PLATFORM_COUNT);
        state.player->Update(FIXED_TIMESTEP, state.enemies, state.platforms, PLATFORM_COUNT);

        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    state.player->CheckCollisionsX(state.enemies, ENEMY_COUNT);
    state.player->CheckCollisionsY(state.enemies, ENEMY_COUNT);
    

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        state.enemies[i].CheckCollisionsX(state.player, 1);
        if (state.enemies[i].collidedLeft || state.enemies[i].collidedRight)
        {
            state.player->isActive = false;
        }
    }
    
    //if (state.player->LastCollided == ENEMY || state.enemies[0].LastCollided == PLAYER)
    if (state.player->LastCollided == ENEMY)
    {
        if (state.player->collidedLeft || state.player->collidedRight || state.player->collidedTop)
        {
            state.player->isActive = false;
        }
        else if (state.player->collidedBottom)
        {
            state.player->LastCollidedEntity->isActive = false;
        }
    }

    if (state.player->position.y < -3.75) state.player->isActive = false;
    
    if (!state.player->isActive) gameFailed = true;
    accumulator = deltaTime;
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    //draw the platform before the player
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        state.enemies[i].Render(&program);
    }
    state.player->Render(&program);

    if (!state.enemies[0].isActive &&
        !state.enemies[1].isActive &&
        !state.enemies[2].isActive)
    {
        DrawText(&program, fontTextureID, "You Win!", 0.5, -0.1, glm::vec3(-1.3, 0, 0));
    }

    if (gameFailed && !gameSuccess)
    {
        DrawText(&program, fontTextureID, "You Lose!", 0.5, -0.1, glm::vec3(-1.25, 0, 0));
    }
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}