#include <math.h>


#include "pov_baseobject.hpp"
#include "pov_lathe.hpp"
#include "pov_scene.hpp"
#include "vec2d.hpp"
#include "gl_object.hpp"

pov_lathe::pov_lathe(pov_scene* s):pov_baseobject(s)
{
	tag = 0x4854414c; // LATH
	spline_type = 0;
	number_of_splines = 0;
	radiuses[0] = 0;
	radiuses[1] = 0;
	heights[0] = 0;
	heights[1] = 0;
	spline = NULL;
}

pov_lathe::~pov_lathe()
{
//	if (spline != NULL)
	delete [] spline;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void pov_lathe::calculate_spline(vec2d* points, unsigned int index) const
{
	vec2d A, B, C, D;
	A = vec2d(spline[index], spline[index+1]);
	B = vec2d(spline[index+2], spline[index+3]);
	C = vec2d(spline[index+4], spline[index+5]);
	D = vec2d(spline[index+6], spline[index+7]);
	unsigned int steps = scene->get_lathe_spline_steps();
	DBL t = double(1)/double(steps);
	DBL r=0;
	for(unsigned int x=0; x<=steps; x++)
	{
		// A*r*r*r + B*r*r + C*r + D
		points[x] = vec2d(0, 0);
		points[x] = points[x] + A * r*r*r;
		points[x] = points[x] + B * r*r;
		points[x] = points[x] + C * r;
		points[x] = points[x] + D;
		r += t;
	}
}

/*
void pov_lathe::update(unsigned int mode)
{
	if (mode != BBOX)
		pov_baseobject::update();

	unsigned int longitudes = scene->get_lathe_num_longitudes();
	unsigned int numlong, i, j;
	DBL angle_step, angle, old_angle;
	unsigned int steps = scene->get_lathe_spline_steps();

	//TFACE quad;

	vec2d *pts;
	pts = new vec2d[steps+10];

	switch (mode)
	{
		case BBOX:
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			pov_baseobject::begin_dots();
			angle_step = PI2/double(longitudes);
			angle = 0;
			old_angle = 0;
			for(numlong=0; numlong<=longitudes; numlong++)
			{
				for(i=0; i<number_of_splines; i++)
				{
					calculate_spline(pts, i*8); // calculate points from coefficients
					for(j=0; j<steps; j++)
					{
						glBegin(GL_POINTS);
						glVertex3d(cos(angle)*pts[j][0], pts[j][1], sin(angle)*pts[j][0]);
						glVertex3d(cos(angle)*pts[j+1][0], pts[j+1][1], sin(angle)*pts[j+1][0]);
						glVertex3d(cos(angle)*pts[j][0], pts[j][1], sin(angle)*pts[j][0]);
						glVertex3d(cos(old_angle)*pts[j][0], pts[j][1], sin(old_angle)*pts[j][0]);
						glEnd();
					}
				}
				old_angle = angle;
				angle += angle_step;
			}
			pov_baseobject::end_dots();
		break;

		case LINES:
			color.gl_color();
			angle_step = PI2/double(longitudes);
			angle = 0;
			old_angle = 0;
			for(numlong=0; numlong<=longitudes; numlong++)
			{
				for(i=0; i<number_of_splines; i++)
				{
					calculate_spline(pts, i*8); // calculate points from coefficients
					for(j=0; j<steps; j++)
					{
						glBegin(GL_LINES);
						glVertex3d(cos(angle)*pts[j][0], pts[j][1], sin(angle)*pts[j][0]);
						glVertex3d(cos(angle)*pts[j+1][0], pts[j+1][1], sin(angle)*pts[j+1][0]);
						glVertex3d(cos(angle)*pts[j][0], pts[j][1], sin(angle)*pts[j][0]);
						glVertex3d(cos(old_angle)*pts[j][0], pts[j][1], sin(old_angle)*pts[j][0]);
						glEnd();
					}
				}
				old_angle = angle;
				angle += angle_step;
			}
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

	delete [] pts;
}
*/

int pov_lathe::read(FILE *f)
{
	int readden = pov_baseobject::read(f);
	fread(&spline_type, sizeof(spline_type), 1, f);
	fread(&number_of_splines, sizeof(number_of_splines), 1, f);
	fread(radiuses, sizeof(radiuses), 1, f);
	fread(heights, sizeof(heights), 1, f);
	readden += sizeof(spline_type) + sizeof(number_of_splines) + sizeof(radiuses) + sizeof(heights);
	spline = new DBL[8*number_of_splines];
	fread(spline, sizeof(DBL)*number_of_splines*8, 1, f);
	readden += sizeof(DBL)*number_of_splines*8;
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_lathe::tesselate() const
{
	unsigned int longitudes = scene->get_lathe_num_longitudes();
	unsigned int numlong, i, j;
	DBL angle_step, angle, old_angle;
	unsigned int steps = scene->get_lathe_spline_steps();

	//TFACE quad;

	vec2d *pts;
	pts = new vec2d[steps+10];

	angle_step = PI2/double(longitudes);
	angle = 0;
	old_angle = 0;
	for(numlong=0; numlong<=longitudes; numlong++)
	{
		for(i=0; i<number_of_splines; i++)
		{
			calculate_spline(pts, i*8); // calculate points from coefficients
			for(j=0; j<steps; j++)
			{
				globj->append_face(
					vec3d(cos(angle)*pts[j][0], pts[j][1], sin(angle)*pts[j][0]),
					vec3d(cos(angle)*pts[j+1][0], pts[j+1][1], sin(angle)*pts[j+1][0]),
					vec3d(cos(old_angle)*pts[j+1][0], pts[j+1][1], sin(old_angle)*pts[j+1][0]),
					vec3d(cos(old_angle)*pts[j][0], pts[j][1], sin(old_angle)*pts[j][0])
				);
			}
		}
		old_angle = angle;
		angle += angle_step;
	}
	delete [] pts;
}

