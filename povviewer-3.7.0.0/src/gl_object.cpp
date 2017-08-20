#include "GL/gl.h"
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glut.h>

using namespace std;
#include "common_funcs.hpp"
#include "vec3d.hpp"
#include "line3d.hpp"
#include "gl_object.hpp"
#include "pov_baseobject.hpp"
#include "pov_transform.hpp"
#include "pov_scene.hpp"
#include "view_camera.hpp"

extern vec3d* v_pool;
extern unsigned int v_pool_size, v_pool_capacity;

gl_object::gl_object(pov_baseobject* p)
{
	parent = p;
	tris_present = false;
	quads_present = false;
	contours_present = false;
	sum_face_centers = vec3d(0);
//	patches_present = false;
//	vertices.reserve(RESERVED_VEC3D);
//	normals.reserve(RESERVED_VEC3D);
	faces.reserve(RESERVED_VEC3D);
}

gl_object::~gl_object()
{
//	vertices.clear();
//	normals.clear();
	faces.clear();
	contours.clear();
	contour_normals.clear();
//	patches.clear();
}

void gl_object::calc_center(IFACE *f)
{
	vec3d c = vec3d(0,0,0);
	for(unsigned int i=0; i<f->numverts; i++)
	{
		c = c + v_pool[(f->vi[i])];
	}
	c = c / double(f->numverts);
	f->center = add_vertex(c);
	sum_face_centers = sum_face_centers + c;
}

void gl_object::calc_normal(IFACE *f)
{
	vec3d p0, p1, p2, p3, n1, n2, n3, n;
	switch (f->numverts)
	{
		case 3:
			p0 = v_pool[f->vi[0]];
			p1 = v_pool[f->vi[1]];
			p2 = v_pool[f->vi[2]];
			n1 = p0 - p1;
			n2 = p2 - p1;
			n = n1^n2;
			f->normal = add_normal(n.normalize());
			calc_center(f);
		break;

		case 4:
			p0 = v_pool[f->vi[0]];
			p1 = v_pool[f->vi[1]];
			p2 = v_pool[f->vi[2]];
			p3 = v_pool[f->vi[3]];
			n1 = p0 - p1;
			n2 = p2 - p1;
			n = n1^n2;
			f->normal = add_normal(n.normalize());
			calc_center(f);
		break;

		default:
			cout << "Bad number of vertices - " << f->numverts << " file:" <<__FILE__ << " line:" << __LINE__ << endl;
			exit(1);
		break;
	}
}

void gl_object::append_line(vec3d a, vec3d b)
{
	//~ DEBUG_MSG("gl_object::append_line()")
	lines_present = true;
	IFACE face = IFACE();
	face.numverts = 2;
	face.normal = -1;
	face.vi[0] = add_vertex(a);
	face.vi[1] = add_vertex(b);
	//~ calc_normal(&face);
	calc_center(&face);
	faces.push_back(face);
}

void gl_object::append_face(vec3d a, vec3d b, vec3d c)
{
	tris_present = true;
	IFACE face = IFACE();
	face.numverts = 3;
	face.normal = -1;
	face.vi[0] = add_vertex(a);
	face.vi[1] = add_vertex(b);
	face.vi[2] = add_vertex(c);
	calc_normal(&face);
	faces.push_back(face);
}

void gl_object::append_face_with_normal(vec3d a, vec3d na, vec3d b, vec3d nb, vec3d c, vec3d nc)
{
	tris_present = true;
	IFACE face = IFACE();
	face.numverts = 3;
	face.normal = -1;
	face.vi[0] = add_vertex(a);
	face.ni[0] = add_vertex(na);
	face.vi[1] = add_vertex(b);
	face.ni[1] = add_vertex(nb);
	face.vi[2] = add_vertex(c);
	face.ni[2] = add_vertex(nc);
	calc_normal(&face);
	faces.push_back(face);
}

