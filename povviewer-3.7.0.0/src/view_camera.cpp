// -*- coding: utf-8 -*-

#include <SDL.h>
#include "GL/gl.h"
#include <GL/glu.h>
#include <GL/glx.h>
#include <sys/stat.h>
#include <fstream>

#include "view_camera.hpp"
#include "common_funcs.hpp"
#include "pov_scene.hpp"

#define BORDER_OFFSET 3

bool view_camera::init_opengl()
{
	video_flags = SDL_OPENGL|SDL_RESIZABLE|SDL_HWSURFACE|SDL_DOUBLEBUF;
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode
	(
		window_width,
		window_height,
		16,
		video_flags
	);
	if ( screen == NULL ) {
		fprintf
		(
			stderr,
			"Couldn't set %dx%d GL video mode: %s\n",
			window_width,
			window_height,
			SDL_GetError()
		);
		SDL_Quit();
		return false;
	}

	SDL_WM_SetCaption("POV-Viewer", "povviewer");
	string logo_filename = get_config_dir()+"/logo.bmp";
	SDL_WM_SetIcon(SDL_LoadBMP(logo_filename.c_str()), NULL);
	SDL_EnableKeyRepeat(KEY_DELAY, KEY_REPEAT);
	last_msg_added = SDL_GetTicks();
	start_time_msec = SDL_GetTicks();
	frame_count = 0;
	glShadeModel( GL_SMOOTH );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	glEnable( GL_CULL_FACE );
	glEnable( GL_MAP2_VERTEX_3);
	glEnable(GL_RESCALE_NORMAL); // src/main.c:119: error: 'GL_RESCALE_NORMALS' was not declared in this scope
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	//opengl_active = true;
	build_font();
	bgcolor.gl_clear_color();
	return true;
}

/** @brief set_window_height
  *
  * @todo: document this function
  */
void view_camera::set_window_height(const unsigned int h)
{
	window_height = h;
	if ( window_height == 0 )
		window_height = 1;
}

/** @brief set_window_width
  *
  * @todo: document this function
  */
void view_camera::set_window_width(const unsigned int w)
{
	window_width = w;
}

int view_camera::get_resize_skip()
{
	return resize_skip;
}

void view_camera::set_resize_skip(const int val)
{
	if (val<0)
	{
		resize_skip = 0;
	} else {
		resize_skip = val;
	}
}

/** @brief resize_window
  *
  * @todo: document this function
  */
bool view_camera::resize_window()
{
	screen = SDL_SetVideoMode(window_width, window_height, 16, video_flags);
	if ( !screen )
	{
		fprintf( stderr, "Could not get a surface after resize: %s\n", \
			SDL_GetError( ) );

		return false;
	}

	//~ window_width = w;
	//~ window_height = h;


//	if (!get_stereo_cam())
//	{
//		// not stereo mode
//		glViewport( 0, 0, window_width, window_height);
//	}

	char msg[1000];

	sprintf(msg, "window_width=%d widow_height=%d", \
		window_width, window_height);

	add_msg_to_console(msg);

	scene->set_caption();

	return true;
}

/** @brief shutdown_opengl
  *
  * @todo: document this function
  */
void view_camera::shutdown_opengl()
{
	cout << "FPS:" << fps << endl;
	SDL_Quit();
}

/** @brief count_fps
  *
  * @todo: document this function
  */
void view_camera::count_fps()
{
	current_time_msec = SDL_GetTicks();
	time_sec = double(current_time_msec - start_time_msec)/double(1000.0);
	fps = double(frame_count)/time_sec;
	if (get_stereo_cam())
	{
		fps = fps / 2.0;
	}
}

/** @brief set_scene
  *
  * @todo: document this function
  */
void view_camera::set_scene(pov_scene* s)
{
	scene = s;
}

/** @brief end_frame
  *
  * @todo: document this function
  */
void view_camera::end_frame()
{
	begin_text();
	if ((scene->parse_error)&&(scene->show_errors))
	{
		// show error messages
		for (unsigned int i=0; i<scene->err_count; i++)
			put_text(10, window_height-30-10*i, vec3d(1, 0, 0), 1, "%s", scene->error_string[i].c_str());
	}
	unsigned int string_width;

	// fps counter
	string_width = XTextWidth(fontInfo, "888.8 fps",9)+BORDER_OFFSET;
	if (get_stereo_cam())
	{
		put_text(window_width-string_width*2, window_height-font_height, vec3d(1)-bgcolor, 1, "%5.1f fps", fps);
	}
	else
	{
		put_text(window_width-string_width, window_height-font_height, vec3d(1)-bgcolor, 1, "%5.1f fps", fps);
	}

	// camera position
	put_text(BORDER_OFFSET, 2*font_height+BORDER_OFFSET, vec3d(0.5, 0, 0), 1, "pos: %s",
		location.print("<%6.3f, %6.3f, %6.3f>").c_str());

	// camera up vector
	if (up_fixed)
		put_text(BORDER_OFFSET, font_height+BORDER_OFFSET, vec3d(0, 0.5, 0), 1, "fup: %s",
			up.print("<%6.3f, %6.3f, %6.3f>").c_str());
	else
		put_text(BORDER_OFFSET, font_height+BORDER_OFFSET, vec3d(0, 0.5, 0), 1, " up: %s",
			up.print("<%6.3f, %6.3f, %6.3f>").c_str());

	// navigator position == camera look_at vector
	put_text(BORDER_OFFSET, BORDER_OFFSET, vec3d(0, 0, 0.5), 1, "lat: %s",
		get_look_at().print("<%6.3f, %6.3f, %6.3f>").c_str());

	// camera movement speed
	put_text(BORDER_OFFSET, 3*font_height+BORDER_OFFSET, vec3d(0.647059, 0.584314, 0.552941), 1,
		"spd: %s", speed.print("<%6.3f, %6.3f, %6.3f>").c_str());

	// camera fov
	string orthostring = "ortho "+dbl2str(ortho_size);
	string perspstring = "persp "+dbl2str(angle);
	put_text(window_width/2+BORDER_OFFSET, 2*font_height+BORDER_OFFSET, vec3d(1)-bgcolor, 1, "angle: %s",
		perspective?perspstring.c_str():orthostring.c_str());

	// scene clock
	put_text(window_width/2+BORDER_OFFSET, font_height+BORDER_OFFSET, vec3d(1)-bgcolor, 1, "clock: %s",
		dbl2str(scene->get_clock()).c_str());

	// navigator size
	put_text(window_width/2+BORDER_OFFSET, BORDER_OFFSET, vec3d(1)-bgcolor, 1, "nav size: %s",
		dbl2str(get_navigator_size()).c_str());

	if (get_stereo_cam())
	{
		put_text(window_width/2+BORDER_OFFSET, 3*font_height+BORDER_OFFSET, vec3d(1)-bgcolor, 1, "base size: %s",
			dbl2str(get_stereo_base_size()).c_str());
	}

	show_console();

}

