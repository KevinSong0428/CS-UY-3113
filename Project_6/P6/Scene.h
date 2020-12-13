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
#include "Util.h"
#include "Entity.h"
#include <time.h>

struct GameState {
	Entity* target;
	int nextScene, goal;
	float time = -1.0;
	int level = 1;
	float spawnTime = 61.0;
	bool startGame = false;
	bool gameFailed = false;
	bool gameSuccess = false;
	bool levelCleared = false;
	bool respawn = false;
	
	//durationTime = how long target is active for every level
	float durationTime;
};

class Scene {
public:
	GameState state;
	virtual void Initialize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render(ShaderProgram* program) = 0;
};