void gl_object::append_face(vec3d a, vec3d b, vec3d c, vec3d d)
{
	quads_present = true;
	IFACE face = IFACE();
	face.numverts = 4;
	face.normal = -1;
	face.vi[0] = add_vertex(a);
	face.vi[1] = add_vertex(b);
	face.vi[2] = add_vertex(c);
	face.vi[3] = add_vertex(d);
	calc_normal(&face);
	faces.push_back(face);
}

void gl_object::show_flat_faces()
{
	if (tris_present||quads_present)
	{
		//DEBUG_MSG("gl_object::show_flat_faces()")
		IFACE face;
		if (tris_present)
		{
			glBegin(GL_TRIANGLES);
			for(unsigned int i=0; i<faces.size(); i++)
			{
				face = faces.at(i);
				if (face.numverts==3)
				{
					v_pool[face.normal].gl_normal();
					v_pool[face.vi[0]].gl_vertex();
					v_pool[face.vi[1]].gl_vertex();
					v_pool[face.vi[2]].gl_vertex();

					// other side

					(vec3d(0.0)-v_pool[face.normal]).gl_normal();
					v_pool[face.vi[2]].gl_vertex();
					v_pool[face.vi[1]].gl_vertex();
					v_pool[face.vi[0]].gl_vertex();

				}
			}
			glEnd();
		}
		if (quads_present)
		{
			glBegin(GL_QUADS);
			for(unsigned int i=0; i<faces.size(); i++)
			{
				face = faces.at(i);
				if (face.numverts==4)
				{
					v_pool[face.normal].gl_normal();
					v_pool[face.vi[0]].gl_vertex();
					v_pool[face.vi[1]].gl_vertex();
					v_pool[face.vi[2]].gl_vertex();
					v_pool[face.vi[3]].gl_vertex();

					// other side

					(vec3d(0.0)-v_pool[face.normal]).gl_normal();
					v_pool[face.vi[3]].gl_vertex();
					v_pool[face.vi[2]].gl_vertex();
					v_pool[face.vi[1]].gl_vertex();
					v_pool[face.vi[0]].gl_vertex();

				}
			}
			glEnd();
		}
	}
}

int gl_object::add_vertex(vec3d v)
{
	unsigned int index = v_pool_add(v);
//	for(index=0; index<vertices.size(); index++)
//	{
//		if (v==vertices.at(index))
//		{
//			return index;
//		}
//	}
//	vertices.push_back(v);
	return index;
}

int gl_object::add_normal(vec3d v)
{
	unsigned int index = v_pool_add(v);
//	unsigned int index = normals.size();
//	for(index=0; index<normals.size(); index++)
//	{
//		if (v==normals.at(index))
//		{
//			return index;
//		}
//	}
//	normals.push_back(v);
	return index;
}

/** @brief show_normals
  *
  * @todo: document this function
  */
void gl_object::show_normals(const DBL size)
{
	//DEBUG_MSG("gl_object::show_normals("<<size<<")")
	IFACE* face;
	vec3d tmp;
	vec3d(0.0).gl_color();
	glBegin(GL_LINES);
	for(unsigned int i=0; i<faces.size(); i++)
	{
		face = &faces[i];
		if (face->numverts>2)
		{
			v_pool[face->center].gl_vertex();
			tmp = v_pool[face->normal] * size;
			tmp = tmp + v_pool[face->center];
			tmp.gl_vertex();

//			v_pool[face->center].gl_vertex();
//			tmp = v_pool[face->normal] * size;
//			tmp = v_pool[face->center] - tmp;
//			tmp.gl_vertex();

		}
	}
	glEnd();
}

/** @brief show_smooth_normals
  *
  * @todo: document this function
  */
