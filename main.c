/*
 * main.c
 *
 *  Created on: 15.12.2018
 *      Author: daniel
 */


#include<stdio.h>
#include "sudoku.h"
#include<string.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<stdlib.h>

#include<time.h>


#define BUTTONWIDTH 150
#define BUTTONHEIGHT 30

#define SUBMITTXTLEN 13
#define RETRYTXTLEN 8
#define EXPORTTXTLEN 6
#define EASYTXTLEN 6
#define DIFFICULTTXTLEN 6

const int diffbuttonwidth = BUTTONWIDTH / 3 - 4;



/* Variablen */
Display *d;
int s;
Window w, submit_button, retry_button, export_button, easy_button, difficult_button, diff_ind;
XEvent e;
Colormap screen_colormap;
Status rc;

struct windowproperties {
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	unsigned int border_width;
	unsigned int depht;
	unsigned long blackp, whitep;
	XColor red, green, yellow, darkdarkgray, darkgray, lightgray, light, gray;
	GC gc;
	XFontStruct *font;
} win;
int width;

struct buttontext {
	char submit[SUBMITTXTLEN+1];
	char retry[RETRYTXTLEN+1];
	char export[EXPORTTXTLEN+1];
	char easy[EASYTXTLEN+1];
	char difficult[DIFFICULTTXTLEN+1];
} buttontxt;


/* eigene Funktionen*/

/* convert int (0..9) to char */
char inttochar (int) ;

/* Connect to X-Server */
static void ConnectXServer();

/* Create the main window */
static void CreateWindow();

/* Creates the drawing properties */
static void CreateGC();

/* Sets the font */
static void SetFont(
		unsigned int	/*FontSize*/
);

/* Draw the content of main window */
static void Draw(
		int, 	/* Selected Field x-Coordinate */
		int, 	/* Selected Field y-Coordinate */
		int*, 	/* first adress Sudoku-Matrix */
		int*	/* first adress empty,const,false,true-Matrix */


);

/* draw the difficulty indicator-underline */
void DrawDiffInd (
	unsigned long,	/* Color for the background */
	int							/* Difficulty level */
);

/* Create the Buttons */
static void CreateButtons();

/* reposition Buttons */
static void MoveButtons();

/* draw the inner of the Button */
static void DrawButton(
		Window, 		/*Buttonselector*/
		int,			/*Buttonwidth*/
		unsigned long, 	/*BackgroundColor*/
		unsigned long, 	/*ForegroundColor*/
		char*, 			/*Buttontext*/
		int				/*Textlenght*/
);



/* Check Answers */
static void CheckAns(
		int*, 	/*first adress Sudoku-Matrix*/
		int*, 	/*first adress empty,const,false,true-Matrix*/
		int*	/*first adress Solution-Matrix*/
);

/*Controlhandler*/
static void Controls(
		int*, 			/*Selected field x-Coordinate*/
		int*, 			/*Selected field y-Coordinate*/
		unsigned int,	/*Keycode*/
		int*, 			/*first adress Sudoku-Matrix*/
		int*			/*first adress empty,const,false,true-Matrix*/
);






