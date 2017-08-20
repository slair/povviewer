#include <stdio.h>
#include <iostream>
#include <GL/gl.h>

#include "vec3d.hpp"
#include "gl_object.hpp"
#include "pov_transform.hpp"
#include "pov_baseobject.hpp"
#include "pov_scene.hpp"

class pov_scene;
class gl_object;
/*
void pov_baseobject::fix_inner_color()
{
	if(config!=NULL)
	{
		vec3d tmp;
		config->get_inner_color_scale(tmp);
//		VMul3d(inner_color, color, tmp);
		inner_color = color * tmp;
//		VNormalize3d(inner_color);
		inner_color = inner_color.normalize();
	}
	else
	{
		// error!!!
	}
}
*/
//void pov_baseobject::set_scene(pov_scene* s)
//{
//	DEBUG_MSG("pov_baseobject::set_scene()")
//	scene = s;
//	//config = cfg;
//	fix_inner_color();
//}

pov_baseobject::pov_baseobject(pov_scene* s)
{
	tag = 0x45534142;	/* BASE */
	transform = NULL;
	color = vec3d(1);
	memset(colour, 0, 5*sizeof(SNGL));
	min_vc = 0.3; //defaultvalue
	min_visual_color = vec3d(0.8, 0.9, 0.7); //defaultvalue
	scene = s;
	point_size = 1; //defaultvalue
	point_scale = 4; //defaultvalue
	globj = new gl_object(this);
	is_sphere = false;
	is_blob = false;
	is_smoothed = false;
}
/*
pov_baseobject::pov_baseobject(pov_scene* s, povviewer_config* cfg)
{
	pov_baseobject::pov_baseobject();
	set_scene(s);
	set_config(cfg);
}
*/

pov_baseobject::~pov_baseobject()
{
//	if (transform!=NULL)
//	DEBUG_MSG("	~pov_baseobject "<<hex<<tag<<dec)
	delete transform;
	delete globj;
}

void pov_baseobject::update() const
{
	//DEBUG_MSG("pov_baseobject::update()")
	glLoadIdentity();
	if (transform!=NULL)
	{
		transform->apply();
		//DEBUG_MSG("applying transform")
	}
	glDisable(GL_LIGHTING);
	//DEBUG_MSG("disable lighting")
}

void pov_baseobject::draw_bbox() const
{
	SNGL X1 = bbox_corners[0][0];	SNGL X2 = bbox_corners[1][0];
	SNGL Y1 = bbox_corners[0][1];	SNGL Y2 = bbox_corners[1][1];
	SNGL Z1 = bbox_corners[0][2];	SNGL Z2 = bbox_corners[1][2];
	glDisable(GL_LIGHTING);
	//DEBUG_MSG("disable lighting")
	color.gl_color();
	// Bottom face
	glBegin(GL_LINE_LOOP);
	glVertex3f(X1, Y1, Z1);
	glVertex3f(X1, Y1, Z2);
	glVertex3f(X2, Y1, Z2);
	glVertex3f(X2, Y1, Z1);
	glEnd();
	// Top face
	glBegin(GL_LINE_LOOP);
	glVertex3f(X1, Y2, Z1);
	glVertex3f(X1, Y2, Z2);
	glVertex3f(X2, Y2, Z2);
	glVertex3f(X2, Y2, Z1);
	glEnd();
	// Lines connecting faces
	glBegin(GL_LINES);
	glVertex3f(X1, Y1, Z1);
	glVertex3f(X1, Y2, Z1);
	glVertex3f(X2, Y1, Z2);
	glVertex3f(X2, Y2, Z2);
	glVertex3f(X1, Y1, Z2);
	glVertex3f(X1, Y2, Z2);
	glVertex3f(X2, Y1, Z1);
	glVertex3f(X2, Y2, Z1);
	glEnd();
}

int pov_baseobject::read(FILE *f)
{
	unsigned int tag = 0;
	fread(&tag, sizeof(tag), 1, f);
	int readden = 4;
	if (tag==0x534e5254)	/* TRNS */
	{
		if (transform==NULL)
			transform = new pov_transform();
		readden += transform->read(f);
		if (is_blob)
			transform->clear();
	}

	fread(&tag, sizeof(tag), 1, f);
	readden += 4;
	if (tag==0x584f4242)	/* BBOX */
	{
		fread(bbox_corners, sizeof(SNGL)*3, 2, f);
		readden += sizeof(SNGL)*3*2;
	}

	fread(&tag, sizeof(tag), 1, f);
	readden += 4;
	if (tag==0x524C4F43) /* COLR */
	{
		fread(colour, sizeof(colour), 1, f);
		readden += sizeof(colour);
		color = vec3d(colour[0], colour[1], colour[2]);
		if (color.length() < min_vc) // check for very dark colors
		{
			color = min_visual_color;
		}
	}
	return readden;
}

void pov_baseobject::begin_dots() const
{
	vec3d dots_color = vec3d(1.0) - color;
//	color.gl_color();
	dots_color.gl_color();
	glPointSize(point_size*point_scale);
}

void pov_baseobject::end_dots() const
{
	glPointSize(point_size);
}

/** @brief get_scale_vector
  *
  * @todo: document this function
  */
vec3d pov_baseobject::get_scale_vector() const
{
	if (transform!=NULL)
		return transform->get_scale();
	return vec3d(1);
}

/** @brief generate_gllist
  *
  * @todo: document this function
  */
