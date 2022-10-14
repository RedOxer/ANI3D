#pragma once

#include "cgp/cgp.hpp"



struct particle_structure
{
    cgp::vec3 p; // Position
    cgp::vec3 v; // Speed
    cgp::vec3 c; // Color
    float a;     // Alpha Channel
    float r;     // Radius
    float m;     // mass
    float t;     // Time Span
};

void simulate(std::vector<particle_structure>& particles, float dt, cgp::rotation_transform rotation);
void simulate_fire(std::vector<particle_structure>& particles, float dt);


