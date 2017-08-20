#include "gl_object.hpp"
#include "pov_baseobject.hpp"
#include "pov_triangle.hpp"
#include "pov_scene.hpp"

pov_triangle::pov_triangle(pov_scene* s):pov_baseobject(s)
{
	tag = 0x41495254; // TRIA
}

pov_triangle::~pov_triangle()
{
}

/*
void pov_triangle::update(unsigned int mode) const
{
	if (mode != BBOX)
		pov_baseobject::update();

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			pov_baseobject::begin_dots();
			glBegin(GL_POINTS);
			pts[0].gl_vertex();
			pts[1].gl_vertex();
			pts[2].gl_vertex();
			glEnd();
			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();
			glBegin(GL_LINE_LOOP);
			pts[0].gl_vertex();
			pts[1].gl_vertex();
			pts[2].gl_vertex();
			glEnd();
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			color.gl_color();
//			glFrontFace(GL_CW);
//			glBegin(GL_TRIANGLES);
//			glVertex3dv(pts[0]);
//			glVertex3dv(pts[1]);
//			glVertex3dv(pts[2]);
//			glEnd();
//			glColor3dv(inner_color);
//			glFrontFace(GL_CCW);
//			glBegin(GL_TRIANGLES);
//			glVertex3dv(pts[0]);
//			glVertex3dv(pts[1]);
//			glVertex3dv(pts[2]);
//			glEnd();
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
int pov_triangle::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	readden += pts[0].read(f);
	readden += pts[1].read(f);
	readden += pts[2].read(f);
	readden += normal_vector.read(f);
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_triangle::tesselate() const
{
	globj->append_face(pts[0], pts[1], pts[2]);
}

