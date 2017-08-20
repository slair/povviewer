#include "pov_baseobject.hpp"
#include "pov_bicubicpatch.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_bicubicpatch::pov_bicubicpatch(pov_scene* s):pov_baseobject(s)
{
	tag = 0x41504942; // BIPA
	for(int i=0; i<4; i++)
	{
		patch[i][0]=vec3d(0, 0, 0);
		patch[i][1]=vec3d(0, 0, 0);
		patch[i][2]=vec3d(0, 0, 0);
		patch[i][3]=vec3d(0, 0, 0);
	}
}

pov_bicubicpatch::~pov_bicubicpatch()
{
}

/*
void pov_bicubicpatch::update(TViewMode mode)
{
	DEBUG_MSG("pov_bicubicpatch::update()")
	int i;
	if (mode != BBOX)
		pov_baseobject::update();

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			color.gl_color();
			glPointSize(scene->get_point_size()*point_scale);
			glBegin(GL_POINTS);
			for(i=0; i<4; i++)
			{
				patch[i][0].gl_vertex();
				patch[i][1].gl_vertex();
				patch[i][2].gl_vertex();
				patch[i][3].gl_vertex();
			}
			glEnd();
			glPointSize(scene->get_point_size());
		break;

		case LINES:
			color.gl_color();
			for(i=0; i<4; i++)
			{
				glBegin(GL_LINE_STRIP);
				glVertex3dv(patch[i][0]);
				glVertex3dv(patch[i][1]);
				glVertex3dv(patch[i][2]);
				glVertex3dv(patch[i][3]);
				glEnd();
			}
			for(i=0; i<4; i++)
			{
				glBegin(GL_LINE_STRIP);
				glVertex3dv(patch[0][i]);
				glVertex3dv(patch[1][i]);
				glVertex3dv(patch[2][i]);
				glVertex3dv(patch[3][i]);
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			glColor3dv(color);
			glFrontFace(GL_CW);
			glBegin(GL_QUADS);
			glVertex3dv(patch[0][0]);
			glVertex3dv(patch[0][1]);
			glVertex3dv(patch[1][1]);
			glVertex3dv(patch[1][0]);

			glVertex3dv(patch[1][0]);
			glVertex3dv(patch[1][1]);
			glVertex3dv(patch[2][1]);
			glVertex3dv(patch[2][0]);

			glVertex3dv(patch[2][0]);
			glVertex3dv(patch[2][1]);
			glVertex3dv(patch[3][1]);
			glVertex3dv(patch[3][0]);

			glVertex3dv(patch[0][1]);
			glVertex3dv(patch[0][2]);
			glVertex3dv(patch[1][2]);
			glVertex3dv(patch[1][1]);

			glVertex3dv(patch[0][2]);
			glVertex3dv(patch[0][3]);
			glVertex3dv(patch[1][3]);
			glVertex3dv(patch[1][2]);

			glVertex3dv(patch[1][1]);
			glVertex3dv(patch[1][2]);
			glVertex3dv(patch[2][2]);
			glVertex3dv(patch[2][1]);

			glVertex3dv(patch[1][2]);
			glVertex3dv(patch[1][3]);
			glVertex3dv(patch[2][3]);
			glVertex3dv(patch[2][2]);

			glVertex3dv(patch[2][1]);
			glVertex3dv(patch[2][2]);
			glVertex3dv(patch[3][2]);
			glVertex3dv(patch[3][1]);

			glVertex3dv(patch[2][2]);
			glVertex3dv(patch[2][3]);
			glVertex3dv(patch[3][3]);
			glVertex3dv(patch[3][2]);
			glEnd();
			glFrontFace(GL_CCW);
			// inner side
			glColor3dv(inner_color);
			glBegin(GL_QUADS);
			glVertex3dv(patch[0][0]);
			glVertex3dv(patch[0][1]);
			glVertex3dv(patch[1][1]);
			glVertex3dv(patch[1][0]);

			glVertex3dv(patch[1][0]);
			glVertex3dv(patch[1][1]);
			glVertex3dv(patch[2][1]);
			glVertex3dv(patch[2][0]);

			glVertex3dv(patch[2][0]);
			glVertex3dv(patch[2][1]);
			glVertex3dv(patch[3][1]);
			glVertex3dv(patch[3][0]);

			glVertex3dv(patch[0][1]);
			glVertex3dv(patch[0][2]);
			glVertex3dv(patch[1][2]);
			glVertex3dv(patch[1][1]);

			glVertex3dv(patch[0][2]);
			glVertex3dv(patch[0][3]);
			glVertex3dv(patch[1][3]);
			glVertex3dv(patch[1][2]);

			glVertex3dv(patch[1][1]);
			glVertex3dv(patch[1][2]);
			glVertex3dv(patch[2][2]);
			glVertex3dv(patch[2][1]);

			glVertex3dv(patch[1][2]);
			glVertex3dv(patch[1][3]);
			glVertex3dv(patch[2][3]);
			glVertex3dv(patch[2][2]);

			glVertex3dv(patch[2][1]);
			glVertex3dv(patch[2][2]);
			glVertex3dv(patch[3][2]);
			glVertex3dv(patch[3][1]);

			glVertex3dv(patch[2][2]);
			glVertex3dv(patch[2][3]);
			glVertex3dv(patch[3][3]);
			glVertex3dv(patch[3][2]);
			glEnd();
		break;

		case NORMALS:
		break;

		default:
		break;
	}
}
*/

