#ifndef HOVERCRAFT_H
#define HOVERCRAFT_H

typedef enum{
    HOVERCRAFT_MOVING,
    HOVERCRAFT_TURNING
} hovercraft_modes;

void hovercraft_init();
void hovercraft_act();

#endif