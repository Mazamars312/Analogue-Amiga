#include <ncurses.h>
#include <string.h>

struct regfile
{
	int regs[8];
	int tmp;
	char z,c,cond;
};

WINDOW *create_newwin(const char *title,int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

#define REGS_WIDTH 48
#define REGS_HEIGHT 6

void draw_regfile(WINDOW *w,struct regfile *rf)
{
	int i;
	for(i=0;i<8;++i)
	{
		mvwprintw(w,1+(i&3),2+(i>>2)*15,"r%d: %08x",i,rf->regs[i]);
	}
	mvwprintw(w,1,32,"tmp: %08x",i,rf->tmp);
	mvwprintw(w,2,32,"z: %d",rf->z&1);
	mvwprintw(w,3,32,"c: %d",rf->c&1);
	mvwprintw(w,4,32,"cond: %d",rf->cond&1);
	wrefresh(w);
}


int confirm()
{
	while(1)
	{
		char ch = getch();
		switch(ch)
		{
			case 'y':
			case 'Y':
				return(1);
				break;
			case 'n':
			case 'N':
			case 27:
				return(0);
				break;
			default:
				break;
		}
	}
}


int main(int argc, char *argv[])
{
	int x, y;
	int ch;
	int running=1;
	struct regfile regs;
	WINDOW *reg_win;
	WINDOW *dis_win;
	WINDOW *stack_win;
	WINDOW *mem_win;
	WINDOW *cmd_win;

	initscr();			/* Start curses mode 		*/
	cbreak();			/* Capture input directly	*/
	keypad(stdscr, TRUE);
	noecho();

	refresh();
	reg_win=create_newwin("Register File",REGS_HEIGHT,REGS_WIDTH,0,0);
	dis_win=create_newwin("Disassembly",LINES-REGS_HEIGHT-1,REGS_WIDTH,REGS_HEIGHT,0);
	stack_win=create_newwin("Stack",LINES/2-1,COLS-REGS_WIDTH,0,REGS_WIDTH);
	mem_win=create_newwin("Memory",LINES-LINES/2,COLS-REGS_WIDTH,LINES/2-1,REGS_WIDTH);
	cmd_win=newwin(1,COLS,LINES-1,0);

	draw_regfile(reg_win,&regs);

	move(LINES-1,2);

	while(running)
	{
		mvwprintw(cmd_win,0,0,"                ");
		curs_set(0);
		wrefresh(cmd_win);

		ch = getch();
		switch(ch)
		{
			case KEY_LEFT:
				break;
			case KEY_RIGHT:
				break;
			case KEY_UP:
				break;
			case KEY_DOWN:
				break;
			case KEY_BACKSPACE: /* backspace */
				break;

			case 'q':
			case 'Q':
				mvwprintw(cmd_win,0,0,"Really quit? ");
				wrefresh(cmd_win);
				curs_set(2);
				if(confirm())
					running=0;
				break;

			case 'r':
			case 'R':
			case 'w':
			case 'W':
				mvwprintw(cmd_win,0,0,"%c: ",ch);
				wrefresh(cmd_win);
				curs_set(2);
				break;

			case 10: /* enter */
				move(LINES-1,2);
				break;

			default:
				break;
		}
	}
	delwin(reg_win);
	delwin(dis_win);
	delwin(stack_win);
	delwin(mem_win);
	endwin();			/* End curses mode		  */
	return 0;
}

WINDOW *create_newwin(const char *title,int height, int width, int starty, int startx)
{
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	mvwprintw(local_win,0,(width-(2+strlen(title)))/2," %s ",title);
	wrefresh(local_win);

	return local_win;
}

