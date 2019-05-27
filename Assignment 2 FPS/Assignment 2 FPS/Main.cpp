/*	FIT2096 - Week 1 Example Code
*	Main.cpp
*	Created by Elliott Wilson - 2015 - Monash University
*	Entry point and main driver for the application
*/

#include "Window.h"
#include <time.h>

void CreateConsole()
{
	AllocConsole();
	FILE* out;
	freopen_s(&out, "CONOUT$", "w", stdout);
}


//Windows API programs have a special Main method, WinMain!
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{

	CoInitialize(0);

	//CreateConsole();

	Window* win = new Window(L"FPS renderer", 1280, 720, false);	//We'll create our window object, set a size and if we want it fullscreen

	if (win->Initialise())	//If the window initialises...
	{
		try {
			win->Start();		//...then we start the message pump running!
		}
	
		catch (int a) {
			std::cout << "Exception:"<< std::to_string(a)<< ".\n";
			MessageBox(NULL, L"An exception has been thrown", L"Exception thrown", MB_OK); //otherwise, show an error message
		}
	}
	else
	{
		MessageBox(NULL, L"Could not create Window", L"Error", MB_OK); //otherwise, show an error message
	}

	win->Shutdown();	//If we have an error, or the message pump stops then we Shutdown our window...
	delete win;			//...delete it...
	win = NULL;

	return 0;			//...and quit!




}


/* error codes

1: 
2:
3:
4: Mesh not yet loaded
5: SpriteFont not yet loaded
6: Attempting to instatiate a spritemanager before initialising the class.
7: Attempting to create a bullet without initialising the bullet class.
*/