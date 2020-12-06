#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other)
{
    if (isActive == false || other->isActive == false) return false;
    if (position.x >= other->position.x &&         // right of the left edge AND
        position.x <= other->position.x + width &&    // left of the right edge AND
        position.y >= other->position.y &&         // below the top AND
        position.y <= other->position.y + height) {    // above the bottom
        return true;
    }
    return false;
}


void Entity::AILinear()
{
    //generate random direction and then move
    if (position.x > 16 - width / 2 ||  //don't let it go past right boundary
        position.x < -16 + width / 2)   //don't let it go past left boundary
    {
        movement.x *= -1;
    }
    if (position.y > 9 - height / 2 ||   //don't let it go past upper boundary
        position.y < -9 + height / 2)    //don't let it go past lower boundary
    {
        movement.y *= -1;
    }
}

void Entity::Update(float deltaTime, Entity* objects, int objectCount)
{

    if (entityType == TARGET)
    {
        AILinear();
    }

    modelMatrix = glm::mat4(1.0f);

    float move = width / 2;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0));

    float scale_x = width;
    float scale_y = height;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_x, scale_y, 1));

    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;
    position.y += velocity.y * deltaTime;
    position.x += velocity.x * deltaTime;

}


void Entity::Render(ShaderProgram* program) 
{

    program->SetModelMatrix(modelMatrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

}