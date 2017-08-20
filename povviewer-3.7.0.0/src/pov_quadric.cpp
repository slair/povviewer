#include "pov_baseobject.hpp"
#include "pov_quadric.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_quadric::pov_quadric(pov_scene* s):pov_baseobject(s)
{
	tag = 0x44415551; // QUAD
	tri_count = 0;
}


pov_quadric::~pov_quadric()
{
//	if (triangles!=NULL) delete [] triangles;
//	if (face_normals!=NULL) delete [] face_normals;
//	if (face_centers!=NULL) delete [] face_centers;
//	if (vertex_normals!=NULL) delete [] vertex_normals;
}

/*
void pov_quadric::update(TViewMode mode)
{
	DEBUG_MSG("pov_quadric::update()")
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

int pov_quadric::read(FILE *f)
{
	DEBUG_MSG("pov_quadric::read()")
	int readden = pov_baseobject::read(f);

	fread(&tri_count, sizeof(tri_count), 1, f);
	readden += sizeof(tri_count);

	// all arrays must be NULL
//	triangles = new TRIANGLED[tri_count];
//	face_normals = new VECTOR3D[tri_count];
//	face_centers = new VECTOR3D[tri_count];
//	vertex_normals = new TRIANGLED[tri_count];

//	VECTOR3D face_center;
//	VECTOR3D face_normal;
	vec3d point0, point1, point2;
	for(int i=0; i<tri_count; i++)
	{
		readden += point0.read(f);
		readden += point1.read(f);
		readden += point2.read(f);
		globj->append_face(point0, point1, point2);
	}

	return readden;
}
