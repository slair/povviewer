#ifndef _POV_ISOSURFACE_HPP_
#define _POV_ISOSURFACE_HPP_

class pov_baseobject;

class pov_isosurface : public pov_baseobject
{
	protected:
		void tesselate() const;
	public:
		//=========================================================================================
		//                                             |x1 y1 z1|   |a[0] a[1] a[2]|
  	// В E^3: ((x1,y1,z1),(x2,y2,z2),(x3,y3,z3)) = |x2 y2 z2| = |b[0] b[1] b[2]|
		//                                             |x3 y3 z3|   |c[0] c[1] c[2]|
		// a[0]*b[1]*c[2] + a[1]*b[2]*c[0] + b[0]*c[1]*a[3] - c[0]*b[1]*a[2] - a[1]*b[0]*c[2] - a[0]*b[2]*c[1]
		pov_isosurface(pov_scene* s);
		~pov_isosurface();
		int read(FILE *f);
};

#endif
