#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

#include "common_funcs.hpp"
#include "pov_baseobject.hpp"
#include "pov_ttf.hpp"
#include "pov_scene.hpp"
#include "vec2d.hpp"
#include "gl_object.hpp"

pov_ttf::pov_ttf(pov_scene* s):pov_baseobject(s)
{
	tag = 0x5f465454; /* TTF_ */
	depth = 1.0;
}

pov_ttf::~pov_ttf()
{
}

/*
void pov_ttf::update(unsigned int mode)
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
			for(unsigned int i=0; i<contours.size(); i++)
			{
				for(unsigned int j=0; j<contours.at(i).size(); j++)
				{
					glVertex3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], 0);
					glVertex3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], depth);
				}
			}
			glEnd();
			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();
			for(unsigned int i=0; i<contours.size(); i++)
			{
				unsigned int num_points = contours.at(i).size();
				for(unsigned int j=0; j<num_points-1; j++)
				{
					glBegin(GL_LINES);
					glVertex3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], 0);
					glVertex3d(contours.at(i).at(j+1)[0], contours.at(i).at(j+1)[1], 0);
					glVertex3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], depth);
					glVertex3d(contours.at(i).at(j+1)[0], contours.at(i).at(j+1)[1], depth);
					glVertex3d(contours.at(i).at(j+1)[0], contours.at(i).at(j+1)[1], 0);
					glVertex3d(contours.at(i).at(j+1)[0], contours.at(i).at(j+1)[1], depth);
					glEnd();
				}
				glBegin(GL_LINES);
				glVertex3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], 0);
				glVertex3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], depth);
				glVertex3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], 0);
				glVertex3d(contours.at(i).at(num_points-1)[0], contours.at(i).at(num_points-1)[1], 0);
				glVertex3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], depth);
				glVertex3d(contours.at(i).at(num_points-1)[0], contours.at(i).at(num_points-1)[1], depth);
				glVertex3d(contours.at(i).at(num_points-1)[0], contours.at(i).at(num_points-1)[1], 0);
				glVertex3d(contours.at(i).at(num_points-1)[0], contours.at(i).at(num_points-1)[1], depth);
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			color.gl_color();
			globj->show_flat_faces();

			// front side
			DBL* p;
			GLUtesselator *pTess;
			glFrontFace(GL_CW);
			pTess = gluNewTess();
			gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
			gluTessCallback(pTess, GLU_TESS_END, glEnd);
			gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &normal_with_vertex);
			gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);
			gluTessCallback(pTess, GLU_TESS_COMBINE, (GLvoid (*) ()) &tessCombine);
			gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
			//gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);

			gluTessBeginPolygon(pTess, NULL);
			for(unsigned int i=0; i<contours.size(); i++)
			{
				unsigned int num_points = contours.at(i).size();
				unsigned int j = 0;
				gluTessBeginContour(pTess);
				for(j=0; j<num_points; j++)
				{
					p = new DBL[6];
					p[0] = contours[i][j][0];
					p[1] = contours[i][j][1];
					p[2] = 0;
					p[3] = 0;
					p[4] = 0;
					p[5] = -1;
					//cout << *p << endl;
					gluTessVertex(pTess, (GLdouble*) p, (GLvoid*) p);
				}
				gluTessEndContour(pTess);
			}
			gluTessEndPolygon(pTess);
			gluDeleteTess(pTess);

			// back side
			glFrontFace(GL_CCW);
			pTess = gluNewTess();
			gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
			gluTessCallback(pTess, GLU_TESS_END, glEnd);
			gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &normal_with_vertex);
			gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);
			gluTessCallback(pTess, GLU_TESS_COMBINE, (GLvoid (*) ()) &tessCombine);
			gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
			//gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);

			gluTessBeginPolygon(pTess, NULL);
			for(unsigned int i=0; i<contours.size(); i++)
			{
				unsigned int num_points = contours.at(i).size();
				unsigned int j = 0;
				gluTessBeginContour(pTess);
				for(j=0; j<num_points; j++)
				{
					p = new DBL[6];
					p[0] = contours[i][j][0];
					p[1] = contours[i][j][1];
					p[2] = depth;
					p[3] = 0;
					p[4] = 0;
					p[5] = 1;
					//cout << *p << endl;
					gluTessVertex(pTess, (GLdouble*) p, (GLvoid*) p);
				}
				gluTessEndContour(pTess);
			}
			gluTessEndPolygon(pTess);
			gluDeleteTess(pTess);
		break;

		case NORMALS:
			globj->show_normals(scene->get_normals_size());
		break;

		default:
		break;
	}
}
*/

int pov_ttf::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	USHORT num_contours = 0;

	fread(&depth, sizeof(depth), 1, f);
	fread(&num_contours, sizeof(num_contours), 1, f);
	readden += sizeof(depth) + sizeof(num_contours);

	for(unsigned int i=0; i<num_contours; i++)
	{
		USHORT num_points = 0;
		fread(&num_points, sizeof(num_points), 1, f);
		readden += sizeof(num_points);
		array_vec2d contour;
		for (int j=0; j<num_points; j++)
		{
			vec2d point;
			readden += point.read(f);
			contour.push_back(point);
		}
		contours.push_back(contour);
	}
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_ttf::tesselate() const
{
	unsigned int j = 0;
	array_vec3d contour, contour1;
	for(unsigned int i=0; i<contours.size(); i++)
	{
		unsigned int num_points = contours.at(i).size();
		for(j=0; j<(num_points-1); j++)
		{
			globj->append_face(
				vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], 0),
				vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], depth),
				vec3d(contours.at(i).at(j+1)[0], contours.at(i).at(j+1)[1], depth),
				vec3d(contours.at(i).at(j+1)[0], contours.at(i).at(j+1)[1], 0)
			);
			contour.push_back(vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], 0));
			contour1.push_back(vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], depth));
		}
		globj->append_face(
			vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], 0),
			vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], depth),
			vec3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], depth),
			vec3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], 0)
		);
		contour.push_back(vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], 0));
		contour.push_back(vec3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], 0));
		contour1.push_back(vec3d(contours.at(i).at(j)[0], contours.at(i).at(j)[1], depth));
		contour1.push_back(vec3d(contours.at(i).at(0)[0], contours.at(i).at(0)[1], depth));
	}
	if (contour.size()!=0)
		globj->append_contour(contour, vec3d(0,0,-1));
	if (contour1.size()!=0)
		globj->append_contour(contour1, vec3d(0,0,1));
//			for(unsigned int i=0; i<contours.size(); i++)
//			{
//				unsigned int num_points = contours.at(i).size();
//				unsigned int j = 0;
//				gluTessBeginContour(pTess);
//				for(j=0; j<num_points; j++)
//				{
//					p = new DBL[6];
//					p[0] = contours[i][j][0];
//					p[1] = contours[i][j][1];
//					p[2] = 0;
//					p[3] = 0;
//					p[4] = 0;
//					p[5] = -1;
//					//cout << *p << endl;
//					gluTessVertex(pTess, (GLdouble*) p, (GLvoid*) p);
//				}
//				gluTessEndContour(pTess);
//			}
//
}

