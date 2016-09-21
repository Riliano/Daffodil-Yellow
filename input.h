#include<SDL2/SDL.h>

struct keyTable_t
{
	bool movUp = false;
	int keyMovUp = SDL_SCANCODE_W;
	bool movDown = false;
	int keyMovDown = SDL_SCANCODE_S;
	bool movLeft = false;
	int keyMovLeft = SDL_SCANCODE_A;
	bool movRight = false;
	int keyMovRight = SDL_SCANCODE_D;
	bool attUp = false;
	int keyAttUp = SDL_SCANCODE_UP;
	bool attDown = false;
	int keyAttDown = SDL_SCANCODE_DOWN;
	bool attLeft = false;
	int keyAttLeft = SDL_SCANCODE_LEFT;
	bool attRight = false;
	int keyAttRight = SDL_SCANCODE_RIGHT;
	bool leftSpell = false;
	int keyLeftSpell = SDL_SCANCODE_Q;
	bool rightSpell = false;
	int keyRightSpell = SDL_SCANCODE_E;
	bool quickSpell = false;
	int keyQuickSpell = SDL_SCANCODE_SPACE;
	bool sprint = false;
	int keySprint = SDL_SCANCODE_LSHIFT;

	bool pause = false;
	int keyPause = SDL_SCANCODE_ESCAPE;
};

keyTable_t input;
const Uint8 *keyboardState = SDL_GetKeyboardState( NULL );

void ScanKeyboard()
{
	if( keyboardState[input.keyMovUp] )
		input.movUp = true;
	if( keyboardState[input.keyMovDown] )
		input.movDown = true;
	if( keyboardState[input.keyMovLeft] )
		input.movLeft = true;
	if( keyboardState[input.keyMovRight] )
		input.movRight = true;
	if( keyboardState[input.keyAttUp] )
		input.attUp = true;
	if( keyboardState[input.keyAttDown] )
		input.attDown = true;
	if( keyboardState[input.keyAttLeft] )
		input.attLeft = true;
	if( keyboardState[input.keyAttRight] )
		input.attRight = true;
	if( keyboardState[input.keyLeftSpell] )
		input.leftSpell = true;
	if( keyboardState[input.keyRightSpell] )
		input.rightSpell = true;
	if( keyboardState[input.keyQuickSpell] )
		input.quickSpell = true;
	if( keyboardState[input.keySprint] )
		input.sprint = true;
	if( keyboardState[input.keyPause] )
		input.pause = true;
}
/*void scanMouse(SDL_Event e)
{
	if(e.button.button == SDL_BUTTON_LEFT)
		input.attack = true;
	if(e.button.button == SDL_BUTTON_RIGHT)
		input.heal = true;
}*/
void ResetInput()
{	
	input.movUp = false;
	input.movDown = false;
	input.movLeft = false;
	input.movRight = false;
	input.attUp = false;
	input.attDown = false;
	input.attLeft = false;
	input.attRight = false;
	input.leftSpell = false;
	input.rightSpell = false;
	input.quickSpell = false;
	input.sprint = false;
	input.pause = false;
}
