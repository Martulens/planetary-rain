#ifndef CALLBACKS_H
#define CALLBACKS_H
#include <memory>
#include "game.h"


void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY);
void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY);

// GLUT callback wrappers
void onDisplay();
void onReshape(int w, int h);

void onMouseClick(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onMouseScroll(int button, int dir, int x, int y);

void onSpecialKeyPress(int key, int x, int y);
void onSpecialKeyRelease(int key, int x, int y);

void updateMovement();

void timer(int);

// GLUT callback wrappers for ImGui
void uiKeyboardCallback(unsigned char key, int x, int y);
void uiMouseCallback(int button, int state, int x, int y);
void uiMotionCallback(int x, int y);
void uiReshapeCallback(int w, int h);


#endif
