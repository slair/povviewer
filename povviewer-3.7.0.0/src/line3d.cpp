#include "GL/gl.h"
#include <GL/glx.h>

#include "common_funcs.hpp"
#include "line3d.hpp"

/** @brief add_line
  *
  * @todo: document this function
  */
void add_line(vector_line3d* lines, const vec3d a, const vec3d b)
{
//	DXF_LINE* line;
	bool found = false;
	for (unsigned int i=0; i<lines->size(); i++)
	{
//		if (compare_ex((*lines)[i].p1, a)&&compare_ex((*lines)[i].p2, b))
		if (((*lines)[i].p1==a)&&((*lines)[i].p2==b))
		{
			found = true;
			break;
		}
//		if (compare_ex((*lines)[i].p1, b)&&compare_ex((*lines)[i].p2, a))
		if (((*lines)[i].p1==b)&&((*lines)[i].p2==a))
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		line3d new_line;
		new_line.p1 = a;
		new_line.p2 = b;
		lines->push_back(new_line);
	}
}

/** @brief dump
  *
  * @todo: document this function
  */
void line3d::dump(ostream& dxf_stream) const
{
	dxf_stream << "0" << endl;
	dxf_stream << "LINE" << endl;
	dxf_stream << "5" << endl;
	dxf_stream << "43" << endl;
	dxf_stream << "100" << endl;
	dxf_stream << "AcDbEntity" << endl;
	dxf_stream << "100" << endl;
	dxf_stream << "AcDbLine" << endl;
	dxf_stream << "8" << endl;
	dxf_stream << "0" << endl;
	dxf_stream << "62" << endl;
	dxf_stream << "256" << endl;
	dxf_stream << "370" << endl;
	dxf_stream << "-1" << endl;
	dxf_stream << "6" << endl;
	dxf_stream << "ByLayer" << endl;
	dxf_stream << "10" << endl;
	dxf_stream << p1[0] << endl;
	dxf_stream << "20" << endl;
	dxf_stream << p1[1] << endl;
	dxf_stream << "30" << endl;
	dxf_stream << p1[2] << endl;
	dxf_stream << "11" << endl;
	dxf_stream << p2[0] << endl;
	dxf_stream << "21" << endl;
	dxf_stream << p2[1] << endl;
	dxf_stream << "31" << endl;
	dxf_stream << p2[2] << endl;
}

/** @brief DXF_LINE
  *
  * @todo: document this function
  */
line3d::line3d()
{
	p1 = vec3d(0);
	p2 = vec3d(0);
}

const line3d &line3d::operator=(const line3d& src)
{
	if(this!= &src)
	{
		p1 = src.p1;
		p2 = src.p2;
	}
	return *this;
}

/** @brief DXF_LINE
  *
  * @todo: document this function
  */
line3d::line3d(const line3d& src)
{
	p1 = src.p1;
	p2 = src.p2;
}

/** @brief line3d
  *
  * @todo: document this function
  */
line3d::line3d(const vec3d a, const vec3d b)
{
	p1 = a;
	p2 = b;
}

/** @brief add_line
  *
  * @todo: document this function
  */
void add_line(set_line3d* lines, const vec3d a, const vec3d b)
{
	line3d new_line;

	if (a<b)
		new_line = line3d(a, b);
	else
		new_line = line3d(b, a);

	if (new_line.length()!=0)
	{
		lines->insert(new_line);
		line3d_DEBUG_MSG("\nline: "<<new_line)
		line3d_DEBUG_MSG("size: "<<lines->size())
	}
}

/** @brief length
  *
  * @todo: document this function
  */
DBL line3d::length() const
{
	vec3d ll = p2 - p1;
	return ll.length();
}

/** @brief operator<<
  *
  * @todo: document this function
  */
std::ostream& operator<<(std::ostream& os, const line3d& t)
{
	t.print(os);
	return os;
}

/** @brief print
  *
  * @todo: document this function
  */
void line3d::print(std::ostream& output) const
{
	output << "<" << p1 << "> - <" << p2 << ">";
}

/** @brief operator<
  *
  * @todo: document this function
  */
bool line3d::operator<(const line3d& src) const
{
	if (p1<src.p1)
		return true;
	if ((p1==src.p1)&&(p2<src.p2))
		return true;
	return false;
}

