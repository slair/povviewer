#ifndef _POV_LIGHTSOURCE_HPP_
#define _POV_LIGHTSOURCE_HPP_

#include <stdio.h>

#include "vec3d.hpp"

class pov_baseobject;

class pov_lightsource : public pov_baseobject
{
	protected:
		vec3d position, direction, points_at, axis1, axis2;
		vec3d light_color;
		SNGL diffuse_color[4], light_pos[4];
		unsigned char type;
		DBL coeff, radius, fallof, fade_distance, fade_power;
		DBL light_size_center;
		int area_size1, area_size2;
		unsigned char area_light, parallel;
		int id;
		bool placed;

	public:
		pov_lightsource(pov_scene* s, int num);
		~pov_lightsource();
		void place_light();
		void remove_light();
		void update();
		int read(FILE *f);
		vec3d get_position() const;
};

#endif
