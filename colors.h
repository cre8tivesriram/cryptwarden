// colors.h
#ifndef COLORS_H
#define COLORS_H

// ANSI color codes
#define COLOR_RESET     "\033[0m"
#define COLOR_RED       "\033[1;31m"
#define COLOR_GREEN     "\033[1;32m"
#define COLOR_YELLOW    "\033[1;33m"
#define COLOR_MAGENTA   "\033[1;35m"
#define COLOR_CYAN      "\033[1;36m"
#define COLOR_BOLD      "\033[1m"

// Function declarations
void print_colored(const char *color, const char *text);
void print_coloredf(const char *color, const char *fmt, ...);

#endif
