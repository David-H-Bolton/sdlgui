/* sdlgui.c Originally published on About.com 2012, now on dice.com. Author D. Bolton then and now.
 Version 2.0 Updated June 14th,2017 */

#ifndef SDLGUI
#include "sdlgui.h"
#define SDLGUI
#endif

#ifndef STDIO
#include <stdio.h>
#define STDIO
#endif

#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif

#ifndef SDLTTF
#include "SDL_ttf.h"
#define SDLTTF
#endif

#ifndef IWINDOWS
#include <Windows.h>
#define IWINDOWS
#endif

#ifndef TIMINGS
#include "timings.h"
#define TIMINGS
#endif

const char * fontname ="GUNPLAY_.ttf";

const int canclick[] ={0,1,1,1,0,1,0,0,0,1};
const int fvoffset= -3;

SDL_Surface* screen;
SDL_Event event;
SDL_Rect source,destination;

int panelx,panely,popupy,popupx;
psdlcontrol list;
TTF_Font * tfont = NULL;
SDL_Color twhite;
SDL_Color tred = { 255, 0, 0,1 };
SDL_Color tblack = { 0,0,0,1 };
SDL_Color tblue = { 0,0,255,1 };
SDL_Color tgreen = { 0,255,0,1 };
SDL_Color tgray = {0,0,0,1};
unsigned int white;
unsigned int gray;
unsigned int red;
unsigned int black;
unsigned int blue;
unsigned int lightgray;
unsigned int background;
enum gamestate state;

SDL_Color u2SDLC(Uint32 col) {
  /* SDL_Color color={(col & 0x000000ff),(col &
0x0000ff00)/0x100,(col & 0x00ff0000)/0x10000,0}; */
        SDL_Color scolor={(col & 0x00ff0000)/0x10000,(col &
0x0000ff00)/0x100,(col & 0x000000ff),0};
  return scolor;
}

void draw_vl(int x, int y, int height, unsigned int color)
{
  unsigned int res = screen->pitch/4;
  unsigned int* fp, *cp = ((unsigned int*)screen->pixels) + x + y*res;

  for (fp = cp + height*res; cp <  fp; cp += res)
  {
    *cp = color;
  }
}


void draw_hl( int x, int y,int lineLenth, unsigned int color)
{
	Uint32 *dest = (Uint32*)((Uint8*)screen->pixels + y * screen->pitch + (x << 2));
	int i;
	for(i=0;i<lineLenth;++i)
	 *dest++ = color;
}

void Log(pchar message) {
	return; // uncomment for logging
	FILE * f = fopen("log.txt", "a+");
	fprintf(f, "\n%s",message);
	fclose(f);
}

void RenderPanel(psdlcontrol self) {
  int result;
  SDL_Rect rect= {(Sint16)self->x,(Sint16)self->y,(Uint16)self->width,(Uint16)self->height};
  result=SDL_FillRect( screen, &rect, self->color  );
}

void RenderButton(psdlcontrol self) {
  int result,xoff,yoff,x,y;
  unsigned int col1,col2;
  char buff[60];
  struct sdlbutton * button= (struct sdlbutton *)self;

  SDL_Rect rect = {(Sint16)self->x,(Sint16)self->y,(Uint16)self->width,(Uint16)self->height};
  x= self->x;
  y=self->y;

  xoff=0;
  yoff=0;
  col1=white;
  col2=gray;
  if (button->isDown) {
	  col2=col1;
	  col1=gray;
	  xoff = 2;
	  yoff = 2;
	  rect.x += xoff;
	  rect.y += yoff;
	  if (button->countDown>0) {
		  button->countDown--;
		  if (button->countDown <=0) {
			  button->isDown =0;
		  }
	  }
  }

  draw_vl(x-1+xoff, y-1+yoff, button->height+2, col1);
  draw_hl(x+xoff, y-1+yoff,button->width, col1);
  draw_vl(x+xoff+button->width, y-1+yoff, button->height+2, col2);
  draw_hl(x+xoff, y+button->height+yoff,button->width, col2);
  result=SDL_FillRect( screen, &rect, self->color  );
  sprintf_s(buff, sizeof(buff), "%s",button->labeltext);
  ttf_print(button->x+10+xoff,button->y+3+yoff,buff,self->textcolor);
}

