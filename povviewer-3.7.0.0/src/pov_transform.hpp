#ifndef _POV_TRANSFORM_HPP_
#define _POV_TRANSFORM_HPP_

#include <iostream>

#include "global_defs.hpp"

class vec3d;

class pov_transform
{
	protected:
		DBL *matrix;
	public:
		pov_transform();
		~pov_transform();
		void apply();
		vec3d transform_vec3d(const vec3d v);
		int read(FILE *f);
		void remove_translation();
		void set_translation(const vec3d v);
		void clear();
		vec3d get_scale() const;
		void print(std::ostream& output) const;
};

#endif