/** @brief get_stereo_cam
  *
  * @todo: document this function
  */
bool view_camera::get_stereo_cam() const
{
	return stereo_cam;
}

/** @brief set_stereo_cam
  *
  * @todo: document this function
  */
void view_camera::set_stereo_cam(const bool value)
{
	stereo_cam = value;
	if (!value)
	{
		glViewport( 0, 0, window_width, window_height);
	}
}

/** @brief get_stereo_base_size
  *
  * @todo: document this function
  */
DBL view_camera::get_stereo_base_size() const
{
	return stereo_base_size;
}

/** @brief set_stereo_base_size
  *
  * @todo: document this function
  */
void view_camera::set_stereo_base_size(const DBL value)
{
	stereo_base_size = value;
	if (stereo_base_size<0)
	{
		stereo_base_size = 0;
	}
}



/** @brief process
  *
  * @todo: document this function
  */
void view_camera::process()
{
	if (speed[0]!=0)
	{
		vec3d ndir = direction.normalize() * speed[0];
		location = location + ndir;
//		if (speed[0]>0)
//			distance_to_nav -= ndir.length();
//		else
//			distance_to_nav += ndir.length();
	}
	if (speed[1]!=0)
	{
		vec3d ndir =right.normalize() * speed[1];
		location = location + ndir;
	}
	if (speed[2]!=0)
	{
		vec3d ndir = up.normalize() * speed[2];
		location = location + ndir;
	}
	if (nav_speed!=0)
	{
		distance_to_nav += nav_speed;
		if (distance_to_nav<0.1)
			distance_to_nav = 0.1;
	}
}

/** @brief begin_frame
  *
  * @todo: document this function
  */
void view_camera::begin_frame(int cam_num)
{
	frame_count++;
	count_fps();
	process();
	if (antialias)
	{
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
	}

//	bgcolor.gl_clear_color();
//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScalef(-1.0, 1.0, 1.0);

	if (get_stereo_cam())
	{
		cam_ratio = float(window_width/2.0) / float(window_height);
	}
	else
	{
		calc_cam_ratio();
	}

	if (perspective)
		setup_perspective(cam_num);
	else
		setup_ortho();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	if (show_axis[0])
	{
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(axis_size, 0, 0);
		glColor3f(0.5, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(-axis_size, 0, 0);
		glEnd();
	}
	if (show_axis[1])
	{
		glBegin(GL_LINES);
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, axis_size, 0);
		glColor3f(0, 0.5, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, -axis_size, 0);
		glEnd();
	}
	if(show_axis[2])
	{
		glBegin(GL_LINES);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, axis_size);
		glColor3f(0, 0, 0.5);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, -axis_size);
		glEnd();
	}
	if (show_navigator)
	{
		if (navigator_ontop)
		{
			//glDisable(GL_DEPTH_TEST);
			get_look_at().draw_navigator(navigator_size);
			//glEnable(GL_DEPTH_TEST);
		}
		else
		{
			get_look_at().draw_navigator(navigator_size);
		}
	}
}

/** @brief setup_ortho
  *
  * @todo: document this function
  */
void view_camera::setup_ortho()
{
	glOrtho (-ortho_size*cam_ratio, \
		ortho_size*cam_ratio, \
		-ortho_size, \
		ortho_size, \
		-ortho_size*10000, \
		ortho_size*10000);
	vec3d look_at = location + direction;
	gluLookAt(location[0], location[1], location[2], look_at[0], look_at[1], look_at[2], up[0], up[1], up[2]);
}

/** @brief set_bgcolor
  *
  * @todo: document this function
  */
void view_camera::set_bgcolor(const vec3d new_value)
{
	bgcolor = new_value;
}

/** @brief set_cam_zfar
  *
  * @todo: document this function
  */
void view_camera::set_cam_zfar(const SNGL new_value)
{
	cam_zfar = new_value;
}

/** @brief set_cam_znear
  *
  * @todo: document this function
  */
void view_camera::set_cam_znear(const SNGL new_value)
{
	cam_znear = new_value;
}

/** @brief setup_perspective
  *
  * @todo: document this function
  */
