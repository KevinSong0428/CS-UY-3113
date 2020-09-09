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
#include "stb_image.h";

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, ctgMatrix, projectionMatrix, chiefMatrix, shipMatrix;

float player_x = 0;
float chief_x = -4.5, chief_y = 3.0;
float chief_scale_x = 1.0, chief_scale_y = 1.0;
float player_rotate_clockwise = 0, player_rotate_counter = 0;

GLuint playerTextureID, cheifTextureID, shipTextureID;
GLuint LoadTexture(const char* filePath)
{
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
    ctgMatrix = glm::mat4(1.0f);
    chiefMatrix = glm::mat4(1.0f);
    shipMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //cheifTextureID = LoadTexture("IU.jpg");
    playerTextureID = LoadTexture("ctg.png");
    cheifTextureID = LoadTexture("mchief.png");
    shipTextureID = LoadTexture("ship.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void move_left()
{
    ctgMatrix = glm::translate(ctgMatrix, glm::vec3(player_x, 0.0f, 0.0f));
    ctgMatrix = glm::rotate(ctgMatrix, glm::radians(player_rotate_counter), glm::vec3(0.0f, 0.0f, 1.0f));
}

void move_right()
{
    ctgMatrix = glm::translate(ctgMatrix, glm::vec3(player_x, 0.0f, 0.0f));
    ctgMatrix = glm::rotate(ctgMatrix, glm::radians(player_rotate_clockwise), glm::vec3(0.0f, 0.0f, 1.0f));
}

bool right_edge = false, left_edge = false;

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    player_rotate_counter += 90.0f * deltaTime; //rotates counterclockwise
    player_rotate_clockwise += -90.0f * deltaTime; //rotates clockwise

    ctgMatrix = glm::mat4(1.0f);

    if (player_x >= 3.75f) right_edge = true;

    if (player_x <= -3.75f) right_edge = false;

    if (right_edge)
    {
        player_x += -1.0f * deltaTime;
        move_left();
    }

    if (!right_edge)
    {
        player_x += 1.0f * deltaTime;
        move_right();
    }

    chief_x += 1.0f * deltaTime;
    chief_y -= 0.75f * deltaTime;
    chief_scale_x += 0.25 * deltaTime;
    chief_scale_y += 0.25 * deltaTime;

    chiefMatrix = glm::mat4(1.0f);

    chiefMatrix = glm::translate(chiefMatrix, glm::vec3(chief_x, chief_y, 0.0f));
    chiefMatrix = glm::scale(chiefMatrix, glm::vec3(chief_scale_x, chief_scale_y, 1.0f));

    shipMatrix = glm::mat4(1.0f);
    shipMatrix = glm::translate(shipMatrix, glm::vec3(4.0f, -3.0f, 0.0f));
    shipMatrix = glm::rotate(shipMatrix, glm::radians(player_rotate_counter), glm::vec3(0.0f, 0.0f, 1.0f));

}

void DrawOne()
{
    program.SetModelMatrix(ctgMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawTwo()
{
    program.SetModelMatrix(chiefMatrix);
    glBindTexture(GL_TEXTURE_2D, cheifTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawThree()
{
    program.SetModelMatrix(shipMatrix);
    glBindTexture(GL_TEXTURE_2D, shipTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);


    DrawOne();
    DrawTwo();
    DrawThree();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

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
