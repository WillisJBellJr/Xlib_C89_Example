/* 
* cc -Wall -Werror -std=c89 -I/usr/local/include -L/usr/local/lib -lX11 -o basicWindow main.c
* cd documents/coding/xlib/ex00
* 
* BSD Zero Clause License
* Copyright (C) 2026 Willis Bell <bellwillis426@gmail.com>
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL 
* THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
* OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

int launch_variables(int argc, char *argv[]);

Window setup_window(Display *display, int screen_num, int screen_width, int screen_height, int x, int y);

GC setup_gc(Display *display, Window win);

void do_draw(Display *display, Window win, GC gc, int font_loc_x, int font_loc_y, int screen_width, int screen_height);

void free_that_thing(Display *display, Window win);

int main(int argc, char *argv[])
{
	char *font_name;
	int screen_num;
	int font_loc_x;
    int font_loc_y;
    int window_running;
    unsigned int screen_width;
    unsigned int screen_height;
	
	/* basic setup */
	Display *display;
	XFontStruct *font_info;
    Window win;
    GC gc;
    XEvent an_event;
    static Atom wm_delete_window;
    
	if(launch_variables(argc, argv))
	{
		return 0;
	}
	
	/* open connection to xserver */
	display = XOpenDisplay(NULL);
	if(display == NULL)
	{
		fputs("ERROR: Unable to open connection to XServer\n", stderr);
		return 0;
	}
	puts("display done...");
	
	/* checks the number for the default screen of XServer */
	screen_num = DefaultScreen(display);
	
	/* find the width of the XServer in int */ /* ps... the book did / 3 */
	screen_width = DisplayWidth(display, screen_num) / 3;

	/* find the height of the XServer in int */
	screen_height = DisplayHeight(display, screen_num)/ 3;
	
	win = setup_window(display, screen_num, screen_width, screen_height, 0, 0);
	
    /* calls the window manager icccm thing */
	wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	
	/* makes it use the wm to close? */
	XSetWMProtocols(display, win, &wm_delete_window, 1);
	
	/* for exiting the loop */
    window_running = 1;
    
	gc = setup_gc(display, win);
	
	/* font setup */
    font_name = "*-helvetica-*-12-*";
    font_loc_x = 100;
    font_loc_y = 100;
	font_info = XLoadQueryFont(display, font_name);
    if(!font_info)
    {
    	fprintf(stderr, "XLoadQueryFont: failed loading font '%s'\n", font_name);
    	XFreeGC(display, gc);
		free_that_thing(display, win);
		return 0;
    }
    puts("fonts done...");
    
    /* assign font to gc */
    XSetFont(display, gc, font_info->fid);
	
	/* ensure that its visable */
	XFlush(display);

	/* register events */
	XSelectInput(display, win, ExposureMask | KeyPressMask);
	do_draw(display, win, gc, font_loc_x, font_loc_y, screen_width, screen_height);
	/* window event loop */
	while(window_running == 1)
	{
		XNextEvent(display, &an_event);
		switch(an_event.type)
		{
			case Expose:
				if(an_event.xexpose.count > 0)
				{
					break;
				}
				do_draw(display, win, gc, font_loc_x, font_loc_y, screen_width, screen_height);
				break;
			case KeyPress:
					puts("user has pressed a button");
				break;
			case ClientMessage:
			/* I kinda get how the atom works partially got off stack overflow */
			/* tells window manager to deal with close window and clean up? */
				if((Atom)an_event.xclient.data.l[0] == wm_delete_window)
				{
					puts("closing loop...");
					/* breaks loop */
					window_running = 0;
				}
				break;
			default:
				break;
		}
	}
	
	puts("loop broken!");
	puts("exiting...");
	/* loop kicks out logic to clean up and close window */
	XFreeFont(display, font_info);
	XFreeGC(display, gc);
	free_that_thing(display, win);
	puts("disconnected from server");
	puts("bye bye >:]");
	return 0;
}

int launch_variables(int argc, char *argv[])
{   
	if(argc == 1)
	{
        return 0;
	}
	 
	/* check for other launching variables */
	if(argc == 2 && strcmp("-v", argv[1]) == 0)
	{
		fputs("window VERSION: 0.3\n", stderr);
		return 1;
	}	

	if(argc == 2 && strcmp("-t", argv[1]) == 0)
	{ 
		fputs("TESTING: ran\n", stderr);
		return 1;
	}

	if(argc == 2 && strcmp("-c", argv[1]) == 0)
	{ 
		puts("Copyright (C) 2026 Willis Bell <bellwillis426@gmail.com>");
		puts("");
		puts("THE SOFTWARE IS PROVIDED ""AS IS"" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS");
		puts("THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES");
		puts("WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,");
		puts("NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE");
		puts("OR PERFORMANCE OF THIS SOFTWARE.");
		puts("");
		return 1;
	}  
 
	puts("usage: [-v -t -c]");
    return 1;
}

Window setup_window(Display *display, int screen_num, int screen_width, int screen_height, int win_loc_x, int win_loc_y)
{
	Window win;

	/* create window */
    win = XCreateSimpleWindow(
		display, 
		RootWindow(display, screen_num), 
		win_loc_x, win_loc_y, 
		screen_width, screen_height, 
		2, 
		XWhitePixel(display, screen_num), XBlackPixel(display, screen_num));
	
	/* set title of the window */	
	XStoreName(display, win, "XWindow");
		
	/* make visable to user */
	XMapWindow(display, win);
	
	/* ensure that its visable */
	XFlush(display);
	
	puts("window done...");
	return win;		
}

GC setup_gc(Display *display, Window win)
{
    /* graphics setup */
    GC gc;   
	XGCValues values;
	values.line_style = LineSolid;
	values.cap_style = CapButt;
	values.join_style = JoinBevel;
	values.line_width = 2;
	unsigned long valuemask = GCCapStyle | GCJoinStyle;

	/* window has to be initialised befor assignment */
	gc = XCreateGC(display, win, valuemask, &values);
	if(!gc)
	{
		fputs("XCreateGC: failed\n", stderr);
		free_that_thing(display, win);
		return 0;
	}
	
	XSetBackground(display, gc, 0x000000);
	XSetForeground(display, gc, 0x9999CC);
	
	puts("graphics done...");
	return gc;
}

void do_draw(Display *display, Window win, GC gc, int font_loc_x, int font_loc_y, int screen_width, int screen_height)
{
	XDrawString(display, win, gc, font_loc_x, font_loc_y, "this took two nights", strlen("this took two nights"));
	XDrawString(display, win, gc, font_loc_x, font_loc_y + 20, "to make this", strlen("to make this"));
	XDrawString(display, win, gc, font_loc_x, font_loc_y + 40, "on my off time.", strlen("on my off time"));
	XDrawString(display, win, gc, font_loc_x - 30, font_loc_y + 60, "Another night now couldnt sleep", strlen("Another night now couldnt sleep"));

	XDrawLine(display, win, gc, 20, 20, 40, 100);
	XDrawRectangle(display, win, gc, 50, 50, screen_width / 3	, screen_height / 3);
	XFlush(display);	
}

void free_that_thing(Display *display, Window win)
{
	XSync(display, 0);
	XUnmapWindow(display, win);
	XDestroyWindow(display, win);
	XCloseDisplay(display);
}
