#include "common_funcs.hpp"
#include "pov_baseobject.hpp"
#include "pov_spheresweep.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_spheresweep::pov_spheresweep(pov_scene* s):pov_baseobject(s)
{
	tag = 0x57535053; // SPSW
	
	Interpolation = -1;
	
	Num_Modeling_Spheres = 0;
	Modeling_Sphere = NULL;
	
	Num_Spheres = 0;
	Sphere = NULL;
	
	Num_Segments = 0;
	Segment = NULL;
	
	Depth_Tolerance = DEPTH_TOLERANCE;
}


pov_spheresweep::~pov_spheresweep()
{
	delete [] Modeling_Sphere;
	delete [] Sphere;
	delete [] Segment;
}

int pov_spheresweep::read(FILE *f)
{
	int readden = pov_baseobject::read(f);

	fread(&Interpolation, sizeof(Interpolation), 1, f);
	readden += sizeof(Interpolation);

	fread(&Num_Modeling_Spheres, sizeof(Num_Modeling_Spheres), 1, f);
	readden += sizeof(Num_Modeling_Spheres);

	Modeling_Sphere = new SPHSWEEP_SPH[Num_Modeling_Spheres];
	fread(Modeling_Sphere, sizeof(SPHSWEEP_SPH), Num_Modeling_Spheres, f);
	readden += sizeof(SPHSWEEP_SPH) * Num_Modeling_Spheres;

	fread(&Num_Spheres, sizeof(Num_Spheres), 1, f);
	readden += sizeof(Num_Spheres);

	Sphere = new SPHSWEEP_SPH[Num_Spheres];
	fread(Sphere, sizeof(SPHSWEEP_SPH), Num_Spheres, f);
	readden += sizeof(SPHSWEEP_SPH) * Num_Spheres;

	fread(&Num_Segments, sizeof(Num_Segments), 1, f);
	readden += sizeof(Num_Segments);

	Segment = new SPHSWEEP_SEG[Num_Segments];
	fread(Segment, sizeof(SPHSWEEP_SEG), Num_Segments, f);
	readden += sizeof(SPHSWEEP_SEG) * Num_Segments;

	fread(&Depth_Tolerance, sizeof(Depth_Tolerance), 1, f);
	readden += sizeof(Depth_Tolerance);
	
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_spheresweep::tesselate() const
{
	int numlongitudes = scene->get_sphere_num_longitudes();
	int numlatitudes = scene->get_sphere_num_latitudes();
	int spheresweepresolution = scene->get_sphere_sweep_resolution();
	//~ int spheresweepresolution = 6;
	vec3d *p = new vec3d[spheresweepresolution];
	//~ for(int i=0; i<Num_Modeling_Spheres; i++)
	//~ {
		//~ create_sphere(Modeling_Sphere[i].Center, Modeling_Sphere[i].Radius, numlongitudes, numlatitudes, globj);
	//~ }

	//~ for(int i=0; i<Num_Spheres; i++)
	//~ {
		//~ create_sphere(Sphere[i].Center, Sphere[i].Radius, numlongitudes, numlatitudes, globj);
	//~ }

	DBL t=double(1.0) / double(spheresweepresolution);
	
	vec3d p1, p2;
	for(int i=0; i<Num_Segments; i++)
	{
		create_sphere(Segment[i].Closing_Sphere[0].Center, Segment[i].Closing_Sphere[0].Radius, numlongitudes, numlatitudes, globj);
		create_sphere(Segment[i].Closing_Sphere[1].Center, Segment[i].Closing_Sphere[1].Radius, numlongitudes, numlatitudes, globj);
		p1 = Segment[i].Closing_Sphere[0].Center;
		p2 = Segment[i].Closing_Sphere[1].Center;
		globj->append_line(p1, p2);
		DBL r = 0;
		for(int step=0; step<spheresweepresolution; step++)
		{
			switch(Segment[i].Num_Coefs)
			{
				case 2:
					p[step] = vec3d(0);
					p[step] = p[step] /*+ Segment[i].Center_Coef[1] * r*/ + Segment[i].Center_Deriv[0] * (1-r); // (Segment[i].Center_Deriv[1] - Segment[i].Center_Deriv[0]);
					//~ p[step] = p[step] + Segment[i].Center_Coef[1] * r; // (Segment[i].Center_Deriv[1] - Segment[i].Center_Deriv[0]);
					p[step] = p[step] /*+ Segment[i].Center_Coef[0]*/; // (Segment[i].Center_Deriv[1] - Segment[i].Center_Deriv[0]);
				break;
				case 4:
					p[step] = vec3d(0);
				/*
					p[step] = p[step] + Segment[i].Center_Coef[3] * r*r*r;
					p[step] = p[step] + Segment[i].Center_Coef[2] * r*r;
					p[step] = p[step] + Segment[i].Center_Coef[1] * r;
					p[step] = p[step] + Segment[i].Center_Coef[0];
				*/
					p[step] = p[step] + Segment[i].Center_Coef[3] * r*r*r;// + Segment[i].Center_Deriv[0] * (1-r)*(1-r)*(1-r);
					p[step] = p[step] + Segment[i].Center_Coef[2] * r*r;// + Segment[i].Center_Deriv[0] * (1-r)*(1-r);
					p[step] = p[step] + Segment[i].Center_Coef[1] * r;// + Segment[i].Center_Deriv[0] * (1-r);
					p[step] = p[step] + Segment[i].Center_Coef[0];// + Segment[i].Center_Deriv[0];
				break;
				default: DEBUG_MSG("!!! Segment[i].Num_Coefs="<<Segment[i].Num_Coefs);
			}
			r += t;
			if (step>0)
				globj->append_line(p[step], p[step-1]);
			//~ DEBUG_MSG(r)
		}
	}
	//~ DEBUG_MSG("pov_spheresweep::tesselate()")
}

