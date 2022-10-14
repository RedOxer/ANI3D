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
float linear_interpolation_coeff(float val, float prev, float next)
{
    return (next - prev) / (val - prev);
}
vec3 handle_color(float dt,particle_structure &p)
{
    float r = 255;
    float g = 255;
    float b = 255;
    float a = 120;
    float alea = (((double) rand() / (RAND_MAX))) ;
    float prev = 0.05f;
    float next =  0.06+alea / 10;
    auto temp = 0.9-dt;
    if (temp >= prev)
    {
        if (temp <= next)
        {
            b *= linear_interpolation_coeff(temp, prev, next) * 2;
            a *= linear_interpolation_coeff(temp, prev, next);
        } else
        {
            prev = next + alea;
            next = 0.40 + alea;
            b = 0;
            if (temp <= next)
            {
                g *= linear_interpolation_coeff(temp, prev, next) * 2;
                a *= linear_interpolation_coeff(temp, prev, next);
            } else
            {
                prev = 0.65 + alea;
                next = 0.79 + alea;
                g = 0;
                if (temp >= prev && temp <= next)
                {
                    r *= linear_interpolation_coeff(temp, prev, next) * 2;
                    a *= linear_interpolation_coeff(temp, prev, next);
                } else if (temp >= next)
                {
                    r = 0;
                    a *= linear_interpolation_coeff(temp, prev, next);
                }
            }
        }
    }
    p.a = a/255;
    return vec3(r/255,g/255,b/255);
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