#ifndef _POV_TTF_HPP_
#define _POV_TTF_HPP_

#include <vector>

class pov_baseobject;
class vec2d;

typedef std::vector<vec2d> array_vec2d;
typedef std::vector<array_vec2d> array_array_vec2d;

class pov_ttf : public pov_baseobject
{
	protected:
		DBL depth;
		array_array_vec2d contours;
		void tesselate() const;
	public:
		pov_ttf(pov_scene* s);
		~pov_ttf();
		int read(FILE *f);
};

#endif
