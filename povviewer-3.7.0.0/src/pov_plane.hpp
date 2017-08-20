#ifndef _POV_PLANE_HPP_
#define _POV_PLANE_HPP_

class pov_baseobject;
class pov_scene;

class pov_plane : public pov_baseobject
{
	protected:
		vec3d normal;
		DBL distance;
		void tesselate() const;
	public:
		pov_plane(pov_scene* s);
		~pov_plane();
		int read(FILE *f);
};

#endif
