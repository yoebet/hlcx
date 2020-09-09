
/*  screen acception using */

#define SPACE 32
#define ESC   27
#define SQUARE   91
#define BS  8
#define ENTER 10
#define CTRL_F 6     /* replace KET_F(0) */
#define CTRL_Q 17    /* touch stdscr window */
#define CTRL_U 21    /* replace KEY_UP */
#define CTRL_D 4     /* replace KEY_DOWN */
#define CTRL_L 12    /* replace KEY_LEFT */
#define CTRL_R 18    /* replace KEY_RIGHT */
#define PgDn 338 
#define PgUp 339

#define Ok    0
#define Exit -1
#define Err  -1

#define Printer  "/dev/lp0"

typedef struct {
    short x;
    short y;
    char *item;
    char *mesg;
} menuitm;

typedef struct {
    short x;
    short y;
    char *item;
} cenitem;