void gl_object::show_smooth_normals(const DBL size)
{
	IFACE* face;
	vec3d tmp;
	unsigned int faces_size = faces.size();
	vec3d(0.0).gl_color();
	glBegin(GL_LINES);
	for(unsigned int i=0; i<faces_size; i++)
	{
		face = &faces[i];
		if (face->numverts>2)
		{
			for(unsigned int j=0; j<face->numverts; j++)
			{
				v_pool[face->vi[j]].gl_vertex();
				tmp = v_pool[face->ni[j]] * size;
				tmp = tmp + v_pool[face->vi[j]];
				tmp.gl_vertex();

//				v_pool[face->vi[j]].gl_vertex();
//				tmp = v_pool[face->ni[j]] * size;
//				tmp = v_pool[face->vi[j]] - tmp;
//				tmp.gl_vertex();
			}
		}
	}
	glEnd();
}

/** @brief show_dots
  *
  * @todo: document this function
  */
void gl_object::show_dots() const
{
	//DEBUG_MSG("gl_object::show_dots()")
	IFACE face;
	glBegin(GL_POINTS);
	for(unsigned int i=0; i<faces.size(); i++)
	{
		face = faces.at(i);
		for(unsigned int j=0; j<face.numverts; j++)
		{
			v_pool[face.vi[j]].gl_vertex();
		}
	}
	glEnd();
}

/** @brief show_lines_with_faces
  *
  * @todo: document this function
  */
void gl_object::show_lines_with_faces() const
{
	//~ DEBUG_MSG("gl_object::show_lines_with_faces()")
	if (lines_present)
	{
		IFACE face;
		for(unsigned int i=0; i<faces.size(); i++)
		{
			face = faces.at(i);
			if (face.numverts==2)
			{
		glBegin(GL_LINES);
				v_pool[face.vi[0]].gl_vertex();
				v_pool[face.vi[1]].gl_vertex();
		glEnd();
			}
		}
	}
}

/** @brief show_lines
  *
  * @todo: document this function
  */
void gl_object::show_lines() const
{
	//DEBUG_MSG("gl_object::show_lines()")
	IFACE face;
	//~ glBegin(GL_LINES);
	for(unsigned int i=0; i<faces.size(); i++)
	{
		glBegin(GL_LINE_LOOP);
		face = faces.at(i);
		if (face.numverts!=0)
		{
			for(unsigned int j=0; j<(face.numverts/*-1*/); j++)
			{
				v_pool[face.vi[j]].gl_vertex();
				//~ v_pool[face.vi[j+1]].gl_vertex();
			}
			//~ v_pool[face.vi[face.numverts-1]].gl_vertex();
			//~ v_pool[face.vi[0]].gl_vertex();
		}
		glEnd();
	}
	//~ glEnd();
}

/** @brief show_line_contours
  *
  * @todo: document this function
  */