int main () {
	int Selectionx = 0, Selectiony = 0;
	int solution[9][9] = { 0 };
	int Matr[9][9] = { 0 };
	int freefields[9][9] = { 0 };
	int t1, t2;
	int difficulty = 57;

	strcpy(buttontxt.retry, "Generate");
	strcpy(buttontxt.submit, "Check Answers");
	strcpy(buttontxt.export, "Export");
	strcpy(buttontxt.easy, "Easier");
	strcpy(buttontxt.difficult, "Harder");


	ConnectXServer();
	CreateWindow();
	CreateGC();
	CreateButtons();


	while(1) {
		XNextEvent(d, &e);

		if (e.type == Expose) {
			Draw(Selectionx, Selectiony, &Matr[0][0], &freefields[0][0]);
			MoveButtons();
		}

		if (e.type == ConfigureNotify) {
			XConfigureEvent xce = e.xconfigure;

			if (xce.width != win.width || xce.height != win.height) {
				win.width = xce.width;
				win.height = xce.height;
				Draw(Selectionx, Selectiony, &Matr[0][0], &freefields[0][0]);
				MoveButtons();
			}

		}

		if (e.type == KeyPress) {
			Controls(&Selectionx, &Selectiony, e.xkey.keycode, &Matr[Selectionx][Selectiony], &freefields[Selectionx][Selectiony]);
			Draw(Selectionx, Selectiony, &Matr[0][0], &freefields[0][0]);
		}

		if (e.type == ClientMessage) {
			break;
		}


		/* handle Submit_Button Buttonevents */

		if (e.xany.window == submit_button) {
			if (e.type == Expose) {
				DrawButton(submit_button, BUTTONWIDTH, win.darkgray.pixel, win.whitep, buttontxt.submit, SUBMITTXTLEN);
			}
			if (e.type == EnterNotify) {
				DrawButton(submit_button, BUTTONWIDTH, win.gray.pixel, win.whitep, buttontxt.submit, SUBMITTXTLEN);
			}
			if (e.type == LeaveNotify) {
				DrawButton(submit_button, BUTTONWIDTH, win.darkgray.pixel, win.whitep, buttontxt.submit, SUBMITTXTLEN);
			}
			if (e.type == ButtonPress) {
				DrawButton(submit_button, BUTTONWIDTH, win.lightgray.pixel, win.whitep, buttontxt.submit, SUBMITTXTLEN);
			}
			if (e.type == ButtonRelease) {
				DrawButton(submit_button, BUTTONWIDTH, win.gray.pixel, win.whitep, buttontxt.submit, SUBMITTXTLEN);
				CheckAns(&Matr[0][0], &freefields[0][0], &solution[0][0]);
				Draw(Selectionx, Selectiony, &Matr[0][0], &freefields[0][0]);
			}
		}

		/*handle Submit_Button Buttonevents */

		if (e.xany.window == retry_button) {
			if (e.type == Expose) {
				DrawButton(retry_button, BUTTONWIDTH, win.darkgray.pixel, win.whitep, buttontxt.retry, RETRYTXTLEN);
			}
			if (e.type == EnterNotify) {
				DrawButton(retry_button, BUTTONWIDTH, win.gray.pixel, win.whitep, buttontxt.retry, RETRYTXTLEN);
			}
			if (e.type == LeaveNotify) {
				DrawButton(retry_button, BUTTONWIDTH, win.darkgray.pixel, win.whitep, buttontxt.retry, RETRYTXTLEN);
			}
			if (e.type == ButtonPress) {
				DrawButton(retry_button, BUTTONWIDTH, win.lightgray.pixel, win.whitep, buttontxt.retry, RETRYTXTLEN);
			}
			if (e.type == ButtonRelease) {
				t1 = clock()/CLOCKS_PER_SEC;

				DrawButton(retry_button, BUTTONWIDTH, win.gray.pixel, win.whitep, buttontxt.retry, RETRYTXTLEN);
				CreateSudoku(&solution[0][0]);
				rmfields(&solution[0][0], &Matr[0][0], difficulty);

				for (int x = 0; x < 9; x++) {
					for (int y = 0; y < 9; y++) {
						if (Matr[x][y] != solution[x][y]) {
							freefields[x][y] = 1;
						} else {
							freefields[x][y] = 0;
						}
					}
				}
				Draw(Selectionx, Selectiony, &Matr[0][0], &freefields[0][0]);

				t2 = clock()/CLOCKS_PER_SEC;

				printf("Benötigte Zeit zum erstellen: %i  s\n\n", t2-t1);
			}
		}

		/* handle Export_Button events */
		if (e.xany.window == export_button) {
			if (e.type == Expose) {
				DrawButton(export_button, BUTTONWIDTH, win.darkgray.pixel, win.whitep, buttontxt.export, EXPORTTXTLEN);
			}
			if (e.type == EnterNotify) {
				DrawButton(export_button, BUTTONWIDTH, win.gray.pixel, win.whitep, buttontxt.export, EXPORTTXTLEN);
			}

			if (e.type == LeaveNotify) {
				DrawButton(export_button, BUTTONWIDTH, win.darkgray.pixel, win.whitep, buttontxt.export, EXPORTTXTLEN);
			}

			if (e.type == ButtonPress) {
				DrawButton(export_button, BUTTONWIDTH, win.lightgray.pixel, win.whitep, buttontxt.export, EXPORTTXTLEN);
			}

			if (e.type == ButtonRelease) {
				DrawButton(export_button, BUTTONWIDTH, win.darkgray.pixel, win.whitep, buttontxt.export, EXPORTTXTLEN);

				printf("Exportiere Sudoku...\n");
				Exporthtml(&Matr[0][0]);
			}
		}



		if (e.xany.window == easy_button) {
			if (e.type == Expose) {
				DrawButton(easy_button, diffbuttonwidth, win.darkgray.pixel, win.whitep, buttontxt.easy, EASYTXTLEN);
			}
			if (e.type == EnterNotify) {
				DrawButton(easy_button, diffbuttonwidth, win.gray.pixel, win.whitep, buttontxt.easy, EASYTXTLEN);
			}

			if (e.type == LeaveNotify) {
				DrawButton(easy_button, diffbuttonwidth, win.darkgray.pixel, win.whitep, buttontxt.easy, EASYTXTLEN);
			}

			if (e.type == ButtonPress) {
				DrawButton(easy_button, diffbuttonwidth, win.lightgray.pixel, win.whitep, buttontxt.easy, EASYTXTLEN);
			}

			if (e.type == ButtonRelease) {
				DrawButton(easy_button, diffbuttonwidth, win.darkgray.pixel, win.whitep, buttontxt.easy, EASYTXTLEN);
				difficulty--;
				DrawDiffInd(win.blackp, difficulty);
			}
		}


		if (e.xany.window == difficult_button) {
			if (e.type == Expose) {
				DrawButton(difficult_button, diffbuttonwidth, win.darkgray.pixel, win.whitep, buttontxt.difficult, DIFFICULTTXTLEN);
			}
			if (e.type == EnterNotify) {
				DrawButton(difficult_button, diffbuttonwidth, win.gray.pixel, win.whitep, buttontxt.difficult, DIFFICULTTXTLEN);
			}

			if (e.type == LeaveNotify) {
				DrawButton(difficult_button, diffbuttonwidth, win.darkgray.pixel, win.whitep, buttontxt.difficult, DIFFICULTTXTLEN);
			}

			if (e.type == ButtonPress) {
				DrawButton(difficult_button, diffbuttonwidth, win.lightgray.pixel, win.whitep, buttontxt.difficult, DIFFICULTTXTLEN);
			}

			if (e.type == ButtonRelease) {
				DrawButton(difficult_button, diffbuttonwidth, win.darkgray.pixel, win.whitep, buttontxt.difficult, DIFFICULTTXTLEN);
				difficulty++;
				DrawDiffInd(win.blackp, difficulty);
			}
		}


		if (e.xany.window == diff_ind) {
			if (e.type == Expose) {
				DrawDiffInd(win.blackp, difficulty);
			}
			if (e.type == EnterNotify) {
				DrawDiffInd(win.darkgray.pixel, difficulty);
			}

			if (e.type == LeaveNotify) {
				DrawDiffInd(win.blackp, difficulty);
			}

			if (e.type == ButtonPress) {
				DrawDiffInd(win.gray.pixel, difficulty);
			}

			if (e.type == ButtonRelease) {
				difficulty = 50;
				DrawDiffInd(win.blackp, difficulty);
			}
		}
	}

	XDestroyWindow(d, w);
	XCloseDisplay(d);

	return 0;
}
