int pov_bicubicpatch::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			readden+=patch[i][j].read(f);
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_bicubicpatch::tesselate()
{
//	globj->append_face(patch[0][0], patch[0][1], patch[1][1], patch[1][0]);
//	globj->append_face(patch[1][0], patch[1][1], patch[2][1], patch[2][0]);
//	globj->append_face(patch[2][0], patch[2][1], patch[3][1], patch[3][0]);
//	globj->append_face(patch[0][1], patch[0][2], patch[1][2], patch[1][1]);
//	globj->append_face(patch[0][2], patch[0][3], patch[1][3], patch[1][2]);
//	globj->append_face(patch[1][1], patch[1][2], patch[2][2], patch[2][1]);
//	globj->append_face(patch[1][2], patch[1][3], patch[2][3], patch[2][2]);
//	globj->append_face(patch[2][1], patch[2][2], patch[3][2], patch[3][1]);
//	globj->append_face(patch[2][2], patch[2][3], patch[3][3], patch[3][2]);
//	globj->append_patch(patch);
	triangulate_patch(scene->get_patch_resolution());
}

/** @brief triangulate_patch
  *
  * @todo: document this function
  */
void pov_bicubicpatch::triangulate_patch(unsigned int divs)
{
	unsigned int u = 0;
	unsigned int v = 0;
	float		py, px;	//, pyold;
	vec3d	temp[4];
	vec3d	*last = new vec3d[divs+1];//(POINT_3D*)malloc(sizeof(POINT_3D)*(divs+1));

//	patch[0][0] = vec3d(a.data[0][0][0],a.data[0][0][1],a.data[0][0][2]);
//	patch[0][1] = vec3d(a.data[0][1][0],a.data[0][1][1],a.data[0][1][2]);
//	patch[0][2] = vec3d(a.data[0][2][0],a.data[0][2][1],a.data[0][2][2]);
//	patch[0][3] = vec3d(a.data[0][3][0],a.data[0][3][1],a.data[0][3][2]);
//
//	patch[1][0] = vec3d(a.data[1][0][0],a.data[1][0][1],a.data[1][0][2]);
//	patch[1][1] = vec3d(a.data[1][1][0],a.data[1][1][1],a.data[1][1][2]);
//	patch[1][2] = vec3d(a.data[1][2][0],a.data[1][2][1],a.data[1][2][2]);
//	patch[1][3] = vec3d(a.data[1][3][0],a.data[1][3][1],a.data[1][3][2]);
//
//	patch[2][0] = vec3d(a.data[2][0][0],a.data[2][0][1],a.data[2][0][2]);
//	patch[2][1] = vec3d(a.data[2][1][0],a.data[2][1][1],a.data[2][1][2]);
//	patch[2][2] = vec3d(a.data[2][2][0],a.data[2][2][1],a.data[2][2][2]);
//	patch[2][3] = vec3d(a.data[2][3][0],a.data[2][3][1],a.data[2][3][2]);
//
//	patch[3][0] = vec3d(a.data[3][0][0],a.data[3][0][1],a.data[3][0][2]);
//	patch[3][1] = vec3d(a.data[3][1][0],a.data[3][1][1],a.data[3][1][2]);
//	patch[3][2] = vec3d(a.data[3][2][0],a.data[3][2][1],a.data[3][2][2]);
//	patch[3][3] = vec3d(a.data[3][3][0],a.data[3][3][1],a.data[3][3][2]);


	temp[0] = patch[0][3];	//vec3d(a.data[0][3][0],a.data[0][3][1],a.data[0][3][2]);
	temp[1] = patch[1][3];	//vec3d(a.data[1][3][0],a.data[1][3][1],a.data[1][3][2]);
	temp[2] = patch[2][3];	//vec3d(a.data[2][3][0],a.data[2][3][1],a.data[2][3][2]);
	temp[3] = patch[3][3];	//vec3d(a.data[3][3][0],a.data[3][3][1],a.data[3][3][2]);

	for (v=0;v<=divs;v++)
	{
		/*  create the first line of points */
		px = ((float)v)/((float)divs);			/*  percent along y axis */
		/*  use the 4 points from the derives curve to calculate the points along that curve */
		last[v] = bernstein(px, temp);
	}

	vec3d A, B, C, D;

	for (u=1;u<=divs;u++)
	{
		py	  = ((float)u)/((float)divs);			/*  Percent along Y axis */
		//~ pyold = ((float)u-1.0f)/((float)divs);		/*  Percent along old Y axis */

		temp[0] = bernstein(py, patch[0]);	/*  Calculate new bezier points */
		temp[1] = bernstein(py, patch[1]);
		temp[2] = bernstein(py, patch[2]);
		temp[3] = bernstein(py, patch[3]);

		//glBegin(GL_TRIANGLE_STRIP);					/*  Begin a new triangle strip */
		for (v=0;v<=divs;v++)
		{
			px = ((float)v)/((float)divs);			/*  Percent along the X axis */

			if (v>0)
				C = A;
			A = last[v];	//glVertex3d(last[v][0], last[v][1], last[v][2]);	/*  Old Point */

			last[v] = bernstein(px, temp);			/*  Generate new point */
			if (v>0)
				D = B;
			B = last[v];	//glVertex3d(last[v][0], last[v][1], last[v][2]);	/*  New Point */
			if (v>0)
			{
				// 2 triangle ABC and CBD
				globj->append_face(A, B, C);
				globj->append_face(C, B, D);
			}
		}

		//glEnd();									/*  END the triangle srip */
	}
}

/** @brief PATCH
  *
  * @todo: document this function
  */
/*
 PATCH::PATCH()
{
	memset(data, 0, 4*4*3*sizeof(GLfloat));
}
*/

/** @brief bernstein
  *
  * @todo: document this function
  */
vec3d pov_bicubicpatch::bernstein(float u, vec3d *p)
{
	vec3d	a, b, c, d, r;

	a = p[0]*pow(u,3);
	b = p[1]*3*pow(u,2)*(1-u);
	c = p[2]*3*u*pow((1-u),2);
	d = p[3]*pow((1-u),3);

	r = ((a+b)+(c+d));

	return r;
}