void view_camera::setup_perspective(int cam_num)
{
	gluPerspective(angle/cam_ratio /*0.82*/, cam_ratio, cam_znear, cam_zfar );
	vec3d look_at = location + direction;
	if (get_stereo_cam())
	{
		vec3d new_loc = location;
		if (cam_num==0)
		{
			// left (- parallel view) (+ crosseyed view)
			new_loc = new_loc - right*stereo_base_size;
			gluLookAt(new_loc[0], new_loc[1], new_loc[2], look_at[0], look_at[1], look_at[2], up[0], up[1], up[2]);
		}
		if (cam_num==1)
		{
			// right (+ parallel view) (- crosseyed view)
			new_loc = new_loc + right*stereo_base_size;
			gluLookAt(new_loc[0], new_loc[1], new_loc[2], look_at[0], look_at[1], look_at[2], up[0], up[1], up[2]);
		}
	}
	else
	{
		gluLookAt(location[0], location[1], location[2], look_at[0], look_at[1], look_at[2], up[0], up[1], up[2]);
	}
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
vec3d view_camera::get_look_at() const
{
/*
	vec3d lat;
	lat = direction * distance_to_nav;
	lat = lat + location;
	return lat;
*/
	return location + direction * distance_to_nav;
}

/** @brief reset_config
  *
  * reset settings to default values
  */
void view_camera::reset_config()
{
	do_animation = true;
	animation_duration = 5000;
	window_width = 512;
	window_height = 384;
	cam_zfar = 2048.0;
	cam_znear = 0.1; //EPSILON;
	console_size = 50;
	scroll_time = 32;
	bgcolor = vec3d(0.847059, 0.784314, 0.752941);
	location = vec3d(0, 0, -5);
	direction = vec3d(0, 0, 1);
	up = vec3d(0, 1, 0);
	fixed_up = vec3d(0, 1, 0);
	right = vec3d(1, 0, 0);
	angle = 63.5;
	ortho_size = 10.0;
	distance_to_nav = 5;
	up_fixed = true;
	speed = vec3d(0, 0, 0);
	nav_speed = 0;
	max_speed = 10;
	delta_speed = 0.01;
	delta_rotation = 0.005;
	perspective = true;
	antialias = false;
	show_navigator = true;
	navigator_ontop = true;
	navigator_size = 0.5;
	show_axis[0] = show_axis[1] = show_axis[2] = true;
	axis_size = 1000;
	stereo_cam = false;
	stereo_base_size = 0.04;
	font_string = "-monotype-andale mono-medium-r-normal--10-0-0-0-c-0-iso8859-1";
	foreign_settings = false;
}

/** @brief view_camera
  *
  * @todo: document this function
  */
 view_camera::view_camera()
{
	// initialization
	scene = NULL;

	reset_config();
	// load new parameters
	load_from_config();

	// continue initialization
	fontInfo = NULL;
	font_base = 0;
	calc_cam_ratio();
	console_buffer = new string[console_size];
	fix_right();
	fix_up();
	fix_direction();
	cam_state = CS_FREE;
	animation_in_progress = false;
}

/** @brief calc_cam_ratio
  *
  * @todo: document this function
  */
void view_camera::calc_cam_ratio()
{
	if ((up_length==0)&&(right_length==0))
	{
		cam_ratio = float(window_width) / float(window_height);
	}
	else
	{
		cam_ratio = float(right_length) / float(up_length);
	}
}

/** @brief fix_direction
  *
  * @todo: document this function
  */
void view_camera::fix_direction()
{
	direction = up ^ right;
	direction = direction.normalize();
}

/** @brief fix_right
  *
  * @todo: document this function
  */
void view_camera::fix_right()
{
	right = direction ^ up;
	right = right.normalize();
}

/** @brief fix_up
  *
  * @todo: document this function
  */
void view_camera::fix_up()
{
	up = right ^ direction;
	up = up.normalize();
}

/** @brief load_from_config
  *
  * @todo: document this function
  */
void view_camera::load_from_config()
{
	#define read_str_key(pname, pvalue) \
	if (param==pname) \
		{ \
			pvalue = value; \
			DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue) \
			continue; \
		}

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

	#define read_flt_key(pname, pvalue) \
	if (param==pname) \
		{ \
			err = sscanf(value.c_str(), "%f", &pvalue); \
			if (err!=1) \
			{ \
				cout << "Config file '" << config_file << "' line " << line_number << " - '" << orgline << "' bad value. Ignored!" << endl; \
			} \
			DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue) \
			continue; \
		}

	#define read_vec3d_key(pname, pvalue) \
	if (param==pname) \
		{ \
			err = sscanf(value.c_str(), "%lf,%lf,%lf", &da, &db, &dc); \
			if (err!=3) \
			{ \
				cout << "Config file '" << config_file << "' line " << line_number << " - '" << orgline << "' bad value. Ignored!" << endl; \
			} \
			else \
			{ \
				pvalue = vec3d(da, db, dc); \
				DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue) \
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

	#define read_bool3_key(pname, pvalue) \
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
				if (tmp==0) pvalue[0] = false; \
				else pvalue[0] = true; \
				if (tmp1==0) pvalue[1] = false; \
				else pvalue[1] = true; \
				if (tmp2==0) pvalue[2] = false; \
				else pvalue[2] = true; \
				DEBUG_MSG("Readed " << err << " values " << pname" = " << pvalue[0] <<", "<< pvalue[1] <<", "<< pvalue[2] ) \
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
	config_file = config_dir + CFG_FILE_CAMERA;
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
	double da, db, dc;
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
		read_int_key("window_width", window_width)
		read_int_key("window_height", window_height)
		read_flt_key("cam_zfar", cam_zfar)
		read_flt_key("cam_znear", cam_znear)
		read_int_key("console_size", console_size)
		read_int_key("scroll_time", scroll_time)
		read_vec3d_key("bgcolor", bgcolor)
		read_vec3d_key("location", location)
		read_vec3d_key("direction", direction)
		read_vec3d_key("up", up)
		read_vec3d_key("fixed_up", fixed_up)
		read_vec3d_key("right", right)
		read_int_key("animation_duration", animation_duration)
		read_flt_key("angle", angle)
		read_flt_key("ortho_size", ortho_size)
		read_dbl_key("distance_to_nav", distance_to_nav)
		read_bool_key("do_animation", do_animation)
		read_bool_key("up_fixed", up_fixed)
		read_dbl_key("max_speed", max_speed)
		read_dbl_key("delta_speed", delta_speed)
		read_dbl_key("delta_rotation", delta_rotation)
		read_bool_key("perspective", perspective)
		read_bool_key("antialias", antialias)
		read_bool_key("show_navigator", show_navigator)
		read_dbl_key("navigator_size", navigator_size)
		read_bool3_key("show_axis", show_axis)
		read_dbl_key("axis_size", axis_size)
		read_bool_key("stereo_cam", stereo_cam)
		read_dbl_key("stereo_base_size", stereo_base_size)
		read_str_key("font", font_string)
		cout << "Error line " << line_number << ". '" << param << "' = " << value <<endl;
	}
	fb.close();
}

