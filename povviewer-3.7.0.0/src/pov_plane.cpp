#include <math.h>

#include "pov_baseobject.hpp"
#include "pov_plane.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_plane::pov_plane(pov_scene* s):pov_baseobject(s)
{
	tag = 0x4e414c50; // PLAN
}

pov_plane::~pov_plane()
{
}

/*
void pov_plane::update(TViewMode mode)
{
	DEBUG_MSG("pov_plane::update()")
	if (mode != BBOX)
		pov_baseobject::update();

	vec3d origin;
	vec3d antinormal;
	vec3d diag;
	vec3d c;
	vec3d d;
	vec3d corner[4]={{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	DBL plane_radius = scene->get_plane_radius();
	unsigned int plane_steps = scene->get_plane_steps();
	DBL angle;

	Assign_Vector3d(origin, normal[0]*(-distance), normal[1]*(-distance), normal[2]*(-distance));
	Assign_Vector3d(antinormal, 1, 0, 0);
	Assign_Vector3d(diag, 0, 0, 0);
	angle = VDotd(antinormal, normal);
	if (fabs(angle)>(1-EPSILON))
	{ // The plane is parallel to the YZ plane.
		Assign_Vector3d(c, 0, 1, 0);
		Assign_Vector3d(d, 0, 0, 1);
	}
	else
	{ // Antinormal can be used in cross product.
		VCross3d(c, antinormal, normal);
		VCross3d(d, c, normal);
	}
	// c and d are normals along the plane,
	// and they are perpendicular to each other.
	VHalf3d(diag, c, d);
	VNormalize3d(diag);
	VAddScaled3d(corner[0], origin, plane_radius, diag);
	VAddScaled3d(corner[2], origin, -plane_radius, diag);
	VScale3d(c, -1);
	VHalf3d(diag, c, d);
	VNormalize3d(diag);
	VAddScaled3d(corner[1], origin, plane_radius, diag);
	VAddScaled3d(corner[3], origin, -plane_radius, diag);
	vec3d deltaRow = {0.0, 0.0, 0.0};
	vec3d deltaCol = {0.0, 0.0, 0.0};
	VSub3d(deltaRow, corner[3], corner[0]);
	VInverseScale3d(deltaRow, plane_steps);
	VSub3d(deltaCol, corner[1], corner[0]);
	VInverseScale3d(deltaCol, plane_steps);
	vec3d p;
	unsigned int i, j;

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			glColor3dv(color);
			glPointSize(scene->get_point_size()*point_scale);
			glBegin(GL_POINTS);
			for(i = 0; i <= plane_steps; i++)
			{
				VAddScaled3d(p, corner[0], i, deltaCol);
				VAddScaled3d(p, p, -1, deltaRow);
				for(j=0; j<=plane_steps; j++)
				{
					VAddScaled3d(p, p, 1, deltaRow);
					glVertex3dv(p);
				}
			}
			glEnd();
			glPointSize(scene->get_point_size());
		break;

		case LINES:
			glColor3dv(color);
			for(i=0; i<=plane_steps; i++)
			{
				glBegin(GL_LINE_STRIP);
				VAddScaled3d(p, corner[0], i, deltaRow);
				glVertex3dv(p);
				VAddScaled3d(p, corner[1], i, deltaRow);
				glVertex3dv(p);
				glEnd();
			}
			for(i=0; i<=plane_steps; i++)
			{
				glBegin(GL_LINE_STRIP);
				VAddScaled3d(p, corner[0], i, deltaCol);
				glVertex3dv(p);
				VAddScaled3d(p, corner[3], i, deltaCol);
				glVertex3dv(p);
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);

			glColor3dv(color);
			glBegin(GL_QUADS);
			glNormal3dv(normal);
			glVertex3dv(corner[0]);
			glNormal3dv(normal);
			glVertex3dv(corner[1]);
			glNormal3dv(normal);
			glVertex3dv(corner[2]);
			glNormal3dv(normal);
			glVertex3dv(corner[3]);
			glEnd();

			glColor3dv(inner_color);
			glFrontFace(GL_CW);
			glBegin(GL_QUADS);
//			glNormal3d(-normal[0], -normal[1], -normal[2]);
			glNormal3dv(normal);
			glVertex3dv(corner[0]);
//			glNormal3d(-normal[0], -normal[1], -normal[2]);
			glNormal3dv(normal);
			glVertex3dv(corner[1]);
//			glNormal3d(-normal[0], -normal[1], -normal[2]);
			glNormal3dv(normal);
			glVertex3dv(corner[2]);
//			glNormal3d(-normal[0], -normal[1], -normal[2]);
			glNormal3dv(normal);
			glVertex3dv(corner[3]);
			glEnd();
			glFrontFace(GL_CCW);
		break;

		case NORMALS:
		break;

		default:
		break;
	}
}
*/

int pov_plane::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	readden += normal.read(f);
	fread(&distance, sizeof(distance), 1, f);
	readden += sizeof(distance);
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_plane::tesselate() const
{
	vec3d origin;
	vec3d antinormal;
	vec3d diag;
	vec3d c;
	vec3d d;
	vec3d corner[4];
	DBL plane_radius = scene->get_plane_radius();
	unsigned int plane_steps = scene->get_plane_steps();
	DBL angle;

	origin = vec3d(normal[0]*(-distance), normal[1]*(-distance), normal[2]*(-distance));
	antinormal = vec3d(1, 0, 0);
	diag = vec3d(0, 0, 0);
	angle = antinormal.vdot(normal);
	if (fabs(angle)>(1-EPSILON))
	{ // The plane is parallel to the YZ plane.
		c = vec3d(0, 1, 0);
		d = vec3d(0, 0, 1);
	}
	else
	{ // Antinormal can be used in cross product.
		c = antinormal^normal;
		d = c^normal;
	}
	// c and d are normals along the plane,
	// and they are perpendicular to each other.
	diag = ((c + d)*0.5).normalize();
	corner[0] = origin + diag*plane_radius;
	corner[2] = origin + diag*(-plane_radius);
	c = c*(-1);
	diag = ((c + d)*0.5).normalize();
	corner[1] = origin + diag*plane_radius;
	corner[3] = origin + diag*(-plane_radius);
	vec3d deltaRow;
	vec3d deltaCol;
	deltaRow = corner[3] - corner[0];
	deltaRow = deltaRow / plane_steps;
	deltaCol = corner[1] - corner[0];
	deltaCol = deltaCol / plane_steps;
	vec3d p;
//	globj->append_face(corner[0], corner[1], corner[2], corner[3]);
	globj->append_face(corner[3], corner[2], corner[1], corner[0]);
}

