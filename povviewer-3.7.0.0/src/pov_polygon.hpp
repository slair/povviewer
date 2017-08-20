#ifndef _POV_POLYGON_HPP_
#define _POV_POLYGON_HPP_

class pov_baseobject;
class pov_scene;

class pov_polygon : public pov_baseobject
{
	protected:
		vec3d normal;
		unsigned int number_of_points;
		vec3d *points;
		void tesselate() const;
	public:
		pov_polygon(pov_scene* s);
		~pov_polygon();
		int read(FILE *f);
};

#endif
