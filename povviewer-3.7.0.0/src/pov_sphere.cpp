#include <math.h>

//#include "pov_baseobject.hpp"
#include "pov_sphere.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_sphere::pov_sphere(pov_scene* s):pov_baseobject(s)
{
	tag = 0x52485053; // SPHR
	center = vec3d(0, 0, 0);
	radius = 0.5;
	is_sphere = true;
	fix_sphere();
}

pov_sphere::~pov_sphere()
{
}
/*
void pov_sphere::update(unsigned int mode)
{
	if (mode != BBOX)
	{
		pov_baseobject::update();
	}

	int numlongitudes = scene->get_sphere_num_longitudes();
	int numlatitudes = scene->get_sphere_num_latitudes();
	DBL u, v, cosu, sinu, sinv;
	vec3d point, n;
	int i,j;

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			pov_baseobject::begin_dots();

			for(i=0; i<numlongitudes; i++)
			{
				u = double(i) / numlongitudes;
				cosu = -cos(u*PI2);
				sinu = sin(u*PI2);
				glBegin(GL_POINTS);
				for(j=0; j<=numlatitudes; j++)
				{
					v = double(j) / numlatitudes;
					sinv = sin(v*PI)*radius;
					point = vec3d(cosu*sinv, cos(v*PI)*radius, sinu*sinv);
					point.gl_vertex();
				}
				glEnd();
			}
			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();
			// horizontal lines
			for(i=0; i<numlatitudes; i++)
			{
				v = double(i) / numlatitudes;
				sinv = sin(v*PI)*radius;
				glBegin(GL_LINE_LOOP);
				for(j=0; j<numlongitudes; j++)
				{
					u = double(j) / numlongitudes;
					point = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
					point.gl_vertex();
				}
				glEnd();
			}
			// vertical lines
			for(i=0; i<numlongitudes; i++)
			{
				u = double(i) / numlongitudes;
				cosu = -cos(u*PI2);
				sinu = sin(u*PI2);
				glBegin(GL_LINE_STRIP);
				for(j=0; j<=numlatitudes; j++)
				{
					v = double(j) / numlatitudes;
					sinv = sin(v*PI)*radius;
					point = vec3d(cosu*sinv, cos(v*PI)*radius, sinu*sinv);
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
int pov_sphere::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	readden += center.read(f);
	fread(&radius, sizeof(radius), 1, f);
	readden += sizeof(radius);
	fix_sphere();
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_sphere::tesselate() const
{
	int numlongitudes = scene->get_sphere_num_longitudes();
	int numlatitudes = scene->get_sphere_num_latitudes();
	DBL u, u2, v, v1, sinv, sinv1;
	vec3d point, n;
	vec3d point0, point1, point2, point3;
	int i,j;

	// top triangle fan
	point0 = vec3d(0, radius, 0);
	v = double(1) / numlatitudes;
	sinv = sin(v*PI)*radius;
	for(i=0; i<numlongitudes; i++)
	{
		u = double(i) / numlongitudes;
		point1 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		u = double(i+1) / numlongitudes;
		point2 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		globj->append_face(point2, point1, point0);
	}

	// bottom triangle fan
	point0 = vec3d(0, -radius, 0);
	v = double(numlatitudes-1) / numlatitudes;
	sinv = sin(v*PI)*radius;
	for(i=0; i<numlongitudes; i++)
	{
		u = double(i) / numlongitudes;
		point1 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		u = double(i+1) / numlongitudes;
		point2 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		globj->append_face(point0, point1, point2);
	}

	// quad strips
	for(i=0; i<numlatitudes-2; i++)
	{
		v = double(i+1) / numlatitudes;
		v1 = double(i+2) / numlatitudes;
		sinv = sin(v*PI)*radius;
		sinv1 = sin(v1*PI)*radius;
		for(j=0; j<numlongitudes; j++)
		{
			u = double(j) / numlongitudes;
			u2 = double(j+1) / numlongitudes;
			point0 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
			point1 = vec3d(-cos(u*PI2)*sinv1, cos(v1*PI)*radius, sin(u*PI2)*sinv1);
			point2 = vec3d(-cos(u2*PI2)*sinv1, cos(v1*PI)*radius, sin(u2*PI2)*sinv1);
			point3 = vec3d(-cos(u2*PI2)*sinv, cos(v*PI)*radius, sin(u2*PI2)*sinv);
			globj->append_face(point3, point2, point1, point0);
		}
	}
}

/** @brief fix_sphere
  *
  * @todo: document this function
  */
void pov_sphere::fix_sphere()
{
	sphere_center = center;
}