char inttochar (int wert) {
	char c = 0;
	switch (wert) {
	case 1 : c = 49; break;
	case 2 : c = 50; break;
	case 3 : c = 51; break;
	case 4 : c = 52; break;
	case 5 : c = 53; break;
	case 6 : c = 54; break;
	case 7 : c = 55; break;
	case 8 : c = 56; break;
	case 9 : c = 57; break;
	default : c = 0; break;
	}
	return c;
}


static void ConnectXServer() {
	/* open connection with the server */
	d=XOpenDisplay(NULL);
	if(d==NULL) {
		printf("Cannot open display\n");
		exit(1);
	}
	s=DefaultScreen(d);


	/* get access to the screen's color map. */
	screen_colormap = DefaultColormap(d, s);

	/* Set default colors */
	win.blackp = BlackPixel(d, s);
	win.whitep = WhitePixel(d, s);
}


static void CreateWindow() {
	/* Set Variables*/
	win.x = 100;
	win.y = 100;
	win.width = 400;
	win.height = 500;
	win.border_width = 1;

	/* create window */
	w = XCreateSimpleWindow(d, RootWindow(d, s), win.x, win.y, win.width, win.height, win.border_width, win.whitep, win.blackp);

	XSetStandardProperties(d,w,"Sudoku","Sudoku",None,NULL,0,NULL);



	// Process Window Close Event through event handler so XNextEvent does Not fail
	Atom delWindow = XInternAtom( d, "WM_DELETE_WINDOW", 0 );
	XSetWMProtocols(d , w, &delWindow, 1);

	/* select kind of events we are interested in */
	XSelectInput(d, w, ExposureMask | KeyPressMask | StructureNotifyMask);

	/* map (show) the window */
	XMapWindow(d, w);
}


