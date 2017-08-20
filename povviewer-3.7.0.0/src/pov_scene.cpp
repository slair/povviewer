// -*- coding: utf-8 -*-

#include <fstream>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "common_funcs.hpp"
#include "pov_scene.hpp"
#include "view_camera.hpp"
#include "pov_camera.hpp"
#include "pov_lightsource.hpp"
#include "pov_triangle.hpp"
#include "pov_cone.hpp"
#include "pov_sphere.hpp"
#include "pov_ttf.hpp"
#include "pov_isosurface.hpp"
#include "pov_lathe.hpp"
#include "pov_prism.hpp"
#include "pov_box.hpp"
#include "pov_plane.hpp"
#include "pov_torus.hpp"
#include "pov_bicubicpatch.hpp"
#include "pov_blob.hpp"
#include "pov_mesh.hpp"
#include "pov_heightfield.hpp"
#include "pov_polygon.hpp"
#include "pov_disc.hpp"
#include "pov_quadric.hpp"
#include "pov_spheresweep.hpp"
#include "gl_object.hpp"
//#include "version.h"

extern vec3d* v_pool;
extern unsigned int v_pool_size, v_pool_capacity;

/** @brief reset_config
  *
  * reset settings to default values
  */
void pov_scene::reset_config()
{
	isoresolution[0] = isoresolution[1] = isoresolution[2] = 32;
	quadresolution[0] = quadresolution[1] = quadresolution[2] = 32;
	blobresolution[0] = blobresolution[1] = blobresolution[2] = 64;
	patchresolution = 4;
	spheresweepresolution = 8;
	cone_num_latitudes = 3; cone_num_longitudes = 16;
	sphere_num_latitudes = 8; sphere_num_longitudes = 16;
	lathe_num_longitudes = 16; lathe_spline_steps = 8;
	prism_spline_steps = 8;
	plane_radius = 100;	plane_step = 10; plane_alpha = 0.15;
	torus_usteps = 16; torus_vsteps = 8;
	disc_usteps = 16; disc_vsteps = 8;

	max_lights = 8;
	free_lights();
	allocate_lights();

	normals_size = 0.5;
	objects_quality = 9;
	scene_clock = 0;
	use_povray_camera = false;
	for (unsigned int i=0; i<NUMBER_OF_GLLISTS; i++)
	{
		show_elem[i] = false;
	}
	show_elem[SOLID]=true;
	show_camera = true;
	frustum_size = 50.0;
	smooth_obj = false;
	smooth_angle = 45;

	foreign_settings = false;
}

void pov_scene::allocate_lights()
{
	lights = (pov_lightsource**)malloc( max_lights*sizeof(pov_lightsource*) );
}

void pov_scene::free_lights()
{
	for(unsigned int i=0; i<lights_count; i++)
	{
		lights[i]->remove_light();
		delete ((pov_lightsource*) lights[i]);
	}
	lights_count = 0;
	free(lights);
	lights = NULL;
}

/** @brief pov_scene
  *
  * @todo: document this function
  */
pov_scene::pov_scene(string scene_fn, view_camera* cam)
{
	// init parameters
	show_errors=true;
	lights = NULL;
	lights_count = 0;
	reset_config();
	for (unsigned int i=0; i<NUMBER_OF_GLLISTS; i++)
	{
		gl_lists[i] = 0;
	}
	// load from config
	load_from_config();
	// create objects
	free(lights);
	lights = (pov_lightsource**)malloc( max_lights*sizeof(pov_lightsource*) );
	scene_filename = scene_fn;
	camera = cam;
	povcamera = NULL;
	camera->set_scene(this);
}

/** @brief ~pov_scene
  *
  * @todo: document this function
  */
 pov_scene::~pov_scene()
{
	clear();
	free(lights);
	save_to_config();
}

/** @brief shutdown_inotify
  *
  * @todo: document this function
  */
void pov_scene::shutdown_inotify()
{
	inotify_rm_watch(ino_fd, IN_CLOSE_WRITE);
}

/** @brief check_inotify_and_load
  *
  * @todo: document this function
  */
void pov_scene::check_inotify_and_load(volatile bool& ls)
{
	if (ls)
	{
		load();
		ls = false;
		if (parse_error)
		{
			camera->add_msg_to_console("scene may be loaded, "
				"there is errors or warnings");
		}
		else
		{
			camera->add_msg_to_console("scene loaded");
		}
		return;
	}
	unsigned int bytes_to_parse = 0;
	unsigned char* buf;
	struct inotify_event* ev;
	if (ioctl(ino_fd, FIONREAD, &bytes_to_parse)==-1)
	{
		cout << "ioctl() inotify broken" << endl;
		return;
	}

	if (bytes_to_parse>=sizeof(struct inotify_event))
	{
		buf = (unsigned char *)malloc(bytes_to_parse);
		if (read(ino_fd, buf, bytes_to_parse) == -1)
		{
			perror("read(): error");
			return;
		}
		unsigned int p;
		for (p = 0; p < bytes_to_parse; p += ev->len + 16)
		{
			ev = (struct inotify_event*) (buf + p);
			/*
			printf("wd = %d, mask = %08X, cookie = %d, len = %d, \
				name = %s\n",ev->wd, ev->mask, ev->cookie, ev->len, \
				&ev->name);
			*/
			load();
			string msg = scene_filename+" changed";
			camera->add_msg_to_console(msg.c_str());
			camera->add_msg_to_console("scene reloaded");
		}
		free(buf);
	}
}

/** @brief init_inotify
  *
  * @todo: document this function
  */
bool pov_scene::init_inotify()
{
	ino_fd = inotify_init();
	if (ino_fd < 0)
	{
		perror("inotify_init(): ");
		return false;
	}
	int wd = inotify_add_watch(ino_fd, scene_filename.c_str(), IN_CLOSE_WRITE);
	if (wd < 0)
	{
		perror("inotify_add_watch(): ");
		return false;
	}
	return true;
}

/** @brief clear_errors
  *
  * @todo: document this function
  */
void pov_scene::clear_errors()
{
	err_count = 0;
	parse_error = false;
}

void pov_scene::toggle_show_errors()
{
	show_errors=!show_errors;
}

/** @brief add_error
  *
  * @todo: document this function
  */
void pov_scene::add_error(const string msg)
{
	parse_error = true;
	if (err_count<MAX_ERR_STRINGS)
	{
		error_string[err_count++] = msg;
	}
}

/** @brief scan_scene_file
  *
  * @todo: document this function
  */
