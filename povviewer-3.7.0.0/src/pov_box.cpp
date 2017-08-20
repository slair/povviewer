#include "pov_baseobject.hpp"
#include "pov_box.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_box::pov_box(pov_scene* s):pov_baseobject(s)
{
	tag = 0x5f584f42; // BOX_
}


pov_box::~pov_box()
{
}

/*
void pov_box::update(unsigned int mode)
{
	if (mode != BBOX)
		pov_baseobject::update();
	SNGL X1 = corner1[0];
	SNGL X2 = corner2[0];
	SNGL Y1 = corner1[1];
	SNGL Y2 = corner2[1];
	SNGL Z1 = corner1[2];
	SNGL Z2 = corner2[2];
	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			pov_baseobject::begin_dots();

			// Bottom face
			glBegin(GL_POINTS);
			glVertex3f(X1, Y1, Z1);
			glVertex3f(X1, Y1, Z2);
			glVertex3f(X2, Y1, Z2);
			glVertex3f(X2, Y1, Z1);
			// Top face
			glVertex3f(X1, Y2, Z1);
			glVertex3f(X1, Y2, Z2);
			glVertex3f(X2, Y2, Z2);
			glVertex3f(X2, Y2, Z1);
			glEnd();

			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();

			// Bottom face
			glBegin(GL_LINE_STRIP);
			glVertex3f(X1, Y1, Z1);
			glVertex3f(X1, Y1, Z2);
			glVertex3f(X2, Y1, Z2);
			glVertex3f(X2, Y1, Z1);
			glVertex3f(X1, Y1, Z1);
			glEnd();
			// Top face
			glBegin(GL_LINE_STRIP);
			glVertex3f(X1, Y2, Z1);
			glVertex3f(X1, Y2, Z2);
			glVertex3f(X2, Y2, Z2);
			glVertex3f(X2, Y2, Z1);
			glVertex3f(X1, Y2, Z1);
			glEnd();
			// Lines connecting faces
			glBegin(GL_LINES);
			glVertex3f(X1, Y1, Z1);
			glVertex3f(X1, Y2, Z1);
			glVertex3f(X2, Y1, Z2);
			glVertex3f(X2, Y2, Z2);
			glVertex3f(X1, Y1, Z2);
			glVertex3f(X1, Y2, Z2);
			glVertex3f(X2, Y1, Z1);
			glVertex3f(X2, Y2, Z1);
			glEnd();
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
int pov_box::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	readden += corner1.read(f);
	readden += corner2.read(f);
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_box::tesselate() const
{
	DBL X1 = corner1[0];
	DBL X2 = corner2[0];
	DBL Y1 = corner1[1];
	DBL Y2 = corner2[1];
	DBL Z1 = corner1[2];
	DBL Z2 = corner2[2];
	// bottom
	globj->append_face(
	vec3d(X1, Y1, Z1),
	vec3d(X1, Y1, Z2),
	vec3d(X2, Y1, Z2),
	vec3d(X2, Y1, Z1)
	);

	//top
	globj->append_face(
	vec3d(X1, Y2, Z1),
	vec3d(X2, Y2, Z1),
	vec3d(X2, Y2, Z2),
	vec3d(X1, Y2, Z2)
	);

	// sides
	globj->append_face(
	vec3d(X2, Y2, Z1),
	vec3d(X2, Y1, Z1),
	vec3d(X2, Y1, Z2),
	vec3d(X2, Y2, Z2)
	);

	globj->append_face(
	vec3d(X1, Y1, Z1),
	vec3d(X1, Y2, Z1),
	vec3d(X1, Y2, Z2),
	vec3d(X1, Y1, Z2)
	);

	globj->append_face(
	vec3d(X1, Y1, Z1),
	vec3d(X2, Y1, Z1),
	vec3d(X2, Y2, Z1),
	vec3d(X1, Y2, Z1)
	);

	globj->append_face(
	vec3d(X2, Y2, Z2),
	vec3d(X2, Y1, Z2),
	vec3d(X1, Y1, Z2),
	vec3d(X1, Y2, Z2)
	);
}

