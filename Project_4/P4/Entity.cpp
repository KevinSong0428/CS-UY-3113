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
    if (other == this) return false;
    if (isActive == false || other->isActive == false)
    {
        return false;
    }
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((width + other->width) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
        return true;
    }
    return false;
}

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
                if (entityType == PLAYER)
                {
                    if (LastCollided == ENEMY)
                    {
                        LastCollidedEntity->isActive = false;
                    }
                }
            }
        }
    }
}

//if collided --> see how much collided into the other platform
//if into the platform below, move object down that penetration amount
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
            }
            else if (velocity.x < 0) 
            {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                LastCollided = object->entityType;
                LastCollidedEntity = object;
            }
        }
    }
}

void Entity::switchWalk()
{
    if (walkRight)
    {
        walkRight = false;
        walkLeft = true;
    }
    else
    {
        walkRight = true;
        walkLeft = false;
    }
}

void Entity::AIWaitAndGo(Entity* player)
{
    switch (aiState)
    {
    case IDLE:
        //switches from idle to walking when player is 3.0 away 
        if (glm::distance(position, player->position) < 3.0f)
        {
            aiState = WALKING;
        }
        break;
    case WALKING:
        if (walkRight)
        {
            movement = glm::vec3(1, 0, 0);
            if (position.x > glm::vec3(4.5).x) switchWalk();
        }
        if (walkLeft)
        {
            movement = glm::vec3(-1, 0, 0);
            if (position.x < glm::vec3(0.5).x) switchWalk();
        }
        break;
    }
}

void Entity::AIPatroller()
{
    if (walkRight)
    {
        movement = glm::vec3(1, 0, 0);
        //if passes right bound, go towards right
        if (position.x > glm::vec3(0).x) switchWalk();
    }
    if (walkLeft)
    {
        movement = glm::vec3(-1, 0, 0);
        if (position.x < glm::vec3(-2.5).x) switchWalk();
    }
}

void Entity::AIAttacker(Entity* player)
{
    switch (aiState)
    {
    case WALKING:
        if (glm::distance(position, player->position) < 5.0f)
        {
            aiState = ATTACKING;
        }
        if (walkRight)
        {
            movement = glm::vec3(1, 0, 0);
            if (position.x > glm::vec3(4.5).x) switchWalk();
        }
        if (walkLeft)
        {
            movement = glm::vec3(-1, 0, 0);
            if (position.x < glm::vec3(1.75).x) switchWalk();
        }
        break;
    case ATTACKING:
        if (walkRight)
        {
            movement = glm::vec3(2, 0, 0);
            if (position.x > glm::vec3(4.5).x)
            {
                switchWalk();
                jump = true;
            }
            if (jump)
            {
                jump = false;
                velocity.y += jumpPower;
            }
        }
        if (walkLeft)
        {
            movement = glm::vec3(-2, 0, 0);
            if (position.x < glm::vec3(2).x)
            {
                switchWalk();
                jump = true;
            }
            if (jump)
            {
                jump = false;
                velocity.y += jumpPower;
            }
        }
        break;
    }
}

//input of an entity so that entity could react
void Entity::AI(Entity* player)
{
    switch (aiType)
    {
    case PATROLLER:
        AIPatroller();
        break;

    case ATTACKER:
        AIAttacker(player);
        break;

    case WAITANDGO:
        AIWaitAndGo(player);
        break;
    }

    CheckCollision(player);

}

void Entity::Update(float deltaTime, Entity* player, Entity* enemies, int enemyCount, Entity* platforms, int platformCount)
{
    if (isActive == false) return;
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (entityType == ENEMY)
    {
        AI(player);
    }

    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }
 
    if (jump)
    {
        jump = false;
        velocity.y += jumpPower;
    }

    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; 
    CheckCollisionsY(platforms, platformCount);

    position.x += velocity.x * deltaTime; 
    CheckCollisionsX(platforms, platformCount);

    CheckCollisionsX(enemies, enemyCount);
    CheckCollisionsY(enemies, enemyCount);
    
    CheckCollisionsX(player, enemyCount);
    CheckCollisionsY(player, enemyCount);

    if (entityType == ENEMY)
    {
        if (LastCollidedEntity == player)
        {
            if (collidedLeft || collidedRight)
            {
                player->isActive = false;
            }
        }
    }

    if (entityType == PLAYER)
    {
        //if (LastCollided == ENEMY)
        //{
        //    if (collidedLeft || collidedRight || collidedTop)
        //    {
        //        isActive = false;
        //    }
        //    //else if (collidedBottom)
        //    //{
        //    //    LastCollidedEntity->isActive = false;
        //    //}
        //}
        if (position.y < -3.75) isActive = false;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) 
{
    if (!isActive) return;

    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

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