void pov_scene::scan_scene_file(const string scene_filename)
{
	#define MAX_LINE_LENGTH 4096
	bool changes_found = false;
	bool save_changes = false;
	bool window_changed = false;
	char buffer[MAX_LINE_LENGTH];
	string param_name, param_value;
	FILE* scene_file = fopen(scene_filename.c_str(), "r");
	if (!scene_file)
	{
		cout << "Can't open file '" << scene_filename << \
			"'. Scanning aborted." << endl;
		return;
	}
	unsigned int line_no=0;
	while(fgets(buffer, sizeof(buffer), scene_file))
	{
		line_no++;
		if (strlen(buffer)>13)
		{
			if (strncmp(buffer, "// povviewer:", 13)==0)
			{
				param_name = get_param_name(buffer);
				param_value = get_param_value(buffer);
				if (param_name=="end")
				{
					// stop scanning parameters
					cout << "Stop scanning parameters at line: " << line_no << \
					endl;
					break;
				}
				if (param_name=="reset")
				{
					cout << "  reset parameters to default values." << endl;
					reset_config();
					camera->reset_config();
					window_changed = true;
					changes_found = true;
					continue;
				}
				if (param_name=="save")
				{
					save_changes = true;
					changes_found = true;
					continue;
				}
				if (param_name=="nosave")
				{
					save_changes = false;
					changes_found = false;
					continue;
				}
				if (param_name=="font")
				{
					camera->set_font(param_value);
					cout << "    'font' changed to " << param_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="cam_zfar")
				{
					SNGL new_value = atof(param_value.c_str());
					camera->set_cam_zfar(new_value);
					cout << "    'cam_zfar' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="frustum_size")
				{
					DBL new_value = atof(param_value.c_str());
					set_frustum_size(new_value);

					cout << "    'frustum_size' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="show_camera")
				{
					bool new_value = atoi(param_value.c_str());
					set_show_camera(new_value);

					cout << "    'show_camera' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="cam_znear")
				{
					SNGL new_value = atof(param_value.c_str());
					camera->set_cam_znear(new_value);
					cout << "    'cam_znear' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="bgcolor")
				{
					vec3d new_value = vec3d(param_value);
					camera->set_bgcolor(new_value);
					cout << "    'bgcolor' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="location")
				{
					vec3d new_value = vec3d(param_value);
					camera->set_location(new_value);
					cout << "    'location' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="direction")
				{
					vec3d new_value = vec3d(param_value);
					camera->set_direction(new_value);
					cout << "    'direction' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="up")
				{
					vec3d new_value = vec3d(param_value);
					camera->set_up(new_value);
					cout << "    'up' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="fixed_up")
				{
					vec3d new_value = vec3d(param_value);
					camera->set_fixed_up(new_value);
					cout << "    'fixed_up' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="right")
				{
					vec3d new_value = vec3d(param_value);
					camera->set_right(new_value);
					cout << "    'right' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="angle")
				{
					SNGL new_value = atof(param_value.c_str());
					camera->set_angle(new_value);
					cout << "    'angle' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="ortho_size")
				{
					SNGL new_value = atof(param_value.c_str());
					camera->set_ortho_size(new_value);
					cout << "    'ortho_size' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="distance_to_nav")
				{
					DBL new_value = atof(param_value.c_str());
					camera->set_distance_to_nav(new_value);

					cout << "    'distance_to_nav' changed to " << new_value \
						<< endl;

					changes_found = true;
					continue;
				}
				if (param_name=="up_fixed")
				{
					bool new_value = atoi(param_value.c_str());
					camera->set_up_fixed(new_value);
					cout << "    'up_fixed' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="perspective")
				{
					bool new_value = atoi(param_value.c_str());
					camera->set_perspective(new_value);

					cout << "    'perspective' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="antialias")
				{
					bool new_value = atoi(param_value.c_str());
					camera->set_antialias(new_value);
					cout << "    'antialias' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="show_navigator")
				{
					bool new_value = atoi(param_value.c_str());
					camera->set_show_navigator(new_value);

					cout << "    'show_navigator' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="stereo_cam")
				{
					bool new_value = atoi(param_value.c_str());
					camera->set_stereo_cam(new_value);
					cout << "    'stereo_cam' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="use_povray_camera")
				{
					bool new_value = atoi(param_value.c_str());
					use_povray_camera = new_value;

					cout << "    'use_povray_camera' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="smooth_obj")
				{
					bool new_value = atoi(param_value.c_str());
					smooth_obj = new_value;
					cout << "    'smooth_obj' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="show_bbox")
				{
					bool new_value = atoi(param_value.c_str());
					show_elem[BBOX] = new_value;
					cout << "    'show_bbox' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="show_dots")
				{
					bool new_value = atoi(param_value.c_str());
					show_elem[DOTS] = new_value;
					cout << "    'show_dots' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="show_lines")
				{
					bool new_value = atoi(param_value.c_str());
					show_elem[LINES] = new_value;
					cout << "    'show_lines' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="show_solid")
				{
					bool new_value = atoi(param_value.c_str());
					show_elem[SOLID] = new_value;
					cout << "    'show_solid' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="show_normals")
				{
					bool new_value = atoi(param_value.c_str());
					show_elem[NORMALS] = new_value;

					cout << "    'show_normals' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="show_planes")
				{
					bool new_value = atoi(param_value.c_str());
					show_elem[PLANES] = new_value;

					cout << "    'show_planes' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="show_axis")
				{
					size_t first_comma, last_comma;
					bool ax, ay, az;
					first_comma = param_value.find_first_of(',');
					last_comma = param_value.find_last_of(',');
					if((first_comma!=string::npos)&&(last_comma!=string::npos))
					{
						ax = atoi(param_value.substr(0,first_comma).c_str());

						ay = atoi(param_value.substr(first_comma+1, \
							last_comma - first_comma-1).c_str());

						az = atoi(param_value.substr(last_comma+1, \
							param_value.size() - last_comma-1).c_str());

						camera->set_show_axis(ax, ay, az);

						cout << "    'show_axis' changed to " << ax << "," << \
							ay << "," << az << endl;

						changes_found = true;
					}
					else
					{
						DEBUG_MSG("Bad string '" << param_value << \
							"' for bool[3] value")
					}
					continue;
				}
				if (param_name=="max_speed")
				{
					DBL new_value = atof(param_value.c_str());
					camera->set_max_speed(new_value);
					cout << "    'max_speed' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="navigator_size")
				{
					DBL new_value = atof(param_value.c_str());
					camera->set_navigator_size(new_value);

					cout << "    'navigator_size' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="axis_size")
				{
					DBL new_value = atof(param_value.c_str());
					camera->set_axis_size(new_value);
					cout << "    'axis_size' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="stereo_base_size")
				{
					DBL new_value = atof(param_value.c_str());
					camera->set_stereo_base_size(new_value);

					cout << "    'stereo_base_size' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="delta_speed")
				{
					DBL new_value = atof(param_value.c_str());
					camera->set_delta_speed(new_value);

					cout << "    'delta_speed' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="delta_rotation")
				{
					DBL new_value = atof(param_value.c_str());
					camera->set_delta_rotation(new_value);

					cout << "    'delta_rotation' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="plane_alpha")
				{
					DBL new_value = atof(param_value.c_str());
					plane_alpha = new_value;

					cout << "    'plane_alpha' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="plane_radius")
				{
					DBL new_value = atof(param_value.c_str());
					plane_radius = new_value;

					cout << "    'plane_radius' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="plane_step")
				{
					DBL new_value = atof(param_value.c_str());
					plane_step = new_value;
					cout << "    'plane_step' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="normals_size")
				{
					DBL new_value = atof(param_value.c_str());
					normals_size = new_value;

					cout << "    'normals_size' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="scene_clock")
				{
					DBL new_value = atof(param_value.c_str());
					scene_clock = new_value;

					cout << "    'scene_clock' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="smooth_angle")
				{
					DBL new_value = atof(param_value.c_str());
					smooth_angle = new_value;

					cout << "    'smooth_angle' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="patchresolution")
				{
					unsigned int new_value = atoi(param_value.c_str());
					patchresolution = new_value;

					cout << "    'patchresolution' changed to " \
						<< new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="spheresweepresolution")
				{
					unsigned int new_value = atoi(param_value.c_str());
					spheresweepresolution = new_value;

					cout << "    spheresweepresolution changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="cone_num_latitudes")
				{
					unsigned int new_value = atoi(param_value.c_str());
					cone_num_latitudes = new_value;

					cout << "    'cone_num_latitudes' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="cone_num_longitudes")
				{
					unsigned int new_value = atoi(param_value.c_str());
					cone_num_longitudes = new_value;

					cout << "    'cone_num_longitudes' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="sphere_num_longitudes")
				{
					unsigned int new_value = atoi(param_value.c_str());
					sphere_num_longitudes = new_value;

					cout << "    'sphere_num_longitudes' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="sphere_num_latitudes")
				{
					unsigned int new_value = atoi(param_value.c_str());
					sphere_num_latitudes = new_value;

					cout << "    'sphere_num_latitudes' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="lathe_num_longitudes")
				{
					unsigned int new_value = atoi(param_value.c_str());
					lathe_num_longitudes = new_value;

					cout << "    'lathe_num_longitudes' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="lathe_spline_steps")
				{
					unsigned int new_value = atoi(param_value.c_str());
					lathe_spline_steps = new_value;

					cout << "    'lathe_spline_steps' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="prism_spline_steps")
				{
					unsigned int new_value = atoi(param_value.c_str());
					prism_spline_steps = new_value;

					cout << "    'prism_spline_steps' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="torus_usteps")
				{
					unsigned int new_value = atoi(param_value.c_str());
					torus_usteps = new_value;

					cout << "    'torus_usteps' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="torus_vsteps")
				{
					unsigned int new_value = atoi(param_value.c_str());
					torus_vsteps = new_value;

					cout << "    'torus_vsteps' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="disc_usteps")
				{
					unsigned int new_value = atoi(param_value.c_str());
					disc_usteps = new_value;

					cout << "    'disc_usteps' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="disc_vsteps")
				{
					unsigned int new_value = atoi(param_value.c_str());
					disc_vsteps = new_value;

					cout << "    'disc_vsteps' changed to " << new_value << \
						endl;

					changes_found = true;
					continue;
				}
				if (param_name=="objects_quality")
				{
					unsigned int new_value = atoi(param_value.c_str());
					objects_quality = new_value;

					cout << "    'objects_quality' changed to " << \
						new_value << endl;

					changes_found = true;
					continue;
				}
				if (param_name=="max_lights")
				{
					unsigned int new_value = atoi(param_value.c_str());
					max_lights = new_value;
					for(unsigned int i=0; i<lights_count; i++)
					{
						lights[i]->remove_light();
						delete ((pov_lightsource*) lights[i]);
					}
					lights_count = 0;
					free(lights);
					lights = NULL;

					lights = (pov_lightsource**)malloc( \
						max_lights*sizeof(pov_lightsource*) );

					cout << "    'max_lights' changed to " << new_value << endl;
					changes_found = true;
					continue;
				}
				if (param_name=="isoresolution")
				{
					size_t first_comma, last_comma;
					unsigned int ax, ay, az;
					first_comma = param_value.find_first_of(',');
					last_comma = param_value.find_last_of(',');
					if((first_comma!=string::npos)&&(last_comma!=string::npos))
					{
						ax = atoi(param_value.substr(0,first_comma).c_str());

						ay = atoi(param_value.substr(first_comma+1, \
							last_comma - first_comma-1).c_str());

						az = atoi(param_value.substr(last_comma+1, \
							param_value.size() - last_comma-1).c_str());

						isoresolution[0] = ax;
						isoresolution[1] = ay;
						isoresolution[2] = az;

						cout << "    'isoresolution' changed to " << ax << \
							"," << ay << "," << az << endl;

						changes_found = true;
					}
					else
					{
						DEBUG_MSG("Bad string '" << param_value << \
							"' for int[3] value")
					}
					continue;
				}
				if (param_name=="quadresolution")
				{
					size_t first_comma, last_comma;
					unsigned int ax, ay, az;
					first_comma = param_value.find_first_of(',');
					last_comma = param_value.find_last_of(',');
					if((first_comma!=string::npos)&&(last_comma!=string::npos))
					{
						ax = atoi(param_value.substr(0,first_comma).c_str());

						ay = atoi(param_value.substr(first_comma+1, \
							last_comma - first_comma-1).c_str());

						az = atoi(param_value.substr(last_comma+1, \
							param_value.size() - last_comma-1).c_str());

						quadresolution[0] = ax;
						quadresolution[1] = ay;
						quadresolution[2] = az;

						cout << "    'quadresolution' changed to " << ax << \
							"," << ay << "," << az << endl;

						changes_found = true;
					}
					else
					{
						DEBUG_MSG("Bad string '" << param_value << \
							"' for int[3] value")
					}
					continue;
				}
				if (param_name=="blobresolution")
				{
					size_t first_comma, last_comma;
					unsigned int ax, ay, az;
					first_comma = param_value.find_first_of(',');
					last_comma = param_value.find_last_of(',');
					if((first_comma!=string::npos)&&(last_comma!=string::npos))
					{
						ax = atoi(param_value.substr(0,first_comma).c_str());

						ay = atoi(param_value.substr(first_comma+1, \
							last_comma - first_comma-1).c_str());

						az = atoi(param_value.substr(last_comma+1, \
							param_value.size() - last_comma-1).c_str());

						blobresolution[0] = ax;
						blobresolution[1] = ay;
						blobresolution[2] = az;

						cout << "    'blobresolution' changed to " << ax << \
							"," << ay << "," << az << endl;

						changes_found = true;
					}
					else
					{
						DEBUG_MSG("Bad string '" << param_value << \
							"' for int[3] value")
					}
					continue;
				}
				if (param_name=="window_width")
				{
					unsigned int new_width = atoi(param_value.c_str());
					if (new_width!=0)
					{
						camera->set_window_width(new_width);

						cout << "    'window_width' changed to " << \
							new_width << endl;

						changes_found = true;
						window_changed = true;
					}
					else
					{
						add_error("Error! At line " + int2str(line_no) + \
							" 'window_width' parameter is not valid.");
					}
					continue;
				}
				if (param_name=="window_height")
				{
					unsigned int new_height = atoi(param_value.c_str());
					if (new_height!=0)
					{
						camera->set_window_height(new_height);

						cout << "    'window_height' changed to " << \
							new_height << endl;

						changes_found = true;
						window_changed = true;
					}
					else
					{
						add_error("Error! At line " + int2str(line_no) + \
							" 'window_height' parameter is not valid.");
					}
					continue;
				}

				DEBUG_MSG("  unknown parameter at line "<< line_no << \
					" found: '"<<param_name<<"' = '"<<param_value<<"'")

				add_error("Warning! Unknown parameter at line " + \
					int2str(line_no) + " found: '" + param_name + "' = '" + \
					param_value+"' ignored.");
			}
		}
	}
	fclose(scene_file);
	if (changes_found)
	{
		if (window_changed)
		{
			camera->calc_cam_ratio();
			camera->set_resize_skip(1);
			camera->resize_window();
		}
		if (save_changes)
		{
			cout << "New settings will be saved." << endl << endl;
			foreign_settings = false;
			camera->foreign_settings = false;
		}
		else
		{
			cout << "New settings will not be saved." << endl << endl;
			foreign_settings = true;
			camera->foreign_settings = true;
		}
	}
	else
	{
		cout << "New settings will be saved." << endl << endl;
		foreign_settings = false;
		camera->foreign_settings = false;
	}
}

/** @brief load
  *
  * @todo: document this function
  */
void pov_scene::load()
{
	clear_errors();
	cout << endl << "Scanning scene '" << scene_filename << \
		"' for parameters change" << endl;

	scan_scene_file(scene_filename);

	cout << "Loading scene '" << scene_filename << "'" << endl;
	unlink(TMPCFG_NAME);
	dump_cfg(TMPCFG_NAME);
	char buffer[1024];
	int len_buffer = 0;

	len_buffer = sprintf(buffer, "povdump -D +w%i +h%i -F "
		"Declare=POVVIEWER=%d +i%s +nr +df/dev/shm/scene.dump +K%f 2>&1",
		camera->get_window_width(), camera->get_window_height(), \
		objects_quality,
		scene_filename.c_str(), scene_clock);

	if (len_buffer < 0)
	{
		cout << "Error creating dump command." << endl;
		return;
	}

	//DEBUG_MSG("VMSIZE = "<<get_vmsize_kb()<<" kB")

	unsigned int start_parse = SDL_GetTicks();
	cout << "Starting '" << buffer << "'." << endl;
	FILE *povdump_out = popen(buffer, "r");
	if (!povdump_out)
	{
		cout << "Error popen command '" << buffer << "'\npovdump_out = " << \
			povdump_out << \
			"\nerrno = " << errno << " (" << strerror(errno) << ")" << endl;
		add_error("Error popen command:");
		add_error(buffer);
		add_error(strerror(errno));
		return;
	}
	string bad_line="";
	bool was_error = false;

	while (fgets(buffer, sizeof(buffer), povdump_out))
	{
		if ((was_error)&&(buffer[0]==' ')&&(buffer[1]!=' '))
		{
			add_error(buffer);
			continue;
		}
		else
		{
			was_error = false;
			//continue;
		}
		//~ if (strncmp(buffer, "Parse Warning:", 14)==0)
		if (strstr(buffer, "Parse Warning:")!=0)
		{
			add_error(bad_line);
			add_error(buffer);
			was_error = true;
			continue;
		}
		//~ if (strncmp(buffer, "Possible Parse Error:", 21)==0)
		if (strstr(buffer, "Possible Parse Error:")!=0)
		{
			add_error(bad_line);
			add_error(buffer);
			was_error = true;
			continue;
		}
		//~ if (strncmp(buffer, "Parse Error:", 12)==0)
		if (strstr(buffer, "Parse Error:")!=0)
		{
			//add_error("---------------------------------------------------");
			add_error(bad_line);
			add_error(buffer);
			was_error = true;
			//fgets(buffer, sizeof(buffer), povdump_out);
			//add_error(buffer);
			//add_error("---------------------------------------------------");
			//return;
			continue;
		}
		if (strncmp(buffer, "File '", 6)==0)
		{
			bad_line = buffer;
			continue;
		}
	}
	int pres = pclose(povdump_out);
	if (pres!=0)
	{
		cout << "Something wrong! (pclose() = " << pres << ")" << endl;
	}
	parse_time = DBL(SDL_GetTicks()-start_parse) / 1000.0;
	load_time = -1;
	set_caption();

	clear();
	// load

	unsigned int start_load = SDL_GetTicks();
	FILE* dump_file = fopen(DUMP_FILENAME, "r");

	if (dump_file==0)
	{
		add_error("dump not created! check povdump output");
		return;
	}

	unsigned int last_readden_tag = 0;
	char last_readden_chartag[5];
	last_readden_chartag[4] = 0;

	fseek(dump_file, 0, SEEK_END);
	long unsigned int fsize = ftell(dump_file);
	fseek(dump_file, 0, SEEK_SET);

	long unsigned int readden = 0;

	unsigned int tag = 0;
	char chartag[5];
	chartag[4] = 0;
	objects_count = 0;
	pov_baseobject* obj;

#ifdef DEBUG
	unsigned int dreadden, dfpos;
	unsigned int readden1;
	unsigned int fpos1;
#endif

	char size_short_int;
	char size_int;
	char size_float;
	char size_double;
	fread(&size_short_int, 1, 1, dump_file);
	fread(&size_int, 1, 1, dump_file);
	fread(&size_float, 1, 1, dump_file);
	fread(&size_double, 1, 1, dump_file);
	readden = 4;

	while (readden < fsize)
	{
//		DEBUG_MSG("\nreadden("<<readden<<") < fsize("<<fsize<<")")
		fread(&tag, sizeof(tag), 1, dump_file);
		memcpy(chartag, &tag, 4);
		readden += 4;
//		DEBUG_MSG("read tag '" << chartag << "'")
		obj = NULL;
#ifdef DEBUG
		readden1 = readden;
		fpos1 = ftell(dump_file);
#endif
		switch (tag)
		{
			case 0x454e4353:	// SCNE
			break;

			case 0x454d5246:	// FRME
				DBL frame_clock;
				fread(&frame_clock, sizeof(frame_clock), 1, dump_file);
				readden += sizeof(frame_clock);
			break;

			case 0x524d4143:	// CAMR
				if (povcamera==NULL)
					povcamera = new pov_camera(this);
				readden += povcamera->read(dump_file);
				if (use_povray_camera)
				{
					setup_camera_from_povcamera();
				}
				last_readden_tag = tag;
				strcpy(last_readden_chartag, chartag);
				continue;
			break;

			case 0x5448474c: // LGHT
				if (lights_count < max_lights)
				{
					lights[lights_count] = new pov_lightsource(this, \
						lights_count);

					readden += ((pov_lightsource*) \
						lights[lights_count])->read(dump_file);

					lights_count++;
				}
				else
				{
					pov_lightsource* skipped_light = new pov_lightsource(this, \
						0);

					readden += skipped_light->read(dump_file);
					delete skipped_light;
					cout << "GL_MAX_LIGHTS ("<< max_lights << \
						") is reached. Light ignored." << endl;
				}
			break;

			case 0x45534142:	// BASE
				obj = new pov_baseobject(this);
				readden += ((pov_baseobject*) obj)->read(dump_file);
			break;

			case 0x41495254: // TRIA
				obj = new pov_triangle(this);
				readden += ((pov_triangle*) obj)->read(dump_file);
			break;

			case 0x454e4f43: // CONE
				obj = new pov_cone(this);
				readden += ((pov_cone*) obj)->read(dump_file);
			break;

			case 0x52485053: // SPHR
				obj = new pov_sphere(this);
				readden += ((pov_sphere*) obj)->read(dump_file);
			break;

			case 0x5f465454: // TTF_
				obj = new pov_ttf(this);
				readden += ((pov_ttf*) obj)->read(dump_file);
			break;

			case 0x534f5349: // ISOS
				obj = new pov_isosurface(this);
				readden += ((pov_isosurface*) obj)->read(dump_file);
			break;

			case 0x4854414c: // LATH
				obj = new pov_lathe(this);
				readden += ((pov_lathe*) obj)->read(dump_file);
			break;

			case 0x4d535250: // PRSM
				obj = new pov_prism(this);
				readden += ((pov_prism*) obj)->read(dump_file);
			break;

			case 0x20584f42: // BOX
				obj = new pov_box(this);
				readden += ((pov_box*) obj)->read(dump_file);
			break;

			case 0x4e414c50: // PLAN
				obj = new pov_plane(this);
				readden += ((pov_plane*) obj)->read(dump_file);
			break;

			case 0x53524f54: // TORS
				obj = new pov_torus(this);
				readden += ((pov_torus*) obj)->read(dump_file);
			break;

			case 0x41504942: // BIPA
				obj = new pov_bicubicpatch(this);
				readden += ((pov_bicubicpatch*) obj)->read(dump_file);
			break;

			case 0x424f4c42: // BLOB
				obj = new pov_blob(this);
				readden += ((pov_blob*) obj)->read(dump_file);
			break;

			case 0x4853454d: // MESH
				obj = new pov_mesh(this);
				readden += ((pov_mesh*) obj)->read(dump_file);
			break;

			case 0x444c4648: // HFLD
				obj = new pov_heightfield(this);
				readden += ((pov_heightfield*) obj)->read(dump_file);
			break;

			case 0x594c4f50: // POLY
				obj = new pov_polygon(this);
				readden += ((pov_polygon*) obj)->read(dump_file);
			break;

			case 0x43534944: // DISC
				obj = new pov_disc(this);
				readden += ((pov_disc*) obj)->read(dump_file);
			break;

			case 0x44415551: // QUAD
				obj = new pov_quadric(this);
				readden += ((pov_quadric*) obj)->read(dump_file);
			break;

			case 0x57535053: // SPSW
				obj = new pov_spheresweep(this);
				readden += ((pov_spheresweep*) obj)->read(dump_file);
			break;

			default:
				cout << "\nError loading scene!\nLast readden tag '0x" << hex \
					<< last_readden_tag << "' - '" << last_readden_chartag \
					<< "'\n";

				cout << "Unknown tag '0x" << hex << tag << "' - '" << chartag \
					<< "' at position 0x"	<< hex << (readden-4) \
					<< "\nParsing dump stopped!\n\n";

				fclose(dump_file);

				cout << "Scene with " << dec << objects_count \
					<< " objects loaded." << endl;

				update();

				cout << "Scene with " << dec << objects_count \
					<< " objects updated.\n" << endl;

				load_time = DBL(SDL_GetTicks() - start_load) / 1000.0;
				set_caption();
				return;
			break;
		}
#ifdef DEBUG
		dreadden = readden - readden1;
		dfpos = ftell(dump_file) - fpos1;
		//DEBUG_MSG("dreadden("<<dreadden<<")==dfpos("<<dfpos<<")")
		DEBUG_ASSERT(dreadden!=dfpos,"dreadden="<<dreadden<<" dfpos="<<dfpos)
#endif
		last_readden_tag = tag;
		strcpy(last_readden_chartag, chartag);
		if (obj!=NULL)
		{
			objects.push_back(obj);
			objects_count++;
		}
	}

	// finishing load
	fclose(dump_file);

	cout << "Scene with " << dec << objects_count << " objects loaded." << \
		endl;

	update();

	cout << "Scene with " << dec << objects_count << " objects updated.\n" << \
		endl;

	load_time = DBL(SDL_GetTicks() - start_load) / 1000.0;

	// print stats
	print_stats(cout);

	//DEBUG_MSG("VMSIZE = "<<get_vmsize_kb()<<" kB")

	// setting caption
	set_caption();
//	caption="povviewer " + string(AutoVersion::FULLVERSION_STRING) + " - [";
//	caption += scene_filename+"]   ";
//	caption += "pt: "+dbl2str(parse_time)+" sec";
//	caption += "   lt: "+dbl2str(load_time)+" sec";
//	SDL_WM_SetCaption(caption.c_str(), "POV-Viewer");

}

/** @brief set_caption
  *
  * @todo: document this function
  */
void pov_scene::set_caption()
{
	char buff[256];
	//~ sprintf(buff," %ld.%02ld build %ld", AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD);
	sprintf(buff," ");
	int w = camera->get_window_width();
	int h = camera->get_window_height();
	string caption="povviewer " + int2str(w);
	caption += "x" + int2str(h) + " - [";
	caption += scene_filename+"]   ";
	if (parse_time>=0)
		caption += "pt: "+dbl2str(parse_time)+" sec";
	if (load_time>=0)
		caption += "   lt: "+dbl2str(load_time)+" sec";
	SDL_WM_SetCaption(caption.c_str(), "POV-Viewer");
}

/** @brief draw
  *
  * @todo: document this function
  */
void pov_scene::draw()
{
//	vec3d(0).draw_navigator(1);
	if (lights_count>0)
	{
		glEnable(GL_LIGHTING);
		for(unsigned int i=0; i<lights_count; i++)
		{
			((pov_lightsource*) lights[i])->place_light();
		}
		glEnable(GL_NORMALIZE);
		glEnable(GL_AUTO_NORMAL);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}
	if (show_elem[BBOX])
		glCallList(gl_lists[BBOX]);
	if (show_elem[SOLID])
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 0.5);
		glCallList(gl_lists[SOLID]);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	if (show_elem[DOTS])
		glCallList(gl_lists[DOTS]);
	if (show_elem[LINES])
		glCallList(gl_lists[LINES]);
	if (show_elem[NORMALS])
		glCallList(gl_lists[NORMALS]);
	if (show_elem[PLANES])
		glCallList(gl_lists[PLANES]);

//	for (unsigned int i=0; i<NUMBER_OF_GLLISTS; i++)
//	{
//		if (show_elem[i])
//			glCallList(gl_lists[i]);
//	}
	if (povcamera!=NULL)
	{
		povcamera->draw();
	}
}

/** @brief dump_cfg
  *
  * @todo: document this function
  */
void pov_scene::dump_cfg(const char *tmpcfg)
{
	FILE *cfg_file = fopen(tmpcfg, "w");
	fwrite(&isoresolution, sizeof(isoresolution), 1, cfg_file);
	fwrite(&quadresolution, sizeof(quadresolution), 1, cfg_file);
	fwrite(&blobresolution, sizeof(blobresolution), 1, cfg_file);
	fclose(cfg_file);
}

/** @brief clear
  *
  * @todo: document this function
  */
void pov_scene::clear()
{
	//DEBUG_MSG("pov_scene::clear(")
	//DEBUG_MSG("VMSIZE = "<<get_vmsize_kb()<<" kB")
	delete povcamera;
	povcamera = NULL;

	for(unsigned int i=0; i<lights_count; i++)
	{
		delete ((pov_lightsource*) lights[i]);
	}
	lights_count = 0;
	for(unsigned int i=0; i<objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
	v_pool_clear();
	//DEBUG_MSG("VMSIZE = "<<get_vmsize_kb()<<" kB")
	//DEBUG_MSG("pov_scene::clear)")
}

/** @brief update
  *
  * @todo: document this function
  */
void pov_scene::update()
{
	//DEBUG_MSG("\n\npov_scene::update()")
	for(unsigned int i=0; i<NUMBER_OF_GLLISTS; i++)
	{
		if (gl_lists[i]!=0)
		{
			glDeleteLists(gl_lists[i], 1);
			gl_lists[i] = 0;
		}
		gl_lists[i] = glGenLists(1);
		glNewList(gl_lists[i], GL_COMPILE);
		update_gllist(i);
		glEndList();
	}
}

/** @brief update_gllists
  *
  * @todo: document this function
  */
void pov_scene::update_gllist(unsigned int mode)
{
	char chartag[5];
	chartag[4] = 0;

	//DEBUG_MSG("\npov_scene::update_gllist("<<mode<<")")

	// all objects except planes
	for(vector<pov_baseobject*>::const_iterator it=objects.begin(); it!=objects.end(); ++it)
	{
		pov_baseobject *obj = *it;
		if (obj->tag!=0x4e414c50)
		{
			memcpy(chartag, &(obj->tag), 4);
			//DEBUG_MSG("\nchartag="<<chartag)
			((pov_baseobject*) *it)->generate_gllist(mode);
		}
	}

	// planes are transparent
	for(vector<pov_baseobject*>::const_iterator it=objects.begin(); it!=objects.end(); ++it)
	{
		pov_baseobject *obj = *it;
		if (obj->tag==0x4e414c50)
		{
			memcpy(chartag, &(obj->tag), 4);
			//DEBUG_MSG("\nchartag="<<chartag)
			((pov_baseobject*) *it)->generate_gllist(mode);
		}
	}
}

/** @brief setup_camera_from_povcamera
  *
  * @todo: document this function
  */
void pov_scene::setup_camera_from_povcamera()
{
	if (povcamera!=NULL)
	{
		camera->setup(povcamera->get_location(), povcamera->get_sky(), povcamera->get_look_at(),
			povcamera->get_angle(), povcamera->get_direction().length(),
			povcamera->get_up().length(), povcamera->get_right().length());
		if (povcamera->get_type()==PERSPECTIVE_CAMERA)
			camera->set_perspective(true);
		if (povcamera->get_type()==ORTHOGRAPHIC_CAMERA)
		{
			camera->set_perspective(false);
			camera->set_ortho_size(povcamera->get_up().length()/2);
		}
	}
	else
	{
		camera->setup(vec3d(0, 0, -5), vec3d(0, 1, 0), vec3d(0, 0, 0), 63.5, 0, 0, 0);
		camera->set_perspective(true);
	}
	camera->toggle_up_fixed();
	camera->toggle_up_fixed();
}

/** @brief aim_camera_to_nearest_object
  *
  * @todo: document this function
  */
void pov_scene::aim_camera_to_nearest_object()
{
	vec3d cam_look_at = camera->get_look_at();
	vec3d cam_location = camera->get_location();
	vec3d ray = (cam_look_at - cam_location).normalize();
	DBL min_distance = 1e9, distance;
	pov_baseobject* obj;
	vec3d obj_center, found_center, ray_dot;
	string object_type;
	for (unsigned int i=0; i<objects.size(); i++)
	{
		obj = objects.at(i);
		obj_center = obj->get_center();
		ray_dot = cam_location + ray*(ray.vdot(obj_center-cam_location));
		distance = (ray_dot - obj_center).length();
		//DEBUG_MSG(obj->get_type()<<" ("<<obj_center<<") "<<distance)
		if (distance<min_distance)
		{
			min_distance = distance;
			found_center = obj_center;
			object_type = obj->get_type();
		}
	}
	// lightings
	if (lights_present())
	{
		for (unsigned int i=0; i<lights_count; i++)
		{
			obj_center = lights[i]->get_position();
			ray_dot = cam_location + ray*(ray.vdot(obj_center-cam_location));
			distance = (ray_dot - obj_center).length();
			//DEBUG_MSG(obj->get_type()<<" ("<<obj_center<<") "<<distance)
			if (distance<min_distance)
			{
				min_distance = distance;
				found_center = obj_center;
				object_type = "light_source";
			}
		}
	}
	//DEBUG_MSG("Found center at: " << found_center << endl);
	camera->look_at(found_center);
	string s = "look at: "+object_type;
	camera->add_msg_to_console(s.c_str());
}

/** @brief lights_present
  *
  * @todo: document this function
  */
bool pov_scene::lights_present()
{
	if (lights_count>0)
		return true;
	return false;
}

/** @brief get_cone_num_latitudes
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_cone_num_latitudes() const
{
	return cone_num_latitudes;
}

/** @brief get_cone_num_longitudes
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_cone_num_longitudes() const
{
	return cone_num_longitudes;
}

/** @brief get_normals_size
  *
  * @todo: document this function
  */
DBL pov_scene::get_normals_size() const
{
	return normals_size;
}

/** @brief get_sphere_num_latitudes
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_sphere_num_latitudes() const
{
	return sphere_num_latitudes;
}

/** @brief get_sphere_num_longitudes
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_sphere_num_longitudes() const
{
	return sphere_num_longitudes;
}

/** @brief get_lathe_num_longitudes
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_lathe_num_longitudes() const
{
	return lathe_num_longitudes;
}

/** @brief get_lathe_spline_steps
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_lathe_spline_steps() const
{
	return lathe_spline_steps;
}

/** @brief get_prism_spline_steps
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_prism_spline_steps() const
{
	return prism_spline_steps;
}

/** @brief toggle_show_normals
  *
  * @todo: document this function
  */
void pov_scene::toggle_show_normals()
{
	set_show_normals(!get_show_normals());
}

/** @brief toggle_show_solid
  *
  * @todo: document this function
  */
void pov_scene::toggle_show_solid()
{
	set_show_solid(!get_show_solid());
}

/** @brief toggle_show_lines
  *
  * @todo: document this function
  */
void pov_scene::toggle_show_lines()
{
	set_show_lines(!get_show_lines());
}

/** @brief toggle_show_dots
  *
  * @todo: document this function
  */
void pov_scene::toggle_show_dots()
{
	set_show_dots(!get_show_dots());
}

/** @brief toggle_show_bbox
  *
  * @todo: document this function
  */
void pov_scene::toggle_show_bbox()
{
	set_show_bbox(!get_show_bbox());
}

/** @brief set_show_normals
  *
  * @todo: document this function
  */
void pov_scene::set_show_normals(bool value)
{
	show_elem[NORMALS] = value;
}

/** @brief set_show_solid
  *
  * @todo: document this function
  */
void pov_scene::set_show_solid(bool value)
{
	show_elem[SOLID] = value;
}

/** @brief set_show_lines
  *
  * @todo: document this function
  */
void pov_scene::set_show_lines(bool value)
{
	show_elem[LINES] = value;
}

/** @brief set_show_dots
  *
  * @todo: document this function
  */
void pov_scene::set_show_dots(bool value)
{
	show_elem[DOTS] = value;
}

/** @brief set_show_bbox
  *
  * @todo: document this function
  */
void pov_scene::set_show_bbox(bool value)
{
	show_elem[BBOX] = value;
}

/** @brief get_show_normals
  *
  * @todo: document this function
  */
bool pov_scene::get_show_normals() const
{
	return show_elem[NORMALS];
}

/** @brief get_show_solid
  *
  * @todo: document this function
  */
bool pov_scene::get_show_solid() const
{
	return show_elem[SOLID];
}

/** @brief get_show_lines
  *
  * @todo: document this function
  */
bool pov_scene::get_show_lines() const
{
	return show_elem[LINES];
}

bool pov_scene::get_show_dots() const
{
	return show_elem[DOTS];
}

bool pov_scene::get_show_bbox() const
{
	return show_elem[BBOX];
}

unsigned int pov_scene::get_plane_steps() const
{
	return int(plane_radius / plane_step);
}

DBL pov_scene::get_plane_radius() const
{
	return plane_radius;
}

unsigned int pov_scene::get_torus_vsteps() const
{
	return torus_vsteps;
}

unsigned int pov_scene::get_torus_usteps() const
{
	return torus_usteps;
}

void pov_scene::make_pov_screenshot()
{
	#define MAX_CHAR 16384
	char buffer[MAX_CHAR];
	ifstream scene_stream(scene_filename.c_str(), ios::in);
	ofstream screenshot_stream(SS_FILE_NAME, ios::out);

	while(scene_stream.getline(buffer, MAX_CHAR))
	{
		screenshot_stream<<buffer<<endl;
	}
	scene_stream.close();
	screenshot_stream<<endl;
	camera->dump_to_povray(screenshot_stream);
	screenshot_stream.close();

	string param = "povray +i";
	param += SS_FILE_NAME;

	param += " +w" + int2str(camera->get_window_width()) + " +h" + \
		int2str(camera->get_window_height()) + " +P Declare=POVVIEWER=" + \
		int2str(objects_quality);

	if (camera->get_antialias())
		param += "+a +a0.1";	/** @todo: добавить в конфиг */

	param += " +k"+dbl2str(scene_clock);
	param += " >/dev/null 2>&1 &";

	system(param.c_str());

	camera->add_msg_to_console( tr("povray launched") );
}

unsigned int pov_scene::get_disc_vsteps() const
{
	return disc_vsteps;
}

unsigned int pov_scene::get_disc_usteps() const
{
	return disc_usteps;
}

SCENE_STATS pov_scene::get_stats() const
{
	SCENE_STATS tmp;
	GLOBJ_STATS globj_tmp;
	pov_baseobject* obj;
	tmp.objects_count = objects.size();
	tmp.vertices_count = 0;
//	tmp.normals_count = 0;
	tmp.faces_count = 0;
	tmp.contours_count = 0;
//	tmp.patches_count = 0;
	tmp.contour_vertices_count = 0;
	tmp.vec3d_count = 0;
	for (unsigned int i=0; i<objects.size(); i++)
	{
		obj = objects.at(i);
		globj_tmp = obj->get_stats();
//		tmp.normals_count += globj_tmp.normals_count;
		tmp.faces_count += globj_tmp.faces_count;
		tmp.contours_count += globj_tmp.contours_count;
//		tmp.patches_count += globj_tmp.patches_count;
		tmp.contour_vertices_count += globj_tmp.contour_vertices_count;
	}
	tmp.vertices_count = v_pool_size;
	tmp.vec3d_count = tmp.contour_vertices_count + v_pool_size;
	return tmp;
}

void pov_scene::print_stats(std::ostream& os) const
{
	SCENE_STATS st = get_stats();
	os <<"=== Scene statistics ==="<<endl;
	os <<"Objects count:	"<<smart_format(st.objects_count)<<endl;
	os <<"Vertices count:	"<<smart_format(st.vertices_count)<<endl;
	os <<"Faces count:	"<<smart_format(st.faces_count)<<endl;
	os <<"Contours count:	"<<smart_format(st.contours_count)<<endl;
//	os <<"Patches count:	"<<smart_format(st.patches_count)<<endl;
//	os <<"Normals count:	"<<smart_format(st.normals_count)<<endl;
	os <<"Vectors count:	"<<smart_format(st.vec3d_count)<<endl;
	os <<"Memory used:	"<<smart_format((st.vec3d_count*sizeof(vec3d)))<<" bytes"<<endl;
	os <<"Process vmsize: "<<smart_format(get_vmsize_kb())<<" kB"<<endl;
	os <<endl<<"Parse time:	"<< parse_time <<" seconds"<<endl;
	os <<"Load time:	"<< load_time <<" seconds"<<endl;
	os <<"Total time:	"<< parse_time+load_time <<" seconds"<<endl;
	os <<endl;
}

/** @brief toggle_use_povray_camera
  *
  * @todo: document this function
  */
void pov_scene::toggle_use_povray_camera()
{
	set_use_povray_camera(!get_use_povray_camera());
}

/** @brief set_use_povray_camera
  *
  * @todo: document this function
  */
void pov_scene::set_use_povray_camera(const bool value)
{
	use_povray_camera = value;
	if (value)
		camera->add_msg_to_console("use povray camera = on");
	else
		camera->add_msg_to_console("use povray camera = off");
}

/** @brief get_use_povray_camera
  *
  * @todo: document this function
  */
bool pov_scene::get_use_povray_camera() const
{
	return use_povray_camera;
}

/** @brief get_camera
  *
  * @todo: document this function
  */
view_camera * pov_scene::get_camera()
{
	return camera;
}

/** @brief toggle_smooth_obj
  *
  * @todo: document this function
  */
void pov_scene::toggle_smooth_obj()
{
	set_smooth_obj(!get_smooth_obj());
	load();
}

/** @brief set_smooth_obj
  *
  * @todo: document this function
  */
void pov_scene::set_smooth_obj(const bool value)
{
	smooth_obj = value;
}

/** @brief get_smooth_obj
  *
  * @todo: document this function
  */
bool pov_scene::get_smooth_obj() const
{
	return smooth_obj;
}

/** @brief get_smooth_angle
  *
  * @todo: document this function
  */
DBL pov_scene::get_smooth_angle() const
{
	return smooth_angle;
}

/** @brief end_dxf_stream
  *
  * @todo: document this function
  */
void pov_scene::end_dxf_stream(std::ostream& dxf_stream)
{
	dxf_stream<<"0"<<endl;
	dxf_stream<<"ENDSEC"<<endl;
	dxf_stream<<"0"<<endl;
	dxf_stream<<"EOF"<<endl;
}

/** @brief begin_dxf_stream
  *
  * @todo: document this function
  */
void pov_scene::begin_dxf_stream(std::ostream& dxf_stream)
{
	dxf_stream<<"0"<<endl;
	dxf_stream<<"SECTION"<<endl;
	dxf_stream<<"2"<<endl;
	dxf_stream<<"HEADER"<<endl;
	dxf_stream<<"0"<<endl;
	dxf_stream<<"ENDSEC"<<endl;
  dxf_stream<<"0"<<endl;
	dxf_stream<<"SECTION"<<endl;
	dxf_stream<<"2"<<endl;
	dxf_stream<<"ENTITIES"<<endl;
}
/** @brief dump_scene
  *
  * @todo: document this function
  */
void pov_scene::dump_scene_to_dxf()
{
	//~ DBL start_time = SDL_GetTicks();
	unsigned int lc;
	const unsigned int REF_COUNT = 5000;
	string dxf_filename = scene_filename.substr(0, scene_filename.length()-4) + ".dxf";
	string msg = "Export scene to '" + dxf_filename + "'";
	camera->add_msg_to_console(msg.c_str());
	camera->add_msg_to_console("Generate list of lines");

#ifdef VECTOR_LINES
	vector_line3d* lines = new vector_line3d();
#else
	set_line3d* lines = new set_line3d();
#endif
	pov_baseobject *obj;
	for (unsigned int i=0; i<objects.size(); i++)
	{
		obj = objects[i];
		camera->add_msg_to_console(obj->get_type().c_str());
		obj->get_lines(lines);
		refresh_screen();
	}
	camera->add_msg_to_console(std::string("Generated "+int2str(lines->size())+" lines.").c_str());
	DEBUG_MSG("Generated "<<lines->size()<<" lines.")

	DBL max_extents[3] = {-1e9, -1e9, -1e9};
	DBL min_extents[3] = {+1e9, +1e9, +1e9};

#ifdef VECTOR_LINES
	vector_line3d::iterator lines_iter = lines->begin();
#else
	set_line3d::iterator lines_iter = lines->begin();
#endif
	//for(unsigned int i=0; i<lines->size(); i++)
	camera->add_msg_to_console("Calculate extents");
	unsigned int lines_count = lines->size();
	DBL lines_proc = 0;
	lc = 0;
	for(lines_iter=lines->begin(); lines_iter!=lines->end(); lines_iter++)
	{
		for(unsigned int j=0; j<3; j++)
		{
			if ((*lines_iter).p1[j]>max_extents[j])
				max_extents[j] = (*lines_iter).p1[j];
			if ((*lines_iter).p2[j]>max_extents[j])
				max_extents[j] = (*lines_iter).p2[j];
			if ((*lines_iter).p1[j]<min_extents[j])
				min_extents[j] = (*lines_iter).p1[j];
			if ((*lines_iter).p2[j]<min_extents[j])
				min_extents[j] = (*lines_iter).p2[j];
		}
		if (lc%REF_COUNT==0)
		{
			camera->add_msg_to_console((dbl2str(lines_proc)+" %%").c_str());
			refresh_screen();
		}
		lc++;
		lines_proc = lc*100/lines_count;
	}
	DEBUG_MSG("MIN_EXTENTS = "<<min_extents[0]<<", "<<min_extents[1]<<", "<<min_extents[2]<<" ")
	DEBUG_MSG("MAX_EXTENTS = "<<max_extents[0]<<", "<<max_extents[1]<<", "<<max_extents[2]<<" ")

	DBL size_x = fabs(max_extents[0] - min_extents[0]);
	DBL size_y = fabs(max_extents[1] - min_extents[1]);
	DBL size_z = fabs(max_extents[2] - min_extents[2]);

	DBL max_size_xy = size_x>size_y?size_x:size_y;
	DBL max_size = max_size_xy>size_z?max_size_xy:size_z;

	DBL center_x = (min_extents[0] + max_extents[0])/2;
	DBL center_y = (min_extents[1] + max_extents[1])/2;
	DBL center_z = (min_extents[2] + max_extents[2])/2;

	DBL offset_drawings = max_size * 0.05;

//	DBL offset_x = -center_x + size_x/2 + max_size*offset_drawings;
//	DBL offset_y = -center_y + size_y/2 + max_size*offset_drawings;
//	DBL offset_z = -center_z + size_z/2 + max_size*offset_drawings;

	filebuf fb;
  fb.open(dxf_filename.c_str(), ios::out);
  std::ostream dxf_stream(&fb);
	begin_dxf_stream(dxf_stream);

	// xy -zy
	// xz
	//line3d* line;
#ifdef VECTOR_LINES
	vector_line3d* lines_dump;
#else
	set_line3d* lines_dump;
#endif

	// xy
	camera->add_msg_to_console("Generate XY projection");
#ifdef VECTOR_LINES
	lines_dump = new vector_line3d();
#else
	lines_dump = new set_line3d();
#endif
	lc = 0;
	for(lines_iter=lines->begin(); lines_iter!=lines->end(); lines_iter++)
	{
		vec3d a = vec3d((*lines_iter).p1[0]-center_x-size_x/2-offset_drawings, (*lines_iter).p1[1]-center_y+size_y/2+offset_drawings, 0);
		vec3d b = vec3d((*lines_iter).p2[0]-center_x-size_x/2-offset_drawings, (*lines_iter).p2[1]-center_y+size_y/2+offset_drawings, 0);
		add_line(lines_dump, a, b);
		if (lc%REF_COUNT==0)
			refresh_screen();
		lc++;
	}

	camera->add_msg_to_console("Dump XY projection");
	lc = 0;
	for(lines_iter=lines_dump->begin(); lines_iter!=lines_dump->end(); lines_iter++)
	{
		(*lines_iter).dump(dxf_stream);
		if (lc%REF_COUNT==0)
			refresh_screen();
		lc++;
	}

	lines_dump->clear();
	delete lines_dump;

	// xz
	camera->add_msg_to_console("Generate XZ projection");
#ifdef VECTOR_LINES
	lines_dump = new vector_line3d();
#else
	lines_dump = new set_line3d();
#endif
	lc = 0;
	for(lines_iter=lines->begin(); lines_iter!=lines->end(); lines_iter++)
	{
		vec3d a = vec3d((*lines_iter).p1[0]-center_x-size_x/2-offset_drawings, (*lines_iter).p1[2]-center_z-size_z/2-offset_drawings, 0);
		vec3d b = vec3d((*lines_iter).p2[0]-center_x-size_x/2-offset_drawings, (*lines_iter).p2[2]-center_z-size_z/2-offset_drawings, 0);
		add_line(lines_dump, a, b);
		if (lc%REF_COUNT==0)
			refresh_screen();
		lc++;
	}

	camera->add_msg_to_console("Dump XZ projection");
	lc = 0;
	for(lines_iter=lines_dump->begin(); lines_iter!=lines_dump->end(); lines_iter++)
	{
		(*lines_iter).dump(dxf_stream);
		if (lc%REF_COUNT==0)
			refresh_screen();
		lc++;
	}

	lines_dump->clear();
	delete lines_dump;

	// -zy
	camera->add_msg_to_console("Generate -ZY projection");
#ifdef VECTOR_LINES
	lines_dump = new vector_line3d();
#else
	lines_dump = new set_line3d();
#endif
	camera->add_msg_to_console("Dump -ZY projection");
	lc = 0;
	for(lines_iter=lines->begin(); lines_iter!=lines->end(); lines_iter++)
	{
		vec3d a = vec3d(-(*lines_iter).p1[2]+center_z+size_z/2+offset_drawings, (*lines_iter).p1[1]-center_y+size_y/2+offset_drawings, 0);
		vec3d b = vec3d(-(*lines_iter).p2[2]+center_z+size_z/2+offset_drawings, (*lines_iter).p2[1]-center_y+size_y/2+offset_drawings, 0);
		add_line(lines_dump, a, b);
		if (lc%REF_COUNT==0)
			refresh_screen();
		lc++;
	}
	lc = 0;
	for(lines_iter=lines_dump->begin(); lines_iter!=lines_dump->end(); lines_iter++)
	{
		(*lines_iter).dump(dxf_stream);
		if (lc%REF_COUNT==0)
			refresh_screen();
		lc++;
	}

	lines_dump->clear();
	delete lines_dump;

  end_dxf_stream(dxf_stream);
  fb.close();
	msg = "Export scene to '" + dxf_filename + "' complete.";
	camera->add_msg_to_console(msg.c_str());
	lines->clear();
	delete lines;

	DEBUG_MSG("Time to export: "<<(SDL_GetTicks()-start_time)/1000.0)
}

/** @brief load_from_config
  *
  * @todo: document this function
  */
void pov_scene::load_from_config()
{
	#define read_int_key(pname, pvalue) \
	if (param==pname) \
		{ \
			err = sscanf(value.c_str(), "%d", &pvalue); \
			if (err!=1) \
			{ \
				cout << "Config file '" << config_file << "' line " << line_number << " - '" << orgline << "' bad value. Ignored!" << endl; \
			} \
			DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue) \
			continue; \
		}

	#define read_int3_key(pname, pvalue) \
	if (param==pname) \
		{ \
			tmp = 90; tmp1 = 90; tmp2 = 90;\
			err = sscanf(value.c_str(), "%d,%d,%d", &tmp, &tmp1, &tmp2); \
			if (err!=3) \
			{ \
				cout << "Config file '" << config_file << "' line " << line_number << " - '" << orgline << "' bad value. Ignored!" << endl; \
			} \
			else \
			{ \
				pvalue[0] = tmp; \
				pvalue[1] = tmp1; \
				pvalue[2] = tmp2; \
				DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue[0]<<", "<< pvalue[1]<<", "<< pvalue[2]) \
			} \
			continue; \
		}

	#define read_dbl_key(pname, pvalue) \
	if (param==pname) \
		{ \
			err = sscanf(value.c_str(), "%lf", &pvalue); \
			if (err!=1) \
			{ \
				cout << "Config file '" << config_file << "' line " << line_number << " - '" << orgline << "' bad value. Ignored!" << endl; \
			} \
			DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue) \
			continue; \
		}

	#define read_bool_key(pname, pvalue) \
	if (param==pname) \
		{ \
			tmp = 90; \
			err = sscanf(value.c_str(), "%d", &tmp); \
			if (err!=1) \
			{ \
				cout << "Config file '" << config_file << "' line " << line_number << " - '" << orgline << "' bad value. Ignored!" << endl; \
			} \
			else \
			{ \
				if (tmp==0) pvalue = false; \
				if (tmp==1) pvalue = true; \
				DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue) \
			} \
			continue; \
		}

	string config_file, config_dir;
	struct stat sb;
	config_dir = getenv("HOME");
	config_dir += CFG_DIR;
	if (stat(config_dir.c_str(), &sb) == -1)
	{
		cout << "Creating config directory '"<< config_dir << "'" << endl;
		if (mkdir(config_dir.c_str(), 0755) == -1)
		{
			perror("mkdir");
		}
	}
	config_file = config_dir + CFG_FILE_SCENE;
	if (stat(config_file.c_str(), &sb) == -1)
	{
		cout << "Creating config file '"<< config_file << "'" << endl;
		FILE *cfg = fopen(config_file.c_str(), "w");
		fclose(cfg);
	}

	// load config file ----------------------------------------------------------------------
	cout << "Loading config file '"<< config_file << "'" << endl;
	filebuf fb;
	fb.open(config_file.c_str(), ios::in);
	istream is(&fb);
	string orgline, line, param, value;
	int line_number=0, bl, el, err, tmp, tmp1, tmp2;
//	double da, db, dc;
	while (is)
	{
		line_number++;
		std::getline(is, orgline);
		if (orgline.length()==0) continue;
		// strip spaces
		bl = orgline.find_first_not_of(" \t\f\v\n\r");
		el = orgline.find_last_not_of(" \t\f\v\n\r");
		line = orgline.substr(bl, el+1);
		// check for comments
		if (line[0]=='#') continue;
		if (line.find("=")==string::npos)
		{
			cout << "Config file: " << config_file << " Bad line " << line_number << " - '" << orgline << "' ignored!" << endl;
			continue;
		}
		// split into param and value
		bl = line.find('=');
		param = line.substr(0, bl);
		value = line.substr(bl+1, line.length()-bl-1);
		read_int3_key("isoresolution", isoresolution)
		read_int3_key("quadresolution", quadresolution)
		read_int3_key("blobresolution", blobresolution)
		read_int_key("patchresolution", patchresolution)
		read_int_key("spheresweepresolution", spheresweepresolution)
		read_int_key("cone_num_latitudes", cone_num_latitudes)
		read_int_key("cone_num_longitudes", cone_num_longitudes)
		read_int_key("sphere_num_latitudes", sphere_num_latitudes)
		read_int_key("sphere_num_longitudes", sphere_num_longitudes)
		read_int_key("lathe_num_longitudes", lathe_num_longitudes)
		read_int_key("lathe_spline_steps", lathe_spline_steps)
		read_int_key("prism_spline_steps", prism_spline_steps)
		read_dbl_key("plane_radius", plane_radius)
		read_dbl_key("plane_step", plane_step)
		read_int_key("torus_usteps", torus_usteps)
		read_int_key("torus_vsteps", torus_vsteps)
		read_int_key("disc_usteps", disc_usteps)
		read_int_key("disc_vsteps", disc_vsteps)
		read_int_key("max_lights", max_lights)
		read_dbl_key("normals_size", normals_size)
		read_int_key("objects_quality", objects_quality)
		read_dbl_key("scene_clock", scene_clock)
		read_bool_key("use_povray_camera", use_povray_camera)
		read_bool_key("show_elem0", show_elem[0])
		read_bool_key("show_elem1", show_elem[1])
		read_bool_key("show_elem2", show_elem[2])
		read_bool_key("show_elem3", show_elem[3])
		read_bool_key("show_elem4", show_elem[4])
		read_bool_key("show_elem5", show_elem[5])
		read_bool_key("smooth_obj", smooth_obj)
		read_dbl_key("smooth_angle", smooth_angle)
		read_dbl_key("plane_alpha", plane_alpha)
		read_bool_key("show_camera", show_camera)
		read_dbl_key("frustum_size", frustum_size)
		cout << "Error line " << line_number << ". '" << param << "' = " << value <<endl;
	}
	fb.close();
}

/** @brief save_to_config
  *
  * @todo: document this function
  */
void pov_scene::save_to_config()
{
	if (!foreign_settings)
	{
		cout << "Saving scene settings" << endl;
		filebuf fb;
		struct stat sb;
		string config_dir = getenv("HOME");
		config_dir += CFG_DIR;
		if (stat(config_dir.c_str(), &sb) == -1)
		{
			cout << "Creating config directory '"<< config_dir << "'" << endl;
			if (mkdir(config_dir.c_str(), 0755) == -1)
			{
				perror("mkdir");
			}
		}
		string config_file = config_dir + CFG_FILE_SCENE;
		if (stat(config_file.c_str(), &sb) == -1)
		{
			cout << "Creating config file '"<< config_file << "'" << endl;
			FILE *cfg = fopen(config_file.c_str(), "w");
			fclose(cfg);
		}
		fb.open(config_file.c_str(), ios::out);
		ostream os(&fb);
		os << "isoresolution=" << isoresolution[0] << "," << isoresolution[1] << "," << isoresolution[2] << endl;
		os << "quadresolution=" << quadresolution[0] << "," << quadresolution[1] << "," << quadresolution[2] << endl;
		os << "blobresolution=" << blobresolution[0] << "," << blobresolution[1] << "," << blobresolution[2] << endl;
		os << "patchresolution=" << patchresolution << endl;
		os << "spheresweepresolution=" << spheresweepresolution << endl;
		os << "cone_num_latitudes=" << cone_num_latitudes << endl;
		os << "cone_num_longitudes=" << cone_num_longitudes << endl;
		os << "sphere_num_latitudes=" << sphere_num_latitudes << endl;
		os << "sphere_num_longitudes=" << sphere_num_longitudes << endl;
		os << "lathe_num_longitudes=" << lathe_num_longitudes << endl;
		os << "lathe_spline_steps=" << lathe_spline_steps << endl;
		os << "prism_spline_steps=" << prism_spline_steps << endl;
		os << "plane_radius=" << plane_radius << endl;
		os << "plane_step=" << plane_step << endl;
		os << "torus_usteps=" << torus_usteps << endl;
		os << "torus_vsteps=" << torus_vsteps << endl;
		os << "disc_usteps=" << disc_usteps << endl;
		os << "disc_vsteps=" << disc_vsteps << endl;
		os << "max_lights=" << max_lights << endl;
		os << "normals_size=" << normals_size << endl;
		os << "objects_quality=" << objects_quality << endl;
		os << "scene_clock=" << scene_clock << endl;
		os << "use_povray_camera=" << use_povray_camera << endl;
		for (unsigned int i=0; i<NUMBER_OF_GLLISTS; i++)
		{
			os << "show_elem" << i << "=" << show_elem[i] << endl;
		}
		os << "smooth_obj=" << smooth_obj << endl;
		os << "smooth_angle=" << smooth_angle << endl;
		os << "plane_alpha=" << plane_alpha << endl;
		os << "show_camera=" << show_camera << endl;
		os << "frustum_size=" << frustum_size << endl;
		fb.close();
	} // if (!foreign_settings)
}

/** @brief change_clock
  *
  * @todo: document this function
  */
void pov_scene::change_clock(const double value)
{
	scene_clock += value;
	if (scene_clock<EPSILON)
		scene_clock = 0;
}

/** @brief get_clock
  *
  * @todo: document this function
  */
double pov_scene::get_clock() const
{
	return scene_clock;
}

/** @brief set_patch_resolution
  *
  * @todo: document this function
  */
void pov_scene::set_patch_resolution(const unsigned int val)
{
	patchresolution = val;
}

/** @brief get_patch_resolution
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_patch_resolution() const
{
	return patchresolution;
}

/** @brief set_sphere_sweep_resolution
  *
  * @todo: document this function
  */
void pov_scene::set_sphere_sweep_resolution(const unsigned int val)
{
	spheresweepresolution = val;
}

/** @brief get_sphere_sweep_resolution
  *
  * @todo: document this function
  */
unsigned int pov_scene::get_sphere_sweep_resolution() const
{
	return spheresweepresolution;
}

/** @brief get_plane_alpha
  *
  * @todo: document this function
  */
DBL pov_scene::get_plane_alpha() const
{
	return plane_alpha;
}

/** @brief set_frustum_size
  *
  * @todo: document this function
  */
void pov_scene::set_frustum_size(const DBL val)
{
	frustum_size = val;
}

/** @brief get_frustum_size
  *
  * @todo: document this function
  */
DBL pov_scene::get_frustum_size() const
{
	return frustum_size;
}

/** @brief set_show_camera
  *
  * @todo: document this function
  */
void pov_scene::set_show_camera(const bool val)
{
	show_camera = val;
}

/** @brief get_show_camera
  *
  * @todo: document this function
  */
bool pov_scene::get_show_camera() const
{
	return show_camera;
}

/** @brief toggle_show_camera
  *
  * @todo: document this function
  */
void pov_scene::toggle_show_camera()
{
	set_show_camera(!get_show_camera());
}

/** @brief refresh_screen
  *
  * @todo: document this function
  */
void pov_scene::refresh_screen()
{
	camera->clear_gl_buffer();
	if (camera->get_stereo_cam())
	{
		for (unsigned int k=0; k<2; k++)
		{
			camera->setup_viewport(k);
			camera->begin_frame(k);
			draw();
			camera->end_frame();
		}
	}
	else
	{
		camera->setup_viewport(-1);
		camera->begin_frame(-1);
		draw();
		camera->end_frame();
	}
	SDL_GL_SwapBuffers();
	SDL_Delay(DELAY_FPS);
}