/** @brief add_msg_to_console
  *
  * @todo: document this function
  */
void view_camera::add_msg_to_console(const char* msg)
{
	for(unsigned int i=(console_size-1); i>0; i--)
		// TODO (slair#1#): check for infinite cycle
		console_buffer[i] = console_buffer[i-1];
	console_buffer[0] = msg;
	last_msg_added = SDL_GetTicks();
}

void view_camera::add_msg_to_console(const string msg)
{
	for(unsigned int i=(console_size-1); i>0; i--)
		// TODO (slair#1#): check for infinite cycle
		console_buffer[i] = console_buffer[i-1];
	console_buffer[0] = msg;
	last_msg_added = SDL_GetTicks();
}

/** @brief clear_console
  *
  * @todo: document this function
  */
void view_camera::clear_console()
{
	for(unsigned int i=0; i<console_size; i++)
		console_buffer[i]="";
}

/** @brief show_console
  *
  * @todo: document this function
  */
void view_camera::show_console()
{
	static int scroll = 0;
	unsigned int dur = SDL_GetTicks() - last_msg_added;
	if (dur>scroll_time)
	{
		scroll++;
		last_msg_added = SDL_GetTicks();
	}

	DBL alpha;
	for(unsigned int i=0; i<console_size; i++)
	{
		// i=10 alpha=0
		// i=0  alpha=1
		alpha = 1 - double(i)/30;

		if (alpha<0) alpha=0;

		put_text(BORDER_OFFSET, 4*font_height+BORDER_OFFSET+i*10+scroll, \
			vec3d(1)-bgcolor, alpha, console_buffer[i].c_str());

		//cout << console_buffer[i] << endl;
	}

	if (scroll>10)
	{
		scroll = 0;
		add_msg_to_console("");
	}
}

/** @brief begin_text
  *
  * @todo: document this function
  */
void view_camera::begin_text()
{
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, window_width - 1.0, 0.0, window_height - 1.0, -1.0, 100.0);
	//glTranslatef( 0.0f, 0.0f, -1.0f );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0, 0, 0);
	glDisable(GL_LIGHTING);
//	DEBUG_MSG("disable lighting")
}

/** @brief put_text
  *
  * @todo: document this function
  */
void view_camera::put_text(int x, int y, vec3d color, DBL a, const char *fmt, ...)
{
	char text[256]; /* Holds our string */
	va_list ap;     /* Pointer to our list of elements */

	/* If there's no text, do nothing */
	if ( fmt == NULL )
		return;

	/* Parses The String For Variables */
	va_start( ap, fmt );
	/* Converts Symbols To Actual Numbers */
	vsprintf( text, fmt, ap );
	va_end( ap );

	/* Pushes the Display List Bits */
	glPushAttrib( GL_LIST_BIT );

	/* Sets base character to 32 */
	glListBase( font_base - 32 );

	/* Draws the text */
	glColor4d(color[0], color[1], color[2], a);
	glRasterPos2i(x,y);
	glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );

	/* Pops the Display List Bits */
	glPopAttrib( );
}

/** @brief put_text
  *
  * @todo: document this function
  */
void view_camera::put_text_3d(vec3d pos, vec3d color, DBL a, const char *fmt, ...)
{
	char text[256]; /* Holds our string */
	va_list ap;     /* Pointer to our list of elements */

	/* If there's no text, do nothing */
	if ( fmt == NULL )
		return;

	/* Parses The String For Variables */
	va_start( ap, fmt );
	/* Converts Symbols To Actual Numbers */
	vsprintf( text, fmt, ap );
	va_end( ap );

	/* Pushes the Display List Bits */
	glPushAttrib( GL_LIST_BIT|GL_CURRENT_BIT );

	/* Sets base character to 32 */
	glListBase( font_base - 32 );

	/* Draws the text */
	glColor4d(color[0], color[1], color[2], a);
	pos.gl_rasterpos();
	glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );

	/* Pops the Display List Bits */
	glPopAttrib( );
}

/** @brief build_font
  *
  * @todo: document this function
  */
void view_camera::build_font()
{
	Display *dpy;

	/* Sotrage for 96 characters */
	font_base = glGenLists( 96 );

	/* Get our current display long enough to get the fonts */
	dpy = XOpenDisplay( NULL );

	/* Get the font information */
	fontInfo = XLoadQueryFont( dpy, font_string.c_str());

	/* If the above font didn't exist try one that should */
	if ( fontInfo == NULL )
	{
		fontInfo = XLoadQueryFont( dpy, "fixed" );
		/* If that font doesn't exist, something is wrong */
		if ( fontInfo == NULL )
		{
			fprintf( stderr, "no X font available?\n" );
			kill_font();
			SDL_Quit();
			exit(1);
		}
	}
	font_height = fontInfo->max_bounds.ascent + fontInfo->max_bounds.descent;
	/* generate the list */
	glXUseXFont( fontInfo->fid, 32, 96, font_base );

}

/** @brief kill_font
  *
  * @todo: document this function
  */
void view_camera::kill_font()
{
	Display *dpy;
	dpy = XOpenDisplay( NULL );

	if (font_base!=0)
	{
		glDeleteLists( font_base, 96 );
	}
	else
	{
		DEBUG_MSG("font_base is 0, glDeleteLists( font_base, 96 ) not called.")
	}
	/* Recover some memory */
	if (fontInfo!=NULL)
	{
		XFreeFont( dpy, fontInfo );
	}
	else
	{
		DEBUG_MSG("fontInfo is NULL, XFreeFont( dpy, fontInfo ) not called.")
	}
	/* close the display now that we're done with it */
	XCloseDisplay( dpy );
}

/** @brief rebuild_font
  *
  * @todo: document this function
  */
void view_camera::rebuild_font()
{
	kill_font();
	build_font();
}

/** @brief set_font
  *
  * @todo: document this function
  */
