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

//music
#include <SDL_mixer.h>

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[4];

int lives = 3;


void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

//variable for musics
Mix_Music* music;
Mix_Chunk* squash;
Mix_Chunk* gameSuccess;
Mix_Chunk* gameFailed;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Project 5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 10);

    squash = Mix_LoadWAV("bounce.wav");
    gameSuccess = Mix_LoadWAV("gameover.wav");
    gameFailed = Mix_LoadWAV("failure.wav");
    Mix_Volume(-1, MIX_MAX_VOLUME / 10);

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);

}

void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {

            //case SDLK_RETURN:
            //    currentScene->state.startGame = true;
            //    //if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
            //    break;

            case SDLK_SPACE:
                if (currentScene->state.player->collidedBottom)
                {
                    currentScene->state.player->jump = true;
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (!currentScene->state.gameSuccess)
    {
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }

        if (currentScene == sceneList[0] && keys[SDL_SCANCODE_RETURN])
        {
            SwitchToScene(sceneList[1]);
        }
    }


    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
    
}

bool dead = false;

//60 fps
#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {


    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) 
    {
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);

    if (currentScene->state.player->position.x > 5) 
    {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }

    if (currentScene->state.player->position.y < -8)
    {
        dead = true;
    }
    
    if (currentScene->state.player->enemyAttack)
    {
        dead = true;
        currentScene->state.player->enemyAttack = false;
    }

    if (currentScene->state.player->LastCollided == ENEMY)
    {
        if (currentScene->state.player->collidedTop ||
            currentScene->state.player->collidedRight ||
            currentScene->state.player->collidedLeft)
        {
            dead = true;
        }
        currentScene->state.player->LastCollided = PLATFORM;
    }

    if (currentScene->state.player->enemyKilled)
    {
        currentScene->state.player->enemyKilled = false;
        Mix_PlayChannel(-1, squash, 0);
    }

    if (dead && lives > 0)
    {
        dead = false;
        lives -= 1;
        if (lives != 0) currentScene->state.player->position = glm::vec3(2, -4, 0);
    }

    if (lives == 0)
    {
        currentScene->state.player->isActive = false;
        currentScene->state.gameFailed = true;
    }

}

void Render() 
{
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);

    GLuint fontTextureID = Util::LoadTexture("font1.png");
    glm::vec3 livesPosition, centerText;
    if (currentScene->state.player->position.x > 5)
    {
        livesPosition = glm::vec3(currentScene->state.player->position.x - 4.3, -0.5, 0);
        centerText = glm::vec3(currentScene->state.player->position.x - 1.5, -3, 0);
    }
    else
    {
        livesPosition = glm::vec3(0.7, -0.5, 0);
        centerText = glm::vec3(3.5 , -3, 0);
    }
    if (currentScene != sceneList[0]) Util::DrawText(&program, fontTextureID, "Lives " + std::to_string(lives), 0.5, -0.1, livesPosition);

    if (currentScene->state.gameSuccess)
    {
        Mix_PlayChannel(-1, gameSuccess, 0);
        Util::DrawText(&program, fontTextureID, "You Win!", 0.5, -0.1, centerText);
    }
    if (currentScene->state.gameFailed)
    {
        Mix_PlayChannel(-1, gameFailed, 0);
        Util::DrawText(&program, fontTextureID, "You Lost!", 0.5, -0.1, centerText);
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

        //checks the flag to switch scene,, if so then go to next scene
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        
        Render();
    }

    Shutdown();
    return 0;
}