void RenderLabel(psdlcontrol self) {
  int result,x,y;
  char buff[60];
  struct sdllabel * label= (struct sdllabel *)self;
  SDL_Rect rect = {(Sint16)self->x,(Sint16)self->y,(Uint16)self->width,(Uint16)self->height};
  x= self->x;
  y=self->y;

  result=SDL_FillRect( screen, &rect, self->color  );
  sprintf_s(buff,sizeof(buff),"%s",label->labeltext);
  ttf_print(x+2,y+2,buff,self->textcolor);
}

void RenderCheckbox(psdlcontrol self) {
  int result,x,y;
  char buff[60];
  struct sdlcheckbox * checkbox= (struct sdlcheckbox *)self;
  SDL_Rect rect = {(Sint16)self->x,(Sint16)self->y,(Uint16)self->width,(Uint16)self->height};
  x= self->x;
  y=self->y;
  draw_vl(x+1, y+1, 16, white);
  draw_hl(x+1, y+1,16, white);
  draw_vl(x+17, y+1, 16, white);
  draw_hl(x+1, y+17,16, white);
  if (checkbox ->isChecked) {
	sprintf_s(buff, sizeof(buff), "%s","x");
	ttf_print(x+4,y-2,buff,twhite);
  }
  rect.x += 25;
  result=SDL_FillRect( screen, &rect, self->color );
  sprintf_s(buff, sizeof(buff), "%s",checkbox->labeltext);
  ttf_print(x+25,y,buff,self->textcolor);
}

void RenderListBox(psdlcontrol self) {
	int tw,th;
	struct sdllistbox * listbox = (struct sdllistbox *)self;
	struct sdllistitem * item;
    int x= self->x;
    int y =self->y;
	int w = self->width;
	int h= self->height;
	char buff[60];
	SDL_Rect rect= {(Sint16)self->x,(Sint16)self->y,(Uint16)self->width,(Uint16)self->height};
    SDL_FillRect( screen, &rect, listbox->backcolor );

	TTF_SizeText(tfont, listbox->listtitle, &tw, &th);
	sprintf_s(buff, sizeof(buff), "%s",listbox->listtitle);
	ttf_print(x+5,y+5,buff,listbox->textcolor);

	if (listbox->border ==1) {

	  draw_vl(x+1, y+1, h, white);
	  draw_hl(x+1, y+1,w, white);
	  draw_vl(x+w+1, y+1, h, white);
	  draw_hl(x+1, y+h,w, white);
	}

	// Render list box items
	if (listbox->nextitem == NULL)
		return; /* just in case no lines*/
	item = listbox->nextitem;

	while (item != NULL) {
		sprintf_s(buff, sizeof(buff), "%s",item->itemtext);
		y+= listbox->rowheight;
		ttf_print(x+10,y+2,buff,item->textcolor);
		item = item->nextitem;
	}
}

void RenderImage(psdlcontrol self) {
	 int ok;
	 SDL_Rect target;
	 struct sdlimage * image = (struct sdlimage *)self;
	 target.h = image->height;
	 target.w = image->width;
	 target.x= image->x;
	 target.y= image->y;
	 ok=SDL_BlitSurface( image->surface,NULL,screen,&target);
}

/* functions */


void RenderGUI(psdlcontrol lib) {
	psdlcontrol control = lib;
	while (control) {
		if (control->pRender && control->visible) {
		    control->pRender(control); /* render self */
		}
		control = control->nextcontrol;
	}
}

int GetGuiObjects(psdlcontrol lib) {
	psdlcontrol control = lib;
	int total = 0;
	while (control) {
		if (control->pRender && control->visible) {
			total++;
		}
		control = control->nextcontrol;
	}
	return total;
}

