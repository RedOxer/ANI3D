#include "scene.hpp"


using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	timer.event_period = 0.5f;

	// Edges of the containing cube in [-1,1]^3
	//  Note: this data structure is set for display purpose - don't use it to compute some information on the cube - it would be un-necessarily complex
	numarray<vec3> border_cube = { {-1,-1,-1},{1,-1,-1}, {1,-1,-1},{1,1,-1}, {1,1,-1},{-1,1,-1}, {-1,1,-1},{-1,-1,-1},
		{-1,-1,1} ,{1,-1,1},  {1,-1,1}, {1,1,1},  {1,1,1}, {-1,1,1},  {-1,1,1}, {-1,-1,1},
		{-1,-1,-1},{-1,-1,1}, {1,-1,-1},{1,-1,1}, {1,1,-1},{1,1,1},   {-1,1,-1},{-1,1,1} };
	//cube_wireframe.initialize_data_on_gpu(border_cube);
	//cube_wireframe.display_type = curve_drawable_display_type::Segments;

	sphere.initialize_data_on_gpu(mesh_primitive_sphere());
    plane.initialize_data_on_gpu(mesh_primitive_quadrangle());
    plane.model.scaling = 100;
    plane.model.translation = {-50,-50,-1};

    bolder.initialize_data_on_gpu(mesh_primitive_sphere());

    this->environment.background_color = {0.1,0.1,0.1};

    mesh catapult_mesh = mesh_load_file_obj("./Assets/catapult.obj");

    catapult.initialize_data_on_gpu(catapult_mesh);
    catapult.model.scaling = 1.5f;
    catapult.material.color = {0.38, 0.22, 0.01};
    catapult.model.rotation = rotation_transform::from_axis_angle({1,0,0}, M_PI/2);
    catapult.model.translation = { 0.4,0.4,-0.2 };

    mesh catapult_arm_mesh = mesh_load_file_obj("./Assets/catapult_arm.obj");

    catapult_arm.initialize_data_on_gpu(catapult_arm_mesh);
    catapult_arm.model.scaling = 1.5f;
    catapult_arm.material.color = {0.38, 0.22, 0.01};
    catapult_arm.model.rotation = rotation_transform::from_axis_angle({1,0,0}, M_PI/2);
    catapult_arm.model.translation = { 0.4,0.4,-0.2 };

    bolder.model.scaling = 0.4;

    hierarchy.add(catapult, "catapult", {});
    hierarchy.add(catapult_arm, "catapult_arm", "catapult",{-0.5,3.7,0.1});
    hierarchy.add(bolder,"bolder","catapult_arm",{0.3,2.2,3.2});

}
void scene_structure::Catapult()
{
    angler += 0.05 * neg;
    if(angler > M_PI/8 || angler < -1 * M_PI/7)
    {
        count++;
        neg *= -1;
    }
    if(count > 1) {
        if (offset == 0)
            offset = timer.t;
        gui.Launch = false;
        arrow = true;
        count = 0;
    }
    hierarchy["catapult_arm"].transform_local.rotation = rotation_transform::from_axis_angle({1,0,0}, angler);
    bolder.material.color = {1,1,1};
    bolder.model.scaling = 0.4;

    if(bolder.model.translation.z <=0)
    {
        emit_fire_particle(bolder.model.translation.x,bolder.model.translation.y,0);
    }
}
void scene_structure::burn_fire()
{
    if(timer.t < fixe)
    {
        emit_fire_particle(impact_x,impact_y,-1);
    }
}
void scene_structure::simulate_arrow()
{
    bolder.model.translation.y -= (timer.t - offset) * 15;
    auto y = bolder.model.translation.y;
    bolder.model.translation.z = -0.02 *y*y + 4;
    draw(bolder,environment);
    emit_fire_particle(bolder.model.translation.x,bolder.model.translation.y,bolder.model.translation.z);
    if(bolder.model.translation.z <= -1)
    {
        arrow = false;
        offset = 0;
        impact_x = bolder.model.translation.x;
        impact_y = bolder.model.translation.y;
        bolder.model.translation = {0,7,2};
        fixe = timer.t +5;
    }
}
void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);
    auto rotation = rotation_transform::from_axis_angle({0,1,0}, M_PI/2 * timer.t );
    cube_wireframe.model.rotation = rotation;
	timer.update();

	// Create a new particle if needed
	// emit_particle();
	// Call the simulation of the particle system
	float const dt = 0.01f * timer.scale;
	// simulate(particles, dt, rotation);
    simulate_fire(fire_particles,dt);
    std::tuple<float,float> res;
    if(arrow)
        simulate_arrow();

    draw(plane,environment);
    draw(catapult,environment);

	// Display the result
	sphere_display();
    bolder.model.translation = {0,5,2};

	if (gui.display_frame)
		draw(global_frame, environment);

    if(gui.Launch)
    {
        Catapult();
    }
    else if(angler >= 0) {
        hierarchy["catapult_arm"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, angler);
        angler -= dt;
    }
    if(arrow)
        hierarchy["bolder"].transform_local.scaling = 0;
    else
        hierarchy["bolder"].transform_local.scaling = 1;
    burn_fire();
    hierarchy.update_local_to_global_coordinates();
    draw(hierarchy,environment);
}

