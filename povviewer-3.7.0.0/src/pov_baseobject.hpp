#ifndef _POV_BASEOBJECT_HPP_
#define _POV_BASEOBJECT_HPP_

#include <stdio.h>

#include "global_defs.hpp"
#include "vec3d.hpp"
#include "line3d.hpp"

struct GLOBJ_STATS;

class pov_transform;
class pov_scene;
class gl_object;
class vec3d;

class pov_baseobject
{
	protected:
		pov_transform *transform;
		SNGL bbox_corners[2][3];
		vec3d color;
		vec3d inner_color;
		SNGL colour[5];
		DBL min_vc;
		vec3d min_visual_color;
		pov_scene* scene;
		DBL point_size;
		DBL point_scale;
		gl_object *globj;
		void fix_inner_color();
		bool is_blob;
		bool is_smoothed;
	public:
		bool is_sphere;
		vec3d sphere_center;
		unsigned int tag;
		pov_baseobject(pov_scene* s);
		virtual ~pov_baseobject();
		void set_scene(pov_scene* s);
		void update() const;
		void draw_bbox() const;
		void begin_dots() const;
		void end_dots() const;
		int read(FILE *f);
		vec3d get_scale_vector() const;
		void generate_gllist(unsigned int mode);
		GLOBJ_STATS get_stats() const;
		void set_smoothed(const bool value);
		pov_scene* get_scene();
		vec3d get_center() const;
		std::string get_type() const;
		void get_lines(vector_line3d* lines) const;
		void get_lines(set_line3d* lines) const;
};
#endif
