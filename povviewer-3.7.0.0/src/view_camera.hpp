#ifndef _VIEW_CAMERA_HPP_
#define _VIEW_CAMERA_HPP_

#include "GL/gl.h"
#include <GL/glx.h>

#include <string>
using namespace std;

#include "vec3d.hpp"

#define CS_VFUP 7
#define CS_VFFRONT 1
#define CS_VFRIGHT 3
#define CS_FREE 9

class pov_scene;
class SDL_Surface;
class view_camera
{
	protected:
		pov_scene* scene;
		string* console_buffer;
		SDL_Surface* screen;
		XFontStruct* fontInfo;

		int video_flags;

		vec3d location;
		vec3d direction;
		vec3d up;
		vec3d right;
		DBL direction_length, up_length, right_length;
		SNGL ortho_size;
		SNGL angle;
		SNGL cam_ratio;
		SNGL cam_zfar;
		SNGL cam_znear;
		vec3d fixed_up;
		DBL distance_to_nav;

		vec3d bgcolor;
		bool antialias;
		bool perspective;
		bool up_fixed;
		bool show_navigator;
		bool navigator_ontop;
		bool hud_visible;

		// axis
		bool show_axis[3];
		DBL axis_size;

		// some stereo
		bool stereo_cam;
		DBL stereo_base_size;

		bool show_grid_plane[3];
		DBL grid_size;
		DBL grid_step;

		DBL navigator_size;
		unsigned int window_width, window_height;
		vec3d speed;
		DBL nav_speed;
		DBL max_speed;
		DBL delta_speed;
		DBL delta_rotation;
		unsigned int console_size;
		unsigned int scroll_time;
		unsigned int current_time_msec, start_time_msec, frame_count;
		DBL time_sec, fps;
		unsigned int last_msg_added;
		int resize_skip;

		unsigned int cam_state;	// view from up, right, front, free

		/** @todo: init and save_to_config */
		bool do_animation;					// use animation in some cases
		/** @todo: init and save_to_config */
		unsigned int animation_duration;	// duration of animations (msec)

		bool animation_in_progress;
		vec3d ani_target_up;
		vec3d ani_target_fixed_up;
		vec3d ani_target_location;
		vec3d ani_target_direction;
		vec3d ani_start_up;
		vec3d ani_start_fixed_up;
		vec3d ani_start_location;
		vec3d ani_start_direction;
		vec3d free_up;				// savings for free state
		vec3d free_fixed_up;		// for return when SDL_KP9 pressed
		vec3d free_location;
		vec3d free_direction;
		unsigned int ani_target_state;
		unsigned int ani_start;

		string font_string;
		unsigned int font_height;
		GLuint font_base;
		void kill_font();
		void build_font();
		void rebuild_font();
		void put_text(int x, int y, vec3d color, DBL a, const char *fmt, ...);
		void begin_text();

		void load_from_config();
		void save_to_config();
		void setup_perspective(int cam_num);
		void setup_ortho();
		void fix_up();
		void fix_right();
		void fix_direction();
		void process();
		void count_fps();
		void save_free_state();
	public:
		void start_animation();
		void process_animation();
		void end_animation();
		void set_resize_skip(const int val);
		int get_resize_skip();
		void set_font(const string font_value);
		void set_cam_zfar(const SNGL new_value);
		void set_cam_znear(const SNGL new_value);
		void set_bgcolor(const vec3d new_value);
		void set_show_axis(const bool ax,const bool ay,const bool az);
		void set_max_speed(const DBL val);
		void set_axis_size(const DBL val);
		void set_delta_speed(const DBL val);
		void set_delta_rotation(const DBL val);
		bool foreign_settings;
		view_camera();
		~view_camera();
		void reset_config();
		vec3d get_look_at() const;
		vec3d get_location();
		void set_location(const vec3d val);
		void set_direction(const vec3d val);
		void set_up(const vec3d val);
		void set_fixed_up(const vec3d val);
		void set_right(const vec3d val);
		void set_angle(const SNGL val);
		void set_ortho_size(const SNGL val);
		void set_distance_to_nav(const DBL val);
		bool init_opengl();
		void calc_cam_ratio();
		bool resize_window();
		void shutdown_opengl();
		void begin_frame(int cam_num);
		void end_frame();
		void set_scene(pov_scene* s);
		void add_msg_to_console(const char* msg);
		void add_msg_to_console(const string msg);
		void clear_console();
		void show_console();
		void rotate_around_right(DBL val);
		void rotate_around_up(DBL val);
		void rotate_around_direction(DBL val);
		void rotate_around_rt_nav(DBL val);
		void rotate_around_up_nav(DBL val);
		void zoom_change(DBL val);
		void move_forward(int dir, DBL mod);
		void move_backward(int dir, DBL mod);
		void move_stop(int dir);
		void move_nav_forward(DBL mod);
		void move_nav_backward(DBL mod);
		void move_nav_stop();

		void setup(vec3d _location, vec3d _up, vec3d _look_at, DBL _angle, \
			DBL dirlen, DBL uplen, DBL rightlen);

		bool get_antialias() const;
		void set_antialias(bool value);
		void toggle_antialias();
		bool get_perspective() const;
		void set_perspective(bool value);
		void toggle_perspective();
		void dump_to_povray(ostream& os) const;
		void dump(ostream& os) const;
		unsigned int get_window_width() const;
		unsigned int get_window_height() const;
		void set_window_width(const unsigned int w);
		void set_window_height(const unsigned int h);
		void view_from_free();
		void view_from_up();
		void view_from_front();
		void view_from_right();
		void toggle_axis_x();
		void toggle_axis_y();
		void toggle_axis_z();
		void put_text_3d(vec3d pos, vec3d color, DBL a, const char *fmt, ...);
		bool get_up_fixed() const;
		void set_up_fixed(const bool value);
		void toggle_up_fixed();
		bool get_show_navigator() const;
		void set_show_navigator(const bool value);
		void toggle_show_navigator();
		void look_at(vec3d lat);
		DBL get_navigator_size() const;
		void set_navigator_size(const DBL val);
		void change_navigator_size(DBL val);
		bool get_stereo_cam() const;
		void set_stereo_cam(const bool value);
		DBL get_stereo_base_size() const;
		void set_stereo_base_size(const DBL value);
		void toggle_stereo_cam();
		void setup_viewport(int vp_num);
		void change_bg(const DBL value);
		void clear_gl_buffer() const;
		void reset_bg();
};

#endif
