#ifndef _POV_DISC_HPP_
#define _POV_DISC_HPP_

class pov_baseobject;
class pov_scene;

class pov_disc : public pov_baseobject
{
	protected:
		vec3d position;
		vec3d normal;
		DBL radiuses[2];
		void tesselate() const;
	public:
		pov_disc(pov_scene* s);
		~pov_disc();
		int read(FILE *f);
};

#endif