void view_camera::set_font(const string font_value)
{
	font_string = font_value;
	rebuild_font();
}

/** @brief rotate_around_up_nav
  *
  * @todo: document this function
  */
void view_camera::rotate_around_up_nav(DBL val)
{
	DBL m[3][3];
	vec3d cam_pos_to_nav, nav_pos;
	up.build_matrix(m, -delta_rotation*val);
	nav_pos = get_look_at();
	cam_pos_to_nav = location - nav_pos;
	cam_pos_to_nav.mult_matrix(m);
	location = nav_pos + cam_pos_to_nav;
	direction.mult_matrix(m);
	direction = direction.normalize();
	fix_right();
	if (up_fixed)
	{
		up = fixed_up;
		fix_right();
	}
}

/** @brief rotate_around_rt_nav
  *
  * @todo: document this function
  */
void view_camera::rotate_around_rt_nav(DBL val)
{
	DBL m[3][3];
	vec3d cam_pos_to_nav, nav_pos;
	right.build_matrix(m, delta_rotation*val);
	nav_pos = get_look_at();
	cam_pos_to_nav = location - nav_pos;
	cam_pos_to_nav.mult_matrix(m);
	location = nav_pos + cam_pos_to_nav;
	direction.mult_matrix(m);
	direction = direction.normalize();
	fix_up();
	if (up_fixed)
	{
		up = fixed_up;
		fix_right();
	}
}

/** @brief rotate_around_up
  *
  * @todo: document this function
  */
void view_camera::rotate_around_up(DBL val)
{
	DBL m[3][3];
	up.build_matrix(m, -delta_rotation*val);
	direction.mult_matrix(m);
	direction.normalize();
	fix_right();
	if (up_fixed)
	{
		up = fixed_up;
		fix_right();
	}
}

/** @brief rotate_around_right
  *
  * @todo: document this function
  */
void view_camera::rotate_around_right(DBL val)
{
	DBL m[3][3];
	right.build_matrix(m, delta_rotation*val);
	direction.mult_matrix(m);
	direction = direction.normalize();
	fix_up();
	if (up_fixed)
	{
		up = fixed_up;
		fix_right();
	}
}

/** @brief rotate_around_direction
  *
  * @todo: document this function
  */
void view_camera::rotate_around_direction(DBL val)
{
	DBL m[3][3];
	direction.build_matrix(m, delta_rotation*val);
	up.mult_matrix(m);
	up = up.normalize();
	fix_right();
	if (up_fixed)
	{
		up = fixed_up;
		fix_right();
		add_msg_to_console("up vector is fixed, press KEYPAD-* to toggle");
	}
}

/** @brief zoom_change
  *
  * @todo: document this function
  */
void view_camera::zoom_change(DBL val)
{
	if (perspective)
		angle += val;
	else
		ortho_size += val;
}

/** @brief move_backward
  *
  * @todo: document this function
  */
void view_camera::move_backward(int dir, DBL mod)
{
	vec3d dspeed;
	if (dir==0)
		dspeed = vec3d(delta_speed*mod, 0, 0);
	else if (dir==1)
		dspeed = vec3d(0, delta_speed*mod, 0);
	else if (dir==2)
		dspeed = vec3d(0, 0, delta_speed*mod);
	else
		dspeed = vec3d(0, 0, 0);

	if (speed[dir] > -max_speed)
		speed = speed - dspeed;

	speed.clamp_min(-max_speed);
}

/** @brief move_forward
  *
  * @todo: document this function
  */
void view_camera::move_forward(int dir, DBL mod)
{
	vec3d dspeed;
	if (dir==0)
		dspeed = vec3d(delta_speed*mod, 0, 0);
	else if (dir==1)
		dspeed = vec3d(0, delta_speed*mod, 0);
	else if (dir==2)
		dspeed = vec3d(0, 0, delta_speed*mod);
	else
		dspeed = vec3d(0, 0, 0);

	if (speed[dir] < max_speed)
		speed = speed + dspeed;

	speed.clamp_max(max_speed);
}

/** @brief move_stop
  *
  * @todo: document this function
  */
void view_camera::move_stop(int dir)
{
	vec3d dspeed;
	if (dir==0)
		dspeed = vec3d(0.5, 1, 1);
	else if (dir==1)
		dspeed = vec3d(1, 0.5, 1);
	else if (dir==2)
		dspeed = vec3d(1, 1, 0.5);
	else
		dspeed = vec3d(1, 1, 1);

	speed = speed * dspeed;
//	if (speed[dir]>0)
//	{
//		speed = speed - dspeed;
//	}
//	else
//	{
//		if (speed[dir]<0)
//		{
//			speed = speed + dspeed;
//		}
//	}

	if (fabs(speed[0])<EPSILON*1e-3)
		speed = vec3d(0, speed[1], speed[2]);
	if (fabs(speed[1])<EPSILON*1e-3)
		speed = vec3d(speed[0], 0, speed[2]);
	if (fabs(speed[2])<EPSILON*1e-3)
		speed = vec3d(speed[0], speed[1], 0);
}

/** @brief setup
  *
  * @todo: document this function
  */
void view_camera::setup(vec3d _location, vec3d _up, vec3d _look_at, DBL _angle, DBL dirlen, DBL uplen, DBL rightlen)
{
	direction_length = dirlen;
	up_length = uplen;
	right_length = rightlen;
	angle = _angle;
	location = _location;
	up = _up;
	direction = _look_at - location;
	cam_ratio = rightlen/uplen;
	distance_to_nav = direction.length();
	direction = direction.normalize();
	fix_right();
}

/** @brief set_location
  *
  * @todo: document this function
  */
void view_camera::set_location(const vec3d val)
{
	vec3d lat = get_look_at();
	location = val;
	direction = lat - location;
	distance_to_nav = direction.length();
	direction = direction.normalize();
	fix_right();
}

/** @brief set_fixed_up
  *
  * @todo: document this function
  */
void view_camera::set_fixed_up(const vec3d val)
{
	fixed_up = val;
	// TODO (slair#1#): need some camera manipulation
}

