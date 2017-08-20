#ifndef _gl_object_HPP_
#define _gl_object_HPP_

#include "GL/gl.h"
#include <vector>
using namespace std;

#include "global_defs.hpp"
#include "vec3d.hpp"
#include "line3d.hpp"

class vec3d;
class line3d;
class pov_baseobject;
class pov_transform;

struct GLOBJ_STATS
{
	unsigned int vertices_count;
	unsigned int faces_count;
	unsigned int contours_count;
	unsigned int contour_vertices_count;
//	unsigned int patches_count;
};

struct IFACE {
	unsigned int numverts;
	int vi[4];
	int ni[4];
	int normal;
	int center;
	IFACE();
};

typedef std::vector<vec3d> array_vec3d;
typedef std::vector<array_vec3d> array_array_vec3d;
typedef std::vector<IFACE> array_iface;

//typedef std::vector<PATCH> array_patch;

#define MAX_FACES 1000

class gl_object
{
	protected:
		pov_baseobject* parent;
//		array_vec3d vertices;
//		array_vec3d normals;
		array_iface faces;
		array_array_vec3d contours;
		array_vec3d contour_normals;
//		array_patch patches;

		int add_vertex(vec3d v);
		int add_normal(vec3d v);
		void calc_center(IFACE *f);
		void calc_normal(IFACE *f);
		bool tris_present;
		bool quads_present;
		bool contours_present;
		bool lines_present;
//		bool patches_present;
		unsigned int find_faces(int face_index, unsigned int our_face, int *array_faces);
//		void show_patch(PATCH a) const;
//		void show_tri_patch(PATCH a, int divs) const;
		vec3d sum_face_centers;
	public:
		gl_object(pov_baseobject* p);
		~gl_object();
		vec3d get_object_center() const;
		void append_line(vec3d a, vec3d b);
		void append_face(vec3d a, vec3d b, vec3d c);
		void append_face_with_normal(vec3d a, vec3d na, vec3d b, vec3d nb, vec3d c, vec3d nc);
		void append_face(vec3d a, vec3d b, vec3d c, vec3d d);
		void append_contour(array_vec3d contour, vec3d normal);
		void append_patch(const vec3d src[4][4]);
		void smooth(const DBL max_angle);
		void show_flat_faces();
		void show_smooth_faces();
		void show_normals(const DBL size);
		void show_smooth_normals(const DBL size);
		void show_dots() const;
		void show_lines() const;
		void show_lines_with_faces() const;
		void show_contours() const;
		void show_line_contours() const;
		//void dump_dxf3d(std::ostream& os) const;
//		void show_patches() const;
		GLOBJ_STATS get_globj_stats() const;
		void get_lines(vector_line3d* lines, pov_transform* transform) const;
		void get_lines(set_line3d* lines, pov_transform* transform) const;
};

#endif
