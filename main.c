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

int main(int argc, char *argv[])
{
	/* basic setup */
	Display *display;
    Window win;
    XEvent an_event;
    static Atom wm_delete_window;

	int screen_num;
	int root_window;
		
	unsigned int screen_width;
	unsigned int screen_height;

	int white_pixel;
	int black_pixel;
    
    /* graphics setup */
    GC gc;   
	XGCValues values;
	values.cap_style = CapButt;
	values.join_style = JoinBevel;
	unsigned long valuemask = GCCapStyle | GCJoinStyle;

	/* font setup */
    XFontStruct *font_info;
    char *font_name = "*-helvetica-*-12-*";

    /* book said its needed */
    int font_loc_x;
    int font_loc_y;
    font_loc_x = 100;
    font_loc_y = 100;
    
    
	/* check for other launching variables */
	if(argc == 2 && strcmp("-v", argv[1]) == 0)
	{
		fputs("window VERSION: 0.1\n", stderr);
		return 0;
	}	

	if(argc == 2 && strcmp("-t", argv[1]) == 0)
	{ 
		fputs("TESTING: ran\n", stderr);
		return 0;
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
		return 0;
	}  
 
	if(argc != 1)
	{
		puts("usage: [-v -t -c]");
        	return 0;
	}

	/* open connection to xserver */
	display = XOpenDisplay(NULL);
	if(display == NULL)
	{
		fputs("ERROR: Unable to open connection to XServer\n", stderr);
		return 0;
	}
	
	/* get all basic information on the server */
	
	/* checks the number for the default screen of XServer */
	screen_num = DefaultScreen(display);
	/* makes ID of root window the the assinged default screen */
	root_window = RootWindow(display, screen_num);
	/* find the width of the XServer in int */ /* ps... the book did / 3 */
	screen_width = DisplayWidth(display, screen_num) / 3;
	/* find the height of the XServer in int */
	screen_height = DisplayHeight(display, screen_num)/ 3;

	/* less necessary */
	/* find the value of the white pixel in int */
	white_pixel = WhitePixel(display, screen_num);
	/* find the value of the black pixel in int ALT: 0x9999CC */
	black_pixel = BlackPixel(display, screen_num);

	/* creates a simple x11 window to appear */
	win = XCreateSimpleWindow(
		display, 
		root_window, 
		0, 0, 
		screen_width, screen_height, 
		0, 
		white_pixel, black_pixel);

	/* window has to be initialised befor assignment */
	gc = XCreateGC(display, win, valuemask, &values);
	if(gc < 0)
	{
		fputs("XCreateGC: \n", stderr);
	}
	font_info = XLoadQueryFont(display, font_name);
    if(!font_info)
    {
    	fprintf(stderr, "XLoadQueryFont: failed loading font '%s'\n", font_name);
		return 0;
    }
    
    /* assign font to gc */
    XSetFont(display, gc, font_info->fid);
	XSetBackground(display, gc, black_pixel);
	XSetForeground(display, gc, 0x9999CC);
	
	/* set title of the window */
	XStoreName(display, win, "My First x window");

	/* make visable to user */
	XMapWindow(display, win);
	/* calls the window manager icccm thing */
	wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	/* makes it use the wm to close? */
	XSetWMProtocols(display, win, &wm_delete_window, 1);
	/* ensure that its visable */
	XFlush(display);

	/* register events */
	XSelectInput(display, win, ExposureMask | KeyPressMask);

	/* window event loop */
	while(1)
	{
		XNextEvent(display, &an_event);
		switch(an_event.type)
		{
			case Expose:
				if(an_event.xexpose.count > 0)
				{
					break;
				}
				XDrawString(display, win, gc, font_loc_x, font_loc_y, "this took two nights", strlen("this took two nights"));
				XDrawString(display, win, gc, font_loc_x, font_loc_y + 20, "to make this", strlen("to make this"));
				XDrawString(display, win, gc, font_loc_x, font_loc_y + 40, "on my off time", strlen("on my off time"));
				
				XDrawLine(display, win, gc, 20, 20, 40, 100);
				XDrawRectangle(display, win, gc, 50, 50, 800, 800);
				XFlush(display);			
				break;
			case KeyPress:
					puts("user has pressed a button");
				break;
			case ClientMessage:
			/* I kinda get how the atom works partially got off stack overflow */
			/* tells window manager to deal close window and clean up? */
				if((Atom)an_event.xclient.data.l[0] == wm_delete_window)
				{
					break;
				}
				break;
			default:
				break;
		}
	}
	/* loop kicks out logic to clean up and close window */
	XFreeGC(display, gc);
	XSync(display, 0);
	XUnmapWindow(display, win);
	XDestroyWindow(display, win);
	XCloseDisplay(display);
	return 0;
}

