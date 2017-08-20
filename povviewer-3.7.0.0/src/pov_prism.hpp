#ifndef _POV_PRISM_HPP_
#define _POV_PRISM_HPP_

#define LINEAR_SWEEP 1

class pov_baseobject;
class vec2d;
class vec3d;

class pov_prism : public pov_baseobject
{
	protected:
		unsigned int number_of_spline;
		unsigned int spline_type;
		unsigned int sweep_type;
		DBL heights[2];
		DBL *spline;
		void calculate_spline(vec2d* points, unsigned int index);
		void calculate_spline3dv(vec2d* points, unsigned int index, DBL val, vec3d* p3d);
		void tesselate();
	public:
		pov_prism(pov_scene* s);
		~pov_prism();
		int read(FILE *f);
};

#endif
