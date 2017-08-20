#ifndef _POV_CAMERA_HPP_
#define _POV_CAMERA_HPP_

#include "vec3d.hpp"
#include "pov_baseobject.hpp"

class pov_baseobject;
class pov_scene;

class pov_camera : public pov_baseobject
{
	protected:
		vec3d location, direction, up, right, sky, look_at;
		int type;
		DBL angle, h_angle, v_angle;
		void tesselate() const;
	public:
		pov_camera(pov_scene* s);
		~pov_camera();
		vec3d get_location();
		vec3d get_sky();
		vec3d get_look_at();
		vec3d get_direction();
		vec3d get_up();
		vec3d get_right();
		DBL get_angle();
		unsigned int get_type() const;
		int read(FILE *f);
		void dump(std::ostream& os) const;
		void draw() const;
};
////////////////////////////////////////////////////////////////////////////////
#endif
