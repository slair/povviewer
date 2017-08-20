
#include <GL/gl.h>
#include <GL/glu.h>

#include "pov_baseobject.hpp"
#include "pov_polygon.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_polygon::pov_polygon(pov_scene* s):pov_baseobject(s)
{
	tag = 0x594c4f50; // POLY
	number_of_points = 0;
	points = NULL;
}

pov_polygon::~pov_polygon()
{
	delete [] points;
}

/*
void pov_polygon::update(TViewMode mode)
{
	DEBUG_MSG("pov_polygon::update()")
	if (mode != BBOX)
		pov_baseobject::update();

	unsigned int i;

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			glColor3dv(color);
			glPointSize(scene->get_point_size()*point_scale);
			glBegin(GL_POINTS);
			for(i=0; i<number_of_points; i++)
			{
				glVertex3d(points[i][0], points[i][1], 0);
			}
			glEnd();
			glPointSize(scene->get_point_size());
		break;

		case LINES:
			glColor3dv(color);
			glBegin(GL_LINE_LOOP);
			for(i=0; i<number_of_points; i++)
			{
				glVertex3d(points[i][0], points[i][1], 0);
			}
			glEnd();
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			glColor3dv(color);

			glFrontFace(GL_CW);
			GLUtesselator *pTess;
			pTess = gluNewTess();
			gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
			gluTessCallback(pTess, GLU_TESS_END, glEnd);
			gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &glVertex3dv);
			gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);

			gluTessBeginPolygon(pTess, NULL);
			gluTessBeginContour(pTess);
			for(i=0; i<(number_of_points); i++)
			{
				gluTessVertex(pTess, points[i], points[i]);
			}
			gluTessEndContour(pTess);
			gluTessEndPolygon(pTess);
			gluDeleteTess(pTess);
			glFrontFace(GL_CCW);
			pTess = gluNewTess();
			gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
			gluTessCallback(pTess, GLU_TESS_END, glEnd);
			gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &glVertex3dv);
			gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);

			gluTessBeginPolygon(pTess, NULL);
			gluTessBeginContour(pTess);
			for(i=0; i<(number_of_points); i++)
			{
				gluTessVertex(pTess, points[i], points[i]);
			}
			gluTessEndContour(pTess);
			gluTessEndPolygon(pTess);
			gluDeleteTess(pTess);

			// simple tesselator
//			glFrontFace(GL_CW);
//			glBegin(GL_POLYGON);
//			for(i=0; i<(number_of_points); i++)
//			{
//				glNormal3dv(normal);
//				glVertex3d(points[i][0], points[i][1], 0);
//			}
//			glEnd();
//			glFrontFace(GL_CCW);
//			glColor3dv(inner_color);
//			glBegin(GL_POLYGON);
//			for(i=0; i<(number_of_points); i++)
//			{
//				glNormal3dv(normal);
//				glVertex3d(points[i][0], points[i][1], 0);
//			}
//			glEnd();

		break;

		case NORMALS:
		break;

		default:
		break;
	}
}
*/

int pov_polygon::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	readden += normal.read(f);
	fread(&number_of_points, sizeof(number_of_points), 1, f);
	readden += sizeof(number_of_points);
	points = new vec3d[number_of_points];
	DBL a, b;
	for (unsigned int i=0; i<number_of_points; i++)
	{
		fread(&a, sizeof(DBL), 1, f);
		fread(&b, sizeof(DBL), 1, f);
		points[i] = vec3d(a, b, 0);
	}
	readden += sizeof(DBL)*number_of_points*2;
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_polygon::tesselate() const
{
	unsigned int i = 0;
	array_vec3d contour;
	for(i=0; i<number_of_points; i++)
	{
		contour.push_back(points[i]);
	}
	if (contour.size()>0)
		globj->append_contour(contour, vec3d(0,0,1));
}