/** @brief set_up
  *
  * @todo: document this function
  */
void view_camera::set_up(const vec3d val)
{
	up = val;
	// TODO (slair#1#): need some camera manipulation
}

/** @brief set_direction
  *
  * @todo: document this function
  */
void view_camera::set_direction(const vec3d val)
{
	direction = val;
	// TODO (slair#1#): need some camera manipulation
}

/** @brief set_right
  *
  * @todo: document this function
  */
void view_camera::set_right(const vec3d val)
{
	right = val;
	// TODO (slair#1#): need some camera manipulation
}

/** @brief set_angle
  *
  * @todo: document this function
  */
void view_camera::set_angle(const SNGL val)
{
	angle = val;
}

/** @brief set_delta_rotation
  *
  * @todo: document this function
  */
void view_camera::set_delta_rotation(const DBL val)
{
	delta_rotation = val;
}

/** @brief set_delta_speed
  *
  * @todo: document this function
  */
void view_camera::set_delta_speed(const DBL val)
{
	delta_speed = val;
}

/** @brief set_axis_size
  *
  * @todo: document this function
  */
void view_camera::set_axis_size(const DBL val)
{
	axis_size = val;
}

/** @brief set_max_speed
  *
  * @todo: document this function
  */
void view_camera::set_max_speed(const DBL val)
{
	max_speed = val;
}

/** @brief set_show_axis
  *
  * @todo: document this function
  */
void view_camera::set_show_axis(const bool ax,const bool ay,const bool az)
{
	show_axis[0] = ax;
	show_axis[1] = ay;
	show_axis[2] = az;
}

/** @brief set_ortho_size
  *
  * @todo: document this function
  */
void view_camera::set_ortho_size(const SNGL val)
{
	ortho_size = val;
}

/** @brief set_distance_to_nav
  *
  * @todo: document this function
  */
void view_camera::set_distance_to_nav(const DBL val)
{
	distance_to_nav = val;
}

/** @brief look_at
  *
  * @todo: document this function
  */
void view_camera::look_at(vec3d lat)
{
	//DEBUG_MSG("view_camera::look_at("<<lat<<")")
	direction = lat - location;
	distance_to_nav = direction.length();
	direction = direction.normalize();
	fix_right();
}

/** @brief get_location
  *
  * @todo: document this function
  */
vec3d view_camera::get_location()
{
	return location;
}

/** @brief ~view_camera
  *
  * @todo: document this function
  */
 view_camera::~view_camera()
{
	save_to_config();
	delete [] console_buffer;
}

/** @brief move_nav_forward
  *
  * @todo: document this function
  */
void view_camera::move_nav_forward(DBL mod)
{
	if (nav_speed < max_speed)
		nav_speed += delta_speed*mod;
	if (nav_speed>max_speed)
		nav_speed = max_speed;
}

/** @brief move_nav_backward
  *
  * @todo: document this function
  */
void view_camera::move_nav_backward(DBL mod)
{
	if (nav_speed > -max_speed)
		nav_speed -= delta_speed*mod;
	if (nav_speed < -max_speed)
		nav_speed = -max_speed;
}

/** @brief move_stop
  *
  * @todo: document this function
  */
void view_camera::move_nav_stop()
{
	nav_speed *= 0.5;
	if (fabs(nav_speed)<EPSILON)
		nav_speed = 0;
}

/** @brief get_antialias
  *
  * @todo: document this function
  */
bool view_camera::get_antialias() const
{
	return antialias;
}

/** @brief set_antialias
  *
  * @todo: document this function
  */
void view_camera::set_antialias(bool value)
{
	antialias = value;
}

/** @brief toggle_antialias
  *
  * @todo: document this function
  */
void view_camera::toggle_antialias()
{
	set_antialias(!get_antialias());
}

void view_camera::dump_to_povray(std::ostream& os) const
{
	os << "camera{" << endl;
	if (get_perspective())
	{
		os << "\tperspective" << endl;
	}
	else
	{
		os << "\torthographic" << endl;
	}
	os << "\tlocation <" << location << ">" << endl;

	vec3d look_at = get_look_at();
	os << "\tlook_at <" << look_at << ">" << endl;

	os << "\tsky <" << up << ">" << endl;
	os << "\tright x*" << cam_ratio << endl;
	os << "\tup y"<< endl;
	os << "\tangle "<< angle << endl;
	os << "}" << endl;
}

/** @brief dump
  *
  * @todo: document this function
  */
void view_camera::dump(std::ostream& os) const
{
	#define ftab "  "
	vec3d look_at;
	look_at = location + direction*distance_to_nav;
	os<<"camera{"<<std::endl;
	os << ftab"location <" << location << ">" << std::endl;
	os << ftab"direction <" << direction << ">" << std::endl;
	os << ftab"up <" << up << ">" << std::endl;
	os << ftab"rigth <" << right << ">" << std::endl;
	os << ftab"distance_to_nav " << distance_to_nav << std::endl;
	os << ftab "angle " << angle <<std::endl;
	os<<"}"<<std::endl;
}

/** @brief get_window_width
  *
  * @todo: document this function
  */
unsigned int view_camera::get_window_width() const
{
	return window_width;
}

unsigned int view_camera::get_window_height() const
{
	return window_height;
}

void view_camera::start_animation()
{
	ani_start = SDL_GetTicks();
	animation_in_progress = true;
	set_perspective(true);
}

void view_camera::process_animation()
{
	if (!animation_in_progress) return;

	unsigned int current_animation_duration = SDL_GetTicks() - ani_start;
	//~ cout << current_animation_duration << endl;
	if (current_animation_duration<animation_duration)
	{
		vec3d delta_up = (ani_target_up - ani_start_up) / animation_duration;

		vec3d delta_fixed_up = (ani_target_fixed_up - \
			ani_start_fixed_up) / animation_duration;

		vec3d delta_location = (ani_target_location - \
			ani_start_location) / animation_duration;

		vec3d delta_direction = (ani_target_direction - \
			ani_start_direction) / animation_duration;

		up = ani_start_up + delta_up * current_animation_duration;

		//~ cout << ani_start_up << " > " << up << " > " << ani_target_up << endl;

		fixed_up = ani_start_fixed_up + \
			delta_fixed_up * current_animation_duration;

		location = ani_start_location + \
			delta_location * current_animation_duration;

		direction = ani_start_direction + \
			delta_direction * current_animation_duration;

		//~ cout << ani_start_direction << " > " << direction << " > " << ani_target_direction << endl;
	} else {
		end_animation();
	}
}