static void CreateGC() {
	win.gc = XCreateGC(d, w, 0, 0);
	XSetBackground(d, win.gc, win.blackp);


	/* Set Colors */
	XAllocNamedColor(d, screen_colormap, "red", &win.red, &win.red);
	XAllocNamedColor(d, screen_colormap, "green", &win.green, &win.green);

	XParseColor(d, screen_colormap, "rgb:FF/FF/00", &win.yellow);
	XAllocColor(d, screen_colormap, &win.yellow);

	XParseColor(d, screen_colormap, "rgb:2a/2a/2a", &win.darkdarkgray);
	XAllocColor(d, screen_colormap, &win.darkdarkgray);

	XParseColor(d, screen_colormap, "rgb:44/44/44", &win.darkgray);
	XAllocColor(d, screen_colormap, &win.darkgray);

	XParseColor(d, screen_colormap, "rgb:88/88/88", &win.lightgray);
	XAllocColor(d, screen_colormap, &win.lightgray);

	XParseColor(d, screen_colormap, "rgb:dd/dd/dd", &win.light);
	XAllocColor(d, screen_colormap, &win.light);

	XParseColor(d, screen_colormap, "rgb:66/66/66", &win.gray);
	XAllocColor(d, screen_colormap, &win.gray);


	/* Set Font (to avoid to much error messages) */

	win.font = XLoadQueryFont (d, "-*-helvetica-*-r-*-*-12-*-*-*-*-*-*-*");
	/* If the font could not be loaded, revert to the "fixed" font. */
	if (! win.font) {
		fprintf (stderr, "unable to load font helvetica: using \"dejavu sans condensed\"\n");
		win.font = XLoadQueryFont (d, "-*-dejavu sans condensed-medium-r-*-*-12-*-100-*-*-*-adobe-*");
	}

	if (! win.font) {
		fprintf (stderr, "unable to load font lucidasans: using fixed\n");
		win.font = XLoadQueryFont (d, "fixed");
	}

	XSetFont (d, win.gc, win.font->fid);
}

static void SetFont(unsigned int size) {
	char fontname[70];
	/*fontsize available in 0, 7, 12, 13, 14, 16, 17, 18, 20, 22, 24, 28, ...*/

	sprintf(&fontname[0], "-*-helvetica-*-r-*-*-%i-*-*-*-*-*-*-*", size);

	win.font = XLoadQueryFont (d, fontname);
	/* If the font could not be loaded, revert to the "fixed" font. */
	if (! win.font) {
		sprintf(fontname, "-*-dejavu sans condensed-medium-r-*-*-%i-*-100-*-*-*-adobe-*", size);
		win.font = XLoadQueryFont (d, fontname);
	}
	if (! win.font) {
		win.font = XLoadQueryFont (d, "fixed");
	}
	XSetFont (d, win.gc, win.font->fid);
}

