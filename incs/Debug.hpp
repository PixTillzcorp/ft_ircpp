#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <stdio.h>

/*----------------------------------------------------------------------------*/

# define BLACK "\033[38;5;0m"
# define NORMAL "\033[0m"

/* ~ */

# define DARK_RED "\033[38;5;88m"
# define RED "\033[38;5;196m"

/* ~ */

# define DARK_GREEN "\033[38;5;28m"
# define GREEN "\033[38;5;46m"

/* ~ */

# define DARK_YELLOW "\033[38;5;136m"
# define YELLOW "\033[38;5;226m"

/* ~ */

# define DARK_ORANGE "\033[38;5;130m"
# define ORANGE "\033[38;5;202m"

/* ~ */

# define DARK_BLUE "\033[38;5;18m"
# define BLUE "\033[38;5;27m"

/* ~ */

# define DARK_PINK "\033[38;5;169m"
# define PINK "\033[38;5;207m"

/* ~ */

# define DARK_PURPLE "\033[38;5;54m"
# define PURPLE "\033[38;5;57m"

/* ~ */

# define DARK_CYAN "\033[38;5;37m"
# define CYAN "\033[38;5;45m"

/* ~ */

# define DARK_GREY "\033[38;5;240m"
# define GREY "\033[38;5;245m"

/* ~ */
# define COLOR_DEMO(x){	DEBUG_DISPC(COUT, x, DARK_RED);\
						DEBUG_DISPC(COUT, x, RED);\
						DEBUG_DISPC(COUT, x, DARK_GREEN);\
						DEBUG_DISPC(COUT, x, GREEN);\
						DEBUG_DISPC(COUT, x, DARK_YELLOW);\
						DEBUG_DISPC(COUT, x, YELLOW);\
						DEBUG_DISPC(COUT, x, DARK_ORANGE);\
						DEBUG_DISPC(COUT, x, ORANGE);\
						DEBUG_DISPC(COUT, x, DARK_BLUE);\
						DEBUG_DISPC(COUT, x, BLUE);\
						DEBUG_DISPC(COUT, x, DARK_PINK);\
						DEBUG_DISPC(COUT, x, PINK);\
						DEBUG_DISPC(COUT, x, DARK_PURPLE);\
						DEBUG_DISPC(COUT, x, PURPLE);\
						DEBUG_DISPC(COUT, x, DARK_CYAN);\
						DEBUG_DISPC(COUT, x, CYAN);\
						DEBUG_DISPC(COUT, x, DARK_GREY);\
						DEBUG_DISPC(COUT, x, GREY);}
/* ~ */

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

#define COUT std::cout
#define ENDL std::endl
#define JUMP std::cout << std::endl

#define _VAR(x) << x
#define VAR_(x) x <<
#define _VAR_(x) << x <<

#define DEBUG_DISP(f, x) f _VAR_(x) ENDL
#define DEBUG_DISP_NOENDL(f, x) f _VAR(x)
#define DEBUG_DDISP(f, x, y) f _VAR(x) _VAR_(y) ENDL
#define DEBUG_DDISP_NOENDL(f, x, y) f _VAR(x) _VAR(y)

/* ~ */

#define REV_BRACKET(b) (b == '[' ? ']' : (b == '(' ? ')' : (b == '{' ? '}' : b)))
#define BRACKET(x, b) VAR_(b) x _VAR(REV_BRACKET(b))
#define DEBUG_DISPB(f, x, b) DEBUG_DISP(f, BRACKET(x,b))
#define DEBUG_DISPB_NOENDL(f, x, b) DEBUG_DISP_NOENDL(f, BRACKET(x,b))
#define DEBUG_DDISPB(f, x, y, b) DEBUG_DDISP(f, BRACKET(x,b), BRACKET(y,b))
#define DEBUG_DDISPB_NOENDL(f, x, y, b) DEBUG_DDISP_NOENDL(f, BRACKET(x,b), BRACKET(y,b))
#define DEBUG_LDISPB(f, x, y, b) DEBUG_DDISP(f, x, BRACKET(y,b))
#define DEBUG_LDISPB_NOENDL(f, x, y, b) DEBUG_DDISP_NOENDL(f, x, BRACKET(y,b))

/* ~ */

#define COLORED(x, c) VAR_(c) VAR_(x) NORMAL
#define CBRACKET(c, x, b) COLORED(BRACKET(x, b), c)
#define DEBUG_DISPC(f, x, c) DEBUG_DISP(f, COLORED(x, c))
#define DEBUG_DISPC_NOENDL(f, x, c) DEBUG_DISP_NOENDL(f, COLORED(x, c))
#define DEBUG_DDISPC(f, x, y, c) DEBUG_DDISP(f, COLORED(x, c), COLORED(y, c))
#define DEBUG_DDISPC_NOENDL(f, x, y, c) DEBUG_DDISP_NOENDL(f, COLORED(x, c), COLORED(y, c))

/* ~ */

#define DEBUG_DISPCB(f, x, c, b) DEBUG_DISP(f, COLORED(BRACKET(x, b), c))
#define DEBUG_DISPCB_NOENDL(f, x, c, b) DEBUG_DISP_NOENDL(f, COLORED(BRACKET(x, b), c))
#define DEBUG_DDISPCB(f, x, y, c, b) DEBUG_DDISP(f, COLORED(BRACKET(x, b), c), COLORED(BRACKET(y, b), c))
#define DEBUG_DDISPCB_NOENDL(f, x, y, c, b) DEBUG_DDISP_NOENDL(f, COLORED(BRACKET(x, b), c), COLORED(BRACKET(y, b), c))
#define DEBUG_LDISPCB(f, x, y, c, b) DEBUG_DDISP(f, COLORED(x, c), COLORED(BRACKET(y, b), c))
#define DEBUG_LDISPCB_NOENDL(f, x, y, c, b) DEBUG_DDISP_NOENDL(f, COLORED(x, c), COLORED(BRACKET(y, b), c))

/* ~ */

#define DEBUG_BAR_DISP(f, chr, n) DEBUG_DISP(f, std::string(n, chr))
#define DEBUG_BAR_DISPB(f, chr, n, b) DEBUG_DISPB(f, std::string(n, chr), b)
#define DEBUG_BAR_DISPC(f, chr, n, c) DEBUG_DISPC(f, std::string(n, chr), c)

/*----------------------------------------------------------------------------*/

#endif
