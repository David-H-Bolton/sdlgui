/* sdlgui.c Originally published on About.com 2012, now on dice.com. Author D. Bolton then and now.
Version 2.0 Updated June 14th,2017 */

#ifndef STDIO
#include <stdio.h>
#define STDIO
#endif

#ifndef IWINDOWS
#include <Windows.h>
#define IWINDOWS
#endif

#ifndef SDL
#include "SDL.h"   /* All SDL App's need this */
#include "SDL_image.h"
#include "SDL_ttf.h"
#endif

#define PLTEXT 0
#define PLHEX1 1
#define PLSPLASH 2
#define PLARMY 3
#define WIDTH 1024
#define HEIGHT 768

typedef void (funcptr) (struct sdlcontrol *);

enum controltype {esdlpanel,esdlbutton,esdlclickabletext,esdlcheckbox,esdllistbox,esdllistitem,
	edslpopup,esdltextentry,esdllabel,esdlimage};

enum gamestate {gsstarting,gsplaying,gsdone};

#define	sdlbase enum controltype ctype;\
int x,y,width,height,color,clickable,visible;\
SDL_Color textcolor;\
void (*pRender)(struct sdlcontrol * self);\
void (*pFree)(struct sdlcontrol * self);\
void (*pClick)(struct sdlcontrol * self);\
void (*pPreClick)(struct sdlcontrol * self);\
struct sdlcontrol * nextcontrol

struct sdlcontrol { sdlbase; };
typedef struct sdlcontrol * psdlcontrol;
typedef char * pchar;

struct sdlbutton {
	sdlbase;
	pchar labeltext;
	int isDown;
	int countDown;
};

struct sdllabel {
	sdlbase;
	pchar labeltext;
};

struct sdlcheckbox {
	sdlbase;
	pchar labeltext;
	int isChecked;
};

struct sdllistitem {
	sdlbase;
	pchar itemtext;
	int index;
	struct sdllistitem * nextitem;
};

struct sdllistbox {
	sdlbase;
	pchar listtitle;
	int rowheight;
	int numcontrols;
	int border;
	int backcolor;
	struct sdllistitem * nextitem; /* list of menu items */
};

struct sdlimage {
	sdlbase;
	SDL_Surface * surface;
};

extern SDL_Color twhite;
extern SDL_Color tgray;
extern SDL_Color tred;
extern SDL_Color tgreen;
extern SDL_Color tblue;
extern SDL_Color tblack;
extern unsigned int white;
extern unsigned int gray;
extern unsigned int red;
extern unsigned int black;
extern unsigned int background;
extern enum gamestate state;

/* Logging- error messages */
void Log(pchar message);

/* init the library, return a pointer to the panel */
psdlcontrol initsdlguilib(int x,int y,int width,int height,int color);

/* free up the controls */
void freesdlguilib(psdlcontrol lib);
void FreeControl(psdlcontrol control);

/* Draw the Control linked list */
void RenderGUI(psdlcontrol lib);

/* Return count of visible gui objects */
int GetGuiObjects(psdlcontrol lib);

/* add a button to the controls list */
psdlcontrol addbutton(int x,int y, pchar text,SDL_Color textcolor,funcptr f);
psdlcontrol addlabel(int x,int y, pchar text,SDL_Color textcolor);
psdlcontrol addcheckbox(int x,int y, pchar text,SDL_Color textcolor,funcptr f);
psdlcontrol addlistitem(struct sdllistbox * box,pchar text, SDL_Color textcolor,funcptr f);
psdlcontrol addlistbox(int x,int y,pchar title, int backcolor,SDL_Color textcolor,int maxlines,int border);
psdlcontrol addimage(int x,int y,SDL_Surface* surface, funcptr f);
void setlabel(struct sdllabel * label,char * text);

void printch(char c,SDL_Rect * target);

/* print string text at x,y pixel coords */
void print(int x,int y,char * text);

/* ttf version of print*/
void ttf_print(int x,int y,char * text, SDL_Color col);

void CheckControlClickAt(int x,int y);

/* line drawing routines */
void draw_vl( int x, int y, int height, unsigned int color);
void draw_hl( int x, int y,int lineLenth, unsigned int color);

/* conversion Uint32 ->SDL_Color */
SDL_Color u2SDLC(Uint32 col);

/* InitColours */
void InitColours();

/* draws the splash screen */
void DrawSplashScreen();


