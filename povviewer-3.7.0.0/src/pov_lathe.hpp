#ifndef _POV_LATHE_HPP_
#define _POV_LATHE_HPP_

#define LINEAR_SPLINE 1
#define QUADRATIC_SPLINE 2
#define CUBIC_SPLINE 3
#define BEZIER_SPLINE 4

#define LINEAR_SWEEP 1

class pov_baseobject;
class vec2d;

class pov_lathe : public pov_baseobject
{
	protected:
		unsigned int spline_type, number_of_splines;
		DBL radiuses[2], heights[2];
		DBL *spline;
		void calculate_spline(vec2d* points, unsigned int index) const;
		void tesselate() const;
	public:
		pov_lathe(pov_scene* s);
		~pov_lathe();
		int read(FILE *f);
};

#endif
