#include "pov_baseobject.hpp"
#include "pov_heightfield.hpp"
#include "pov_scene.hpp"
#include "gl_object.hpp"

pov_heightfield::pov_heightfield(pov_scene* s):pov_baseobject(s)
{
	tag = 0x444c4648; // HFLD
	max_x = 0;
	max_z = 0;
	min_y = 0;
	max_y = 0;
	block_max_x = 0;
	block_max_z = 0;
	block_width_x = 0;
	block_width_z = 0;
	map = NULL;
	normals = NULL;
}

pov_heightfield::~pov_heightfield()
{
	delete [] map;
	delete [] normals;
}

#define _LERP(a, b, t) ( ((b) - (a)) * (t) + (a) )

/*
void pov_heightfield::update(TViewMode mode)
{
	DEBUG_MSG("pov_heightfield::update()")
	int x, z;
	DBL u, u1, v, v1, X, X1, X2, X3, Y, Y1, Y2, Y3, Z, Z1, Z2, Z3;
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
			for(z=0; z<max_z; z++)
			{
				v = double(z) / double(max_z - 1);
				glBegin(GL_POINTS);
				for(x=0; x<max_x; x++)
				{
					u = double(x) / double(max_x - 1);
					glVertex3d(
						_LERP(bounding_corners[0][0], bounding_corners[1][0], u),
						map[z*max_x + x],
						_LERP(bounding_corners[0][2], bounding_corners[1][2], v)
					);
				}
				glEnd();
			}
			glPointSize(scene->get_point_size());
		break;

		case LINES:
			glColor3dv(color);
			for(z=0; z<max_z; z++)
			{
				v = double(z) / double(max_z - 1);
				glBegin(GL_LINE_STRIP);
				for(x=0; x<max_x; x++)
				{
					u = double(x) / double(max_x - 1);
					glVertex3d(
						_LERP(bounding_corners[0][0], bounding_corners[1][0], u),
						map[z*max_x + x],
						_LERP(bounding_corners[0][2], bounding_corners[1][2], v)
					);
				}
				glEnd();
			}
			for(x=0; x<max_x; x++)
			{
				u = double(x) / double(max_x-1);
				glBegin(GL_LINE_STRIP);
				for(z=0; z<max_z; z++)
				{
					v = double(z) / double(max_z - 1);
					glVertex3d(
						_LERP(bounding_corners[0][0], bounding_corners[1][0], u),
						map[z*max_x + x],
						_LERP(bounding_corners[0][2], bounding_corners[1][2], v)
					);
				}
				glEnd();
			}
		break;

		case SOLID:
			if (scene->lights_present())
				glEnable(GL_LIGHTING);
			glColor3dv(color);
			VECTOR3D n1, a, b, c, d, e;
			glFrontFace(GL_CW);
			glBegin(GL_TRIANGLES);
			for(z=0; z<(max_z-1); z++)
			{
				v = double(z) / double(max_z - 1);
				v1 = double(z+1) / double(max_z - 1);
				Z = _LERP(bounding_corners[0][2], bounding_corners[1][2], v);
				Z1 = _LERP(bounding_corners[0][2], bounding_corners[1][2], v1);
				Z2 = Z;
				Z3 = Z1;
				for(x=0; x<(max_x-1); x++)
				{
					u = double(x) / double(max_x - 1);
					u1 = double(x+1) / double(max_x - 1);

					X = _LERP(bounding_corners[0][0], bounding_corners[1][0], u);
					X1 = X;

					X2 = _LERP(bounding_corners[0][0], bounding_corners[1][0], u1);
					X3 = X2;

					Y = map[z*max_x + x];
					Y1 = map[(z+1)*max_x + x];
					Y2 = map[z*max_x + x+1];
					Y3 = map[(z+1)*max_x + x+1];

					Assign_Vector3d(a, X, Y, Z);
					Assign_Vector3d(b, X1, Y1, Z1);
					Assign_Vector3d(c, X2, Y2, Z2);
					VSub3d(d, b, a);
					VSub3d(e, c, a);
					VCross3d(n1, d, e);
					VNormalize3d(n1);
					glNormal3dv(n1);
					glVertex3d(X, Y, Z);
					glVertex3d(X1, Y1, Z1);
					glVertex3d(X2, Y2, Z2);

					Assign_Vector3d(a, X2, Y2, Z2);
					Assign_Vector3d(b, X1, Y1, Z1);
					Assign_Vector3d(c, X3, Y3, Z3);
					VSub3d(d, b, a);
					VSub3d(e, c, a);
					VCross3d(n1, d, e);
					VNormalize3d(n1);
					glNormal3dv(n1);
					glVertex3d(X2, Y2, Z2);
					glVertex3d(X1, Y1, Z1);
					glVertex3d(X3, Y3, Z3);
				}
			}
			glEnd();
			glFrontFace(GL_CCW);
			glBegin(GL_TRIANGLES);
			for(z=0; z<(max_z-1); z++)
			{
				v = double(z) / double(max_z - 1);
				v1 = double(z+1) / double(max_z - 1);
				Z = _LERP(bounding_corners[0][2], bounding_corners[1][2], v);
				Z1 = _LERP(bounding_corners[0][2], bounding_corners[1][2], v1);
				Z2 = Z;
				Z3 = Z1;
				for(x=0; x<(max_x-1); x++)
				{
					u = double(x) / double(max_x - 1);
					u1 = double(x+1) / double(max_x - 1);

					X = _LERP(bounding_corners[0][0], bounding_corners[1][0], u);
					X1 = X;

					X2 = _LERP(bounding_corners[0][0], bounding_corners[1][0], u1);
					X3 = X2;

					Y = map[z*max_x + x];
					Y1 = map[(z+1)*max_x + x];
					Y2 = map[z*max_x + x+1];
					Y3 = map[(z+1)*max_x + x+1];

					Assign_Vector3d(a, X, Y, Z);
					Assign_Vector3d(b, X1, Y1, Z1);
					Assign_Vector3d(c, X2, Y2, Z2);
					VSub3d(d, b, a);
					VSub3d(e, c, a);
					VCross3d(n1, d, e);
					VNormalize3d(n1);
					glNormal3dv(n1);
					glVertex3d(X, Y, Z);
					glVertex3d(X1, Y1, Z1);
					glVertex3d(X2, Y2, Z2);

					Assign_Vector3d(a, X2, Y2, Z2);
					Assign_Vector3d(b, X1, Y1, Z1);
					Assign_Vector3d(c, X3, Y3, Z3);
					VSub3d(d, b, a);
					VSub3d(e, c, a);
					VCross3d(n1, d, e);
					VNormalize3d(n1);
					glNormal3dv(n1);
					glVertex3d(X2, Y2, Z2);
					glVertex3d(X1, Y1, Z1);
					glVertex3d(X3, Y3, Z3);
				}
			}
			glEnd();
		break;

		case NORMALS:
		break;

		default:
		break;
	}
}
*/

