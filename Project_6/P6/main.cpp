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
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[2];

int score = 0;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

//variable for musics
Mix_Music* music;
Mix_Chunk* squash;
Mix_Chunk* gameSuccess;
Mix_Chunk* gameFailed;

//timer varaible
time_t start;

//durationTime = how long target is active for every level
int durationTime;
void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Aim Training", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 720);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    //Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 10);

    squash = Mix_LoadWAV("bounce.wav");
    gameSuccess = Mix_LoadWAV("gameover.wav");
    gameFailed = Mix_LoadWAV("failure.wav");
    Mix_Volume(-1, MIX_MAX_VOLUME / 10);

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    SwitchToScene(sceneList[0]);

    if (currentScene != sceneList[0])
    {
        start = time(0);
    }

    
    //6 - 5 - 4 - 3 - 2
    durationTime = 7 - currentScene->state.level;
}

bool CheckCollision(int unit_x, int unit_y, Entity* other)
{
    if (other->isActive == false) return false;
    if (unit_x >= other->position.x &&                       // right of the left edge
        unit_x <= other->position.x + other->width &&        // left of the right edge
        unit_y >= other->position.y &&                       // below the top
        unit_y <= other->position.y + other->height) {       // above the bottom
        return true;
    }
    return false;
}

float val1 = 0;
int val2 = 0;

void ProcessInput() 
{

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) 
        {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;


        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {

            case SDLK_RETURN:
                currentScene->state.startGame = true;
                if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            int x = event.button.x;
            int y = event.button.y;
            int button = event.button.button;   // button that was clicked (1,2,3)

            int unit_x = ((x / 1280.0) * 32.0) - (32.0 / 2.0);
            int unit_y = (((720.0 - y) / 720.0) * 18.0) - (18.0 / 2.0);
            //if (unit_x == x/1280 * 32 - 16) Mix_PlayChannel(-1, gameSuccess, 0);
            

            if (currentScene != sceneList[0] && 
                button == 1 &&
                !currentScene->state.gameFailed &&
                !currentScene->state.gameSuccess)
            {
                val1 = (((720.0 - y) / 720.0) * 18.0) - (18.0 / 2.0);
                val2 = currentScene->state.target[0].position.y;
                bool hit = false;
                hit = CheckCollision(unit_x, unit_y, &currentScene->state.target[0]);
                if (hit && !currentScene->state.gameFailed)
                {
                    currentScene->state.spawnTime = currentScene->state.time;
                    currentScene->state.target[0].respawn = true;
                    Mix_PlayChannel(-1, squash, 0);
                    score++;
                }

                //if (button == 1) Mix_PlayChannel(-1, gameFailed, 0); //left click

            }
            break;

        }
    }
    
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (!currentScene->state.gameSuccess)
    {
        //while have not passed the level of difficulty

        if (currentScene == sceneList[0] && keys[SDL_SCANCODE_RETURN])
        {
            currentScene->state.startGame = true;
            SwitchToScene(sceneList[1]);
        }
    }
    
}

//60 fps
#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
float diffTick = 0;
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

    while (deltaTime >= FIXED_TIMESTEP) 
    {
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    if (currentScene == sceneList[0])
    {
        diffTick = ticks;
    }

    if (currentScene->state.time != 0.0 &&
        currentScene != sceneList[0])
    {
        currentScene->state.time = ticks - diffTick;
        currentScene->state.time = 61.0 - currentScene->state.time;

        //after a certain time, it has to spawn at new location
        //if (currentScene->state.spawnTime - currentScene->state.time >= durationTime)
        //{
        //    currentScene->state.target[0].respawn = true;
        //    currentScene->state.spawnTime = currentScene->state.time;
        //}
    }


    if (currentScene->state.time < -1.0)
    {
        currentScene->state.time = 0.0;
        currentScene->state.gameFailed = true;
        currentScene->state.target[0].isActive = false;
    }

    //winning condition for level
    if (currentScene != sceneList[0] &&
        currentScene->state.goal == score)
    {
        //CHANGE SOUND
        Mix_PlayChannel(-1, gameSuccess, 0);
        currentScene->state.spawnTime = 61;
        //5 levels total
        //ISSUE WITH LEVEL AND GOAL NOT MATCHING FOR CURRENT SCENE AND SCENELIST            

        if (currentScene->state.level == 4)
        {
            currentScene->state.gameSuccess = true;
        }
        else
        {
            sceneList[0]->state.level++;
            sceneList[0]->state.startGame = false;
            sceneList[1]->state.level++;
            //sceneList[1]->state.spawnTime = 61.0;
            score = 0;
        }
    }
}

void Render() 
{
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);

    GLuint fontTextureID = Util::LoadTexture("font1.png");

    if (currentScene != sceneList[0] && 
        !currentScene->state.gameSuccess &&
        !currentScene->state.gameFailed)
    {
        //currentScene is going to be sceneList[1]
        Util::DrawText(&program, fontTextureID, "Level " + std::to_string(int(currentScene->state.level)), 0.75, -0.1, glm::vec3(-2, 8.5, 0));
        Util::DrawText(&program, fontTextureID, "Timer: " + std::to_string(int(currentScene->state.time)) + " sec", 0.75, -0.1, glm::vec3(-4.5, 7.5, 0));
        Util::DrawText(&program, fontTextureID, "Goal: " + std::to_string(int(currentScene->state.goal)) , 0.75, -0.1, glm::vec3(11, 8.5, 0));
        Util::DrawText(&program, fontTextureID, "Score: " + std::to_string(score), 0.75, -0.1, glm::vec3(-15, 8.5, 0));
        Util::DrawText(&program, fontTextureID, std::to_string(sceneList[1]->state.spawnTime), 0.75, -0.1, glm::vec3(0, -2, 0));
        Util::DrawText(&program, fontTextureID, std::to_string(val1), 2, -0.1, glm::vec3(-2, -4, 0));
        Util::DrawText(&program, fontTextureID, std::to_string(val2), 2, -0.1, glm::vec3(-2, -5, 0));
    }

    if (currentScene->state.gameSuccess)
    {
        Mix_PlayChannel(-1, gameSuccess, 0);
        Util::DrawText(&program, fontTextureID, "You Win!", 0.75, -0.1, glm::vec3(-2, 0, 0));
        currentScene->state.target[0].isActive = false;
        currentScene->state.respawn = false;
    }
    if (currentScene->state.gameFailed)
    {
        Mix_PlayChannel(-1, gameFailed, 0);
        Util::DrawText(&program, fontTextureID, "You Lost!", 0.75, -0.1, glm::vec3(-2, 0, 0));
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