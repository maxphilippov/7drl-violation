//
//  curses.hpp
//  7drl-violation
//
//  Just for a macro
//
//  Copyright © 2016 Max Philippov
//

#ifndef curses_h
#define curses_h

#if defined(_WIN32)
#include <curses.h>
#else
#include <ncurses.h>
#endif

#endif // curses_h