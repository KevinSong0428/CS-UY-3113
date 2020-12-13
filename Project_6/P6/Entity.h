#pragma once
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

enum EntityType {TARGET};

enum AIType { AILINEAR, AICIRCLE1, AICIRCLE2};
enum AIState { LINEAR};

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;

    float speed;
    float angle = 0.0f;
    int roamRand = 0;

    GLuint textureID;

    glm::mat4 modelMatrix;

    float width = 1;
    float height = 1;

    bool isActive = true;
    bool respawn = false;

    Entity();

    void Update(float deltaTime, Entity* objects);
    void Render(ShaderProgram* program);

    void AILinear(float deltaTime);
    void AICircle1(float deltaTime);
    void AICircle2(float deltaTime);

};