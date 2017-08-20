#ifndef _POV_BOX_HPP_
#define _POV_BOX_HPP_

class pov_baseobject;
class pov_scene;

class pov_box : public pov_baseobject
{
	protected:
		vec3d corner1;
		vec3d corner2;
		void tesselate() const;
	public:
		pov_box(pov_scene* s);
		~pov_box();
		int read(FILE *f);
};

#endif
