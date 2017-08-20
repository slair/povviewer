
#include <iostream>
#include <unistd.h>
using namespace std;

//~ #include "version.h"
#include "global_defs.hpp"
#include "common_funcs.hpp"
#include "view_camera.hpp"
#include "pov_scene.hpp"

#ifdef __APPLE__
	#include <SDL/SDL.h>
#else
	#include <SDL.h>
#endif

unsigned int my_pid;

vec3d* v_pool;
unsigned int v_pool_size, v_pool_capacity;

string get_binary(string binary_name)
{
	string res="";
	res=binary_name;
	return res;
}

int main ( int argc, char** argv )
{
//	if (fork()==0)
//	{
//		cout << "i'm forked." << endl;
//		return 0;
//	}
	my_pid = getpid();
	DEBUG_MSG("pid=" << my_pid);

	string povdump_bin=get_binary("povdump");
	if (povdump_bin=="")
	{
		cout << "Can't find povdump executable. "
			"Place them in any PATH folder." \
			<< endl;

		exit(10);
	}
	else
	{
		cout << "Using povdump - '" << povdump_bin << "'." << endl;
	}

	string povray_bin=get_binary("povray");

	string parameter;
	if (argc>1)
		parameter = argv[1];
	else
	{
		print_usage_and_help();
		parameter = DEFAULT_SCENE_NAME;
	}
	string scene_workdir = get_folder_from_path(parameter);
	string scene_filename = get_file_from_path(parameter);
	string cwd = get_current_folder();
	//~ cout << "povviewer v " << AutoVersion::FULLVERSION_STRING << endl;
	cout << "Starting in '" << cwd << "'" << endl;

	cout << "Scene is '" << scene_filename << "' in '" << scene_workdir \
		<< "'" << endl;

	chdir(scene_workdir.c_str());
	cwd = get_current_folder();
	cout << "Current dir is '" << cwd << "'" << endl;

	view_camera* camera = new view_camera();
	pov_scene* scene = new pov_scene(scene_filename, camera);

	camera->init_opengl();
	scene->init_inotify();

	bool done = false;
	//~ volatile bool load_scene = true;
	bool load_scene = true;
	DBL mod = 1;
	SDLMod mod_state;
	SDL_Event event;
	bool forward_pressed	= false;
	bool backward_pressed	= false;
	bool left_pressed		= false;
	bool right_pressed		= false;
	bool up_pressed			= false;
	bool down_pressed		= false;
	bool q_pressed			= false;
	bool e_pressed			= false;
	bool cup_pressed		= false;
	bool cdown_pressed		= false;
	bool shift_pressed		= false;
	bool ctrl_pressed		= false;
	bool alt_pressed		= false;
	//~ bool input_active		= false;

	#define ACTIONS_COUNT 10
	KEY_ACTION actions[ACTIONS_COUNT] = {
		{SDLK_w, &forward_pressed},		// 1
		{SDLK_s, &backward_pressed},	// 2
		{SDLK_a, &left_pressed},		// 3
		{SDLK_d, &right_pressed},		// 4
		{SDLK_r, &up_pressed},			// 5
		{SDLK_f, &down_pressed},		// 6
		{SDLK_UP, &cup_pressed},		// 7
		{SDLK_DOWN, &cdown_pressed},	// 8
		{SDLK_q, &q_pressed},			// 9
		{SDLK_e, &e_pressed}			// 10
	};

	#define TOGGLES_COUNT 25
	KEY_TOGGLES toggles[TOGGLES_COUNT] = {
	/*  1 */	{SDLK_F1, &pov_scene::toggle_show_bbox, NULL},
	/*  2 */	{SDLK_F2, &pov_scene::toggle_show_dots, NULL},
	/*  3 */	{SDLK_F3, &pov_scene::toggle_show_lines, NULL},
	/*  4 */	{SDLK_F4, &pov_scene::toggle_show_solid, NULL},
	/*  5 */	{SDLK_F12, &pov_scene::toggle_show_normals, NULL},
	/*  6 */	{SDLK_p, &pov_scene::make_pov_screenshot, NULL},
	/*  7 */	{SDLK_BACKSPACE, NULL, &view_camera::toggle_antialias},
	/*  8 */	{SDLK_KP0, &pov_scene::setup_camera_from_povcamera, NULL},
	/*  9 */	{SDLK_KP7, NULL, &view_camera::view_from_up},
	/* 10 */	{SDLK_KP1, NULL, &view_camera::view_from_front},
	/* 11 */	{SDLK_KP3, NULL, &view_camera::view_from_right},
	/* 12 */	{SDLK_KP9, NULL, &view_camera::view_from_free},
	/* 13 */	{SDLK_KP5, NULL, &view_camera::toggle_perspective},
	/* 14 */	{SDLK_x, NULL, &view_camera::toggle_axis_x},
	/* 15 */	{SDLK_y, NULL, &view_camera::toggle_axis_y},
	/* 16 */	{SDLK_z, NULL, &view_camera::toggle_axis_z},
	/* 17 */	{SDLK_u, &pov_scene::toggle_use_povray_camera, NULL},
	/* 18 */	{SDLK_F6, &pov_scene::toggle_smooth_obj, NULL},
	/* 19 */	{SDLK_KP_MULTIPLY, NULL, &view_camera::toggle_up_fixed},
	/* 20 */	{SDLK_SPACE, NULL, &view_camera::toggle_show_navigator},
	/* 21 */	{SDLK_F10, NULL, &view_camera::toggle_stereo_cam},
	/* 22 */	{SDLK_c, NULL, &view_camera::reset_bg},
	/* 23 */	{SDLK_F7, &pov_scene::toggle_show_camera, NULL},
	/* 24 */	{SDLK_RETURN, &pov_scene::dump_scene_to_dxf, NULL},
	/* 25 */	{SDLK_g, &pov_scene::toggle_show_errors, NULL}
	};

	camera->add_msg_to_console("povviewer started");

	if (!v_pool_init())
	{
		cout << "Can't allocate global vertex pool!" << endl;
		exit(10);
	}

	int i;
	while ( !done )
	{
		scene->check_inotify_and_load(load_scene);

		while ( SDL_PollEvent(&event) )
		{
			mod = 1;
			mod_state = SDL_GetModState();
			shift_pressed = mod_state&KMOD_SHIFT;
			ctrl_pressed = mod_state&KMOD_CTRL;
			alt_pressed = mod_state&KMOD_ALT;

			if (ctrl_pressed)
				mod = 0.1;
			if (shift_pressed)
				mod *= 0.01;
			if (alt_pressed)
				mod *= 100;

/*
			if (input_active)
			{
				// process input line via scene
				if (event.type==SDL_KEYUP)
				{
				}
				break; // exit from event processing cycle
			}
*/

			switch (event.type)
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym==SDLK_ESCAPE)
					{
						done = true;
					}
					for(i=0; i<ACTIONS_COUNT; i++)
					{
						if (event.key.keysym.sym==actions[i].sdl_key)
						{
							*(actions[i].bool_var) = true;
						}
					}
				break; // SDL_KEYDOWN

				case SDL_KEYUP:
					for(i=0; i<ACTIONS_COUNT; i++)
					{
						if (event.key.keysym.sym==actions[i].sdl_key)
						{
							*(actions[i].bool_var) = false;
						}
					}
					for(i=0; i<TOGGLES_COUNT; i++)
					{
						if (event.key.keysym.sym==toggles[i].sdl_key)
						{
							if (toggles[i].pov_scene_toggle_func!=NULL)
								(scene->*toggles[i].pov_scene_toggle_func)();
							else if (toggles[i].view_camera_toggle_func!=NULL)
								(camera->*toggles[i].view_camera_toggle_func)();
							else
							{
								cout << "bad functions array" << endl;
								exit(1);
							}
						}
					}
					if (event.key.keysym.sym==SDLK_F5)
						load_scene = true;
					if (event.key.keysym.sym==SDLK_KP_PLUS)
					{
						scene->change_clock(mod);
						load_scene = true;
					}
					if (event.key.keysym.sym==SDLK_KP_MINUS)
					{
						scene->change_clock(-mod);
						load_scene = true;
					}
					if (event.key.keysym.sym==SDLK_PAGEUP)
						camera->change_navigator_size(mod);
					if (event.key.keysym.sym==SDLK_PAGEDOWN)
						camera->change_navigator_size(-mod);

					if (event.key.keysym.sym==SDLK_l)
					{
						camera->dump(cout);
					}

					/**
					@todo: background changes
					if (event.key.keysym.sym==SDLK_b)
						camera->change_bg(mod);
					if (event.key.keysym.sym==SDLK_v)
						camera->change_bg(-mod);
					*/

					if (camera->get_stereo_cam())
					{
						if (event.key.keysym.sym==SDLK_n)
							camera->set_stereo_base_size( \
								camera->get_stereo_base_size()-mod*0.1);

						if (event.key.keysym.sym==SDLK_m)
							camera->set_stereo_base_size( \
								camera->get_stereo_base_size()+mod*0.1);
					}

				break; // SDL_KEYUP

				case SDL_VIDEORESIZE:
					//~ cout << "SDL_VIDEORESIZE" << endl;
					if (camera->get_resize_skip()==0)
					{
						camera->set_window_width(event.resize.w);
						camera->set_window_height(event.resize.h);
						camera->resize_window();
					} else {
						camera->set_resize_skip(camera->get_resize_skip()-1);
					}
				break; // SDL_VIDEORESIZE

				case SDL_QUIT:
					done = true;
				break;

				case SDL_MOUSEMOTION:
					if (event.motion.state & SDL_BUTTON(1))	// left button
					{
						if (event.motion.yrel!=0)
							camera->rotate_around_right(event.motion.yrel*mod);

						if (event.motion.xrel!=0)
							camera->rotate_around_up(event.motion.xrel*mod);
					}

					if (event.motion.state &  SDL_BUTTON(2))	// middle button
					{
						if (event.motion.yrel!=0)
							camera->rotate_around_rt_nav(event.motion.yrel*mod);

						if (event.motion.xrel!=0)
							camera->rotate_around_up_nav(event.motion.xrel*mod);
					}
				break; // SDL_MOUSEMOTION

				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button==4) // mwheelup
						camera->zoom_change(-mod);

					if (event.button.button==5) // mwheeldown
						camera->zoom_change(mod);

					if (event.button.button==3) // RMB
						scene->aim_camera_to_nearest_object();
				break; // SDL_MOUSEBUTTONDOWN

			}
		} // while ( SDL_PollEvent(&event) )

		// movement
		if (forward_pressed)
			camera->move_forward(0, mod);
		if (backward_pressed)
			camera->move_backward(0, mod);
		if (!forward_pressed and !backward_pressed)
			camera->move_stop(0);

		if (left_pressed)
			camera->move_forward(1, mod);
		if (right_pressed)
			camera->move_backward(1, mod);
		if (!left_pressed and !right_pressed)
			camera->move_stop(1);

		if (up_pressed)
			camera->move_forward(2, mod);
		if (down_pressed)
			camera->move_backward(2, mod);
		if (!up_pressed and !down_pressed)
			camera->move_stop(2);

		if (cup_pressed)
			camera->move_nav_forward(mod);
		if (cdown_pressed)
			camera->move_nav_backward(mod);
		if (!cup_pressed and !cdown_pressed)
			camera->move_nav_stop();

		if (q_pressed)
			camera->rotate_around_direction(mod);
		if (e_pressed)
			camera->rotate_around_direction(-mod);

		camera->process_animation();

		// drawing code
		scene->refresh_screen();
//		camera->clear_gl_buffer();
//		if (camera->get_stereo_cam())
//		{
//			for (unsigned int k=0; k<2; k++)
//			{
//				camera->setup_viewport(k);
//				camera->begin_frame(k);
//				scene->draw();
//				camera->end_frame();
//			}
//		}
//		else
//		{
//			camera->setup_viewport(-1);
//			camera->begin_frame(-1);
//			scene->draw();
//			camera->end_frame();
//		}
//		SDL_GL_SwapBuffers();
//		SDL_Delay(DELAY_FPS);
	} // while ( !done )

	scene->shutdown_inotify();
	camera->shutdown_opengl();

	cout << "Cleaning up" << endl;
	delete scene;
	delete camera;
	v_pool_destroy();
	cout << "Exiting" << endl;
	return 0;
}
