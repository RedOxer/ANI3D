#include "simulation.hpp"

using namespace cgp;

void simulate(std::vector<particle_structure>& particles, float dt, rotation_transform rotation)
{

    numarray<vec3> cube_normals = { {1,0,0}, {0,1,0}, {0,0,1}, {-1,0,0}, {0,-1,0}, {0,0,-1} };
    numarray<vec3> cube_face = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{1,1,1},{1,1,1},{1,1,1}};
    for(int rot = 0 ; rot < cube_normals.size(); ++rot)
    {
        cube_normals[rot] = rotation * cube_normals[rot];
        cube_face[rot] = rotation * cube_face[rot];
    }
    vec3 const g = { 0,0,-9.81f };
	size_t const N = particles.size();
	for (size_t k = 0; k < N; ++k)
	{
		particle_structure& particle = particles[k];
		vec3 const f = particle.m * g;
		particle.v = (1 - 0.9f * dt) * particle.v + dt * f;
		particle.p = particle.p + dt * particle.v;
        // dot product to determine if the particle is going to touch the plane
        for (int i = 0 ; i < cube_face.size(); ++i)
        {
            if (dot(particle.p-cube_face[i], cube_normals[i]) <= particle.r)
            {
                auto v_perp = dot(particle.v, cube_normals[i]) * cube_normals[i];
                auto v_para = particle.v - dot(particle.v, cube_normals[i]) * cube_normals[i];
                particle.v = 0.63f*v_para - 0.95f*v_perp;
                particle.p += (particle.r - (dot(particle.p-cube_face[i], cube_normals[i])))*cube_normals[i];
            }
        }
        for(int j = 0; j < N; ++j)
        {
            particle_structure& particle_aux = particles[j];
            //Collision Detection
            if(norm(particle_aux.p - particle.p) <= particle_aux.r + particle.r && j != k)
            {

                if(norm(particle_aux.v - particle.v) > 0.25f)
                {
                    auto u = (particle_aux.p - particle.p)/norm(particle_aux.p - particle.p);
                    auto J1 = 2 * (particle_aux.m * particle.m) / (particle_aux.m + particle.m) * dot(particle.v - particle_aux.v, u);
                    auto J = J1 * u;
                    particle_aux.v = particle_aux.v + (J /particle_aux.m);
                    particle.v = particle.v - (J /particle.m);
                }
                //Static Context
                if(norm(particle_aux.v - particle.v) <= 0.25f)
                {
                    //let's stop the sphere from moving
                   // particle_aux.v = {0,0,0};
                   // particle.v = {0,0,0};
                }
                //Correct Position

                auto u = (particle_aux.p - particle.p)/norm(particle_aux.p - particle.p);
                auto d = particle_aux.r + particle.r - norm(particle_aux.p - particle.p);
                particle_aux.p = particle_aux.p + d / 2 * u ;
                particle.p = particle.p - d / 2 * u ;
            }
        }
    }
}
vec3 handle_color(float dt,particle_structure &p)
{
    float r = p.c.x;
    float g = p.c.y;
    float b = p.c.z;
    float alea = (((double) rand() / (RAND_MAX))) ;
    auto temp = dt;
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
        particle.c = handle_color(particle.t,particle);
        if(particle.t <= 0)
        {
            particles.erase(particles.begin()+k);
        }
    }
}