void gl_object::show_line_contours() const
{
	if (contours_present)
	{
		//DEBUG_MSG("gl_object::show_contours()")
		unsigned int bc_index = 0;
		vec3d begin_cnt;
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
		gluTessProperty(pTess, GLU_TESS_BOUNDARY_ONLY, GL_TRUE);

		for(unsigned int i=0; i<contours.size(); i++)
		{
			gluTessBeginPolygon(pTess, NULL);
			unsigned int num_points = contours.at(i).size();
			unsigned int j = 0;
			begin_cnt = contours.at(i).at(0);
			bc_index = 0;
			gluTessBeginContour(pTess);
			for(j=0; j<num_points; j++)
			{
				if ((contours.at(i).at(j)==begin_cnt)&&(j!=bc_index))
				{
					bc_index = j+1;
					if (bc_index<num_points)
					{
						gluTessEndContour(pTess);
						begin_cnt = contours.at(i).at(j+1);
						gluTessBeginContour(pTess);
					}
				}
				else
				{
					p = new DBL[6];
					p[0] = contours.at(i).at(j)[0];
					p[1] = contours.at(i).at(j)[1];
					p[2] = contours.at(i).at(j)[2];
					p[3] = contour_normals.at(i)[0];
					p[4] = contour_normals.at(i)[1];
					p[5] = contour_normals.at(i)[2];
					gluTessVertex(pTess, p, p);
				}
			}
			gluTessEndContour(pTess);
			gluTessEndPolygon(pTess);
		}
		gluDeleteTess(pTess);
		glFrontFace(GL_CCW);
		// back side
		pTess = gluNewTess();
		gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
		gluTessCallback(pTess, GLU_TESS_END, glEnd);
		gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &normal_with_vertex);
		gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);
		gluTessCallback(pTess, GLU_TESS_COMBINE, (GLvoid (*) ()) &tessCombine);
		gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
		gluTessProperty(pTess, GLU_TESS_BOUNDARY_ONLY, GL_TRUE);

		for(unsigned int i=0; i<contours.size(); i++)
		{
			gluTessBeginPolygon(pTess, NULL);
			unsigned int num_points = contours.at(i).size();
			unsigned int j = 0;
			begin_cnt = contours.at(i).at(0);
			bc_index = 0;
			gluTessBeginContour(pTess);
			for(j=0; j<num_points; j++)
			{
				if ((contours.at(i).at(j)==begin_cnt)&&(j!=bc_index))
				{
					bc_index = j+1;
					if (bc_index<num_points)
					{
						gluTessEndContour(pTess);
						begin_cnt = contours.at(i).at(j+1);
						gluTessBeginContour(pTess);
					}
				}
				else
				{
					p = new DBL[6];
					p[0] = contours.at(i).at(j)[0];
					p[1] = contours.at(i).at(j)[1];
					p[2] = contours.at(i).at(j)[2];
					p[3] = contour_normals.at(i)[0];
					p[4] = contour_normals.at(i)[1];
					p[5] = contour_normals.at(i)[2];
					gluTessVertex(pTess, p, p);
				}
			}
			gluTessEndContour(pTess);
			gluTessEndPolygon(pTess);
		}
		gluDeleteTess(pTess);
	} // if (contours_present)
}

/** @brief show_contours
  *
  * @todo: document this function
  */
