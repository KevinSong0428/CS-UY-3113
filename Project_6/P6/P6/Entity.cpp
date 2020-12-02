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

//bool Entity::CheckCollision(Entity* other)
//{
//    if (other == this) return false; //cannot collide with itself (the buggy in and out movement)
//    if (isActive == false || other->isActive == false) return false;
//    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
//    float ydist = fabs(position.y - other->position.y) - ((width + other->width) / 2.0f);
//
//    if (xdist < 0 && ydist < 0) return true;
//
//    return false;
//}

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

/*
//if collided --> see how much collided into the other platform
//if into the platofrm below, move object up that penetration amount
void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0)
            {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                LastCollided = object->entityType;
                LastCollidedEntity = object;
            }
            else if (velocity.y < 0)
            {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                LastCollided = object->entityType;
                LastCollidedEntity = object;
                if (entityType == MOUSE)
                {
                    if (LastCollided == TARGET)
                    {
                        position.y -= penetrationY;
                        LastCollidedEntity->isActive = false;
                    }
                }
            }
        }
    }
}

//if collided --> see how much collided into the other platform
//if into the platofrm below, move object down that penetration amount
void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0)
            {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                LastCollided = object->entityType;
                LastCollidedEntity = object;
                if (entityType == MOUSE && LastCollided == TARGET)
                {
                    position.x += penetrationX;
                }
            }
            else if (velocity.x < 0)
            {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                LastCollided = object->entityType;
                LastCollidedEntity = object;
                if (entityType == MOUSE && LastCollided == TARGET)
                {
                    position.x -= penetrationX;
                }
            }
        }
    }
}
*/


void Entity::AILinear()
{
    //generate random direction and then move
}

//input of an entity so that entity could react
void Entity::AI(Entity* mouse)
{
    switch (aiType)
    {
    case LINEAR:
        AILinear();
        break;
    }

}

void Entity::Update(float deltaTime, Entity* mouse, Entity* objects, int objectCount)
{

    if (entityType == TARGET)
    {
        AI(mouse);
    }

    bool hit = false;
    if (entityType == MOUSE)
    {
        hit = CheckCollision(objects);
    }

    if (hit)
    {
        objects->position = glm::vec3(0);
        hit = false;
        objects->isActive = false;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
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