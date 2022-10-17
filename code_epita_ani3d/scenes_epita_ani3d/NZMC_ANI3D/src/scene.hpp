#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "simulation.hpp"

using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = false;
	bool add_sphere = true;
    float FireLength = 0.9f;
    int FireDensity = 50;
    bool Launch = false;
    float x_v0 = 0;
    float x_angle = 0;
};

// The structure of the custom scene
struct scene_structure : scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;
    float angler = 0;
    int neg = -1;
    int count = 0;
    float offset = 0;
    bool arrow =false;
    float fixe = 0;
    float impact_x =5;
    float impact_y = 5;
    cgp::hierarchy_mesh_drawable hierarchy;
    mesh_drawable catapult;
    mesh_drawable catapult_arm;
    mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	cgp::timer_event_periodic timer;
	std::vector<particle_structure> particles;
    std::vector<particle_structure> fire_particles;
    cgp::mesh_drawable sphere;
    cgp::mesh_drawable bolder;
    cgp::mesh_drawable plane;
	cgp::curve_drawable cube_wireframe;




    // ****************************** //
	// Functions
	// ****************************** //

    void Catapult();

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();
    void burn_fire();
    void simulate_arrow();
	void emit_particle();
	void simulation_step(float dt);
	void sphere_display();

    void emit_fire_particle(float x, float y, float z);
};