void scene_structure::sphere_display()
{
	// Display the particles as spheres
	size_t N = particles.size();
	for (size_t k = 0; k < N; ++k)
	{
		particle_structure const& particle = particles[k];
		sphere.material.color = particle.c;
		sphere.model.translation = particle.p;
		sphere.model.scaling = particle.r;

		draw(sphere, environment);
	}

	// Display the box in which the particles should stay
	draw(cube_wireframe, environment);
    // Display the particles as spheres
    N = fire_particles.size();
    for (size_t k = 0; k < N; ++k)
    {
        particle_structure const& particle = fire_particles[k];
        sphere.material.color = particle.c;
        sphere.material.alpha= 0.01;
        sphere.model.translation = particle.p;
        sphere.model.scaling = particle.r;

        draw(sphere, environment);
    }
}

void scene_structure::emit_particle()
{
	// Emit particle with random velocity
	//  Assume first that all particles have the same radius and mass
	static numarray<vec3> const color_lut = { {1,0,0},{0,1,0},{0,0,1},{1,1,0},{1,0,1},{0,1,1} };
	if (timer.event && gui.add_sphere) {
		float const theta = rand_interval(0, 2 * Pi);
		vec3 const v = vec3(1.0f * std::cos(theta), 1.0f * std::sin(theta), 4.0f);

		particle_structure particle;
		particle.p = { 0,0,0 };
		particle.r = 0.1f;
		particle.c = color_lut[int(rand_interval() * color_lut.size())];
		particle.v = v;
		particle.m = 1.0f; //
        particle.t = 1.0f;

		particles.push_back(particle);
	}
}

void scene_structure::emit_fire_particle(float x, float y, float z)
{
    // Emit particle with random velocity
    //  Assume first that all particles have the same radius and mass
    for (int i = 0; i < gui.FireDensity; ++i)
    {
    if (gui.add_sphere)
    {
        /* generate secret number between 1 and 10: */
        float const rng =  ((double) rand() / (RAND_MAX));
        float const rng_aux =  ((double) rand() / (RAND_MAX));

        vec3 const v = vec3(1.0f * std::cos(rng), 1.0f * std::sin(rng), 4.0f);

        particle_structure particle;
        particle.p = { x+rng/2, y+rng_aux/2, z};
        particle.r = 0.05f;
        particle.c = {1,1,1};
        particle.v = {0,0,100};

        float spread = 0.3f;
        vec3 maindir = vec3(0.0f, 0, 1);
        vec3 randomdir = vec3((rand() % 2000 - 1000.0f) * 2 / 1000.0f, (rand() % 2000 - 1000.0f) * 2 / 1000.0f,1);
        particle.v = maindir + randomdir * spread;
        particle.m = 0.3f; //
        particle.t = gui.FireLength;

        fire_particles.push_back(particle);
    }
}
}


void scene_structure::display_gui()
{
	ImGui::SliderFloat("Fire Intensity", &timer.scale, 0.05f, 2.0f, "%.2f %");
    ImGui::SliderFloat("Fire Length", &gui.FireLength, 0.05f, 2.0f, "%.2f %");
    ImGui::SliderInt("Fire Density", &gui.FireDensity, 10, 150, "%.2f %");

    ImGui::Checkbox("Turn Fire On/Off", &gui.add_sphere);
    ImGui::Checkbox("Launch Bomb", &gui.Launch);


}

void scene_structure::mouse_move_event()
{
    if (!inputs.keyboard.shift)
        camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

