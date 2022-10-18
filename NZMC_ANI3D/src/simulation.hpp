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

float model_trajectory(float v0,float theta, float x);
void simulate_fire(std::vector<particle_structure>& particles, float dt);


