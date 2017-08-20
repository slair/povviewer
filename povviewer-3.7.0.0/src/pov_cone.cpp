#include <math.h>

#include "pov_cone.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

#define _LERP(a, b, t) ( ((b) - (a)) * (t) + (a) )

pov_cone::pov_cone(pov_scene *s):pov_baseobject(s)
{
	tag = 0x454e4f43; // CONE
	base = vec3d(0, 0, 0);
	apex = vec3d(0, 1, 0);
	base_radius = 0.5;
	apex_radius = 0.5;
}

pov_cone::~pov_cone()
{
}

/*
void pov_cone::update(unsigned int mode) const
{
	if (mode != BBOX)
		pov_baseobject::update();

	int i, j;
	DBL ar, br, u, v, r, cosu, sinu;
	vec3d point, n;
	DBL numlatitudes = scene->get_cone_num_latitudes();
	DBL numlongitudes = scene->get_cone_num_longitudes();
	if (apex_radius>base_radius)
	{
			ar = 1;
			br = dist;
	}
	else
	{
		if (base_radius>apex_radius)
		{
			br = 1;
			ar = dist;
		}
		else
		{
			br = ar = 1;
		}
	}
	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			pov_baseobject::begin_dots();

			for (i=0; i<=numlatitudes; i++)
			{
				v = double(i) / numlatitudes;
				r = _LERP(br, ar, v);
				glBegin(GL_POINTS);
				for(j=0; j<=numlongitudes; j++)
				{
					u = double(j) / numlongitudes;
					point = vec3d(r*cos(u*PI2), r*sin(u*PI2), _LERP(dist, 1, v));
					point.gl_vertex();
				}
				glEnd();
			}
			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();
			// draw rings around the axis
			for(i=0; i<=numlatitudes; i++)
			{
				v = double(i) / numlatitudes;
				r = _LERP(br, ar, v);
				glBegin(GL_LINE_LOOP);
				for(j=0; j<=numlongitudes; j++)
				{
					u = double(j) / numlongitudes;
					point = vec3d(r*cos(u*PI2), r*sin(u*PI2), _LERP(dist, 1, v));
					point.gl_vertex();
				}
				glEnd();
			}
			// draw lines from tip to base
			for(i=0; i<numlongitudes; i++)
			{
				u = double(i) / numlongitudes;
				cosu = cos(u*PI2);
				sinu = sin(u*PI2);
				glBegin(GL_LINE_STRIP);
				for(j=0; j<=numlatitudes; j++)
				{
					v = double(j) / numlatitudes;
					r = _LERP(br, ar, v);
					point = vec3d(r*cosu, r*sinu, _LERP(dist, 1, v));
					point.gl_vertex();
				}
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			color.gl_color();
			globj->show_flat_faces();
		break;

		case NORMALS:
			globj->show_normals(scene->get_normals_size());
		break;

		default:
		break;
	}
}
*/
int pov_cone::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	readden += base.read(f);
	fread(&base_radius, sizeof(base_radius), 1, f);
	readden += apex.read(f);
	fread(&apex_radius, sizeof(apex_radius), 1, f);
	fread(&dist, sizeof(dist), 1, f);
	readden += sizeof(DBL)*3;
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_cone::tesselate() const
{
	DBL numlatitudes = scene->get_cone_num_latitudes();
	DBL numlongitudes = scene->get_cone_num_longitudes();
	int i, j;
	DBL ar, br, u, u2, v, v1, r, r1;
	vec3d point, n;
	vec3d point0, point1, point2, point3;
	if (apex_radius>base_radius)
	{
			//~ ar = 1; br = dist;
			br = 1; ar = dist;
	}
	else
	{
		if (base_radius>apex_radius)
		{
			//~ br = 1; ar = dist;
			ar = 1; br = dist;
		}
		else
		{
			br = ar = 1;
		}
	}

	// base faces
	v = 0;
	r = _LERP(br, ar, v);
	point0 = vec3d(0, 0, _LERP(dist, 1, v));
	for(i=0; i<numlongitudes; i++)
	{
		u = double(i) / numlongitudes;
		point1 = vec3d(r*cos(u*PI2), r*sin(u*PI2), _LERP(dist, 1, v));
		u = double(i+1) / numlongitudes;
		point2 = vec3d(r*cos(u*PI2), r*sin(u*PI2), _LERP(dist, 1, v));
		globj->append_face(point0, point1, point2);
	}

	// apex faces
	v = 1.0; //double(numlatitudes-1) / (numlatitudes);
	r = _LERP(br, ar, v);
	point0 = vec3d(0, 0, _LERP(dist, 1, v));
	for(i=0; i<numlongitudes; i++)
	{
		u = double(i) / numlongitudes;
		point1 = vec3d(r*cos(u*PI2), r*sin(u*PI2), _LERP(dist, 1, v));
		u = double(i+1) / numlongitudes;
		point2 = vec3d(r*cos(u*PI2), r*sin(u*PI2), _LERP(dist, 1, v));
		globj->append_face(point0, point1, point2);
	}

	// quad strips
	for(i=0; i<numlatitudes; i++)
	{
		v = double(i) / numlatitudes;
		v1 = double(i+1) / numlatitudes;
		r = _LERP(br, ar, v);
		r1 = _LERP(br, ar, v1);
		for(j=0; j<numlongitudes; j++)
		{
			u = double(j) / numlongitudes;
			u2 = double(j+1) / numlongitudes;
			point0 = vec3d(r*cos(u*PI2), r*sin(u*PI2), _LERP(dist, 1, v));
			point1 = vec3d(r1*cos(u*PI2), r1*sin(u*PI2), _LERP(dist, 1, v1));
			point2 = vec3d(r1*cos(u2*PI2), r1*sin(u2*PI2), _LERP(dist, 1, v1));
			point3 = vec3d(r*cos(u2*PI2), r*sin(u2*PI2), _LERP(dist, 1, v));
			globj->append_face(point0, point1, point2, point3);
//			return;
		}
	}
}

