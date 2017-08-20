#ifndef _POV_QUADRIC_HPP_
#define _POV_QUADRIC_HPP_

class pov_baseobject;
class pov_scene;

class pov_quadric : public pov_baseobject
{
	protected:
		int tri_count;
	public:
		pov_quadric(pov_scene* s);
		~pov_quadric();
		int read(FILE *f);
};

#endif