/* loop through controls freeing them */
void freesdlguilib(psdlcontrol lib) {
	psdlcontrol control = (psdlcontrol)lib;
	psdlcontrol lastcontrol;
	while (control) {
		lastcontrol = control->nextcontrol;
		if (control->pFree)
			control->pFree(control); /* remove self */
		control = lastcontrol;
	}
	TTF_CloseFont(tfont);
}

void FreeControl(psdlcontrol control) {
	struct sdlbutton * button;
	struct sdllabel * label;
	struct sdlcheckbox * checkbox;
	struct sdllistbox * listbox;
	struct sdlimage * image;
	switch(control->ctype) {
	case esdlbutton:
		{
			button = (struct sdlbutton *)control;
			free(button->labeltext);
			free(button);
			break;
		}
	case esdlpanel:
		{
			free(control);
			break;
		}
	case esdllabel:
		{
			label = (struct sdllabel *)control;
			free(label->labeltext);
			free(label);
			break;
		}
	case esdlcheckbox:
		{
			checkbox = (struct sdlcheckbox *)control;
			free(checkbox->labeltext);
			free(checkbox);
			break;
		}
	case esdllistbox:
		{
			listbox = (struct sdllistbox *)control;
			free(listbox->listtitle);
			free(listbox);
			break;
		}
	case esdlimage: 
		{
			image = (struct sdlimage *)control;
			free(image);
		}
   }
}

psdlcontrol addcontrol(enum controltype type,int x,int y,int width,int height, int size, SDL_Color textcolor) {
	psdlcontrol control = (psdlcontrol)malloc(size);
	psdlcontrol listptr = list;
	control->ctype= type;
	control->clickable= canclick[type];
	control->x=x + panelx;
	control->y=y + panely;
	control->width = width;
	control->height=height;
	control->visible = 1;
	control->color = background;
	control->textcolor = textcolor;
	control->nextcontrol = NULL;
	control->pPreClick = NULL;
	control->pFree= &FreeControl;
	switch (type) {
	case esdlpanel:
		control->pRender=&RenderPanel;break;
	case esdlbutton:
		control->pRender= &RenderButton;break;
	case esdllabel:
		control->pRender=&RenderLabel;break;
	case esdlcheckbox:
		control->pRender=&RenderCheckbox;break;
	case esdllistbox:
		control->pRender=&RenderListBox;break;	
	case esdllistitem:
		control->pRender=NULL;break;
	case esdlimage:
		control->pRender=&RenderImage;break;
	}
	if (type !=esdlpanel) { /* try not to add panel into list of controls! */
		while (listptr->nextcontrol)
			listptr = listptr->nextcontrol;
		listptr->nextcontrol = control;
	}
	return control;
}

psdlcontrol initsdlguilib(int x, int y, int width, int height, int colour) {
	/* init font here */
	if (TTF_Init() == -1) {
		Log("err1-ttf");
		printf("TTF_Init: %s\n", TTF_GetError());
		return 0;
	}

	tfont = TTF_OpenFont(fontname, 20);
	if (!tfont)
	{
		Log("err2 - ttf");
	    printf("Could not open %s %s", fontname, SDL_GetError());
		return 0;
    }
	psdlcontrol panel = (psdlcontrol)malloc(sizeof(struct sdlcontrol));
	background = colour;
	panel= (psdlcontrol)addcontrol(esdlpanel,x,y,width,height,sizeof(struct sdlcontrol),twhite);
	panel->pRender = &RenderPanel;
	panel->pFree =  &FreeControl;
	panel->pClick = NULL;
	panel->pPreClick = NULL;
	panel->nextcontrol= NULL;
	panelx = x;
	panely = y;

	list = panel;
	//splash = (psdlcontrol)addimage(50,50,planes[PLSPLASH],NULL,1);
	return panel;
}

/* Handlers for pre/post click on controls */
void buttonPreclick(psdlcontrol self) {
	struct sdlbutton * pb= (struct sdlbutton *)self;
	pb->isDown = 1;
	pb->countDown = 20;
}

