/* ex1.c Originally published on About.com 2012, now on dice.com. Author D. Bolton then and now.
Version 2.0 Updated June 14th,2017 */

#include <SDL.h>
#include <stdio.h>
#include "sdlgui.h"

const SCREEN_WIDTH = 800;
const SCREEN_HEIGHT = 500;
const QUITKEY = SDLK_ESCAPE;
const Uint32 BLINKPERIOD = 450;
#define NUMIMAGES 1

const char * imagenames[NUMIMAGES] = { "dice.png" };
SDL_Surface* images[NUMIMAGES];
SDL_Surface* screen;
SDL_Event event;
SDL_Rect source, destination;
psdlcontrol panel;
char gameovermsg[100];
struct sdllabel * mylabel;
struct sdllistbox * listbox;
int framecount;
char buffer[255];
SDL_Window* window = NULL;
SDL_Renderer *renderer = NULL;
struct sdlbutton * button1;
struct sdlbutton * button2;
struct sdlbutton * button4;
struct sdlcheckbox * checkbox;
int mousebuttonx, mousebuttony,mousex,mousey,mousexmove, mouseymove, mousebutton;
int gameRunning,tickcount,paused,lasttick,gameExited,blinktime;
int prodpanel,keypressed, pauseblink,showfps;

extern void Log(pchar message);

/* Loads all graphical images */
void LoadAllImages() {
	for (int i = 0; i<NUMIMAGES; i++) {
		images[i] = IMG_Load(imagenames[i]);
		if (!images[i])
			printf("Failed to load %s", imagenames[i]);
	}
}


void SetCaption(char * title) {
	SDL_SetWindowTitle(window,title);
}


void ShowMessage(pchar message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
		"You clicked",
		message,
		window);
}
void menu1(psdlcontrol self) {
	ShowMessage("First");
}

void menu2(psdlcontrol self) {
	ShowMessage("Second");
}

void menu3(psdlcontrol self) {
	ShowMessage("Third");
}

void clickbutton(psdlcontrol self) {
	mylabel->labeltext = " You changed me to a much longer string";
}

void clickbutton2(psdlcontrol self) {
	SetCaption("Hi from David");
	showfps = 0;
	checkbox->isChecked = 0;
}

void clicktoremove(psdlcontrol self) {
	gameRunning = 0;
}

void togglefps(psdlcontrol self) {
	showfps = 1 - showfps;
	if (!showfps) {
		SetCaption("");
	}
}

int Random(int max) {
	return (rand() % max) + 1;
}

void clickimage(psdlcontrol self) {
	SDL_Rect rect = { (Sint16)0,(Sint16)0,(Uint16)images[0]->w,(Uint16)images[0]->h };
	for (int i = 0; i < 100; i++) {
		addimage(Random(400)+50, Random(400) , images[0], NULL);
	}
	SDL_UpdateWindowSurface(window); // flip to show surface
}

// colour is colour of panel
int InitPanel(int colour) {

	panel = initsdlguilib(100, 50, 600,400, colour);
	if (!panel) {
		return 0;
	};
	mylabel = (struct sdllabel *)addlabel(10, 20, "Test Controls", twhite);
	button1 = (struct sdlbutton *)addbutton(10, 50, "Change Label", twhite, clickbutton);
	button2 = (struct sdlbutton *)addbutton(10, 90, "Say Hi-in Caption",tblue, clickbutton2);

	checkbox = (struct sdlcheckbox *)addcheckbox(10, 130, "Show FPS", tgreen, togglefps);
	listbox = (struct sdllistbox *)addlistbox(10, 170, "Click First, Second or Third", gray, twhite, 3, 1);
	addlistitem(listbox, "First", tred, menu1);
	addlistitem(listbox, "Second", tblue, menu2);
	addlistitem(listbox, "Third", tgreen, menu3);
		
	addimage(10, 300, images[0], clickimage);
	button4 = (struct sdlbutton *)addbutton(510, 360, "Close", tblack, clicktoremove);	
	return 1;
}



void ShowDebug() {
	SDL_Color col = twhite;
	//sprintf_s(buffer,254,"Top Left %d,%d",hx,hy);
	ttf_print(25, 720, buffer, col);
	sprintf_s(buffer, 254, "Debug mx %d my %d mxr %d myr %d mb %d mbx %d mby:%d",
		mousex, mousey, mousexmove, mouseymove, mousebutton, mousebuttonx, mousebuttony);
	ttf_print(250, 720, buffer, col);
}

void RenderScreen() {
	if (!screen)
		return;
	
	SDL_FillRect(screen, NULL, gray);//Fill the surface gray
	//ShowDebug(); // uncomment for mouse moves
	RenderGUI(panel);
	SDL_UpdateWindowSurface(window); // flip to show surface
}

void RunGame(){
	framecount = 0;
	gameRunning = 1;
	prodpanel = 0;
	showfps = 0;
	while (gameRunning)
	{
		framecount++;
		tickcount = SDL_GetTicks();

		if (tickcount - lasttick >= 1000) {
			lasttick = tickcount;
			if (showfps) {
				int GuiCount = GetGuiObjects(panel);
				sprintf_s(buffer, 32, "FPS = %d GUI Count = %d", ++framecount,GuiCount);
				SetCaption(buffer);
			}
			framecount = 0;
		}
		if (paused && ((SDL_GetTicks() - blinktime)> BLINKPERIOD)) {
			pauseblink ^= 1;
			blinktime = SDL_GetTicks();
		}
		else /* main game play action here */
		{
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_MOUSEMOTION:
						mousex = event.motion.x;
						mousey = event.motion.y;
						mousexmove = event.motion.xrel;
						mouseymove = event.motion.yrel;
						break;

					case SDL_MOUSEBUTTONDOWN:
						mousebutton = event.button.button;
						mousebuttonx = event.button.x;
						mousebuttony = event.button.y;

						if (((mousebutton &  SDL_BUTTON_LEFT) == SDL_BUTTON_LEFT)) {
								CheckControlClickAt(mousebuttonx, mousebuttony);
							};
						break;
					case SDL_KEYDOWN:
						keypressed = event.key.keysym.sym;
						if (keypressed == QUITKEY)
						{
							strcpy_s(gameovermsg, 100, "User hit the ESC Key");
							gameRunning = 0;
							gameExited = 1;
							goto exitgame;
						}

						//ProcessKey();
						break;
					case SDL_QUIT: /* if mouse click to close window */
					{
						gameRunning = 0;
						gameExited = 1;
						goto exitgame;
						break;
					}
				} /* switch */

			} /* while SDL_PollEvent */
		}
		RenderScreen();

		while (SDL_GetTicks() - tickcount < 17) {
			SDL_PumpEvents(); /* delay it to ~60 fps */
		}
	} /* while (GameRunning) */
exitgame:
return;
}


int InitSetup() {	
	//The window we'll be rendering to
	window = NULL;
	screen = NULL;
	//The surface contained by the window

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return 0;
		}
		else
		{
			LoadAllImages();
			//Get window surface
			screen = SDL_GetWindowSurface(window);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			//Fill the surface white
			SDL_FillRect(screen, NULL, white);

			//Update the surface
			SDL_UpdateWindowSurface(window);

		}			
		return 1;
	}
}

void FinishOff() {
	SDL_DestroyWindow(window);
	SDL_Quit();
	return;
}



int main(int argc, char* args[])
{	
	if (!InitSetup()) {
		printf("Initsetup failed");
		return 1;
	}
	InitColours();
	if (InitPanel(red)) {
		RunGame();
	}
	FinishOff();
	return 0;
}