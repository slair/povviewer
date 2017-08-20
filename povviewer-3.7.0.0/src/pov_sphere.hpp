#ifndef _POV_SPHERE_HPP_
#define _POV_SPHERE_HPP_

#include "global_defs.hpp"
#include "pov_baseobject.hpp"

class pov_baseobject;

class pov_sphere : public pov_baseobject
{
	protected:
		vec3d center;
		DBL radius;
		void fix_sphere();
		void tesselate() const;
	public:
		pov_sphere(pov_scene* s);
		~pov_sphere();
		int read(FILE *f);
};

#endif
