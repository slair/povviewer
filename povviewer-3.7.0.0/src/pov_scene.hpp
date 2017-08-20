#ifndef _POV_SCENE_HPP_
#define _POV_SCENE_HPP_

#include "GL/gl.h"
#include <GL/glu.h>
#include <GL/glx.h>
#include <string>
#include <vector>
using namespace std;

#include "global_defs.hpp"
#include "vec3d.hpp"

class view_camera;
class pov_baseobject;
class pov_camera;
class pov_lightsource;
class pov_cone;

struct SCENE_STATS
{
	unsigned int objects_count;
	unsigned int vertices_count;
//	unsigned int normals_count;
	unsigned int faces_count;
	unsigned int contours_count;
//	unsigned int patches_count;
	unsigned int contour_vertices_count;
	unsigned int vec3d_count;
};

class pov_scene
{
	protected:
		#define NUMBER_OF_GLLISTS 6
		GLuint gl_lists[NUMBER_OF_GLLISTS]; // bbox dots lines normals planes solid
		bool show_elem[NUMBER_OF_GLLISTS];
		view_camera* camera;
		pov_camera* povcamera;
		bool show_camera;
		DBL frustum_size;
		string scene_filename;
		bool use_povray_camera;
		bool smooth_obj;
		unsigned int lights_count;
		unsigned int max_lights;
		vector<pov_baseobject*> objects;
		pov_lightsource** lights;
		unsigned int objects_quality;
		DBL scene_clock;
		DBL normals_size;
		DBL smooth_angle;
		// tesselation parameters
		bool foreign_settings;
		unsigned int isoresolution[3], quadresolution[3], blobresolution[3];
		unsigned int patchresolution, spheresweepresolution;
		unsigned int cone_num_latitudes, cone_num_longitudes;
		unsigned int sphere_num_latitudes, sphere_num_longitudes;
		unsigned int lathe_spline_steps, lathe_num_longitudes;
		unsigned int prism_spline_steps;
		DBL plane_radius, plane_step, plane_alpha;
		unsigned int torus_usteps, torus_vsteps;
		unsigned int disc_usteps, disc_vsteps;

		int ino_fd; // for inotify
		void clear();
		void dump_cfg(const char *tmpcfg);
		void update();
		void update_gllist(unsigned int mode);
		void reset_config();
		void load_from_config();
		void save_to_config();
		void scan_scene_file(const string scene_filename);
		void allocate_lights();
		void free_lights();
	public:
		bool parse_error;
		bool show_errors;
		string error_string[MAX_ERR_STRINGS];
		unsigned int err_count;
		DBL parse_time, load_time;
		unsigned int objects_count;
		pov_scene(string scene_fn, view_camera* cam);
		~pov_scene();
		void set_caption();
		void load();
		bool init_inotify();
		void check_inotify_and_load(volatile bool& ls);
		void shutdown_inotify();
		void draw();
		void setup_camera_from_povcamera();
		void aim_camera_to_nearest_object();
		bool lights_present();
		void dump_scene_to_dxf();
		// some gets methods
		unsigned int get_cone_num_latitudes() const;
		unsigned int get_cone_num_longitudes() const;
		unsigned int get_sphere_num_latitudes() const;
		unsigned int get_sphere_num_longitudes() const;
		unsigned int get_lathe_spline_steps() const;
		unsigned int get_lathe_num_longitudes() const;
		unsigned int get_prism_spline_steps() const;
		unsigned int get_patch_resolution() const;
		unsigned int get_sphere_sweep_resolution() const;
		void set_patch_resolution(const unsigned int val);
		void set_sphere_sweep_resolution(const unsigned int val);
		DBL get_normals_size() const;
		bool get_show_bbox() const;
		bool get_show_dots() const;
		bool get_show_lines() const;
		bool get_show_solid() const;
		bool get_show_normals() const;
		bool get_use_povray_camera() const;
		bool get_smooth_obj() const;
		bool get_show_camera() const;
		void set_show_camera(const bool val);
		void toggle_show_camera();
		DBL get_frustum_size() const;
		void set_frustum_size(const DBL val);
		DBL get_plane_radius() const;
		double get_clock() const;
		unsigned int get_plane_steps() const;
		unsigned int get_torus_usteps() const;
		unsigned int get_torus_vsteps() const;
		unsigned int get_disc_usteps() const;
		unsigned int get_disc_vsteps() const;
		SCENE_STATS get_stats() const;
		view_camera* get_camera();
		DBL get_smooth_angle() const;
		DBL get_plane_alpha() const;
		// set methods
		void set_show_bbox(bool value);
		void set_show_dots(bool value);
		void set_show_lines(bool value);
		void set_show_solid(bool value);
		void set_show_normals(bool value);
		void set_use_povray_camera(const bool value);
		void set_smooth_obj(const bool value);
		void change_clock(const double value);
		// toggle functions
		void toggle_show_bbox();
		void toggle_show_dots();
		void toggle_show_lines();
		void toggle_show_solid();
		void toggle_show_normals();
		void toggle_show_errors();
		void make_pov_screenshot();
		void toggle_use_povray_camera();
		void toggle_smooth_obj();
		//
		void print_stats(std::ostream& os) const;
		void clear_errors();
		void add_error(const string msg);
		void refresh_screen();
		void begin_dxf_stream(std::ostream& dxf_stream);
		void end_dxf_stream(std::ostream& dxf_stream);
};

#endif
