//#include "vec3d.hpp"

#include "GL/gl.h"
#include <GL/glx.h>

#include "pov_baseobject.hpp"
#include "pov_scene.hpp"
#include "pov_camera.hpp"

pov_camera::pov_camera(pov_scene* s):pov_baseobject(s)
{
	tag = 0x524d4143;	/* CAMR */
	//~ pov_baseobject::pov_baseobject();
	location = vec3d(0, 0, 0);
	direction = vec3d(0, 0, 1);
	up = vec3d(0, 1, 0);
	right = vec3d(1.333, 0, 0);
	sky = vec3d(0, 1, 0);
	look_at = vec3d(0, 0, 1);
	angle = 90;
}

vec3d pov_camera::get_location()
{
	return location;
}

vec3d pov_camera::get_sky()
{
	return sky;
}

vec3d pov_camera::get_look_at()
{
	return look_at;
}

DBL pov_camera::get_angle()
{
	return angle;
}

pov_camera::~pov_camera()
{
}

int pov_camera::read(FILE *f)
{
	unsigned int readden = 0;

	fread(&type, sizeof(type), 1, f);
	readden = sizeof(type);

	fread(&h_angle, sizeof(h_angle), 1, f);
	readden += sizeof(h_angle);

	fread(&v_angle, sizeof(v_angle), 1, f);
	readden += sizeof(v_angle);

	readden += location.read(f);
	readden += direction.read(f);
	readden += up.read(f);
	readden += right.read(f);
	readden += sky.read(f);
	readden += look_at.read(f);

	fread(&angle, sizeof(angle), 1, f);
	readden += sizeof(angle);

//	direction = look_at - location;
	#ifdef DEBUG
	dump(cout);
	#endif
	return readden;
}

/** @brief dump
  *
  * @todo: document this function
  */
void pov_camera::dump(std::ostream& os) const
{
	os << "\npov_camera.type " << type << std::endl;

	os << "pov_camera.location <" << location[0] << ", " << location[1] << ", " << location[2]
		<< ">" << std::endl;

	os << "pov_camera.direction <" << direction[0] << ", " << direction[1] << ", " <<
		direction[2] << ">" << " length:" << direction.length() << std::endl;

	os << "pov_camera.up <" << up[0] << ", " << up[1] << ", " << up[2] << ">" << " length:" <<
		up.length() << std::endl;

	os << "pov_camera.right <" << right[0] << ", " << right[1] << ", " << right[2] << ">" <<
		" length:" << right.length() << std::endl;

	os << "pov_camera.sky <" << sky[0] << ", " << sky[1] << ", " << sky[2] << ">" << std::endl;

	os << "pov_camera.look_at <" << look_at[0] << ", " << look_at[1] << ", " << look_at[2] << ">"
		<< std::endl;

	os << "pov_camera.angle " << angle << std::endl << std::endl;
}

/** @brief get_right
  *
  * @todo: document this function
  */
vec3d pov_camera::get_right()
{
	return right;
}

/** @brief get_up
  *
  * @todo: document this function
  */
vec3d pov_camera::get_up()
{
	return up;
}

/** @brief get_direction
  *
  * @todo: document this function
  */
vec3d pov_camera::get_direction()
{
	return direction;
}

/** @brief draw
  *
  * @todo: document this function
  */
