#ifndef _POV_TRIANGLE_HPP_
#define _POV_TRIANGLE_HPP_

class pov_baseobject;

class pov_triangle : public pov_baseobject
{
	protected:
		vec3d pts[3];
		vec3d normal_vector;
		void tesselate() const;
	public:
		pov_triangle(pov_scene* s);
		~pov_triangle();
		int read(FILE *f);
};

/*class pov_smoothtriangle : public pov_triangle
{
	protected:
		vec3d normals[3];
		void tesselate() const;
	public:
		pov_triangle(pov_scene* s);
		~pov_triangle();
		int read(FILE *f);
};*/

#endif
