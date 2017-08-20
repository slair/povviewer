#include <GL/gl.h>

#include "pov_baseobject.hpp"
#include "pov_isosurface.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_isosurface::pov_isosurface(pov_scene* s):pov_baseobject(s)
{
	tag = 0x534f5349; /* ISOS */
	//Assign_Vector3f(bbox_corner1,	-0.5,	-0.5,	-0.5);
	//Assign_Vector3f(bbox_corner2,	0.5,	0.5,	0.5);
	//tri_count = 0;
//	triangles = NULL;
//	face_centers = NULL;
//	face_normals = NULL;
	//vertex_normals = NULL;
}

//==========================================================================
pov_isosurface::~pov_isosurface()
{

//	if (triangles != NULL)
//		delete [] triangles;
//
//	if (face_centers != NULL)
//		delete [] face_centers;
//
//	if (face_normals != NULL)
//		delete [] face_normals;

//	if (vertex_normals !=NULL)
//		delete [] vertex_normals;
}

/*
void pov_isosurface::update(unsigned int mode)
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
//			glBegin(GL_POINTS);
//			for(int i=0; i<tri_count; i++)
//			{
//				glVertex3dv(triangles[i][0]);
//				glVertex3dv(triangles[i][1]);
//				glVertex3dv(triangles[i][2]);
//			}
//			glEnd();
			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();
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
int pov_isosurface::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	int tri_count;
	fread(&tri_count, sizeof(tri_count), 1, f);
	readden += sizeof(tri_count);
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