void gl_object::show_contours() const
{
	if (contours_present)
	{
		//DEBUG_MSG("gl_object::show_contours()")
		unsigned int bc_index = 0;
		vec3d begin_cnt;
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

		for(unsigned int i=0; i<contours.size(); i++)
		{
			gluTessBeginPolygon(pTess, NULL);
			unsigned int num_points = contours.at(i).size();
			unsigned int j = 0;
			begin_cnt = contours.at(i).at(0);
			bc_index = 0;
			gluTessBeginContour(pTess);
			for(j=0; j<num_points; j++)
			{
				if ((contours.at(i).at(j)==begin_cnt)&&(j!=bc_index))
				{
					bc_index = j+1;
					if (bc_index<num_points)
					{
						gluTessEndContour(pTess);
						begin_cnt = contours.at(i).at(j+1);
						gluTessBeginContour(pTess);
					}
				}
				else
				{
					p = new DBL[6];
					p[0] = contours.at(i).at(j)[0];
					p[1] = contours.at(i).at(j)[1];
					p[2] = contours.at(i).at(j)[2];
					p[3] = contour_normals.at(i)[0];
					p[4] = contour_normals.at(i)[1];
					p[5] = contour_normals.at(i)[2];
					gluTessVertex(pTess, p, p);
				}
			}
			gluTessEndContour(pTess);
			gluTessEndPolygon(pTess);
		}
		gluDeleteTess(pTess);
		glFrontFace(GL_CCW);
		// back side
		pTess = gluNewTess();
		gluTessCallback(pTess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
		gluTessCallback(pTess, GLU_TESS_END, glEnd);
		gluTessCallback(pTess, GLU_TESS_VERTEX, (GLvoid (*) ()) &normal_with_vertex);
		gluTessCallback(pTess, GLU_TESS_ERROR, (GLvoid (*) ()) &tessError);
		gluTessCallback(pTess, GLU_TESS_COMBINE, (GLvoid (*) ()) &tessCombine);
		gluTessProperty(pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

		for(unsigned int i=0; i<contours.size(); i++)
		{
			gluTessBeginPolygon(pTess, NULL);
			unsigned int num_points = contours.at(i).size();
			unsigned int j = 0;
			begin_cnt = contours.at(i).at(0);
			bc_index = 0;
			gluTessBeginContour(pTess);
			for(j=0; j<num_points; j++)
			{
				if ((contours.at(i).at(j)==begin_cnt)&&(j!=bc_index))
				{
					bc_index = j+1;
					if (bc_index<num_points)
					{
						gluTessEndContour(pTess);
						begin_cnt = contours.at(i).at(j+1);
						gluTessBeginContour(pTess);
					}
				}
				else
				{
					p = new DBL[6];
					p[0] = contours.at(i).at(j)[0];
					p[1] = contours.at(i).at(j)[1];
					p[2] = contours.at(i).at(j)[2];
					p[3] = contour_normals.at(i)[0];
					p[4] = contour_normals.at(i)[1];
					p[5] = contour_normals.at(i)[2];
					gluTessVertex(pTess, p, p);
				}
			}
			gluTessEndContour(pTess);
			gluTessEndPolygon(pTess);
		}
		gluDeleteTess(pTess);
	} // if (contours_present)
/*
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
*/
}

/** @brief append_contour
  *
  * @todo: document this function
  */
void gl_object::append_contour(array_vec3d contour, vec3d normal)
{
	contours_present = true;
	contours.push_back(contour);
	contour_normals.push_back(normal);
}

/** @brief get_stats
  *
  * @todo: document this function
  */
GLOBJ_STATS gl_object::get_globj_stats() const
{
	GLOBJ_STATS tmp;
	tmp.vertices_count = v_pool_size;
	tmp.faces_count = faces.size();
	tmp.contours_count = contours.size();
	tmp.contour_vertices_count = 0;
	for (unsigned int i=0; i<contours.size(); i++)
	{
		tmp.contour_vertices_count += contours.at(i).size();
	}
//	tmp.patches_count = patches.size();
	return tmp;
}

/** @brief smooth
  *
  * @todo: document this function
  */
void gl_object::smooth(const DBL max_angle)
{
	DBL cos_max_angle = cos(max_angle);
	vec3d new_normal;
	int face_indices[MAX_FACES];
	unsigned int i, j, common_vi, found, n, normal_index;
	DBL cos_angle;
	IFACE* face;
	unsigned int faces_size = faces.size();
	unsigned int smoothed_vertices = 0;
	for (i=0; i<faces_size;i++)
	{
		face = &faces[i];
		//DEBUG_MSG("face number="<<i<<" numverts="<<face->numverts)
		if (face->numverts>2)
		{
			for (j=0; j<face->numverts; j++)
			{
				if (face->ni[j]==-1)
				{
					new_normal = v_pool[face->normal];
					common_vi = face->vi[j]; // common vertex index
					// find all faces, which use this vertex
					found = find_faces(common_vi, i, face_indices);
					//DEBUG_MSG("vertex="<<j<<v_pool[common_vi].print(" <%f, %f, %f>")<<" neighbors="<<found)
					for (n=0; n<found; n++)
					{
						cos_angle = cos_alpha(v_pool[face->normal], v_pool[faces[face_indices[2*n]].normal]);
						//DEBUG_MSG("\t"<<face_indices[2*n])
						if (cos_angle>=cos_max_angle)
						{
							// smooth
							new_normal = new_normal + v_pool[faces[face_indices[2*n]].normal];
							smoothed_vertices++;
						}
						else
						{
							// no smooth
							face_indices[2*n+1]=-1;
						}
					}
					//DEBUG_MSG(endl)
					normal_index = add_normal(new_normal.normalize());
					face->ni[j] = normal_index;
					for (n=0; n<found; n++)
					{
						if (face_indices[2*n+1]!=-1)
							faces[face_indices[2*n]].ni[face_indices[2*n+1]] = normal_index;
					}
				}
			}
		}
	}
	if (smoothed_vertices>0)
		parent->set_smoothed(true);
}

/** @brief find_faces
  *
  * @todo: document this function
  */
unsigned int gl_object::find_faces(int vert_index, unsigned int our_face, int *array_faces)
{
	unsigned int found = 0;
	unsigned int faces_size = faces.size();
	IFACE* face;
	for (unsigned int i=0; i<faces_size; i++)
	{
		if (i!=our_face)
		{
			face = &faces[i];
			for (unsigned int j=0; j<face->numverts; j++)
			{
				if (v_pool[face->vi[j]]==v_pool[vert_index])
				{
					array_faces[found*2] = i;		// number of face
					array_faces[found*2+1] = j;	// number of vertex in face
					//DEBUG_MSG("f="<<i<<" v="<<j)
					found++;
				}
			}
		}
	}
	return found;
}

/** @brief IFACE
  *
  * @todo: document this function
  */
IFACE::IFACE()
{
	numverts = 0;
	vi[0] = -1;
	vi[1] = -1;
	vi[2] = -1;
	vi[3] = -1;
	ni[0] = -1;
	ni[1] = -1;
	ni[2] = -1;
	ni[3] = -1;
	normal = -1;
	center = -1;
}

/** @brief show_smooth_faces
  *
  * @todo: document this function
  */
void gl_object::show_smooth_faces()
{
	if (tris_present||quads_present)
	{
		//DEBUG_MSG("gl_object::show_smooth_faces()")
		IFACE* face;
		if (tris_present)
		{
			unsigned int faces_size = faces.size();
			for(unsigned int i=0; i<faces_size; i++)
			{
				face = &faces[i];
				#ifdef DEBUG
				//parent->get_scene()->get_camera()->put_text(v_pool[face->center]*0.1, 0,0,0,0,"%d",i);
				#endif
				glBegin(GL_TRIANGLES);
				if (face->numverts==3)
				{
					for (unsigned int k=0; k<3; k++)
					{
						if  (face->ni[k]!=-1)
							v_pool[face->ni[k]].gl_normal();
						v_pool[face->vi[k]].gl_vertex();
					}
					// other side
					for (unsigned int k=0; k<3; k++)
					{
						if (face->ni[2-k]!=-1)
							(vec3d(0)-v_pool[face->ni[2-k]]).gl_normal();
						v_pool[face->vi[2-k]].gl_vertex();
					}
				}
				glEnd();
			}
		}
		if (quads_present)
		{
			unsigned int faces_size = faces.size();
			for(unsigned int i=0; i<faces_size; i++)
			{
				face = &faces[i];
				#ifdef DEBUG
				//parent->get_scene()->get_camera()->put_text(v_pool[face->center]+v_pool[face->normal]*0.3, 0,0,0,0,"%d",i);
				#endif
				glBegin(GL_QUADS);
				if (face->numverts==4)
				{
					for (unsigned int k=0; k<4; k++)
					{
						if  (face->ni[k]!=-1)
							v_pool[face->ni[k]].gl_normal();
						v_pool[face->vi[k]].gl_vertex();
					}
					// other side
					for (unsigned int k=0; k<4; k++)
					{
						if (face->ni[3-k]!=-1)
							(vec3d(0)-v_pool[face->ni[3-k]]).gl_normal();
						v_pool[face->vi[3-k]].gl_vertex();
					}
				}
				glEnd();
			}
		}
	}
}

/** @brief show_patches
  *
  * @todo: document this function
  */
/*
void gl_object::show_patches() const
{
	if (patches_present)
	{
		DEBUG_MSG("gl_object::show_patches()")
	//    GLfloat ambient[] =
	//    {0.2, 0.2, 0.2, 1.0};
	//    GLfloat position[] =
	//    {0.0, 0.0, 2.0, 1.0};
	//    GLfloat mat_diffuse[] =
	//    {0.6, 0.6, 0.6, 1.0};
	//    GLfloat mat_specular[] =
	//    {1.0, 1.0, 1.0, 1.0};
	//    GLfloat mat_shininess[] =
	//    {50.0};
	//
	//    glEnable(GL_LIGHTING);
	//    glEnable(GL_LIGHT0);
	//
	//    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	//    glLightfv(GL_LIGHT0, GL_POSITION, position);
	//
	//    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

		unsigned int patches_count = patches.size();
		unsigned int i;
		glEnable(GL_RESCALE_NORMAL);
		glFrontFace(GL_CCW);
		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glEnable(GL_NORMALIZE);
		glEvalMesh2(GL_FILL, 0, 20, 0, 20);
		for (i=0; i<patches_count; i++)
		{
//			show_tri_patch(patches[i],10);
			show_patch(patches[i]);
		}
		glDisable(GL_MAP2_VERTEX_3);
		glDisable(GL_AUTO_NORMAL);
		glDisable(GL_NORMALIZE);

		glFrontFace(GL_CW);
		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glEnable(GL_NORMALIZE);
		glEvalMesh2(GL_LINE, 0, 20, 0, 20);
		for (i=0; i<patches_count; i++)
		{
//			show_tri_patch(patches[i],10);
			show_patch(patches[i]);
		}
		glDisable(GL_MAP2_VERTEX_3);
		glDisable(GL_AUTO_NORMAL);
		glDisable(GL_NORMALIZE);
		glFrontFace(GL_CCW);
	}
}
*/
/** @brief show_patch
  *
  * @todo: document this function
  */
/*
void gl_object::show_patch(PATCH a) const
{
	DEBUG_MSG("gl_object::show_patch()")
//	glFrontFace(GL_CW);
//	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &a.data[0][0][0]);
//	glEnable(GL_MAP2_VERTEX_3);
//	glEnable(GL_AUTO_NORMAL);
//	glEnable(GL_NORMALIZE);
//	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glMap2f(
		GL_MAP2_VERTEX_3,
		0,	// u1
		1,	// u2
		3,	// ustride
		4,	// uorder
		0,	// v1
		1,	// v2
		12,	// vstride
		4,	// vorder
		&a.data[0][0][0]
	);
	//glMap2f(GL_MAP2_VERTEX_3, u1, u2, ustride, uorder, 0, 1, 12, 4, &a.data[0][0][0]);
//	glEnable(GL_MAP2_VERTEX_3);
//	glEnable(GL_AUTO_NORMAL);
//	glEnable(GL_NORMALIZE);
	glMapGrid2f(
		20,		// un
		0.0,	// u1
		1.0,	// u2
		20,		// vn
		0.0,	// v1
		1.0		// v2
	);
}
*/


/** @brief append_patch
  *
  * @todo: document this function
  */
/*
void gl_object::append_patch(const vec3d src[4][4])
{
	patches_present = true;
	PATCH a = PATCH();
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
		{
			a.data[i][j][0] = float(src[i][j][0]);
			a.data[i][j][1] = float(src[i][j][1]);
			a.data[i][j][2] = float(src[i][j][2]);
		}
	patches.push_back(a);
}
*/

/** @brief get_object_center
  *
  * @todo: document this function
  */
vec3d gl_object::get_object_center() const
{
	vec3d oc;
	oc = sum_face_centers / double(faces.size());
	return oc;
}

/** @brief get_lines
  *
  * @todo: document this function
  */
void gl_object::get_lines(vector_line3d* lines, pov_transform* transform) const
{
	if (tris_present||quads_present)
	{
		const int show_interval = 100;
		unsigned int face_index;
		IFACE face;
		vec3d a, b, c, d;
		if (tris_present)
		{
			face_index = 0;
			for(unsigned int i=0; i<faces.size(); i++)
			{
				face = faces.at(i);
				if (face.numverts==3)
				{
					a = transform->transform_vec3d(v_pool[face.vi[0]]);
					b = transform->transform_vec3d(v_pool[face.vi[1]]);
					c = transform->transform_vec3d(v_pool[face.vi[2]]);
					if (parent->is_sphere)
					{
						a = a + parent->sphere_center;
						b = b + parent->sphere_center;
						c = c + parent->sphere_center;
					}
					add_line(lines, a, b);
					add_line(lines, b, c);
					add_line(lines, c, a);
					face_index++;
					if (face_index%show_interval==0)
					{
						/* parent->get_scene()->get_camera()->add_msg_to_console((parent->get_type()
							+ " " + int2str(lines->size())+" lines").c_str()); */
						parent->get_scene()->refresh_screen();
					}
				}
			}
		}
		if (quads_present)
		{
			face_index = 0;
			for(unsigned int i=0; i<faces.size(); i++)
			{
				face = faces.at(i);
				if (face.numverts==4)
				{
					a = transform->transform_vec3d(v_pool[face.vi[0]]);
					b = transform->transform_vec3d(v_pool[face.vi[1]]);
					c = transform->transform_vec3d(v_pool[face.vi[2]]);
					d = transform->transform_vec3d(v_pool[face.vi[3]]);
					if (parent->is_sphere)
					{
						a = a + parent->sphere_center;
						b = b + parent->sphere_center;
						c = c + parent->sphere_center;
						d = d + parent->sphere_center;
					}
					add_line(lines, a, b);
					add_line(lines, b, c);
					add_line(lines, c, d);
					add_line(lines, d, a);
					face_index++;
					if (face_index%show_interval==0)
					{
						/* parent->get_scene()->get_camera()->add_msg_to_console((parent->get_type()
							+ " " + int2str(lines->size())+" lines").c_str()); */
						parent->get_scene()->refresh_screen();
					}
				}
			}
		}
	}
}

/** @brief get_lines
  *
  * @todo: document this function
  */
void gl_object::get_lines(set_line3d* lines, pov_transform* transform) const
{
	if (tris_present||quads_present)
	{
		const int show_interval = 100;
		unsigned int face_index;
		IFACE face;
		vec3d a, b, c, d;
		if (tris_present)
		{
			face_index = 0;
			for(unsigned int i=0; i<faces.size(); i++)
			{
				face = faces.at(i);
				if (face.numverts==3)
				{
					a = transform->transform_vec3d(v_pool[face.vi[0]]);
					b = transform->transform_vec3d(v_pool[face.vi[1]]);
					c = transform->transform_vec3d(v_pool[face.vi[2]]);
					if (parent->is_sphere)
					{
						a = a + parent->sphere_center;
						b = b + parent->sphere_center;
						c = c + parent->sphere_center;
					}
					add_line(lines, a, b);
					add_line(lines, b, c);
					add_line(lines, c, a);
					face_index++;
					if (face_index%show_interval==0)
					{
						/* parent->get_scene()->get_camera()->add_msg_to_console((parent->get_type()
							+ " " + int2str(lines->size())+" lines").c_str()); */
						parent->get_scene()->refresh_screen();
					}
				}
			}
		}
		if (quads_present)
		{
			face_index = 0;
			for(unsigned int i=0; i<faces.size(); i++)
			{
				face = faces.at(i);
				if (face.numverts==4)
				{
					a = transform->transform_vec3d(v_pool[face.vi[0]]);
					b = transform->transform_vec3d(v_pool[face.vi[1]]);
					c = transform->transform_vec3d(v_pool[face.vi[2]]);
					d = transform->transform_vec3d(v_pool[face.vi[3]]);
					if (parent->is_sphere)
					{
						a = a + parent->sphere_center;
						b = b + parent->sphere_center;
						c = c + parent->sphere_center;
						d = d + parent->sphere_center;
					}
					add_line(lines, a, b);
					add_line(lines, b, c);
					add_line(lines, c, d);
					add_line(lines, d, a);
					face_index++;

					if (face_index%show_interval==0)
					{
						/* parent->get_scene()->get_camera()->add_msg_to_console((parent->get_type()
							+ " " + int2str(lines->size())+" lines").c_str()); */
						parent->get_scene()->refresh_screen();
					}

				}
			}
		}
	}
}

