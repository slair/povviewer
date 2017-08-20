#ifndef _POV_TORUS_HPP_
#define _POV_TORUS_HPP_

class pov_baseobject;
class pov_scene;

class pov_torus : public pov_baseobject
{
	protected:
		DBL major, minor;
		void tesselate() const;
	public:
		pov_torus(pov_scene* s);
		~pov_torus();
		int read(FILE *f);
};

#endif
