#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_AUDIO);

	std::cout << "Hawk Tuah\n";

	SDL_Quit();
	return 0;
}
