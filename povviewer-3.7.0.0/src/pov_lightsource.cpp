#include <GL/gl.h>

#include "pov_baseobject.hpp"
#include "pov_lightsource.hpp"

pov_lightsource::pov_lightsource(pov_scene* s, int num):pov_baseobject(s)
{
	tag = 0x5448474c; /* LGHT */
	light_size_center = 1.0;
	id = num;
	parallel = 0;
	area_light = 0;
	area_size1 = area_size2 = 0;
	fade_power = 0;
	fade_distance = 0;
	fallof = 0;
	radius = 0;
	coeff = 0;
	type = 0;
	light_pos[0] = light_pos[1] = light_pos[2] = light_pos[3] = 0;
	diffuse_color[0] = diffuse_color[1] = diffuse_color[2] = diffuse_color[3] = 0;
	placed = false;
}

pov_lightsource::~pov_lightsource()
{
	remove_light();
}

void pov_lightsource::place_light()
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0+id, GL_DIFFUSE, diffuse_color);
	//glLightfv(GL_LIGHT0+id, GL_AMBIENT, diffuse_color);
	glLightfv(GL_LIGHT0+id, GL_POSITION, light_pos);
	glEnable(GL_LIGHT0+id);
	placed = true;
	update();
}

/** @brief remove_light
  *
  * @todo: document this function
  */
void pov_lightsource::remove_light()
{
	if (placed)
	{
		glDisable(GL_LIGHT0+id);
		placed = false;
	}
}

void pov_lightsource::update()
{
	glDisable(GL_LIGHTING);
	color.gl_color();
	position.draw_star(light_size_center);
	glEnable(GL_LIGHTING);
//	vec3d(0).gl_color();
}

int pov_lightsource::read(FILE *f)
{

	unsigned int readden = pov_baseobject::read(f);

	SNGL colour[3];
	fread(colour, sizeof(colour), 1, f);
	readden += sizeof(colour);
	light_color = vec3d(colour[0], colour[1], colour[2]);

	readden += position.read(f);
	readden += direction.read(f);
	readden += points_at.read(f);
	readden += axis1.read(f);
	readden += axis2.read(f);
	fread(&type, sizeof(type), 1, f);
	fread(&coeff, sizeof(coeff), 1, f);
	fread(&radius, sizeof(radius), 1, f);
	fread(&fallof, sizeof(fallof), 1, f);
	fread(&fade_distance, sizeof(fade_distance), 1, f);
	fread(&fade_power, sizeof(fade_power), 1, f);
	fread(&area_size1, sizeof(area_size1), 1, f);
	fread(&area_size2, sizeof(area_size2), 1, f);
	fread(&area_light, sizeof(area_light), 1, f);
	fread(&parallel, sizeof(parallel), 1, f);

	readden += sizeof(type)+
		sizeof(coeff)+
		sizeof(radius)+
		sizeof(fallof)+
		sizeof(fade_distance)+
		sizeof(fade_power)+
		sizeof(area_size1)+
		sizeof(area_size2)+
		sizeof(area_light)+
		sizeof(parallel);

	light_color.to4f(diffuse_color);
	position.to4f(light_pos);
	return readden;
}

/** @brief get_position
  *
  * @todo: document this function
  */
vec3d pov_lightsource::get_position() const
{
	vec3d res = position;
	return res;
}

