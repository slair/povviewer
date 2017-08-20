#ifndef _POV_BICUBICPATCH_HPP_
#define _POV_BICUBICPATCH_HPP_

#include "GL/gl.h"

class pov_baseobject;
class pov_scene;

/*
struct PATCH {
	GLfloat data[4][4][3];
	PATCH();
};
*/

class pov_bicubicpatch : public pov_baseobject
{
	protected:
		vec3d patch[4][4];
		void tesselate();
		vec3d bernstein(float u, vec3d *p);
		void triangulate_patch(unsigned int divs);
	public:
		pov_bicubicpatch(pov_scene* s);
		~pov_bicubicpatch();
		int read(FILE *f);
};

#endif
