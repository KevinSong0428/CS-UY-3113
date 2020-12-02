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
float seconds = -1.0;
time_t start;

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
}

bool CheckCollision(Entity* mouse, Entity* other)
{
    if (other->isActive == false) return false;
    if (mouse->position.x >= other->position.x &&                       // right of the left edge
        mouse->position.x <= other->position.x + other->width &&        // left of the right edge
        mouse->position.y >= other->position.y &&                       // below the top
        mouse->position.y <= other->position.y + other->height) {       // above the bottom
        return true;
    }
    return false;
}


void ProcessInput() {

    currentScene->state.mouse->movement = glm::vec3(0);

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

        case SDL_MOUSEBUTTONDOWN:
            int x = event.button.x;
            int y = event.button.y;
            int unit_x = ((x / 1280) * 18) - (18 / 2.0);
            int unit_y = (((1280 - y) / 32) * 720) - (720 / 2.0);
            
            if (currentScene != sceneList[0])
            {
                bool hit = CheckCollision(currentScene->state.mouse, currentScene->state.target);
                if (hit)
                {
                    //currentScene->state.target->isActive = false;
                    Mix_PlayChannel(-1, gameSuccess, 0);
                }
                currentScene->state.mouse[0].position = glm::vec3(unit_x, unit_y, 0);
            }
            break;

            break; // SDL_KEYDOWN
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

    //int x, y;
    //SDL_GetMouseState(&x, &y);
    //int unit_x = ((x / 1280) * 18) - (18 / 2.0);
    //int unit_y = (((1280 - y) / 32) * 720) - (720 / 2.0);

    //if (currentScene != sceneList[0]) currentScene->state.mouse[0].position = glm::vec3(unit_x, unit_y, 0);

    if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(SDL_BUTTON_LEFT) && currentScene != sceneList[0])
    {
        bool hit = CheckCollision(currentScene->state.mouse, currentScene->state.target);
        if (hit)
        {
            //currentScene->state.target->isActive = false;
            Mix_PlayChannel(-1, gameSuccess, 0);
        }
        
        //call update function to check if collides with enemy entity, if so then make noise
            //make enemy render false
        //CheckCollision(currentScene->state.mouse, currentScene->state.target);
        //currentScene->state.mouse->CheckCollision(currentScene->state.target);

        currentScene->state.click = true;
    }

    if (glm::length(currentScene->state.mouse->movement) > 1.0f) {
        currentScene->state.mouse->movement = glm::normalize(currentScene->state.mouse->movement);
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

    while (deltaTime >= FIXED_TIMESTEP) 
    {
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    if (seconds < currentScene->state.time_limit &&
        currentScene != sceneList[0])
    {
        seconds += deltaTime;
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    
    //if (currentScene->state.mouse->enemyAttack)
    //{
    //    currentScene->state.mouse->enemyAttack = false;
    //}

    //if (currentScene->state.mouse->enemyKilled)
    //{
    //    currentScene->state.mouse->enemyKilled = false;
    //    Mix_PlayChannel(-1, squash, 0);
    //}


    //update score


}

void Render() 
{
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);

    GLuint fontTextureID = Util::LoadTexture("font1.png");
    glm::vec3 scorePosition, centerText;
    if (currentScene->state.mouse->position.x > 5)
    {
        scorePosition = glm::vec3(currentScene->state.mouse->position.x - 4.3, -0.5, 0);
        centerText = glm::vec3(currentScene->state.mouse->position.x - 1.5, -3, 0);
    }
    else
    {
        scorePosition = glm::vec3(0.7, -0.5, 0);
        centerText = glm::vec3(3.5 , -3, 0);
    }

    int limit = currentScene->state.time_limit;
    if (currentScene != sceneList[0]) Util::DrawText(&program, fontTextureID, "Time Limit: " + std::to_string(limit) + " sec", 0.75, -0.1, glm::vec3(1, 5, 0));
    if (currentScene != sceneList[0]) Util::DrawText(&program, fontTextureID, "Timer: " + std::to_string(int(seconds)) + " sec", 0.75, -0.1, glm::vec3(2.5, 4, 0));
    if (currentScene != sceneList[0]) Util::DrawText(&program, fontTextureID, "Score: " + std::to_string(score), 0.5, -0.1, scorePosition);


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