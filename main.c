/*
 *  WMBlueCPU - a cpu monitor
 *
 *  Copyright (C) 2003 Draghicioiu Mihai Andrei <misuceldestept@go.ro>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>

#include "mwm.h"
#include "menu.h"
#include "gpu.h"

#include "pixmap.xpm"
#include "icon.xpm"

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 64
#define WINDOW_NAME "WMBlueGPU"
#define ICON_NAME "wmbluegpu"
#define CLASS_NAME "wmbluegpu"
#define CLASS_CLASS "WMBlueGPU"

#define OPT_DISPLAY NULL
#define OPT_MILISECS 4000
#define OPT_BGCOLOR  "rgb:00/00/00"
#define OPT_ONCOLOR  "rgb:87/D7/FF"
#define OPT_OFFCOLOR "rgb:00/95/E0"
#define OPT_WINDOW 0
#define OPT_SHAPE 1
//#define OPT_FONTNAME "fixed"
#define OPT_FONTNAME "-*-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1"

int            argc;
char         **argv;
Display       *display;
int            screen;
Colormap       colormap;
Visual        *visual;
int            depth;
GC             backgc, offgc, ongc;
Window         rootwindow, window, iconwindow, mapwindow;
XEvent         event;
int            exitloop;
Atom           wm_delete_window;
Atom           _motif_wm_hints;
MWMHints       mwmhints;
Pixmap         buffer, pixmap, pixmask, iconpixmap;
unsigned long  bgcolor, offcolor, oncolor;
struct timeval tv = { 0, 0 };
int            oldx, oldy;
menu_t *       m;

char *opt_display  = OPT_DISPLAY;
int   opt_milisecs = OPT_MILISECS;
char *opt_bgcolor  = OPT_BGCOLOR;
char *opt_offcolor = OPT_OFFCOLOR;
char *opt_oncolor  = OPT_ONCOLOR;
int   opt_window   = OPT_WINDOW;
int   opt_shape    = OPT_SHAPE;
char *opt_fontname = OPT_FONTNAME;

float gpu_temp = 0.0f;
float max_temp = 120.0f;
XFontStruct *fontstruct;

unsigned long get_color(char *colorname)
{
 XColor c, c2;
 c2.pixel = 0;
 XAllocNamedColor(display, colormap, colorname, &c, &c2);
 return c.pixel;
}

void bad_option(int arg)
{
 fprintf(stderr, "%s needs an argument.\n"
                 "Try `%s --help' for more information.\n",
                 argv[arg-1], argv[0]);
 exit(1);
}

void print_usage()
{
 printf("Usage: %s [option]\n"
	"Options:\n"
	"    -h,  --help           Print out this help and exit\n"
	"    -v,  --version        Print out version number and exit\n"
	"    -d,  --display  <dpy> The X11 display to connect to\n"
	"    -m,  --milisecs <ms>  The number of milisecs between updates\n"
	"    -b,  --bgcolor  <col> The background color\n"
	"    -f,  --offcolor <col> Color for Off leds\n"
	"    -o,  --oncolor  <col> Color for On leds\n"
	"    -fn, --font    <font> Specify the font to use\n"
	"    -w,  --window         Run in a window\n"
	"    -nw, --no-window      Run as a dockapp\n"
	"    -s,  --shape          Use XShape extension\n"
	"    -ns, --no-shape       Don't use XShape extension\n",
	argv[0]);
 exit(0);
}

void print_version()
{
 printf(WINDOW_NAME " version " VERSION "\n");
 exit(0);
}

void parse_args()
{
 int n;

 for(n = 1; n < argc; n++)
 {
  if(!strcmp(argv[n], "-h") ||
     !strcmp(argv[n], "--help"))
  {
   print_usage();
  }
  else if(!strcmp(argv[n], "-v") ||
          !strcmp(argv[n], "--version"))
  {
   print_version();
  }
  else if(!strcmp(argv[n], "-d") ||
     !strcmp(argv[n], "--display"))
  {
   if(argc <= ++n)
   {
    bad_option(n);
   }
   opt_display = argv[n];
  }
  else if(!strcmp(argv[n], "-m") ||
          !strcmp(argv[n], "--milisecs"))
  {
   if(argc <= ++n)
   {
    bad_option(n);
   }
   opt_milisecs = strtol(argv[n], NULL, 0);
  }
  else if(!strcmp(argv[n], "-b") ||
          !strcmp(argv[n], "--bgcolor"))
  {
   if(argc <= ++n)
   {
    bad_option(n);
   }
   opt_bgcolor = argv[n];
  }
  else if(!strcmp(argv[n], "-f") ||
          !strcmp(argv[n], "--offcolor"))
  {
   if(argc <= ++n)
   {
    bad_option(n);
   }
   opt_offcolor = argv[n];
  }
  else if(!strcmp(argv[n], "-o") ||
          !strcmp(argv[n], "--oncolor"))
  {
   if(argc <= ++n)
   {
    bad_option(n);
   }
   opt_oncolor = argv[n];
  }
  else if(!strcmp(argv[n], "-fn") ||
          !strcmp(argv[n], "--font"))
  {
   if(argc <= ++n)
   {
    bad_option(n);
   }
   opt_fontname = argv[n];
  }
  else if(!strcmp(argv[n], "-w") ||
          !strcmp(argv[n], "--window"))
  {
   opt_window = 1;
  }
  else if(!strcmp(argv[n], "-nw") ||
          !strcmp(argv[n], "--no-window"))
  {
   opt_window = 0;
  }
  else if(!strcmp(argv[n], "-s") ||
          !strcmp(argv[n], "--shape"))
  {
   opt_shape = 1;
  }
  else if(!strcmp(argv[n], "-ns") ||
          !strcmp(argv[n], "--no-shape"))
  {
   opt_shape = 0;
  }
  else
  {
   fprintf(stderr, "Bad option. Try `%s --help' for more information.\n", argv[0]);
   exit(1);
  }
 }
}

#include "common.c"

void gpu_getusage()
{
  gpu_temp = nvidia_gpu_temperature(display, screen);
  max_temp = nvidia_gpu_max_temperature;
}

static int old_height = 0;
void draw_window()
{
	char buf[10];
  
	sprintf(buf, "%.0f\xB0""C", gpu_temp);
	int bufl = strlen(buf);

	XCharStruct xch;
	int dummy1, dummy2, dummy3;
	XTextExtents(fontstruct, buf, bufl, &dummy1, &dummy2, &dummy3, &xch);

	XFillRectangle(display, buffer, backgc, 17, 5, 44, xch.ascent + xch.descent);

	XDrawString(display, buffer, ongc, 60 - xch.width, 5 + xch.ascent, buf, strlen(buf));
  
	XFillRectangle(display, buffer, offgc, 19, 7 + xch.ascent + xch.descent, 40, 2);
  
	XCopyArea(display, buffer, buffer, backgc, 20, 11 +  xch.ascent + xch.descent, 39, 48 - xch.ascent - xch.descent, 19, 11 + xch.ascent + xch.descent);

	XDrawLine(display, buffer, backgc, 58, 58, 58, 9 + xch.ascent + xch.descent);
	int new_height = gpu_temp * (47 - xch.ascent + xch.descent) / max_temp;

	XDrawLine(display, buffer, ongc, 58, 58 - old_height, 58, 58 - new_height);
	old_height = new_height;
}

void proc()
{
 int i;
 fd_set fs;
 int fd = ConnectionNumber(display);

 process_events();
 FD_ZERO(&fs); FD_SET(fd, &fs);
 i = select(fd + 1, &fs, 0, 0, &tv);
 if(i == -1)
 {
  fprintf(stderr, "Error with select(): %s", strerror(errno));
  exit(1);
 }
 if(!i)
 {
  gpu_getusage();
  draw_window();
  update_window();
  tv.tv_sec = opt_milisecs / 1000;
  tv.tv_usec = (opt_milisecs % 1000) * 1000;
 }
}

void free_stuff()
{
 XUnmapWindow(display, mapwindow);
 XDestroyWindow(display, iconwindow);
 XDestroyWindow(display, window);
 XFreePixmap(display, buffer);
 XFreePixmap(display, iconpixmap);
 XFreePixmap(display, pixmask);
 XFreePixmap(display, pixmap);
 XFreeGC(display, ongc);
 XFreeGC(display, offgc);
 XFreeGC(display, backgc);
 XCloseDisplay(display);
}

void set_refresh(menuitem_t *i)
{
 opt_milisecs = i->i;
 
 for(i = m->first; i && i->i > 0; i = i->next)
  if(i->i == opt_milisecs) i->checked = 1;
  else i->checked = 0;

 tv.tv_sec = 0;
 tv.tv_usec = 0;
}

int main(int ac, char **av)
{
 menuitem_t *i;

 argc = ac;
 argv = av;
 parse_args();
 make_window();

 fontstruct = XLoadQueryFont(display, opt_fontname);
 if(!fontstruct) {
	fprintf(stderr, "Could not parse font: %s\n", opt_fontname);
	exit(1);
 }
 XSetFont(display, ongc, fontstruct->fid);
 
 nvidia_gpu_init(display, screen);
 
 menu_init(display);
 m = menu_new();
 i = menu_append(m, "Refresh: 1s");
 i->i = 1000; i->callback = set_refresh; if(opt_milisecs == 1000) i->checked = 1; else i->checked = 0;
 i = menu_append(m, "Refresh: 2s");
 i->i = 2000; i->callback = set_refresh; if(opt_milisecs == 2000) i->checked = 1; else i->checked = 0;
 i = menu_append(m, "Refresh: 4s");
 i->i = 4000; i->callback = set_refresh; if(opt_milisecs == 4000) i->checked = 1; else i->checked = 0;
 i = menu_append(m, "Refresh: 6s");
 i->i = 6000; i->callback = set_refresh; if(opt_milisecs == 6000) i->checked = 1; else i->checked = 0;
 i = menu_append(m, "Refresh: 8s");
 i->i = 8000; i->callback = set_refresh; if(opt_milisecs == 8000) i->checked = 1; else i->checked = 0;
 menu_append(m, "Exit");
 while(!exitloop)
  proc();
 XFreeFont(display, fontstruct);
 free_stuff();
 return 0;
}
