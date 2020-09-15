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
#include "stb_image.h";

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, rightPlayer, leftPlayer, ball, projectionMatrix;

glm::vec3 right_position = glm::vec3(4.75, 0, 0);
glm::vec3 left_position = glm::vec3(-4.75, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);

glm::vec3 right_movement = glm::vec3(0, 0, 0);
glm::vec3 left_movement = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);



float player_speed = 1.0f;


//LOADING TEXTURE CODE
GLuint blockTextureID, fontTextureID;
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
    rightPlayer = glm::mat4(1.0f);
    leftPlayer = glm::mat4(1.0f);
    ball = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    //orth --> orthographic view (2D) distance forward and backwards don't matter, only left and right

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    fontTextureID = LoadTexture("font1.png");
}

bool gameOver = false;
bool restartGame = false;
bool gameStart = false;
bool freezePaddle = false;

void ProcessInput() 
{

    //IMPORTANT --> wanna reset player movement to zero (if nothing is pressed, don't wanna move
    right_movement = glm::vec3(0);
    left_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_SPACE:
                if (gameOver && gameStart)
                {
                    freezePaddle = false;
                    restartGame = true;
                    gameOver = false;
                    gameStart = false;
                }
                //ball starts to move --> increases position
                if (!gameStart)
                {
                    gameOver = false;
                    ball_movement.x = 3.0;
                    ball_movement.y = -3.0;
                    gameStart = true;
                }
                break;
            }
            break;
        }

    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    //hold up OR down key
    if (keys[SDL_SCANCODE_UP] && right_position.y < 2.85)
    {
        if (!freezePaddle) right_movement.y = 2.5f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && right_position.y > -2.85f)
    {
        if (!freezePaddle) right_movement.y = -2.5f;
    }

    if (keys[SDL_SCANCODE_W] && left_position.y < 2.85f)
    {
        if (!freezePaddle) left_movement.y = 2.5f;
    }
    else if (keys[SDL_SCANCODE_S] && left_position.y > -2.85f)
    {
        if (!freezePaddle) left_movement.y = -2.5f;
    }

}

float lastTicks = 0.0f;


//ball movements are 3.0
//paddle movements are 2.5
void Update()
{
    //game has to stop (paddle and ball does not and cannot be moved)
    if (ball_position.x >= 4.75 || ball_position.x <= -4.75)
    {
        gameOver = true;
        freezePaddle = true;
    }
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    bool left = false, right = true;
    //left true is going left
    //right true is going right

    //Collide with left block
    float Lxdist = fabs(left_position.x - ball_position.x) - ((0.5f) / 2.0f);
    float Lydist = fabs(left_position.y - ball_position.y) - ((1.8f) / 2.0f);
    if (Lxdist < 0 && Lydist < 0)
    {
        //LEFT AND BALL IS COLLIDING
        right = true;
        left = false;
        ball_movement.x = 3.0;
    }

    //Collide with right block
    float Rxdist = fabs(right_position.x - ball_position.x) - ((0.5f) / 2.0f);
    float Rydist = fabs(right_position.y - ball_position.y) - ((1.8f) / 2.0f);
    if (Rxdist < 0 && Rydist < 0)
    {
        //RIGHT AND BALL IS COLLIDING
        right = false;
        left = true;
        ball_movement.x = -3.0;
    }

    // Add (direction * units per second * elapsed time)
    right_position += right_movement * player_speed * deltaTime;
    rightPlayer = glm::mat4(1.0f);
    rightPlayer = glm::translate(rightPlayer, right_position);

    left_position += left_movement * player_speed * deltaTime;
    leftPlayer = glm::mat4(1.0f);
    leftPlayer = glm::translate(leftPlayer, left_position);

    //checks if ball has hit roof then starts going down
    if (ball_position.y >= 3.5) ball_movement.y = -3.0;

    //check if ball has hit floor starts going up
    if (ball_position.y <= -3.5) ball_movement.y = 3.0;

    ball_position += ball_movement * player_speed * deltaTime;
    ball = glm::mat4(1.0f);
    ball = glm::translate(ball, ball_position);
}

void drawObject(float* vertices)
{
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glBindTexture(GL_TEXTURE_2D, blockTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
}


void Render() 
{
    DrawText(&program, fontTextureID, "Press", 0.5, -0.25, glm::vec3(0));
    //if the ball has reached one of the sides, it can no longer move
    //set the ball movement to zero
    if (gameOver)
    {
        /*DrawText(&program, fontTextureID, "Press 'Space' to reset and space again to start", 0.5, -0.25, glm::vec3(0, 0, 0));*/
        ball_movement = glm::vec3(0);
    }

    if (restartGame)
    {
        freezePaddle = false;
        ball_position = glm::vec3(0, 0, 0);
        restartGame = false;
    }

    glClear(GL_COLOR_BUFFER_BIT);
 
    float Ball[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
    //vertices left paddle
    float LPaddle[] = { -0.25, -0.9, 0.25, -0.9, 0.25, 0.9, -0.25, -0.9, 0.25, 0.9, -0.25, 0.9 };
    //vertices right paddle
    float RPaddle[] = { -0.25, -0.9, 0.25, -0.9, 0.25, 0.9, -0.25, -0.9, 0.25, 0.9, -0.25, 0.9 };

    program.SetModelMatrix(ball);
    drawObject(Ball);
    program.SetModelMatrix(leftPlayer);
    drawObject(LPaddle);
    program.SetModelMatrix(rightPlayer);
    drawObject(RPaddle);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() 
{
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
