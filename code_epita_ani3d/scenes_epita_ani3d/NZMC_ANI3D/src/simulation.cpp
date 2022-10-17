#include "simulation.hpp"

#include <cmath>

using namespace cgp;

float model_trajectory(float v0,float theta, float x)
{
    return 2 + x* std::tan(theta) - ((9.8 * (x*x))/ 2 * v0*v0* std::cos(theta) * std::cos(theta)) * (x*x);
}
vec3 handle_color(particle_structure &p)
{
    float r = p.c.x;
    float g = p.c.y;
    float b = p.c.z;
    if(b <= 0.05)
    {
        if(g <= 0.05)
        {
            if(r <= 0.05)
            {
                return {r,g,b};
            }
            else
            {
                r *= 0.85f;
            }
        }else
        {
            g *= 0.90;
        }
    }
    else
    {
        b *= 0.93;
    }
    return {r,g,b};
}
void simulate_fire(std::vector<particle_structure>& particles, float dt)
{
    vec3 const g = { 0,0,9.81f };
    size_t const N = particles.size();
    for (size_t k = 0; k < N; ++k)
    {
        particle_structure& particle = particles[k];
        vec3 const f = particle.m * g;
        //Inital Position and Speed Update
        particle.v = (1 - 0.9f * dt) * particle.v + dt * f;
        particle.p = particle.p + dt * particle.v;
        //Add Perturbation to enhance realism
        particle.v += vec3(0.0f, 0, 5 * ((double) rand() / RAND_MAX)) * (float) dt * 0.5f;
        particle.p += (particle.v * vec3(0.8, 0.8, 0.4 + ((double) rand() / RAND_MAX))) * (float) dt;
        particle.a = 0;
        particle.t -= dt;
        particle.c = handle_color(particle);
        if(particle.t <= 0)
        {
            particles.erase(particles.begin()+k);
        }
    }
}