void view_camera::end_animation()
{
	animation_in_progress = false;
	up = ani_target_up;
	fixed_up = ani_target_fixed_up;
	location = ani_target_location;
	direction = ani_target_direction;
	cam_state = ani_target_state;
	if (cam_state==CS_VFUP)
	{
		set_perspective(false);
	} else if (cam_state==CS_VFFRONT) {
		set_perspective(false);
	} else if (cam_state==CS_VFRIGHT) {
		set_perspective(false);
	} else if (cam_state==CS_FREE) {
		set_perspective(true);
	}
}

void view_camera::save_free_state()
{
	free_up = up;
	free_fixed_up = fixed_up;
	free_location = location;
	free_direction = direction;
}

void view_camera::view_from_up()
{
	if (cam_state==CS_VFUP) return;
	if (cam_state==CS_FREE)
	{
		save_free_state();
	}
	if (!do_animation)
	{
		vec3d look_at = location + direction*distance_to_nav;
		up = vec3d(0, 0, 1);
		fixed_up = up;
		location = look_at + vec3d(0, 1, 0)*distance_to_nav;
		direction = (look_at-location).normalize();
		set_perspective(false);
		cam_state = CS_VFUP;
	} else {
		ani_start_up = up;
		ani_start_fixed_up = fixed_up;
		ani_start_location = location;
		ani_start_direction = direction;

		vec3d look_at = location + direction*distance_to_nav;

		ani_target_up = vec3d(0, 0, 1);
		ani_target_fixed_up = ani_target_up;
		ani_target_location = look_at + vec3d(0, 1, 0)*distance_to_nav;
		ani_target_direction = (look_at-ani_target_location).normalize();
		//~ ani_target_direction = vec3d(0, -1, 0);

		ani_target_state = CS_VFUP;
		start_animation();
	}
}

void view_camera::view_from_front()
{
	//~ vec3d look_at = location + direction*distance_to_nav;
	//~ up = vec3d(0, 1, 0);
	//~ fixed_up = up;
	//~ location = look_at - vec3d(0, 0, 1)*distance_to_nav;
	//~ direction = (look_at-location).normalize();
	//~ set_perspective(false);
	if (cam_state==CS_VFFRONT) return;
	if (cam_state==CS_FREE)
	{
		save_free_state();
	}
	if (!do_animation)
	{
		vec3d look_at = location + direction*distance_to_nav;
		up = vec3d(0, 1, 0);
		fixed_up = up;
		location = look_at - vec3d(0, 0, 1)*distance_to_nav;
		direction = (look_at-location).normalize();
		set_perspective(false);
		cam_state = CS_VFFRONT;
	} else {
		ani_start_up = up;
		ani_start_fixed_up = fixed_up;
		ani_start_location = location;
		ani_start_direction = direction;

		vec3d look_at = location + direction*distance_to_nav;

		ani_target_up = vec3d(0, 1, 0);
		ani_target_fixed_up = ani_target_up;
		ani_target_location = look_at - vec3d(0, 0, 1)*distance_to_nav;
		ani_target_direction = (look_at-ani_target_location).normalize();

		ani_target_state = CS_VFFRONT;
		start_animation();
	}
}

void view_camera::view_from_free()
{
	if (cam_state==CS_FREE) return;
	if (!do_animation)
	{
		vec3d look_at = location + direction*distance_to_nav;
		up = free_up;
		fixed_up = up;
		location = look_at - free_direction*distance_to_nav;
		direction = (look_at-location).normalize();
		set_perspective(true);
		cam_state = CS_FREE;
	} else {
		ani_start_up = up;
		ani_start_fixed_up = fixed_up;
		ani_start_location = location;
		ani_start_direction = direction;

		vec3d look_at = location + direction*distance_to_nav;

		ani_target_up =free_up;
		ani_target_fixed_up = ani_target_up;
		ani_target_location = look_at - free_direction*distance_to_nav;
		ani_target_direction = (look_at-ani_target_location).normalize();

		ani_target_state = CS_FREE;
		start_animation();
	}
}

void view_camera::view_from_right()
{
	//~ vec3d look_at = location + direction*distance_to_nav;
	//~ up = vec3d(0, 1, 0);
	//~ fixed_up = up;
	//~ location = look_at + vec3d(1, 0, 0)*distance_to_nav;
	//~ direction = (look_at-location).normalize();
	//~ set_perspective(false);
	if (cam_state==CS_VFRIGHT) return;
	if (cam_state==CS_FREE)
	{
		save_free_state();
	}
	vec3d look_at = location + direction*distance_to_nav;
	if (!do_animation)
	{
		up = vec3d(0, 1, 0);
		fixed_up = up;
		location = look_at + vec3d(1, 0, 0)*distance_to_nav;
		direction = (look_at-location).normalize();
		set_perspective(false);
		cam_state = CS_VFRIGHT;
	} else {
		ani_start_up = up;
		ani_start_fixed_up = fixed_up;
		ani_start_location = location;
		ani_start_direction = direction;

		ani_target_up = vec3d(0, 1, 0);
		ani_target_fixed_up = ani_target_up;
		ani_target_location = look_at + vec3d(1, 0, 0)*distance_to_nav;
		ani_target_direction = (look_at-ani_target_location).normalize();

		ani_target_state = CS_VFRIGHT;
		start_animation();
	}
}

/** @brief set_perspective
  *
  * @todo: document this function
  */
void view_camera::set_perspective(bool value)
{
	perspective = value;
}

