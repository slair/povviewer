#include <math.h>

#include "pov_baseobject.hpp"
#include "pov_disc.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_disc::pov_disc(pov_scene* s):pov_baseobject(s)
{
	tag = 0x43534944; // DISC
	radiuses[0] = 1;
	radiuses[1] = 0;
}

pov_disc::~pov_disc()
{
}

#define _LERP(a, b, t) ( ((b) - (a)) * (t) + (a) )

/*
void pov_disc::update(TViewMode mode)
{
	DEBUG_MSG("pov_disc::update()")
	if (mode != BBOX)
		pov_baseobject::update();

	unsigned int usteps = scene->get_disc_usteps();
	unsigned int vsteps = scene->get_disc_vsteps();
	unsigned int i, j;
	DBL ro = radiuses[0];
	DBL ri = radiuses[1];
	DBL u, u2, v, v1, r, r1, cosu, sinu;

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			glColor3dv(color);
			glPointSize(scene->get_point_size()*point_scale);
			for(i=0; i<=vsteps; i++)
			{
				v = double(i) / double(vsteps);
				r = _LERP(ri, ro, v);
				glBegin(GL_POINTS);
				for(j=0; j<usteps; j++)
				{
					u = double(j) / double(usteps);
					glVertex3d(cos(u*PI2)*r, sin(u*PI2)*r, 0);
				}
				glEnd();
			}
			glPointSize(scene->get_point_size());
		break;

		case LINES:
			glColor3dv(color);
			// draw rings
			for(i=0; i<=vsteps; i++)
			{
				v = double(i) / double(vsteps);
				r = _LERP(ri, ro, v);
				glBegin(GL_LINE_LOOP);
				for(j=0; j<usteps; j++)
				{
					u = double(j) / double(usteps);
					glVertex3d(cos(u*PI2)*r, sin(u*PI2)*r, 0);
				}
				glEnd();
			}
			// draw spokes (radial lines)
			for(i=0; i<usteps; i++)
			{
				u = double(i) / double(usteps);
				cosu = cos(u*PI2);
				sinu = sin(u*PI2);
				glBegin(GL_LINE_LOOP);
				for(j=0; j<=vsteps; j++)
				{
					v = double(j) / double(vsteps);
					r = _LERP(ri, ro, v);
					glVertex3d(cosu * r, sinu * r, 0);
				}
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			glColor3dv(color);
			glFrontFace(GL_CW);
			for(i=0; i<vsteps; i++)
			{
				v = double(i) / double(vsteps);
				v1 = double(i+1) / double(vsteps);
				r = _LERP(ri, ro, v);
				r1 = _LERP(ri, ro, v1);
				glBegin(GL_QUAD_STRIP);
				for(j=0; j<=usteps; j++)
				{
					u = double(j) / double(usteps);
					u2 = double(j) / double(usteps);
					glNormal3dv(normal);
					glVertex3d(cos(u*PI2)*r, sin(u*PI2)*r, 0);
					glNormal3dv(normal);
					glVertex3d(cos(u*PI2)*r1, sin(u*PI2)*r1, 0);
					glNormal3dv(normal);
					glVertex3d(cos(u2*PI2)*r, sin(u2*PI2)*r, 0);
					glNormal3dv(normal);
					glVertex3d(cos(u2*PI2)*r1, sin(u2*PI2)*r1, 0);
				}
				glEnd();
			}
			glFrontFace(GL_CCW);
			glColor3dv(inner_color);
			for(i=0; i<vsteps; i++)
			{
				v = double(i) / double(vsteps);
				v1 = double(i+1) / double(vsteps);
				r = _LERP(ri, ro, v);
				r1 = _LERP(ri, ro, v1);
				glBegin(GL_QUAD_STRIP);
				for(j=0; j<=usteps; j++)
				{
					u = double(j) / double(usteps);
					u2 = double(j) / double(usteps);
					glNormal3dv(normal);
					glVertex3d(cos(u*PI2)*r, sin(u*PI2)*r, 0);
					glNormal3dv(normal);
					glVertex3d(cos(u*PI2)*r1, sin(u*PI2)*r1, 0);
					glNormal3dv(normal);
					glVertex3d(cos(u2*PI2)*r, sin(u2*PI2)*r, 0);
					glNormal3dv(normal);
					glVertex3d(cos(u2*PI2)*r1, sin(u2*PI2)*r1, 0);
				}
				glEnd();
			}
		break;

		case NORMALS:
		break;

		default:
		break;
	}
}
*/

int pov_disc::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	readden += position.read(f);
	readden += normal.read(f);
	fread(radiuses, sizeof(DBL), 2, f);
	readden += sizeof(DBL)*2;
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_disc::tesselate() const
{
	unsigned int usteps = scene->get_disc_usteps();
	unsigned int vsteps = scene->get_disc_vsteps();
	unsigned int i, j;
	DBL ro = radiuses[0];
	DBL ri = radiuses[1];
	DBL u, u2, v, v1, r, r1;
	for(i=0; i<vsteps; i++)
	{
		v = double(i) / double(vsteps);
		v1 = double(i+1) / double(vsteps);
		r = _LERP(ri, ro, v);
		r1 = _LERP(ri, ro, v1);
		for(j=0; j<=usteps; j++)
		{
			u = double(j) / double(usteps);
			u2 = double(j+1) / double(usteps);
			globj->append_face(
				vec3d(cos(u*PI2)*r, sin(u*PI2)*r, 0),
				vec3d(cos(u*PI2)*r1, sin(u*PI2)*r1, 0),
				vec3d(cos(u2*PI2)*r1, sin(u2*PI2)*r1, 0),
				vec3d(cos(u2*PI2)*r, sin(u2*PI2)*r, 0)
			);
		}
	}
}