static void Draw(int Auswahlx, int Auswahly, int *Wert00, int *freefields) {
	XClearWindow(d,w);
	int x0, y0;

	if (win.width > win.height-140) {
		width = win.height - 180;
	} else {
		width = win.width - 40;
	}


	x0 = (win.width-width)/2;
	y0 = (win.height-width)/2-20;


	/* Überschrift zeichnen */
	SetFont(22);
	XSetForeground(d, win.gc, win.whitep);
	XDrawString(d, w, win.gc, (win.width-width)/2, 40, "Good Luck", 9);

	/* Bearbeitbare Felder hervorheben */
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++) {
			if (*(freefields+y+9*x) == 0 || *(freefields+y+9*x) == 2) {
				XSetForeground(d, win.gc, win.darkdarkgray.pixel);
				XFillRectangle(d, w, win.gc, x0+x*width/9, y0+y*width/9, width/9, width/9);
			}
		}
	}


	/* Auswahlfeld hervorheben */
	XSetForeground(d, win.gc, win.gray.pixel);
	XFillRectangle(d, w, win.gc, x0+Auswahlx*width/9, y0+Auswahly*width/9, width/9, width/9);

	/* Aussenquadrate und Innenquadrate zeichnen */
	XSetLineAttributes(d, win.gc, 3, LineSolid, CapRound, JoinRound);
	XSetForeground(d, win.gc, win.whitep);
	XDrawRectangle(d, w, win.gc, x0, y0, width, width);

	XDrawLine(d, w, win.gc, x0+width/3, y0, x0+width/3, y0+width);
	XDrawLine(d, w, win.gc, x0+2*width/3, y0, x0+2*width/3, y0+width);
	XDrawLine(d, w, win.gc, x0, y0+width/3, x0+width, y0+width/3);
	XDrawLine(d, w, win.gc, x0, y0+2*width/3, x0+width, y0+2*width/3);



	/* Felder Zeichnen */
	XSetLineAttributes(d, win.gc, 1, LineSolid, CapRound, JoinRound);

	for (int x = 0; x < 3; x++) {
		XDrawLine(d, w, win.gc, x0+x*width/3+width/9, y0, x0+x*width/3+width/9, y0+width);
		XDrawLine(d, w, win.gc, x0+x*width/3+2*width/9, y0, x0+x*width/3+2*width/9, y0+width);
	}
	for (int y = 0; y < 3; y++) {
		XDrawLine(d, w, win.gc, x0, y0+y*width/3+width/9, x0+width, y0+y*width/3+width/9);
		XDrawLine(d, w, win.gc, x0, y0+y*width/3+2*width/9, x0+width, y0+y*width/3+2*width/9);
	}

	/* Zahlen zeichnen */
	SetFont(width/15);
	/* Abmessungen ermitteln */
	int direction, ascent, descent;
	XCharStruct overall;

	XTextExtents(win.font, "4", 1, &direction, &ascent, &descent, &overall);
	int ydistance = (width/9+ascent-descent)/2;

	/* Text mittig ausrichten */
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++) {
			char c = inttochar(*(Wert00+y+9*x));
			if (c != 0) {
				switch (*(freefields+y+9*x)) {
				/* vorgegebene Felder */
				case 0: {
					XSetForeground(d, win.gc, win.whitep);
					XDrawString(d, w, win.gc, x0+x*width/9+(width/9-overall.width)/2+2, y0+y*width/9+ydistance, &c, 1);
				} break;

				/* Leer Felder */
				case 1: {
					XSetForeground(d, win.gc, win.light.pixel);
					XDrawString(d, w, win.gc, x0+x*width/9+(width/9-overall.width)/2+2, y0+y*width/9+ydistance, &c, 1);
				} break;
				/* Richtig eingetragene Felder */
				case 2: {
					XSetForeground(d, win.gc, win.green.pixel);
					XDrawString(d, w, win.gc, x0+x*width/9+(width/9-overall.width)/2+2, y0+y*width/9+ydistance, &c, 1);
				} break;

				/* Falsch eingetragene Felder */
				case 3: {
					XSetForeground(d, win.gc, win.red.pixel);
					XDrawString(d, w, win.gc, x0+x*width/9+(width/9-overall.width)/2+2, y0+y*width/9+ydistance, &c, 1);
				} break;
				}
			}
		}
	}
}



