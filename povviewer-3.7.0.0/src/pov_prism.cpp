#include <GL/gl.h>
#include <GL/glu.h>

#include "common_funcs.hpp"
#include "pov_baseobject.hpp"
#include "pov_prism.hpp"
#include "pov_scene.hpp"
#include "vec2d.hpp"
#include "gl_object.hpp"

pov_prism::pov_prism(pov_scene* s):pov_baseobject(s)
{
	tag = 0x4d535250; // PRSM
	number_of_spline = 0;
	spline_type = 0;
	sweep_type = 0;
	heights[0] = 0;
	heights[1] = 0;
	spline = NULL;
}

pov_prism::~pov_prism()
{
//	if(spline!=NULL)
	delete [] spline;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void pov_prism::calculate_spline(vec2d* points, unsigned int index)
{
	vec2d p1, p2, A, B, C, D;
	p1 = vec2d(spline[index], spline[index+1]);
	p2 = vec2d(spline[index+2], spline[index+3]);
	A = vec2d(spline[index+4], spline[index+5]);
	B = vec2d(spline[index+6], spline[index+7]);
	C = vec2d(spline[index+8], spline[index+9]);
	D = vec2d(spline[index+10], spline[index+11]);
	unsigned int steps = scene->get_prism_spline_steps();
	DBL t = double(1) / double(steps);
	//VECTOR2D f;
	//Assign_Vector2d(f, p2);
	DBL r = 0;
	//for x in range(steps+1):
	for(unsigned int x=0; x<=steps; x++)
	{
		//f = A*r*r*r + B*r*r + C*r + D
		points[x] = vec2d(0, 0);
		points[x] = points[x] + A * r*r*r;
		points[x] = points[x] + B * r*r;
		points[x] = points[x] + C * r;
		points[x] = points[x] + D;
		r += t;
		//points.append(f)
		//Assign_Vector2d(points[x], f);
	}
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void pov_prism::calculate_spline3dv(vec2d* points, unsigned int index, DBL val, vec3d* p3d)
{
	vec2d p1, p2, A, B, C, D;
	p1 = vec2d(spline[index], spline[index+1]);
	p2 = vec2d(spline[index+2], spline[index+3]);
	A = vec2d(spline[index+4], spline[index+5]);
	B = vec2d(spline[index+6], spline[index+7]);
	C = vec2d(spline[index+8], spline[index+9]);
	D = vec2d(spline[index+10], spline[index+11]);
	unsigned int steps = scene->get_prism_spline_steps();
	DBL t = double(1) / double(steps);
	//VECTOR2D f;
	//Assign_Vector2d(f, p2);
	DBL r = 0;
	//for x in range(steps+1):
	for(unsigned int x=0; x<=steps; x++)
	{
		//f = A*r*r*r + B*r*r + C*r + D
		points[x] = vec2d(0, 0);
		points[x] = points[x] + A * r*r*r;
		points[x] = points[x] + B * r*r;
		points[x] = points[x] + C * r;
		points[x] = points[x] + D;
		r += t;
		//points.append(f)
		//Assign_Vector2d(points[x], f);
		p3d[x] = vec3d(points[x][0], val, points[x][1]);
	}
}

/*
void pov_prism::update(unsigned int mode)
{
	if (mode != BBOX)
		pov_baseobject::update();

	unsigned int i, j;
	unsigned int steps = scene->get_prism_spline_steps();
	vec2d *pts;
	vec3d *pts3dv;
	pts = new vec2d[steps+10];
	pts3dv = new vec3d[steps+10];
	//vector<TFACE*> quads;
	vec3d up, down;
	vec3d begin_cnt;


	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			pov_baseobject::begin_dots();
			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();
			if (sweep_type==LINEAR_SWEEP)
			{
				for(i=0; i<number_of_spline; i++)
				{
					calculate_spline(pts, i*12);
					for (j=0; j<steps; j++)
					{
						glBegin(GL_LINES);
						glVertex3d(pts[j][0],heights[0], pts[j][1]);
						glVertex3d(pts[j+1][0],heights[0], pts[j+1][1]);
						glVertex3d(pts[j][0],heights[1], pts[j][1]);
						glVertex3d(pts[j+1][0],heights[1], pts[j+1][1]);
						glVertex3d(pts[j][0],heights[0], pts[j][1]);
						glVertex3d(pts[j][0],heights[1], pts[j][1]);
						glEnd();
					}
				}
			}
			else
			{ //CONIC_SWEEP
				for(i=0; i<number_of_spline; i++)
				{
					calculate_spline(pts, i*12);
					for (j=0; j<steps; j++)
					{
						glBegin(GL_LINES);
						glVertex3d(pts[j][0]*heights[0], heights[0], pts[j][1]*heights[0]);
						glVertex3d(pts[j+1][0]*heights[0], heights[0], pts[j+1][1]*heights[0]);
						glVertex3d(pts[j][0]*heights[1], heights[1], pts[j][1]*heights[1]);
						glVertex3d(pts[j+1][0]*heights[1], heights[1], pts[j+1][1]*heights[1]);
						glVertex3d(pts[j][0]*heights[0], heights[0], pts[j][1]*heights[0]);
						glVertex3d(pts[j][0]*heights[1], heights[1], pts[j][1]*heights[1]);
						glEnd();
					}
				}
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			color.gl_color();
			globj->show_flat_faces();

			unsigned int ppts_count;
			unsigned int bc_index;
			up = vec3d(0, 1, 0);
			down = vec3d(0, -1, 0);
			GLUtesselator *pTess;
			//VECTOR6D *p;
			unsigned int total_points;
			total_points = number_of_spline*steps*2;

			vec3d *ppts;
			ppts = new vec3d[total_points];

			ppts_count=0;
			for(i=0; i<number_of_spline; i++)
			{
				calculate_spline3dv(pts, i*12, heights[1], pts3dv);
				for (j=0; j<=steps; j++)
				{
					ppts[ppts_count++] = pts3dv[j];
				}
			}

			// front side
			glFrontFace(GL_CCW);
			pTess = gluNewTess();
			gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
			gluTessCallback(pTess, GLU_TESS_END, glEnd);
			gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &normal_with_vertex);
			//gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &normal_with_vertex);
			gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);
			gluTessCallback(pTess, GLU_TESS_COMBINE, (GLvoid (*) ()) &tessCombine);
			gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

			begin_cnt = ppts[0];
			bc_index = 0;

			gluTessBeginPolygon(pTess, NULL);
			gluTessBeginContour(pTess);

			for (i=0; i<(ppts_count); i++)
			{

				if ((begin_cnt == ppts[i])&&(i!=bc_index))
				{
					gluTessEndContour(pTess);
					bc_index = i+1;
					begin_cnt = ppts[i+1];
					if (bc_index<(ppts_count))
						gluTessBeginContour(pTess);
				}
				else
				{
//					p = new VECTOR6D[1];
//					Assign_Vector6d(*p, ppts[i][0], ppts[i][1], ppts[i][2], 0, 1, 0);
//					gluTessVertex(pTess, *p, *p);
					DBL* p = new DBL[6];
					p[0] = ppts[i][0];
					p[1] = ppts[i][1];
					p[2] = ppts[i][2];
					p[3] = up[0];
					p[4] = up[1];
					p[5] = up[2];
//					gluTessNormal(pTess, 0, -1, 0);
					gluTessVertex(pTess, p, p);
				}
			}
			gluTessEndPolygon(pTess);
			gluDeleteTess(pTess);

			// another side
			glFrontFace(GL_CW);
			ppts_count=0;
			for(i=0; i<number_of_spline; i++)
			{
				calculate_spline3dv(pts, i*12, heights[0], pts3dv);
				for (j=0; j<=steps; j++)
				{
					ppts[ppts_count++] = pts3dv[j];
				}
			}

			pTess = gluNewTess();
			gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
			gluTessCallback(pTess, GLU_TESS_END, glEnd);
			gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &normal_with_vertex);
			gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);
			gluTessCallback(pTess, GLU_TESS_COMBINE, (GLvoid (*) ()) &tessCombine);
			gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

			begin_cnt = ppts[0];
			bc_index = 0;

			gluTessBeginPolygon(pTess, NULL);
			gluTessBeginContour(pTess);

			for (i=0; i<(ppts_count); i++)
			{

				if ((begin_cnt == ppts[i])&&(i!=bc_index))
				{
					gluTessEndContour(pTess);
					bc_index = i+1;
					begin_cnt = ppts[i+1];
					if (bc_index<(ppts_count))
						gluTessBeginContour(pTess);
				}
				else
				{
					DBL* p = new DBL[6];
					p[0] = ppts[i][0];
					p[1] = ppts[i][1];
					p[2] = ppts[i][2];
					p[3] = down[0];
					p[4] = down[1];
					p[5] = down[2];
//					gluTessNormal(pTess, 0, -1, 0);
					gluTessVertex(pTess, p, p);
				}
			}
			gluTessEndPolygon(pTess);
			gluDeleteTess(pTess);
			glFrontFace(GL_CCW);
			delete [] ppts;
		break;

		case NORMALS:
			globj->show_normals(scene->get_normals_size());
		break;

		default:
		break;
	}
	delete [] pts;
	delete [] pts3dv;
}
*/

int pov_prism::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	fread(&number_of_spline, sizeof(number_of_spline), 1, f);
	fread(&spline_type, sizeof(spline_type), 1, f);
	fread(&sweep_type, sizeof(sweep_type), 1, f);
	fread(heights, sizeof(DBL), 2, f);
	readden += sizeof(number_of_spline) + sizeof(spline_type) + sizeof(sweep_type) + sizeof(DBL)*2;
	spline = new DBL[number_of_spline*12];
	fread(spline, sizeof(DBL), number_of_spline*12, f);
	readden += sizeof(DBL)*number_of_spline*12;
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_prism::tesselate()
{
	unsigned int ppts_count = 0;
	unsigned int bc_index = 0;
	vec3d begin_cnt;
//	vec3d up, down;
//	up = vec3d(0, 1, 0);
//	down = vec3d(0, -1, 0);
	unsigned int i, j;
	unsigned int steps = scene->get_prism_spline_steps();
	unsigned int total_points = number_of_spline*steps*2;
	vec3d *ppts;
	ppts = new vec3d[total_points];
	vec2d *pts;
	pts = new vec2d[steps+10];
	vec3d *pts3dv;
	pts3dv = new vec3d[steps+10];
	array_vec3d contour1, contour2;
	for(i=0; i<number_of_spline; i++)
	{
		calculate_spline3dv(pts, i*12, heights[1], pts3dv);
		for (j=0; j<=steps; j++)
		{
			ppts[ppts_count++] = pts3dv[j];
		}
	}
	// quad strip
	begin_cnt = ppts[0];
	bc_index = 0;
	DBL h0=1, h1=1;
	if(sweep_type==2)
	{
		h0 = heights[0];
		h1 = heights[1];
		DEBUG_MSG("h0 = "<<h0<<", h1 = "<<h1)
		if (h0==0) h0=1e-6;
		if (h1==0) h1=1e-6;
	}
	for (i=0; i<(ppts_count-1); i++)
	{
		if ((begin_cnt == ppts[i])&&(i!=bc_index))
		{
			bc_index = i+1;
			begin_cnt = ppts[i+1];
			contour1.push_back(vec3d(ppts[i][0]*h0, heights[0], ppts[i][2]*h0));
			contour2.push_back(vec3d(ppts[i][0]*h1, heights[1], ppts[i][2]*h1));
		}
		else
		{
			globj->append_face(
				vec3d(ppts[i][0]*h1, heights[1], ppts[i][2]*h1),
				vec3d(ppts[i][0]*h0, heights[0], ppts[i][2]*h0),
				vec3d(ppts[i+1][0]*h0, heights[0], ppts[i+1][2]*h0),
				vec3d(ppts[i+1][0]*h1, heights[1], ppts[i+1][2]*h1)
			);
			contour1.push_back(vec3d(ppts[i][0]*h0, heights[0], ppts[i][2]*h0));
			contour2.push_back(vec3d(ppts[i][0]*h1, heights[1], ppts[i][2]*h1));
		}
	}
	globj->append_contour(contour1, vec3d(0, -1, 0));
	globj->append_contour(contour2, vec3d(0, 1, 0));
	delete [] pts;
	delete [] pts3dv;
	delete [] ppts;
}