void CheckBoxPreClick(psdlcontrol self) {
	struct sdlcheckbox * checkbox = (struct sdlcheckbox *)self;
	checkbox->isChecked ^= 1; /* 0 -> 1 -> 0 */
}


psdlcontrol addbutton(int x,int y, pchar text,SDL_Color textcolor,funcptr f){
	struct sdlbutton * button;
	int width, height;
	int textlen = strlen(text) + 1;
	TTF_SizeText(tfont, text, &width, &height);
	button = (struct sdlbutton *)addcontrol(esdlbutton, x, y, width+20, height+2, sizeof(struct sdlbutton), textcolor);
	button->isDown = 0;
	button->countDown = 0;
	button->pClick = f;
	button->pPreClick = buttonPreclick;
	button->labeltext = (pchar)malloc(textlen);
	strcpy_s(button->labeltext, textlen,text);
	return (psdlcontrol)button;
}

psdlcontrol addcheckbox(int x,int y, pchar text,SDL_Color textcolor,funcptr f) {
	struct sdlcheckbox * checkbox;
	int width,height;
	int textlen = strlen(text) + 1;
    TTF_SizeText(tfont, text, &width, &height);
	checkbox = (struct sdlcheckbox *)addcontrol(esdlcheckbox,x,y,width,height,sizeof(struct sdlcheckbox),textcolor);
	checkbox->isChecked = 0;
	checkbox->pClick = f;
	checkbox->pPreClick = CheckBoxPreClick;
	checkbox->labeltext = (pchar)malloc(textlen);
	strcpy_s(checkbox->labeltext,textlen,text);
	return (psdlcontrol)checkbox;
}

psdlcontrol addlabel(int x,int y, pchar text,SDL_Color textcolor) {
	struct sdllabel * label;
	int width,height;
	int textlen = strlen(text) + 1;
	pchar textptr=(pchar)malloc(textlen);
    TTF_SizeText(tfont, text, &width, &height);
	label = (struct sdllabel *)addcontrol(esdllabel,x,y,width,height,sizeof(struct sdllabel),textcolor);
	label->labeltext = textptr;
	label->pClick=NULL;
	strcpy_s(label->labeltext, textlen,text);
	return (psdlcontrol)label;
}

psdlcontrol addlistbox(int x,int y,pchar title,int backcolor,SDL_Color textcolor,int maxlines,int border) {
	struct sdllistbox * listbox;
	int width,height,rowheight;
	int textlen = strlen(title) + 1;
    TTF_SizeText(tfont, title, &width, &height);
	width += 15; // adjust for font
	rowheight = height+2;
	height = (height+5)*(maxlines+1);
	listbox  = (struct sdllistbox *)addcontrol(esdllistbox,x,y,width,height,sizeof(struct sdllistbox),textcolor);
	listbox->border=border;
	listbox->textcolor=textcolor;
	listbox->backcolor= backcolor;
	listbox->pClick = NULL;
	listbox->nextitem = NULL;

	listbox->numcontrols=0; 
	listbox->rowheight=rowheight;	
	listbox->listtitle = (pchar)malloc(textlen);
	strcpy_s(listbox->listtitle,textlen,title);
	return (psdlcontrol)listbox;
}

psdlcontrol addlistitem(struct sdllistbox * box,pchar text, SDL_Color textcolor,funcptr f) {
	struct sdllistitem * item,* boxitem;
	int width,height,bx,by;
	int controlnum= ++box->numcontrols;
	int textlen = strlen(text) + 1;
	bx= box->x-panelx;
	by= box->y-panely+ (controlnum* box->rowheight);

    TTF_SizeText(tfont, text, &width, &height);
	item = (struct sdllistitem *)addcontrol(esdllistitem,bx,by,width,height,sizeof(struct sdllistitem),textcolor);
	item->nextitem= NULL;	
	item->itemtext = (pchar)malloc(textlen);
	strcpy_s(item->itemtext,textlen,text);
	item->textcolor = textcolor;
	item->pClick = f;
	item->index=0;
	if (box -> nextitem == NULL) {
		box -> nextitem = item;	
	}
	else {
		    boxitem = box->nextitem;	
			item->index=1;
		    while (boxitem->nextitem != NULL) {
			  boxitem = boxitem->nextitem;
			  item->index++;
		    }
			boxitem->nextitem = item;
	}
	return (psdlcontrol)item;
}