/** @brief get_perspective
  *
  * @todo: document this function
  */
bool view_camera::get_perspective() const
{
	return perspective;
}

/** @brief toggle_antialias
  *
  * @todo: document this function
  */
void view_camera::toggle_perspective()
{
	set_perspective(!get_perspective());
}

/** @brief toggle_axis_x
  *
  * @todo: document this function
  */
void view_camera::toggle_axis_x()
{
	show_axis[0] = !show_axis[0];
}

/** @brief toggle_axis_y
  *
  * @todo: document this function
  */
void view_camera::toggle_axis_y()
{
	show_axis[1] = !show_axis[1];
}

/** @brief toggle_axis_z
  *
  * @todo: document this function
  */
void view_camera::toggle_axis_z()
{
	show_axis[2] = !show_axis[2];
}

/** @brief save_to_config
  *
  * @todo: document this function
  */
void view_camera::save_to_config()
{
	if (!foreign_settings)
	{
		cout << "Saving camera settings" << endl;
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
		string config_file = config_dir + CFG_FILE_CAMERA;
		if (stat(config_file.c_str(), &sb) == -1)
		{
			cout << "Creating config file '"<< config_file << "'" << endl;
			FILE *cfg = fopen(config_file.c_str(), "w");
			fclose(cfg);
		}
		fb.open(config_file.c_str(), ios::out);
		ostream os(&fb);

		os << "window_width=" << window_width << endl;
		os << "window_height=" << window_height << endl;
		os << "cam_zfar=" << cam_zfar << endl;
		os << "cam_znear=" << cam_znear << endl;
		os << "console_size=" << console_size << endl;
		os << "scroll_time=" << scroll_time << endl;
		os << "bgcolor=" << bgcolor << endl;
		os << "location=" << location << endl;
		os << "direction=" << direction << endl;
		os << "up=" << up << endl;
		os << "fixed_up=" << fixed_up << endl;
		os << "right=" << right << endl;
		os << "angle=" << angle << endl;
		os << "ortho_size=" << ortho_size << endl;
		os << "distance_to_nav=" << distance_to_nav << endl;
		os << "up_fixed=" << up_fixed << endl;
		os << "max_speed=" << max_speed << endl;
		os << "delta_speed=" << delta_speed << endl;
		os << "delta_rotation=" << delta_rotation << endl;
		os << "perspective=" << perspective << endl;
		os << "antialias=" << antialias << endl;
		os << "do_animation=" << do_animation << endl;
		os << "animation_duration=" << animation_duration << endl;
		os << "show_navigator=" << show_navigator << endl;
		os << "navigator_size=" << navigator_size << endl;
		os << "show_axis=" << show_axis[0] << ","<< show_axis[1] << ","<< show_axis[2] << endl;
		os << "axis_size=" << axis_size << endl;
		os << "stereo_cam=" << stereo_cam << endl;
		os << "stereo_base_size=" << stereo_base_size << endl;
		os << "font=" << font_string << endl;

		fb.close();
	} // if (!foreign_settings)
}

/** @brief get_up_fixed
  *
  * @todo: document this function
  */
bool view_camera::get_up_fixed() const
{
	return up_fixed;
}

/** @brief set_up_fixed
  *
  * @todo: document this function
  */
void view_camera::set_up_fixed(const bool value)
{
	up_fixed = value;
	if (value)
		fixed_up = up;
}

/** @brief toggle_up_fixed
  *
  * @todo: document this function
  */
void view_camera::toggle_up_fixed()
{
	set_up_fixed(!get_up_fixed());
	if (get_up_fixed())
	{
		add_msg_to_console(tr("vector up fixed"));
	} else {
		add_msg_to_console(tr("vector up free"));
	}
}

/** @brief set_show_navigator
  *
  * @todo: document this function
  */
void view_camera::set_show_navigator(const bool value)
{
	show_navigator = value;
}

/** @brief get_show_navigator
  *
  * @todo: document this function
  */
bool view_camera::get_show_navigator() const
{
	return show_navigator;
}

/** @brief toggle_show_navigator
  *
  * @todo: document this function
  */
void view_camera::toggle_show_navigator()
{
	set_show_navigator(!get_show_navigator());
}

/** @brief set_navigator_size
  *
  * @todo: document this function
  */
void view_camera::set_navigator_size(const DBL val)
{
	navigator_size = val;
}

/** @brief get_navigator_size
  *
  * @todo: document this function
  */
DBL view_camera::get_navigator_size() const
{
	return navigator_size;
}

/** @brief change_navigator_size
  *
  * @todo: document this function
  */
void view_camera::change_navigator_size(DBL val)
{
	DBL tmp = get_navigator_size();
	tmp += val;
	if (tmp<0.01)
		tmp = 0.01;
	set_navigator_size(tmp);
}

/** @brief toggle_stereo_cam
  *
  * @todo: document this function
  */
void view_camera::toggle_stereo_cam()
{
	set_stereo_cam(!get_stereo_cam());
}

/** @brief setup_viewport
  *
  * @todo: document this function
  */
void view_camera::setup_viewport(int vp_num)
{
	if (vp_num==1)
	{
		glViewport( window_width/2+1, 0, window_width/2-2, window_height);
		return;
	}
	if (vp_num==0)
	{
		glViewport( 0, 0, window_width/2, window_height);
		return;
	}
	if (vp_num==-1)
	{
		glViewport( 0, 0, window_width, window_height);
		return;
	}
}

/** @brief change_bg
  *
  * @todo: document this function
  */
void view_camera::change_bg(const DBL value)
{
	bgcolor = bgcolor + value;
	bgcolor.clamp_min(0);
	bgcolor.clamp_max(1);
}

/** @brief clear_gl_buffer
  *
  * @todo: document this function
  */
void view_camera::clear_gl_buffer() const
{
	bgcolor.gl_clear_color();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/** @brief reset_bg
  *
  * @todo: document this function
  */
void view_camera::reset_bg()
{
	bgcolor = vec3d(0.847059, 0.784314, 0.752941);
}

