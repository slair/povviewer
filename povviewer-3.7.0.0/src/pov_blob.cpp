#include "pov_baseobject.hpp"
#include "pov_blob.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_blob::pov_blob(pov_scene* s):pov_baseobject(s)
{
	tag = 0x424f4c42; // BLOB
	triangles = NULL;
	is_blob = true;
}

pov_blob::~pov_blob()
{
	delete [] triangles;
}

/*
void pov_blob::update(TViewMode mode)
{
	DEBUG_MSG("pov_blob::update()")
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
			for(int i=0; i<tri_count; i++)
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
			for(int i=0; i<tri_count; i++)
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
			glColor3dv(color);
			glBegin(GL_TRIANGLES);
			for(int i=0; i<tri_count; i++)
			{
//				ov = or_vol(triangles[i][0], triangles[1][0], triangles[i][2]);
//				if (ov>0)
//				{
				glNormal3dv(face_normals[i]);
				glVertex3dv(triangles[i][2]);
				glVertex3dv(triangles[i][1]);
				glVertex3dv(triangles[i][0]);
//				}
//				else
//				{
//				glVertex3dv(triangles[i][2]);
//				glVertex3dv(triangles[i][1]);
//				glVertex3dv(triangles[i][0]);
//				}
			}
			glEnd();
			glFrontFace(GL_CW);
			glColor3dv(inner_color);
//			DBL ov;
			glBegin(GL_TRIANGLES);
			for(int i=0; i<tri_count; i++)
			{
//				ov = or_vol(triangles[i][0], triangles[1][0], triangles[i][2]);
//				if (ov>0)
//				{
				glNormal3dv(face_normals[i]);
				glVertex3dv(triangles[i][2]);
				glVertex3dv(triangles[i][1]);
				glVertex3dv(triangles[i][0]);
//				}
//				else
//				{
//				glVertex3dv(triangles[i][2]);
//				glVertex3dv(triangles[i][1]);
//				glVertex3dv(triangles[i][0]);
//				}
			}
			glEnd();
			glFrontFace(GL_CCW);
		break;

		case NORMALS:
			VECTOR3D normal_end;
			glColor3dv(color);
			for(int i=0; i<tri_count; i++)
			{
				Assign_Vector3d(normal_end, face_centers[i]);
				VAdd3d(normal_end, face_normals[i]);
				glBegin(GL_LINES);
				glVertex3dv(face_centers[i]);
				glVertex3dv(normal_end);
				glEnd();
			}
		break;

		default:
		break;
	}
}
*/

int pov_blob::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	fread(&tri_count, sizeof(tri_count), 1, f);
	readden += sizeof(tri_count);

	triangles = new triangle[tri_count];

	for(int i=0; i<tri_count; i++)
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
void pov_blob::tesselate() const
{
	for (int i=0; i<tri_count; i++)
	{
		globj->append_face(triangles[i][0], triangles[i][1], triangles[i][2]);
	}
}
