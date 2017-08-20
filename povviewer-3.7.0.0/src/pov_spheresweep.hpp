#ifndef _POV_SPHERESWEEP_HPP_
#define _POV_SPHERESWEEP_HPP_

class pov_baseobject;
class pov_scene;

/* Maximum number of coefficients of the polynomials describing one segment */
#define SPH_SWP_MAX_COEFS               4
const DBL DEPTH_TOLERANCE       = 1.0e-6;

typedef struct Sphere_Sweep_Sphere_Struct SPHSWEEP_SPH;
typedef struct Sphere_Sweep_Segment_Struct SPHSWEEP_SEG;

/* Single sphere, used to connect two adjacent segments */
struct Sphere_Sweep_Sphere_Struct
{
	vec3d		Center;
	DBL			Radius;
};

/* One segment of the sphere sweep */
struct Sphere_Sweep_Segment_Struct
{
	SPHSWEEP_SPH	Closing_Sphere[2];							/* Spheres closing the segment   */
	vec3d					Center_Deriv[2];								/* Derivatives of center funcs for 0 and 1   */
	DBL						Radius_Deriv[2];								/* Derivatives of radius funcs for 0 and 1   */
	int						Num_Coefs;											/* Number of coefficients        */
	vec3d					Center_Coef[SPH_SWP_MAX_COEFS];	/* Coefs of center polynomial    */
	DBL						Radius_Coef[SPH_SWP_MAX_COEFS];	/* Coefs of radius polynomial    */
};

class pov_spheresweep : public pov_baseobject
{
	protected:
		void tesselate() const;
	public:
		int						Interpolation;
		int						Num_Modeling_Spheres;
		SPHSWEEP_SPH	*Modeling_Sphere;			/* Spheres describing the shape  */
		int						Num_Spheres;					/* Number of single spheres      */
		SPHSWEEP_SPH	*Sphere;							/* Spheres that close segments   */
		int						Num_Segments;					/* Number of tubular segments    */
		SPHSWEEP_SEG	*Segment;							/* Tubular segments              */
		DBL						Depth_Tolerance;			/* Preferred depth tolerance     */
	
		pov_spheresweep(pov_scene* s);
		~pov_spheresweep();
		int read(FILE *f);
};

#endif
