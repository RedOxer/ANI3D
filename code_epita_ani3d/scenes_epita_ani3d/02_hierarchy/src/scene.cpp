#include "scene.hpp"

#include <cmath>


using namespace cgp;


void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	// Create the hierarchy
	// ************************************ //

	// Initialize the temporary mesh_drawable that will be inserted in the hierarchy
	mesh_drawable body;
	mesh_drawable wing_left;
	mesh_drawable wing_right;
    mesh_drawable wing_extremity_left;
    mesh_drawable wing_extremity_right;
	mesh_drawable head;
	mesh_drawable right_eye;
	mesh_drawable left_eye;
    mesh_drawable nose;



    // Create the geometry of the meshes
	//   Note: this geometry must be set in their local coordinates with respect to their position in the hierarchy (and with respect to their animation)
    body.initialize_data_on_gpu(mesh_primitive_ellipsoid({0.8f, 0.4f, 0.6f}));
    wing_left.initialize_data_on_gpu(mesh_primitive_quadrangle({0,0,0}, {1,0,0}, {1,1,0}, {0,1,0}));
    wing_right.initialize_data_on_gpu(mesh_primitive_quadrangle({0,0,0}, {-1,0,0}, {-1,-1,0}, {0,-1,0}));
    head.initialize_data_on_gpu(mesh_primitive_sphere(0.35f));
    left_eye.initialize_data_on_gpu(mesh_primitive_sphere(0.1f));
    right_eye.initialize_data_on_gpu(mesh_primitive_sphere(0.1f));
    nose.initialize_data_on_gpu(mesh_primitive_cone(0.1f, 0.2f));
    wing_extremity_left.initialize_data_on_gpu(mesh_primitive_quadrangle({0,0,0}, {1,0,0}, {0.6f,0.6f,0}, {0.3f,0.6f,0}));
    wing_extremity_right.initialize_data_on_gpu(mesh_primitive_quadrangle({0,0,0}, {-1,0,0}, {-0.6f,-0.6f,0}, {-0.3f,-0.6f,0}));




	// Add the elements in the hierarchy
	//   The syntax is hierarchy.add(mesh_drawable, "name of the parent element", [optional: local translation in the hierarchy])
	//   Notes: 
	//     - An element must necessarily be added after its parent
	//     - The first element (without explicit name of its parent) is assumed to be the root.
    hierarchy.add(body, "Body", {});
    hierarchy.add(wing_left, "Wing_left", "Body",{-0.5f,0.3f,0});
    hierarchy.add(wing_right, "Wing_right", "Body",{0.5f,-0.3f,0});
    hierarchy.add(wing_extremity_left, "Wing_extremity_left", "Wing_left",{0,1,0});
    hierarchy.add(wing_extremity_right, "Wing_extremity_right", "Wing_right",{0,-1,0});
    hierarchy.add(head, "Head", "Body",{0.8f,0,.45f});
    hierarchy.add(left_eye, "Left_eye", "Head",{0.2f,0.2f,0.2f});
    hierarchy.add(right_eye, "Right_eye", "Head",{0.2f,-0.2f,0.2f});
    hierarchy.add(nose, "Nose", "Head",{0.35f,0,0},rotation_transform::from_axis_angle({0,1,0},3.14f/2));


}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	// Update the current time
	timer.update();

	
	// Apply transformation to some elements of the hierarchy
    hierarchy["Wing_left"].transform_local.rotation = rotation_transform::from_axis_angle({1,0,0}, M_PI/4 * std::sin(timer.t * 4));
    hierarchy["Wing_right"].transform_local.rotation = rotation_transform::from_axis_angle({-1,0,0},  M_PI/4 *std::sin(timer.t*4));
    hierarchy["Head"].transform_local.rotation = rotation_transform::from_axis_angle({0,1,0}, M_PI/8 * std::sin(timer.t*2));
    hierarchy["Wing_extremity_left"].transform_local.rotation = rotation_transform::from_axis_angle({1,0,0}, M_PI/2.7f * std::sin(timer.t * 4));
    hierarchy["Wing_extremity_right"].transform_local.rotation = rotation_transform::from_axis_angle({-1,0,0},  M_PI/2.7f *std::sin(timer.t*4));


    // This function must be called before the drawing in order to propagate the deformations through the hierarchy
	hierarchy.update_local_to_global_coordinates();

	// Draw the hierarchy as a single mesh
	draw(hierarchy, environment);
	if (gui.display_wireframe)
		draw_wireframe(hierarchy, environment);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
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

