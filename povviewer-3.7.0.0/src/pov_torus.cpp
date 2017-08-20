#include <math.h>

#include "pov_baseobject.hpp"
#include "pov_torus.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_torus::pov_torus(pov_scene* s):pov_baseobject(s)
{
	tag = 0x53524f54; // TORS
}

pov_torus::~pov_torus()
{
//	DEBUG_MSG("~pov_torus")
}

/*
void pov_torus::update(TViewMode mode)
{
	DEBUG_MSG("pov_torus::update()")
	if (mode != BBOX)
		pov_baseobject::update();

	unsigned int torus_usteps = scene->get_torus_usteps();
	unsigned int torus_vsteps = scene->get_torus_vsteps();
	unsigned int i, j;
	VECTOR3D n, n1, n2, n3, np, np1, np2, np3;
	DBL u, u2, v, v1, r, r1, X, X1, X2, X3, Y, Y1, Y2, Y3, Z, Z1, Z2, Z3, cosu, sinu;


	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			glColor3dv(color);
			glPointSize(scene->get_point_size()*point_scale);
			for(i=0; i<torus_usteps; i++)
			{
				u = double(i) / torus_usteps;
				cosu = cos(u * PI2);
				sinu = sin(u * PI2);
				glBegin(GL_POINTS);
				for(j=0; j<torus_vsteps; j++)
				{
					v = double(j) / torus_vsteps;
					r = cos(v * PI2) * minor + major;
					glVertex3d(cosu * r, sin(v * PI2) * minor, sinu * r);
				}
				glEnd();
			}
			glPointSize(scene->get_point_size());
		break;

		case LINES:
			glColor3dv(color);
			// draw major rings
			for(i=0; i<torus_vsteps; i++)
			{
				v = double(i) / torus_vsteps;
				Y = sin(v * PI2) * minor;
				r = cos(v * PI2) * minor + major;
				glBegin(GL_LINE_LOOP);
				for(j=0; j < torus_usteps; j++)
				{
					u = double(j) / torus_usteps;
					X = cos(u * PI2) * r;
					Z = sin(u * PI2) * r;
					glVertex3d(X, Y, Z);
				}
				glEnd();
			}
			// draw minor rings
			for(i=0; i<torus_usteps; i++)
			{
				u = double(i) / torus_usteps;
				cosu = cos(u * PI2);
				sinu = sin(u * PI2);
				glBegin(GL_LINE_LOOP);
				for(j=0; j<torus_vsteps; j++)
				{
					v = double(j) / torus_vsteps;
					r = cos(v * PI2) * minor + major;
					glVertex3d(cosu * r, sin(v * PI2) * minor, sinu * r);
				}
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			glColor3dv(color);
			// draw quad strips
			glFrontFace(GL_CW);
			for(i=0; i<torus_vsteps; i++)
			{
				v = double(i) / torus_vsteps;
				v1 = double(i+1) / torus_vsteps;
				Y = sin(v * PI2) * minor;
				Y2 = Y;
				Y1 = sin(v1 * PI2) * minor;
				Y3 = Y1;
				r = cos(v * PI2) * minor + major;
				r1 = cos(v1 * PI2) * minor + major;
				glBegin(GL_QUAD_STRIP);
				for(j=0; j<torus_usteps; j++)
				{
					u = double(j) / torus_usteps;
					u2 = double(j+1) / torus_usteps;
					X = cos(u * PI2) * r;
					X2 = cos(u2 * PI2) * r;
					X1 = cos(u * PI2) * r1;
					X3 = cos(u2 * PI2) * r1;
					Z = sin(u * PI2) * r;
					Z2 = sin(u2 * PI2) * r;
					Z3 = sin(u2 * PI2) * r1;
					Z1 = sin(u * PI2) * r1;
					VSub3d(n, X, Y, Z, major*cos(u*PI2), 0, major*sin(u*PI2));
					VNormalize3d(n);
					glNormal3dv(n);
					glVertex3d(X, Y, Z);
					VSub3d(n1, X1, Y1, Z1, major*cos(u*PI2), 0, major*sin(u*PI2));
					VNormalize3d(n1);
					glNormal3dv(n1);
					glVertex3d(X1, Y1, Z1);
					VSub3d(n2, X2, Y2, Z2, major*cos(u2*PI2), 0, major*sin(u2*PI2));
					VNormalize3d(n2);
					glNormal3dv(n2);
					glVertex3d(X2, Y2, Z2);
					VSub3d(n3, X3, Y3, Z3, major*cos(u2*PI2), 0, major*sin(u2*PI2));
					VNormalize3d(n3);
					glNormal3dv(n3);
					glVertex3d(X3, Y3, Z3);
				}
				glEnd();
			}
			glFrontFace(GL_CCW);
		break;

		case NORMALS:
			glColor3dv(color);
			// draw quad strips
			glPointSize(scene->get_point_size()*point_scale);
			glFrontFace(GL_CW);
			for(i=0; i<torus_vsteps; i++)
			{
				v = double(i) / torus_vsteps;
				v1 = double(i+1) / torus_vsteps;
				Y = sin(v * PI2) * minor;
				Y2 = Y;
				Y1 = sin(v1 * PI2) * minor;
				Y3 = Y1;
				r = cos(v * PI2) * minor + major;
				r1 = cos(v1 * PI2) * minor + major;
				glBegin(GL_LINES);
				for(j=0; j<torus_usteps; j++)
				{
					u = double(j) / torus_usteps;
					u2 = double(j+1) / torus_usteps;
					X = cos(u * PI2) * r;
					X2 = cos(u2 * PI2) * r;
					X1 = cos(u * PI2) * r1;
					X3 = cos(u2 * PI2) * r1;
					Z = sin(u * PI2) * r;
					Z2 = sin(u2 * PI2) * r;
					Z3 = sin(u2 * PI2) * r1;
					Z1 = sin(u * PI2) * r1;

					VSub3d(n, X, Y, Z, major*cos(u*PI2), 0, major*sin(u*PI2));
					glVertex3d(X, Y, Z);
					VAdd3d(np, X, Y, Z, n);
					glVertex3dv(np);

					VSub3d(n1, X1, Y1, Z1, major*cos(u*PI2), 0, major*sin(u*PI2));
					glVertex3d(X1, Y1, Z1);
					VAdd3d(np1, X1, Y1, Z1, n1);
					glVertex3dv(np1);

					VSub3d(n2, X2, Y2, Z2, major*cos(u2*PI2), 0, major*sin(u2*PI2));
					glVertex3d(X2, Y2, Z2);
					VAdd3d(np2, X2, Y2, Z2, n2);
					glVertex3dv(np2);

					VSub3d(n3, X3, Y3, Z3, major*cos(u2*PI2), 0, major*sin(u2*PI2));
					glVertex3d(X3, Y3, Z3);
					VAdd3d(np3, X3, Y3, Z3, n3);
					glVertex3dv(np3);
				}
				glEnd();
			}
			glFrontFace(GL_CCW);
			glPointSize(scene->get_point_size());
		break;

		default:
		break;
	}
}
*/

