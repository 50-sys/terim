#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pty.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define TERM_NAME "terim"
 
/* #define GET_BUF_Y(TERM) (int) (TERM.buf_ptr / TERM.cols)
   #define GET_BUF_X(BUF) (int) (TERM.buf_ptr % TERM.cols)
*/


typedef struct{ 

	char *shell;
	char *geo;
	
}Args;

typedef struct{
	
	XFontStruct font;
	int w, h; /* size: rows * font.h X cols * font.w */

	Display *dpy;
	int screen;
	Window root;

	int x_fd;

	Window win;
	GC gc;

}TermWindow;

typedef struct{
	
	Args args;

	TermWindow *win;
	char *buf; 
	int cols, rows; 
	unsigned int bufptr; /* starts from 0 */
	int m_fd;

}Terminal;


/* Using macros can be a debugging problem in the feature, just a little sacrifice. */
int
max(int a, int b)
{
	return a > b ? a : b;
}

int
min(int a, int b)
{
	return a < b ? a : b;
}

void 
fail(const char* msg)
{
		perror(msg);
		exit(1);
}

void
spawn_sh()
{
	struct passwd *usr;
	
	(usr = getpwuid(getuid())) ? NULL : fail("getpwuid failed in __LINE__ of __FILE__."	);

	setenv("USER", usr->pw_name, 1);
	setenv("LOGNAME", usr->pw_name, 1);
	setenv("HOME", usr->pw_dir, 1);
	setenv("SHELL", usr->pw_shell, 1); /* TODO: add option to specify the shell from command line */
	setenv("TERM", TERM_NAME, 1);


	execlp(getenv("SHELL"), "-", (char *) NULL); 
	fail("execvp failed in __LINE__ of __FILE__.");
	
}

int
init_sh()
{

		int m_fd, s_fd; /* master - slave fds */

		if (openpty(&m_fd, &s_fd, NULL, NULL, NULL) == -1)
				fail("openpty failed in __LINE__ of __FILE__."); /* TODO: that __LINE__ + __FILE___ combination can be implemented via a macro */

		switch (fork()){
				case -1:
				 	fail("fork failed in __LINE__ of __FILE__.");

				case 0:
					close(m_fd);
					setsid();
					ioctl(s_fd, TIOCSCTTY, NULL) < 0 ? fail("ioctl failed in __LINE__ of __FILE__.") : NULL;


					dup2(s_fd, 0);
					dup2(s_fd, 1);
					dup2(s_fd, 2);
					close(s_fd);


					spawn_sh();

				default:

					printf("about to close the s_fd\n");
					close(s_fd);
					printf("closed the s_fd\n");
					return m_fd;


		}}

int
init_x(Terminal *term, TermWindow *win)
{
	int error;

	return error;
}

int
init(Terminal *term, TermWindow *win)
{
	int error;

	if (!(term->m_fd = init_sh()))
			return 1;

	printf("passed init_sh\n");
	init_x(term, win); 
	printf("passed init_x\n");

	error = 0;
	return error;
}

int
draw(Terminal *term)
{
		return 0;
}

void
write_to_main_buf(Terminal *term, char *buf, unsigned int bufptr)
{
	memmove(term->buf + term->bufptr, buf, bufptr);
	term->buftr += bufptr;
}

unsigned int
proc_s(Terminal *term, static char *buf, static unsigned int bufptr) /* processes the chars, adds them to main buffer and returns which are unprocessed */
{
	write_to_main_buf(term, buf, bufptr);	
	return 0;
		
}


int 
read_s(Terminal *term)
{
	static char buf[BUFSIZ]; /* chars will be kept here before being processed */
	static unsigned int bufptr = 0; /* starts from 0, denotes exactly where to stop processing (last char to be processed + 1), start writing */
	int char_read, remain;

	if ((char_read = read(term->m_fd, buf + bufptr, BUFSIZ - bufptr)) == -1)
			fail("read failed in __LINE__ of __FILE__.");

	remain = proc_s(term, buf, bufptr);
	memmove(buf, buf + remain, (bufptr + char_read - remain));
	bufptr = remain; 

	exit(0);

}

int 
read_x(Terminal *term)
{
	return 0;

}


int
reads(Terminal *term, int s, int x)
{
	int ret = 0;
	
	if (s)
			ret |= read_s(term);
	if (x) 
			ret |= read_x(term);

	return ret;
}

int
run(Terminal *term, TermWindow *win)
{
	printf("inside of  run\n");
	fd_set fd_r; /* read */
	int redraw;
	int sread, xread;
	int m_fd, x_fd;
	int error;
	const struct timespec timeout = {1, 10};

	m_fd = term->m_fd;
	/* TODO: x_fd = term->win->x_fd; */
	x_fd = 10;



	printf("now in main loop\n");
	for (;;)
	{

		FD_ZERO(&fd_r);
		FD_SET(m_fd, &fd_r);
		/* FD_SET(x_fd, &fd_r); */

		/* pselect is used for the timespec struct and it not modifying this argument */
		/* TODO: use poll in supporting systems */
		if (pselect(max(m_fd, x_fd) + 1, &fd_r, NULL, NULL, &timeout, NULL) == -1)
				fail("pselect failed in __LINE__ of __FILE__.");


		if (FD_ISSET(m_fd, &fd_r))
				sread = 1;
		if (FD_ISSET(x_fd, &fd_r))
				xread = 1;

		redraw = reads(term, sread, xread);


		if (redraw)
				draw(term);
	}

	return error;

}

int
procargs(int argc, char **argv, Terminal *term)
{
	
	int error;
	Args args;


	error = 0;
	term->args = args;
	return error;
}

int main(int argc, char **argv)
{
		TermWindow *win;
		Terminal *term;

		printf("started\n");

		/* 0 means no error, 1 means error */
		return !(!procargs(argc, argv, term) && !init(term, win) && !run(term, win)); 
}
