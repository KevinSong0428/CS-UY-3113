#include <string>
using namespace std;

//The heart of Game Programming

void Startup();
void gameIsRunning();
void ProcessInput();
void Update();
void Render();
void Shutdown();

int main()
{
	Startup();

	/*while game is runnning, it will continuously process user input and use 
	that to update the game. Then after updating the data, make sure to use render
	function.*/

	while (gameIsRunning)
	{
		ProcessInput();
		Update;
		Render();
	}

	Shutdown();
}