psdlcontrol addimage(int x,int y,SDL_Surface* surface, funcptr f) {
	struct sdlimage * image;
	int width,height;
	width= surface->w;
	height=surface->h;
	image = (struct sdlimage *)addcontrol(esdlimage,x,y,width,height,sizeof(struct sdlimage),twhite);
	image->surface = surface;
	image->pClick=f; // pass in 0 for unclickable
	return (psdlcontrol)image;
}

/* print char at rect target */
void printch(char c,SDL_Rect * target) {
	SDL_Rect charblock;
	int ok;
	int start= (c-'!');
	if (c!= ' ') {
		charblock.h=23;
		charblock.w=12;
		charblock.x = (Sint16)start*12;
		charblock.y = 0;
		ok=SDL_BlitSurface(screen,&charblock,screen,target);
		if (ok==-1)
		{
			printf("error %s",SDL_GetError());
		}
	}
	(*target).x+= 13;
}

/* print string text at x,y pixel coords */
void print(int x,int y,char * text) {
	int len=strlen(text);
	int i;
	SDL_Rect destr;
	if (len==0)
		return;
	destr.h = 23;
	destr.w = 12;
	destr.x = (Sint16)x;
	destr.y = (Sint16)y;
	for (i=0;i<len;i++)
		printch(text[i],&destr);
}

void ttf_print(int x,int y,char * text,SDL_Color col) {
	SDL_Rect target;
	SDL_Surface *text_surface;
	int w,h;
    TTF_SizeText(tfont, text, &w, &h);
	target.h = h;
	target.w=  w;
	target.x = (Sint16)x;
	target.y = (Sint16)(y +fvoffset);
	text_surface=TTF_RenderText_Solid(tfont,text,col);
	SDL_BlitSurface(text_surface,NULL,screen,&target);
	SDL_FreeSurface(text_surface);
}

void CheckControlClickAt(int x,int y) {
	psdlcontrol control = list;

	/* walk the controls looking for x,y in target area that has a non-zero pClick */

	while (control) {
		if (control->clickable)
		{
			if (x >= control->x && x < (control->x + control->width) &&
				y >= control->y && y < (control->y + control->height)) {
					if (control-> pPreClick)
						control-> pPreClick(control);
					if (control-> pClick)
						control-> pClick(control);
					    return;
			}
		}

		control = control->nextcontrol;
	}
}

/* frees and reallocates memory if new string longer */
void setlabel(struct sdllabel * label,char * text){
	pchar textptr;
	struct sdllabel * changelabel = (struct sdllabel *)label;	
	return; // crashes! to be fixed
	if (strlen(changelabel->labeltext) < strlen(text)) {
		free(changelabel->labeltext);
		textptr=(pchar)malloc(strlen(text)+1);
		changelabel->labeltext= textptr;
	}
	strcpy_s(changelabel->labeltext, strlen(changelabel->labeltext),text);
}

void InitColours() {
	twhite = u2SDLC(0xffffff);
	tred = u2SDLC(0xff0000);
	tblack = u2SDLC(0);
	tblue = u2SDLC(0x0000ff);
	tgreen = u2SDLC(0x00ff00);
	white = SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);
	gray = SDL_MapRGB(screen->format, 0x9c, 0x94, 0x97);
	red = SDL_MapRGB(screen->format, 0xff, 0x0, 0x0);
	black = SDL_MapRGB(screen->format, 0x0, 0x0, 0x0);
	blue= SDL_MapRGB(screen->format, 0x0, 0x0, 0xff);
}