void DrawDiffInd(unsigned long background, int diff) {
	char diffstr[3];
	sprintf(diffstr, "%i", diff);

	if (diff < 49)
		XSetForeground(d, win.gc, win.green.pixel);
	else if (diff < 57)
		XSetForeground(d, win.gc, win.yellow.pixel);
	else
		XSetForeground(d, win.gc, win.red.pixel);

	if (diff > 58)
		XSetWindowBackground(d, diff_ind, win.red.pixel);
	else
		XSetWindowBackground(d, diff_ind, background);
	XClearWindow(d, diff_ind);

	XFillRectangle(d, diff_ind, win.gc, 0, BUTTONHEIGHT-1, diffbuttonwidth, 2);

	/* round edges */
	XSetForeground(d, win.gc, win.blackp);
	XDrawPoint(d, diff_ind, win.gc, 0, 0);
	XDrawPoint(d, diff_ind, win.gc, 1, 0);
	XDrawPoint(d, diff_ind, win.gc, 0, 1);

	XDrawPoint(d, diff_ind, win.gc, 0, BUTTONHEIGHT);
	XDrawPoint(d, diff_ind, win.gc, 1, BUTTONHEIGHT);
	XDrawPoint(d, diff_ind, win.gc, 0, BUTTONHEIGHT-1);

	XDrawPoint(d, diff_ind, win.gc, diffbuttonwidth, 0);
	XDrawPoint(d, diff_ind, win.gc, diffbuttonwidth-1, 0);
	XDrawPoint(d, diff_ind, win.gc, diffbuttonwidth, 1);

	XDrawPoint(d, diff_ind, win.gc, diffbuttonwidth, BUTTONHEIGHT);
	XDrawPoint(d, diff_ind, win.gc, diffbuttonwidth-1, BUTTONHEIGHT);
	XDrawPoint(d, diff_ind, win.gc, diffbuttonwidth, BUTTONHEIGHT-1);

	/* Button Text */
	int direction, ascent, descent;
	XCharStruct overall;
	XSetForeground(d, win.gc, win.whitep);
	SetFont(12);
	XTextExtents(win.font, diffstr, 2, &direction, &ascent, &descent, &overall);
	XDrawString(d, diff_ind, win.gc, (diffbuttonwidth-overall.width)/2, (BUTTONHEIGHT+ascent)/2, diffstr, 2);
}



