#include <stdio.h>
#include <stdlib.h>
#include <pty.h>
#include <errno.h>

/* 
#define GET_BUF_Y(TERM) (int) (TERM.buf_ptr / TERM.cols)
#define GET_BUF_X(BUF) (int) (TERM.buf_ptr % TERM.cols)

typedef struct{
	
	XFontStruct font;
	int x_fd;
	int w, h; * size: rows * font.h X cols * font.w * 

	Display *dpy;
	int screen;
	Window root;

	Window win;
	GC gc;
	unsigned long 
}TermWindow;

typedef struct{
	
	TermWindow win;
	char *buf; 
	int cols, rows; 
	int buf_ptr; /* starts from 0 *

}Terminal;
*/

void 
fail(const char* msg)
{
		perror(msg);
		exit(1);
}

void
spawn_sh()
{
	printf("");
}

int
init_sh()
{

		int m_fd, s_fd; /* master - slave fds */

		if (openpty(&m_fd, &s_fd, NULL, NULL, NULL) == -1)
				fail("openpty failed! in __LINE__ of __FILE__."); /* TODO: that __LINE__ + __FILE___ combination can be implemented via a macro */

		switch (fork()){
				case -1:
				 	fail("fork failed in __LINE__ of __FILE__.");

				case 0:
					ioctl(s_fd, TIOCSCTTY, NULL) < 0 ? fail("ioctl failed in __LINE__ of __FILE__.") : NULL;

					close(m_fd);

					dup2(s_fd, 0);
					dup2(s_fd, 1);
					dup2(s_fd, 2);
					close(s_fd);

					setsid();

					spawn_sh();

				default:

					close(s_fd);
					return m_fd;


		}
}

void 
init_x()
{
	printf("");
}

void 
init()
{
	init_sh();
	init_x();

}

void 
run()
{
	return 0;
}

void 
procargs(int argc, char **argv)
{
		return 0;
}

int main(int argc, char **argv)
{

		procargs(argc, argv);
	
		init();
		run();
}