int pov_heightfield::read(FILE *f)
{
	int readden = pov_baseobject::read(f);

	readden += bounding_corners[0].read(f);
	readden += bounding_corners[1].read(f);
	fread(&max_x, sizeof(max_x), 1, f);
	max_x += 2;
	fread(&max_z, sizeof(max_z), 1, f);
	max_z += 2;
	fread(&min_y, sizeof(min_y), 1, f);
	fread(&max_y, sizeof(max_y), 1, f);
	fread(&block_max_x, sizeof(block_max_x), 1, f);
	fread(&block_max_z, sizeof(block_max_z), 1, f);
	fread(&block_width_x, sizeof(block_width_x), 1, f);
	fread(&block_width_z, sizeof(block_width_z), 1, f);
	readden += sizeof(max_x) + sizeof(max_z) + sizeof(min_y) + sizeof(max_y)
		+ sizeof(block_max_x) + sizeof(block_max_z) + sizeof(block_width_x) + sizeof(block_width_z);

	unsigned int tag = 0;
	fread(&tag, sizeof(tag), 1, f);
	readden += sizeof(tag);
	if (tag!=0)
	{
		map = new DBL[max_z*max_x];
		fread(map, sizeof(DBL), max_z*max_x, f);
		readden += sizeof(DBL)*max_z*max_x;
	}
	fread(&tag, sizeof(tag), 1, f);
	readden += sizeof(tag);
	if (tag!=0)
	{
		normals = new vec3s[max_z*max_x];
		fread(normals, sizeof(vec3s), max_z*max_x, f);
		readden += sizeof(vec3s)*max_z*max_x;
		is_smoothed = true;
	}
	tesselate();
	return readden;
}

/** @brief tesselate
  *
  * @todo: document this function
  */
void pov_heightfield::tesselate() const
{
//	DEBUG_MSG("void pov_heightfield::tesselate() const")
//	DBL time_sec;
//	unsigned int st, et;
	if (map!=NULL)
	{
		DBL u, u1, v, v1, X, X1, X2, X3, Y, Y1, Y2, Y3, Z, Z1, Z2, Z3;
		int x, z;
		vec3d n, n1, n2, n3, a, b, c, d, e;
		for(z=0; z<(max_z-1); z++)
		{
			v = double(z) / double(max_z - 1);
			v1 = double(z+1) / double(max_z - 1);
			Z = _LERP(bounding_corners[0][2], bounding_corners[1][2], v);
			Z1 = _LERP(bounding_corners[0][2], bounding_corners[1][2], v1);
			Z2 = Z;
			Z3 = Z1;
	//		st = SDL_GetTicks();
			for(x=0; x<(max_x-1); x++)
			{
				u = double(x) / double(max_x - 1);
				u1 = double(x+1) / double(max_x - 1);

				X = _LERP(bounding_corners[0][0], bounding_corners[1][0], u);
				X1 = X;

				X2 = _LERP(bounding_corners[0][0], bounding_corners[1][0], u1);
				X3 = X2;

				Y = map[z*max_x + x];
				Y1 = map[(z+1)*max_x + x];
				Y2 = map[z*max_x + x+1];
				Y3 = map[(z+1)*max_x + x+1];

				if (normals!=NULL)
				{
					// normals
					n = vec3d(0) - vec3d_from_vec3s(normals[z*max_x + x]);
					n1 = vec3d(0) - vec3d_from_vec3s(normals[(z+1)*max_x + x]);
					n2 = vec3d(0) - vec3d_from_vec3s(normals[z*max_x + x+1]);
					n3 = vec3d(0) - vec3d_from_vec3s(normals[(z+1)*max_x + x+1]);
				}

				a = vec3d(X, Y, Z);
				b = vec3d(X1, Y1, Z1);
				c = vec3d(X2, Y2, Z2);
				if (normals!=NULL)
					globj->append_face_with_normal(a, n, b, n1, c, n2);
				else
					globj->append_face(a, b, c);

				a = vec3d(X2, Y2, Z2);
				b = vec3d(X1, Y1, Z1);
				c = vec3d(X3, Y3, Z3);
				if (normals!=NULL)
					globj->append_face_with_normal(a, n2, b, n1, c, n3);
				else
					globj->append_face(a, b, c);
			}
	//		et = SDL_GetTicks();
	//		time_sec = double(et - st) / 1000.0;
	//		DEBUG_MSG("z="<<z<<" (max_z-1)="<<(max_z-1)<<" time="<<time_sec)
		}
	}
}

