#include "Mouse.h"

void Mouse::Initialize()
{
	state.mouse = new Entity();
	//stating what type mouse is to the program, not the state it's in, the type
	state.mouse->entityType = MOUSE;
	state.mouse->position = glm::vec3(2, 0, 0);
	state.mouse->movement = glm::vec3(0);

	state.mouse->height = 0.8f;
	state.mouse->width = 0.8f;

	//GLuint fontTextureID = Util::LoadTexture("font1.png");
}