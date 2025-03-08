#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>
#include "modules/display/display.h"

typedef void (*MenuAction)(DisplayManager *dm);

typedef struct {
    const char *label;
    const char* menuIcon;
    MenuAction action;
} MenuItem;

typedef struct {
    const char *title;
    MenuItem *items;
    size_t num_items;
} Menu;

#endif