static void CreateButtons () {
	submit_button = XCreateSimpleWindow(d, w, (win.width+width)/2-BUTTONWIDTH, win.height-90, BUTTONWIDTH+1, BUTTONHEIGHT+1, 0, win.blackp, win.darkgray.pixel);
	XSelectInput(d, submit_button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
	XMapWindow(d, submit_button);

	retry_button = XCreateSimpleWindow(d, w, (win.width+width)/2-2*BUTTONWIDTH-20, win.height-90, BUTTONWIDTH+1, BUTTONHEIGHT+1, 0, win.blackp, win.darkgray.pixel);
	XSelectInput(d, retry_button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
	XMapWindow(d, retry_button);

	export_button = XCreateSimpleWindow(d, w, (win.width+width)/2-BUTTONWIDTH, win.height-50, BUTTONWIDTH+1, BUTTONHEIGHT+1, 0, win.blackp, win.darkgray.pixel);
	XSelectInput(d, export_button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
	XMapWindow(d, export_button);



	easy_button = XCreateSimpleWindow(d, w, (win.width+width)/2-2*BUTTONWIDTH-20, win.height-50, diffbuttonwidth+1, BUTTONHEIGHT+1, 0, win.blackp, win.darkgray.pixel);
	XSelectInput(d, easy_button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
	XMapWindow(d, easy_button);


	difficult_button = XCreateSimpleWindow(d, w, (win.width+width)/2-4*BUTTONWIDTH/3-16, win.height-50, diffbuttonwidth+1, BUTTONHEIGHT+1, 0, win.blackp, win.darkgray.pixel);
	XSelectInput(d, difficult_button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
	XMapWindow(d, difficult_button);

	diff_ind = XCreateSimpleWindow(d, w, (win.width+width)/2-5*BUTTONWIDTH/3-18, win.height-50, diffbuttonwidth+1, BUTTONHEIGHT+1, 0, win.blackp, win.darkgray.pixel);
	XSelectInput(d, diff_ind, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
	XMapWindow(d, diff_ind);

}

static void MoveButtons() {
	/* Button verschieben */
	XMoveWindow(d, submit_button, (win.width+width)/2-BUTTONWIDTH, win.height-90);
	XMoveWindow(d, retry_button, (win.width+width)/2-2*BUTTONWIDTH-20, win.height-90);
	XMoveWindow(d, export_button, (win.width+width)/2-BUTTONWIDTH, win.height-50);

	XMoveWindow(d, easy_button, (win.width+width)/2-2*BUTTONWIDTH-20, win.height-50);
	XMoveWindow(d, difficult_button, (win.width+width)/2-4*BUTTONWIDTH/3-16, win.height-50);
	XMoveWindow(d, diff_ind, (win.width+width)/2-5*BUTTONWIDTH/3-18, win.height-50);

}

static void DrawButton(Window button, int buttonwidth, unsigned long backgroundcolor, unsigned long foregroundcolor, char *buttontext, int txtlen) {

	XSetWindowBackground(d, button, backgroundcolor);
	XClearWindow(d, button);

	/* round edges */
	XSetForeground(d, win.gc, win.blackp);
	XDrawPoint(d, button, win.gc, 0, 0);
	XDrawPoint(d, button, win.gc, 1, 0);
	XDrawPoint(d, button, win.gc, 0, 1);

	XDrawPoint(d, button, win.gc, 0, BUTTONHEIGHT);
	XDrawPoint(d, button, win.gc, 1, BUTTONHEIGHT);
	XDrawPoint(d, button, win.gc, 0, BUTTONHEIGHT-1);

	XDrawPoint(d, button, win.gc, buttonwidth, 0);
	XDrawPoint(d, button, win.gc, buttonwidth-1, 0);
	XDrawPoint(d, button, win.gc, buttonwidth, 1);

	XDrawPoint(d, button, win.gc, buttonwidth, BUTTONHEIGHT);
	XDrawPoint(d, button, win.gc, buttonwidth-1, BUTTONHEIGHT);
	XDrawPoint(d, button, win.gc, buttonwidth, BUTTONHEIGHT-1);

	/* Button Text */
	int direction, ascent, descent;
	XCharStruct overall;
	XSetForeground(d, win.gc, foregroundcolor);
	SetFont(12);
	XTextExtents(win.font, buttontext, txtlen, &direction, &ascent, &descent, &overall);
	XDrawString(d, button, win.gc, (buttonwidth-overall.width)/2, (BUTTONHEIGHT+ascent)/2, buttontext, txtlen);
}




static void CheckAns(int *Wert00, int *freefields, int *solution) {
	for (int i = 0; i < 81; i++) {
		if (*(freefields+i) == 1 && *(Wert00+i) == *(solution+i)) {
			*(freefields+i) = 2;
		} else if (*(freefields+i) == 1) {
			*(freefields+i) = 3;
		}
	}
}



static void Controls(int *Auswahlx, int *Auswahly, unsigned int keycode, int *wert, int *freefield) {
	switch (keycode) {

	/* Pfeiltasten */
	case 111 : *Auswahly = (*Auswahly <= 0) ? 0 : *Auswahly - 1; break;
	case 114 : *Auswahlx = (*Auswahlx >= 8) ? 8 : *Auswahlx + 1; break;
	case 116 : *Auswahly = (*Auswahly >= 8) ? 8 : *Auswahly + 1; break;
	case 113 : *Auswahlx = (*Auswahlx <= 0) ? 0 : *Auswahlx - 1; break;
	}

	if (*freefield == 1 || *freefield == 3) {
		switch (keycode) {
		/* Zahlentasten */
		case 10 : *wert = 1; break;
		case 11 : *wert = 2; break;
		case 12 : *wert = 3; break;
		case 13 : *wert = 4; break;
		case 14 : *wert = 5; break;
		case 15 : *wert = 6; break;
		case 16 : *wert = 7; break;
		case 17 : *wert = 8; break;
		case 18 : *wert = 9; break;
		case 19 : case 119 : case 22 : *wert = 0; break;
		}
		*freefield = 1;
	}
}
