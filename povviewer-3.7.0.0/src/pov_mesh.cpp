#include "pov_baseobject.hpp"
#include "pov_mesh.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_mesh::pov_mesh(pov_scene* s):pov_baseobject(s)
{
	tag = 0x4853454d; // MESH
	triangles = NULL;
	tri_count = 0;
}

pov_mesh::~pov_mesh()
{
	delete [] triangles;
}

/*
void pov_mesh::update(TViewMode mode)
{
	DEBUG_MSG("pov_mesh::update()")
	unsigned int i;

	if (mode != BBOX)
		pov_baseobject::update();

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			glColor3dv(color);
			glPointSize(scene->get_point_size()*point_scale);
			glBegin(GL_POINTS);
			for(i=0; i<tri_count; i++)
			{
				glVertex3dv(triangles[i][0]);
				glVertex3dv(triangles[i][1]);
				glVertex3dv(triangles[i][2]);
			}
			glEnd();
			glPointSize(scene->get_point_size());
		break;

		case LINES:
			glColor3dv(color);
			for(i=0; i<tri_count; i++)
			{
				glBegin(GL_LINE_LOOP);
				glVertex3dv(triangles[i][0]);
				glVertex3dv(triangles[i][1]);
				glVertex3dv(triangles[i][2]);
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			glEnable(GL_NORMALIZE);
			glColor3dv(color);
			glFrontFace(GL_CW);
			glBegin(GL_TRIANGLES);
			for(i=0; i<tri_count; i++)
			{
				glNormal3dv(normals_ccw[i]);
				glVertex3dv(triangles[i][0]);
				glNormal3dv(normals_ccw[i]);
				glVertex3dv(triangles[i][1]);
				glNormal3dv(normals_ccw[i]);
				glVertex3dv(triangles[i][2]);
			}
			glEnd();
			glFrontFace(GL_CCW);

			glBegin(GL_TRIANGLES);
			for(i=0; i<tri_count; i++)
			{
				glNormal3dv(normals_ccw[i]);
				glVertex3dv(triangles[i][0]);
				glNormal3dv(normals_ccw[i]);
				glVertex3dv(triangles[i][1]);
				glNormal3dv(normals_ccw[i]);
				glVertex3dv(triangles[i][2]);
			}
			glEnd();

		break;

		case NORMALS:
		break;

		default:
		break;
	}
}
*/

int pov_mesh::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	fread(&tri_count, sizeof(tri_count), 1, f);
	readden += sizeof(tri_count);
	triangles = new triangle[tri_count];
	for (unsigned int i=0; i<tri_count; i++)
	{
		readden += triangles[i][0].read(f);
		readden += triangles[i][1].read(f);
		readden += triangles[i][2].read(f);
	}
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_mesh::tesselate() const
{
	for (unsigned int i=0; i<tri_count; i++)
	{
		globj->append_face(triangles[i][0], triangles[i][1], triangles[i][2]);
	}
}

