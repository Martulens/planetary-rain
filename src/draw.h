//
// Created by martul3ns on 1/19/26.
//

#ifndef DRAW_H
#define DRAW_H
#include "glm/vec3.hpp"

// drawing functions
// -> all that visualises

void drawWindow();

void drawSphere(glm::vec3 position, float radius);
void drawCube(glm::vec3 position, float radius);

#endif //DRAW_H
