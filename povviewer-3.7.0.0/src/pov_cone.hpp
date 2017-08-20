#ifndef _POV_CONE_HPP_
#define _POV_CONE_HPP_

#include "global_defs.hpp"
#include "pov_baseobject.hpp"
#include "vec3d.hpp"

class pov_baseobject;

class pov_cone : public pov_baseobject
{
	protected:
		vec3d base, apex;
		DBL base_radius, apex_radius, dist;
		void tesselate() const;
	public:
		pov_cone(pov_scene* s);
		~pov_cone();
		int read(FILE *f);
};

#endif