void pov_camera::draw() const
{
	if (scene->get_show_camera())
	{
		if (type==PERSPECTIVE_CAMERA)
		{
			DBL hor_view_length = direction.length()*tan(angle/2.0*PI/180.0);
			DBL cam_ratio = right.length()/up.length();
			DBL ver_view_length = hor_view_length/cam_ratio;

			DBL persp_frustum_size = scene->get_frustum_size();

			vec3d up_norm = up.normalize();
			vec3d right_norm = right.normalize();
			vec3d dir_norm = direction.normalize();

			vec3d p0 = location+direction;
			vec3d p1 = p0 - right_norm*hor_view_length + up_norm*ver_view_length;
			vec3d p2 = p0 + right_norm*hor_view_length + up_norm*ver_view_length;
			vec3d p3 = p0 + right_norm*hor_view_length - up_norm*ver_view_length;
			vec3d p4 = p0 - right_norm*hor_view_length - up_norm*ver_view_length;

			vec3d p1_dir = (p1 - location).normalize();
			vec3d p2_dir = (p2 - location).normalize();
			vec3d p3_dir = (p3 - location).normalize();
			vec3d p4_dir = (p4 - location).normalize();

			vec3d p1_ = p1 + p1_dir*persp_frustum_size;
			vec3d p2_ = p2 + p2_dir*persp_frustum_size;
			vec3d p3_ = p3 + p3_dir*persp_frustum_size;
			vec3d p4_ = p4 + p4_dir*persp_frustum_size;

			glLoadIdentity();
			glDisable(GL_LIGHTING);

			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			// draw quads
			glPushAttrib(GL_ENABLE_BIT);
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glEnable(GL_POLYGON_SMOOTH);
			glEnable(GL_BLEND);
			glColor4d(0.7, 0.7, 0.7, 0.3);

			glFrontFace(GL_CW);
			glBegin(GL_QUADS);
			p1.gl_vertex();p2.gl_vertex();p3.gl_vertex();p4.gl_vertex();
			p1_.gl_vertex();p2_.gl_vertex();p3_.gl_vertex();p4_.gl_vertex();
			p1.gl_vertex();p1_.gl_vertex();p4_.gl_vertex();p4.gl_vertex();
			p1.gl_vertex();p2.gl_vertex();p2_.gl_vertex();p1_.gl_vertex();
			p2.gl_vertex();p3.gl_vertex();p3_.gl_vertex();p2_.gl_vertex();
			p3.gl_vertex();p4.gl_vertex();p4_.gl_vertex();p3_.gl_vertex();
			p4.gl_vertex();p4_.gl_vertex();p1_.gl_vertex();p1.gl_vertex();
			glEnd();

			glFrontFace(GL_CCW);
			glBegin(GL_QUADS);
			p1.gl_vertex();p2.gl_vertex();p3.gl_vertex();p4.gl_vertex();
			p1_.gl_vertex();p2_.gl_vertex();p3_.gl_vertex();p4_.gl_vertex();
			p1.gl_vertex();p1_.gl_vertex();p4_.gl_vertex();p4.gl_vertex();
			p1.gl_vertex();p2.gl_vertex();p2_.gl_vertex();p1_.gl_vertex();
			p2.gl_vertex();p3.gl_vertex();p3_.gl_vertex();p2_.gl_vertex();
			p3.gl_vertex();p4.gl_vertex();p4_.gl_vertex();p3_.gl_vertex();
			p4.gl_vertex();p4_.gl_vertex();p1_.gl_vertex();p1.gl_vertex();
			glEnd();

			glPopAttrib();
			glDisable(GL_POLYGON_OFFSET_FILL);

			direction.draw_vector_a(location, vec3d(0,0,1));
			right.draw_vector_a(p0, vec3d(1,0,0));
			up.draw_vector_a(p0, vec3d(0,1,0));

			(p2-p1).draw_vector(p1, vec3d(0.5, 0.5, 0.5));
			(p3-p2).draw_vector(p2, vec3d(0.5, 0.5, 0.5));
			(p4-p3).draw_vector(p3, vec3d(0.5, 0.5, 0.5));
			(p1-p4).draw_vector(p4, vec3d(0.5, 0.5, 0.5));

			(p2_-p1_).draw_vector(p1_, vec3d(0.5, 0.5, 0.5));
			(p3_-p2_).draw_vector(p2_, vec3d(0.5, 0.5, 0.5));
			(p4_-p3_).draw_vector(p3_, vec3d(0.5, 0.5, 0.5));
			(p1_-p4_).draw_vector(p4_, vec3d(0.5, 0.5, 0.5));

			(p1_-p1).draw_vector(p1, vec3d(0.5, 0.5, 0.5));
			(p2_-p2).draw_vector(p2, vec3d(0.5, 0.5, 0.5));
			(p3_-p3).draw_vector(p3, vec3d(0.5, 0.5, 0.5));
			(p4_-p4).draw_vector(p4, vec3d(0.5, 0.5, 0.5));
			return;
		}
		if (type==ORTHOGRAPHIC_CAMERA)
		{
			DBL hor_view_length = right.length()/2 /* *tan(angle/2.0*PI/180.0) */;
			//DBL cam_ratio = right.length()/up.length();
			DBL ver_view_length = up.length()/2 /* hor_view_length/cam_ratio */;

			DBL persp_frustum_size = scene->get_frustum_size();

			vec3d up_norm = up.normalize();
			vec3d right_norm = right.normalize();
			vec3d dir_norm = direction.normalize();

			vec3d p0 = location+direction;
			vec3d p1 = p0 - right_norm*hor_view_length + up_norm*ver_view_length;
			vec3d p2 = p0 + right_norm*hor_view_length + up_norm*ver_view_length;
			vec3d p3 = p0 + right_norm*hor_view_length - up_norm*ver_view_length;
			vec3d p4 = p0 - right_norm*hor_view_length - up_norm*ver_view_length;
/*
			vec3d p1_dir = (p1 - location).normalize();
			vec3d p2_dir = (p2 - location).normalize();
			vec3d p3_dir = (p3 - location).normalize();
			vec3d p4_dir = (p4 - location).normalize();
*/
			vec3d p1_ = p1 + dir_norm*persp_frustum_size;
			vec3d p2_ = p2 + dir_norm*persp_frustum_size;
			vec3d p3_ = p3 + dir_norm*persp_frustum_size;
			vec3d p4_ = p4 + dir_norm*persp_frustum_size;

			glLoadIdentity();
			glDisable(GL_LIGHTING);

			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			// draw quads
			glPushAttrib(GL_ENABLE_BIT);
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glEnable(GL_POLYGON_SMOOTH);
			glEnable(GL_BLEND);
			glColor4d(0.7, 0.7, 0.7, 0.3);

			glFrontFace(GL_CW);
			glBegin(GL_QUADS);
			p1.gl_vertex();p2.gl_vertex();p3.gl_vertex();p4.gl_vertex();
			p1_.gl_vertex();p2_.gl_vertex();p3_.gl_vertex();p4_.gl_vertex();
			p1.gl_vertex();p1_.gl_vertex();p4_.gl_vertex();p4.gl_vertex();
			p1.gl_vertex();p2.gl_vertex();p2_.gl_vertex();p1_.gl_vertex();
			p2.gl_vertex();p3.gl_vertex();p3_.gl_vertex();p2_.gl_vertex();
			p3.gl_vertex();p4.gl_vertex();p4_.gl_vertex();p3_.gl_vertex();
			p4.gl_vertex();p4_.gl_vertex();p1_.gl_vertex();p1.gl_vertex();
			glEnd();

			glFrontFace(GL_CCW);
			glBegin(GL_QUADS);
			p1.gl_vertex();p2.gl_vertex();p3.gl_vertex();p4.gl_vertex();
			p1_.gl_vertex();p2_.gl_vertex();p3_.gl_vertex();p4_.gl_vertex();
			p1.gl_vertex();p1_.gl_vertex();p4_.gl_vertex();p4.gl_vertex();
			p1.gl_vertex();p2.gl_vertex();p2_.gl_vertex();p1_.gl_vertex();
			p2.gl_vertex();p3.gl_vertex();p3_.gl_vertex();p2_.gl_vertex();
			p3.gl_vertex();p4.gl_vertex();p4_.gl_vertex();p3_.gl_vertex();
			p4.gl_vertex();p4_.gl_vertex();p1_.gl_vertex();p1.gl_vertex();
			glEnd();

			glPopAttrib();
			glDisable(GL_POLYGON_OFFSET_FILL);

			direction.draw_vector_a(location, vec3d(0,0,1));
			right.draw_vector_a(p0, vec3d(1,0,0));
			up.draw_vector_a(p0, vec3d(0,1,0));

			(p2-p1).draw_vector(p1, vec3d(0.5, 0.5, 0.5));
			(p3-p2).draw_vector(p2, vec3d(0.5, 0.5, 0.5));
			(p4-p3).draw_vector(p3, vec3d(0.5, 0.5, 0.5));
			(p1-p4).draw_vector(p4, vec3d(0.5, 0.5, 0.5));

			(p2_-p1_).draw_vector(p1_, vec3d(0.5, 0.5, 0.5));
			(p3_-p2_).draw_vector(p2_, vec3d(0.5, 0.5, 0.5));
			(p4_-p3_).draw_vector(p3_, vec3d(0.5, 0.5, 0.5));
			(p1_-p4_).draw_vector(p4_, vec3d(0.5, 0.5, 0.5));

			(p1_-p1).draw_vector(p1, vec3d(0.5, 0.5, 0.5));
			(p2_-p2).draw_vector(p2, vec3d(0.5, 0.5, 0.5));
			(p3_-p3).draw_vector(p3, vec3d(0.5, 0.5, 0.5));
			(p4_-p4).draw_vector(p4, vec3d(0.5, 0.5, 0.5));
			return;
		}
	}
}

/** @brief get_type
  *
  * @todo: document this function
  */
unsigned int pov_camera::get_type() const
{
	return type;
}

