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

enum AIType { AILINEAR, AIROAM };
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

    GLuint textureID;

    glm::mat4 modelMatrix;

    float width = 1;
    float height = 1;

    bool isActive = true;
    bool respawn = false;

    bool walkLeft = false;
    bool walkRight = true;

    Entity();

    bool CheckCollision(Entity* other);
    void Update(float deltaTime, Entity* objects, int objectCount);
    void Render(ShaderProgram* program);
    void switchDirection();

    void AILinear();
    void AIRoam(float deltaTime);

};