void pov_baseobject::generate_gllist(unsigned int mode)
{
	//~ DEBUG_MSG("pov_baseobject::generate_gllist("<<mode<<")")
	if ((mode != BBOX))
	{
		pov_baseobject::update();
		if (is_sphere)
		{
			DBL *m = new DBL[16];
			memset (m, 0, 16*sizeof(DBL));
			m[0] = 1; m[5] = 1; m[10] = 1; m[15] = 1;
			m[12] = sphere_center[0];
			m[13] = sphere_center[1];
			m[14] = sphere_center[2];
			glMultMatrixd(m);
			delete [] m;
		}
	}
	switch (mode)
	{
		case BBOX:
			glDisable(GL_LIGHTING);
			//DEBUG_MSG("disable lighting")
			pov_baseobject::draw_bbox();
		break;

		case DOTS:
			pov_baseobject::begin_dots();
			glDisable(GL_LIGHTING);
			//DEBUG_MSG("disable lighting")
			globj->show_dots();
			pov_baseobject::end_dots();
		break;

		case LINES:
			(vec3d(1.0)-color).gl_color();
			//color.gl_color();
			glDisable(GL_LIGHTING);
			//DEBUG_MSG("disable lighting")
			//~ glLineWidth(2);
			globj->show_lines();
			globj->show_line_contours();
			//~ glLineWidth(1);
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
				//DEBUG_MSG("enable lighting")
			if (tag==0x4e414c50) // planes
			{
//				glPushAttrib(GL_COLOR_BUFFER_BIT);
				glPushAttrib(GL_ENABLE_BIT);
//				glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glEnable(GL_POLYGON_SMOOTH);
				glEnable(GL_BLEND);
				glColor4d(color[0], color[1], color[2], scene->get_plane_alpha());
			}
			else // other objects, not planes
			{
				color.gl_color();
			}
			if (scene->get_smooth_obj())
			{
				if (!is_smoothed)
					globj->smooth(scene->get_smooth_angle()*RAD);
			}
			if (is_smoothed)
				globj->show_smooth_faces();
			else
				globj->show_flat_faces();
			globj->show_contours();
			
			glDisable(GL_LIGHTING);
			globj->show_lines_with_faces();
			if (tag==0x4e414c50) // planes
			{
				glPopAttrib();
			}
//			globj->show_patches();
			//get_center().draw_star(1.1);
		break;

		case NORMALS:
			glDisable(GL_LIGHTING);
			//DEBUG_MSG("disable lighting")
			if (is_smoothed)
				globj->show_smooth_normals(scene->get_normals_size());
			else
				globj->show_normals(scene->get_normals_size());
			//~ DEBUG_MSG("breakpoint")
		break;

		default:
		break;
	}
}

/** @brief get_stats
  *
  * @todo: document this function
  */
GLOBJ_STATS pov_baseobject::get_stats() const
{
	GLOBJ_STATS a = globj->get_globj_stats();
	return a;
}

/** @brief set_smoothed
  *
  * @todo: document this function
  */
void pov_baseobject::set_smoothed(const bool value)
{
	is_smoothed = value;
}

/** @brief get_scene
  *
  * @todo: document this function
  */
pov_scene * pov_baseobject::get_scene()
{
	return scene;
}

/** @brief get_center
  *
  * @todo: document this function
  */
vec3d pov_baseobject::get_center() const
{
	vec3d oc = globj->get_object_center();
	if (transform!=NULL)
	{
		//DEBUG_MSG("apply transform to object center")
		//transform->print(cout);
		oc = transform->transform_vec3d(oc);
	}
	if (is_sphere)
	{
		oc = oc + vec3d(sphere_center[0], sphere_center[1], sphere_center[2]);
	}

	return oc;
}

/** @brief get_type
  *
  * @todo: document this function
  */
string pov_baseobject::get_type() const
{
	switch (tag)
	{
		case 0x524d4143:    // CAMR
			return "camera";
		break;

		case 0x5448474c: // LGHT
			return "light_source";
		break;

		case 0x41495254: // TRIA
			return "triangle";
		break;

		case 0x454e4f43: // CONE
			return "cone";
		break;

		case 0x52485053: // SPHR
			return "sphere";
		break;

		case 0x5f465454: // TTF_
			return "ttf";
		break;

		case 0x534f5349: // ISOS
			return "isosurface";
		break;

		case 0x4854414c: // LATH
			return "lathe";
		break;

		case 0x4d535250: // PRSM
			return "prism";
		break;

		case 0x5f584f42: // BOX_
			return "box";
		break;

		case 0x4e414c50: // PLAN
			return "plane";
		break;

		case 0x53524f54: // TORS
			return "torus";
		break;

		case 0x41504942: // BIPA
			return "bicubic_patch";
		break;

		case 0x424f4c42: // BLOB
			return "blob";
		break;

		case 0x4853454d: // MESH
			return "mesh";
		break;

		case 0x444c4648: // HFLD
			return "heightfield";
		break;

		case 0x594c4f50: // POLY
			return "polygon";
		break;

		case 0x43534944: // DISC
			return "disc";
		break;

		case 0x44415551: // QUAD
			return "quadric";
		break;

		case 0x57535053: // SPSW
			return "sphere_sweep";
		break;

		default:
			return "unknown tag";
		break;
	}
	return "never return value";
}

/** @brief get_lines
  *
  * @todo: document this function
  */
void pov_baseobject::get_lines(vector_line3d* lines) const
{
	if (globj!=NULL)
	{
		pov_transform* trns = transform;
		if (trns==NULL)
		{
			trns = new pov_transform();
			trns->clear();
		}
		globj->get_lines(lines, trns);
	}
}

/** @brief get_lines
  *
  * @todo: document this function
  */
void pov_baseobject::get_lines(set_line3d* lines) const
{
	if (globj!=NULL)
	{
		pov_transform* trns = transform;
		if (trns==NULL)
		{
			trns = new pov_transform();
			trns->clear();
		}
		globj->get_lines(lines, trns);
	}
}

