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
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, 
space1Matrix, space2Matrix, space3Matrix, space4Matrix;

Scene* currentScene;
Scene* sceneList[2];

int score = 0;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

//variable for musics
Mix_Music* music;
Mix_Chunk* laser;
Mix_Chunk* gameSuccess;
Mix_Chunk* gameFailed;

//space background
GLuint space1TextureID, space2TextureID;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Shoot the Aliens!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 720);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("supernatural.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 10);

    /*
    Supernatural by Kevin MacLeod
    Link: https://incompetech.filmmusic.io/song/4446-supernatural
    License: http://creativecommons.org/licenses/by/4.0/ 
    */

    laser = Mix_LoadWAV("laser.wav");
    gameSuccess = Mix_LoadWAV("success.wav");
    gameFailed = Mix_LoadWAV("failure.wav");
    Mix_Volume(-1, MIX_MAX_VOLUME / 10);

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    space1Matrix = glm::mat4(1.0f);
    space2Matrix = glm::mat4(1.0f);
    space3Matrix = glm::mat4(1.0f);
    space4Matrix = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);

    //background
    space1TextureID = Util::LoadTexture("space1.png");
    space2TextureID = Util::LoadTexture("space2.png");
    space1Matrix = glm::translate(space1Matrix, glm::vec3(8, 4.5, 0));
    space1Matrix = glm::scale(space1Matrix, glm::vec3(16, 9, 1));
    space2Matrix = glm::translate(space2Matrix, glm::vec3(-8, 4.5, 0));
    space2Matrix = glm::scale(space2Matrix, glm::vec3(16, 9, 1));
    space3Matrix = glm::translate(space3Matrix, glm::vec3(-8, -4.5, 0));
    space3Matrix = glm::scale(space3Matrix, glm::vec3(16, 9, 1));
    space4Matrix = glm::translate(space4Matrix, glm::vec3(8, -4.5, 0));
    space4Matrix = glm::scale(space4Matrix, glm::vec3(16, 9, 1));

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    
    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    SwitchToScene(sceneList[0]);

}

bool CheckCollision(float unit_x, float unit_y, Entity* other)
{
    if (other->isActive == false) return false;
    if (unit_x >= other->position.x - (other->width / 2) &&         // right of the left edge
        unit_x <= other->position.x + (other->width / 2) &&         // left of the right edge
        unit_y >= other->position.y - (other->height / 2) &&        // below the top
        unit_y <= other->position.y + (other->height / 2))          // above the bottom
    {       
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
            float x = event.button.x;
            float y = event.button.y;
            int button = event.button.button;   // button that was clicked (1,2,3)

            float unit_x = ((x / 1280.0) * 32.0) - (32.0 / 2.0);
            float unit_y = (((720.0 - y) / 720.0) * 18.0) - (18.0 / 2.0);            

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
                    Mix_PlayChannel(-1, laser, 0);
                    score++;
                }
            }
            break;

        }
    }
    
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (!currentScene->state.gameSuccess)
    {
        if (currentScene == sceneList[0] && keys[SDL_SCANCODE_RETURN])
        {
            currentScene->state.startGame = true;
            SwitchToScene(sceneList[1]);
        }
    }

    if (currentScene->state.gameSuccess ||
        currentScene->state.gameFailed)
    {
        if (keys[SDL_SCANCODE_SPACE])
        {
            currentScene->state.gameFailed = false;
            currentScene->state.gameSuccess = false;
            sceneList[0]->state.level = 1;
            sceneList[0]->state.startGame = false;
            sceneList[1]->state.level = 1;
            score = 0;
            currentScene->state.time = 61.0;
            sceneList[0]->state.level = true;
            SwitchToScene(sceneList[0]);
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
        if ((currentScene->state.spawnTime - currentScene->state.time) >= 
            sceneList[1]->state.durationTime)
        {
            currentScene->state.target[0].respawn = true;
        }
    }


    if (currentScene->state.time < -1.0)
    {
        currentScene->state.time = 0.0;
        currentScene->state.gameFailed = true;
    }

    //winning condition for level
    if (currentScene != sceneList[0] &&
        currentScene->state.goal == score && 
        !currentScene->state.gameSuccess)
    {
        if (currentScene->state.level == 5)
        {
            currentScene->state.gameSuccess = true;
        }
        else
        {
            sceneList[0]->state.level++;
            sceneList[0]->state.startGame = false;
            sceneList[1]->state.level++;
            score = 0;
        }
    }
}

void drawBackground()
{
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(space1Matrix);
    glBindTexture(GL_TEXTURE_2D, space1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(space2Matrix);
    glBindTexture(GL_TEXTURE_2D, space2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(space3Matrix);
    glBindTexture(GL_TEXTURE_2D, space1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(space4Matrix);
    glBindTexture(GL_TEXTURE_2D, space2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

}

void Render() 
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawBackground();

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
    }

    if (currentScene->state.gameSuccess &&
        !currentScene->state.gameFailed)
    {
        Mix_PlayChannel(-1, gameSuccess, 1);
        Util::DrawText(&program, fontTextureID, "You Win!", 0.75, -0.1, glm::vec3(-2, 0, 0));
        Util::DrawText(&program, fontTextureID, "Press Spacebar to Play Again!", 0.75, -0.1, glm::vec3(-9.7, -1, 0));
    }
    if (currentScene->state.gameFailed &&
        !currentScene->state.gameSuccess)
    {
        Mix_PlayChannel(-1, gameFailed, 1);
        Util::DrawText(&program, fontTextureID, "You Lost!", 0.75, -0.1, glm::vec3(-2, 0, 0));
        Util::DrawText(&program, fontTextureID, "Press Spacebar to Play Again!", 0.75, -0.1, glm::vec3(-9.7, -1, 0));
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