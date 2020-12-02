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

enum EntityType {MOUSE, TARGET};

enum AIType { AILINEAR };
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

    GLuint textureID;

    glm::mat4 modelMatrix;

    int spawn_x = (rand() % 30) - 15; //to get random value ranging from -14 to 14
    int spawn_y = (rand() % 15) - 8; //to get random value ranging from -7 to 7


    float width = 1;
    float height = 1;

    bool isActive = true;
    bool respawn = false;

    Entity();
    Entity* LastCollidedEntity;
    EntityType LastCollided;

    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);

    bool CheckCollision(Entity* other);
    void Update(float deltaTime, Entity* player, Entity* objects, int objectCount);
    void Render(ShaderProgram* program);

    void AI(Entity* player);
    void AILinear();

};