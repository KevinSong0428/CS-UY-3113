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

#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

//ranges from 0 to 29
#define PLATFORM_COUNT 30


struct GameState {
    Entity* ship;
    Entity* platforms;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameSuccess = false;
bool gameFailed = false;

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

void Initialize() {
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

    // Initialize Player
    state.ship = new Entity();
    state.ship->position = glm::vec3(0, 4.5, 0);
    state.ship->movement = glm::vec3(0);
    state.ship->acceleration = glm::vec3(0, -0.1, 0);
    state.ship->speed = 1.5f;
    state.ship->textureID = LoadTexture("playerShip.png");

    state.ship->height = 1.0f;
    state.ship->width = 1.0f;

    //make new platform and initialize
    state.platforms = new Entity[PLATFORM_COUNT];
    
    GLuint platformTextureID = LoadTexture("platformPack_tile041.png");
    GLuint landingTextureID = LoadTexture("platformPack_tile015.png");
    fontTextureID = LoadTexture("font1.png");

    //bottom tiles left of land
    for (int i = 0; i < 6; i++)
    {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(i - 4.5, -3.25, 0);
    }

    //state.platforms[5 and 6] are landing zones
    for (int i = 0; i < 2; i++)
    {
        state.platforms[i + 6].textureID = landingTextureID;
        state.platforms[i + 6].position = glm::vec3(i + 1.5, -3.25, 0);
    }


    //bottom tiles right of land
    for (int i = 0; i < 2; i++)
    {
        state.platforms[i + 8].textureID = platformTextureID;
        state.platforms[i + 8].position = glm::vec3(i + 3.5, -3.25, 0);
    }

    //left tiles of the screen
    for (int i = 0; i != 7; i++)
    {
        state.platforms[i + 10].textureID = platformTextureID;
        state.platforms[i + 10].position = glm::vec3(-4.5, -2.25 + i, 0);
    }
    
    //right tiles of the screen
    for (int i = 0; i != 7; i++)
    {
        state.platforms[i + 17].textureID = platformTextureID;
        state.platforms[i + 17].position = glm::vec3(4.5, -2.25 + i, 0);
    }

    //random three tiles
    for (int i = 0; i < 3; i++)
    {
        state.platforms[i + 24].textureID = platformTextureID;
        state.platforms[i + 24].position = glm::vec3(i + 0.75, 2.25, 0);
    }

    //random two tiles
    for (int i = 0; i < 2; i++)
    {
        state.platforms[i + 27].textureID = platformTextureID;
        state.platforms[i + 27].position = glm::vec3(i - 2.8, -0.25, 0);
    }

    //random tile above land
    state.platforms[29].textureID = platformTextureID;
    state.platforms[29].position = glm::vec3(2.0, 0.25, 0);


    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].Update(0, NULL, 0);
    }

}

void ProcessInput() {

    state.ship->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if (!state.ship->collidedBottom && !gameFailed && !gameSuccess)
        {
            state.ship->acceleration.x += -0.05;
        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (!state.ship->collidedBottom && !gameFailed && !gameSuccess)
        {
            state.ship->acceleration.x += 0.05;
        }
    }

    if (glm::length(state.ship->movement) > 1.0f) {
        state.ship->movement = glm::normalize(state.ship->movement);
    }

}

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
        //if not enough time has gone by from previous update, it will accumulate
        accumulator = deltaTime;
        return;
    }

    //while enough time has gone by (60 fps) it will call update on player
    //so if there's only a little amount of time left, it will exit the loop
    //ex: 3.5s, once per second, it will happen only 3x
    //***THIS IS TO UPDATE AT THE SAME TIME INTERVAL
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.ship->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    //if collided with landing platforms then success
    if (state.ship->other == &state.platforms[6] || state.ship->other == &state.platforms[7])
    {
        gameSuccess = true;
    }

    //if collided with anything anything else but the two landing platforms, it will fail
    if (state.ship->collidedBottom || state.ship->collidedLeft || state.ship->collidedRight)
    {
        gameFailed = true;
    }

    if (gameSuccess || gameFailed)
    {
        state.ship->acceleration = glm::vec3(0);
        state.ship->movement = glm::vec3(0);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    //draw the platform before the player
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].Render(&program);
    }

    state.ship->Render(&program);

    if (gameSuccess)
    {
        DrawText(&program, fontTextureID, "Mission Success!", 0.5, -0.1, glm::vec3(-3, 0, 0));
    }

    if (gameFailed && !gameSuccess)
    {
        DrawText(&program, fontTextureID, "Mission Failed!", 0.5, -0.1, glm::vec3(-2.5, 0, 0));
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