int pov_torus::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	fread(&major, sizeof(major), 1, f);
	fread(&minor, sizeof(minor), 1, f);
	readden += sizeof(major) + sizeof(minor);
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_torus::tesselate() const
{
	unsigned int torus_usteps = scene->get_torus_usteps();
	unsigned int torus_vsteps = scene->get_torus_vsteps();
	unsigned int i, j;
	vec3d n, n1, n2, n3, np, np1, np2, np3;
	DBL u, u2, v, v1, r, r1, X, X1, X2, X3, Y, Y1, Y2, Y3, Z, Z1, Z2, Z3;

	for(i=0; i<torus_vsteps; i++)
	{
		v = double(i) / torus_vsteps;
		v1 = double(i+1) / torus_vsteps;
		Y = sin(v * PI2) * minor;
		Y2 = Y;
		Y1 = sin(v1 * PI2) * minor;
		Y3 = Y1;
		r = cos(v * PI2) * minor + major;
		r1 = cos(v1 * PI2) * minor + major;
		for(j=0; j<torus_usteps; j++)
		{
			u = double(j) / torus_usteps;
			u2 = double(j+1) / torus_usteps;
			X = cos(u * PI2) * r;
			X2 = cos(u2 * PI2) * r;
			X1 = cos(u * PI2) * r1;
			X3 = cos(u2 * PI2) * r1;
			Z = sin(u * PI2) * r;
			Z2 = sin(u2 * PI2) * r;
			Z3 = sin(u2 * PI2) * r1;
			Z1 = sin(u * PI2) * r1;
			globj->append_face(vec3d(X, Y, Z), vec3d(X2, Y2, Z2), vec3d(X3, Y3, Z3), vec3d(X1, Y1, Z1));
		}
	}
}

