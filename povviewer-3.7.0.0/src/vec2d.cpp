#include "GL/gl.h"
#include <GL/glx.h>

#include "common_funcs.hpp"
#include "vec2d.hpp"

//void vec2d::mult_matrix(const DBL m[3][3])
//{
//	DBL a, b, c;
//	// v[0]*mtx[0][0]+v[1]*mtx[1][0]+v[2]*mtx[2][0]
//	a = data[0]*m[0][0]+data[1]*m[1][0]+data[2]*m[2][0];
//	// v[0]*mtx[0][1]+v[1]*mtx[1][1]+v[2]*mtx[2][1]
//	b = data[0]*m[0][1]+data[1]*m[1][1]+data[2]*m[2][1];
//	// v[0]*mtx[0][2]+v[1]*mtx[1][2]+v[2]*mtx[2][2]
//	c = data[0]*m[0][2]+data[1]*m[1][2]+data[2]*m[2][2];
//	data[0] = a;
//	data[1] = b;
//	data[2] = c;
//}
//
//void vec2d::build_matrix(DBL m[3][3], const DBL angle) const
//{
//	DBL cosa = cos(angle);
//	DBL sina = sin(angle);
//	DBL x = data[0], y = data[1], z = data[2];
//	// [cosa+(1.0-cosa)*x*x, (1.0-cosa)*x*y-sina*z, (1.0-cosa)*x*z+sina*y]
//	// [(1.0-cosa)*y*x+sina*z, cosa+(1.0-cosa)*y*y, (1.0-cosa)*y*z-sina*x]
//	// [(1.0-cosa)*z*x-sina*y, (1.0-cosa)*z*y+sina*x, cosa+(1.0-cosa)*z*z]
//	m[0][0] = cosa+(1-cosa)*x*x;
//	m[0][1] = (1.0-cosa)*x*y-sina*z;
//	m[0][2] = (1.0-cosa)*x*z+sina*y;
//
//	m[1][0] = (1.0-cosa)*y*x+sina*z;
//	m[1][1] = cosa+(1.0-cosa)*y*y;
//	m[1][2] = (1.0-cosa)*y*z-sina*x;
//
//	m[2][0] = (1.0-cosa)*z*x-sina*y;
//	m[2][1] = (1.0-cosa)*z*y+sina*x;
//	m[2][2] = cosa+(1.0-cosa)*z*z;
//}

void vec2d::clamp_max(const DBL max_value)
{
	if (data[0]>max_value)
		data[0] = max_value;
	if (data[1]>max_value)
		data[1] = max_value;
}

void vec2d::clamp_min(const DBL min_value)
{
	if (data[0]<min_value)
		data[0] = min_value;
	if (data[1]<min_value)
		data[1] = min_value;
}

//void vec2d::gl_clear_color() const
//{
//	glClearColor(data[0], data[1], data[2], 0.0);
//}

string vec2d::print(const char* fmt) const
{
	char buffer[2048];
	string s;
	sprintf(buffer, fmt, data[0], data[1]);
	s = buffer;
	//s += "<"+dbl2str(data[0])+", "+dbl2str(data[1])+", "+dbl2str(data[2])+">";
	return s;
}

//void vec2d::draw_navigator(DBL size) const
//{
//	glBegin(GL_LINES);
//	// x-axis
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0]-size, data[1], data[2]);
//	glColor3f(1.0, 0.0, 0.0);
//	glVertex3d(data[0]+size, data[1], data[2]);
//	glVertex3d(data[0]+size, data[1], data[2]);
//	glColor3f(0.0, 1.0, 0.0);
//	glVertex3d(data[0], data[1]+size, data[2]);
//	glColor3f(1.0, 0.0, 0.0);
//	glVertex3d(data[0]+size, data[1], data[2]);
//	glColor3f(0.0, 0.0, 1.0);
//	glVertex3d(data[0], data[1], data[2]+size);
//	glColor3f(1.0, 0.0, 0.0);
//	glVertex3d(data[0]+size, data[1], data[2]);
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0], data[1], data[2]-size);
//	glVertex3d(data[0], data[1]-size, data[2]);
//	glColor3f(1.0, 0.0, 0.0);
//	glVertex3d(data[0]+size, data[1], data[2]);
//	// y-axis
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0], data[1]-size, data[2]);
//	glColor3f(0.0, 1.0, 0.0);
//	glVertex3d(data[0], data[1]+size, data[2]);
//	glVertex3d(data[0], data[1]+size, data[2]);
//	glColor3f(0.0, 0.0, 1.0);
//	glVertex3d(data[0], data[1], data[2]+size);
//	glColor3f(0.0, 1.0, 0.0);
//	glVertex3d(data[0], data[1]+size, data[2]);
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0]-size, data[1], data[2]);
//	glColor3f(0.0, 1.0, 0.0);
//	glVertex3d(data[0], data[1]+size, data[2]);
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0], data[1], data[2]-size);
//	// z-axis
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0], data[1], data[2]-size);
//	glColor3f(0.0, 0.0, 1.0);
//	glVertex3d(data[0], data[1], data[2]+size);
//	glColor3f(0.0, 0.0, 1.0);
//	glVertex3d(data[0], data[1], data[2]+size);
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0], data[1]-size, data[2]);
//	glColor3f(0.0, 0.0, 1.0);
//	glVertex3d(data[0], data[1], data[2]+size);
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0]-size, data[1], data[2]);
//	// black-axis
//	glColor3f(0.0, 0.0, 0.0);
//	glVertex3d(data[0]-size, data[1], data[2]);
//	glVertex3d(data[0], data[1]-size, data[2]);
//	glVertex3d(data[0], data[1], data[2]-size);
//	glVertex3d(data[0], data[1]-size, data[2]);
//	glVertex3d(data[0], data[1], data[2]-size);
//	glVertex3d(data[0]-size, data[1], data[2]);
//	glEnd();
//}

//void vec2d::to4f(SNGL tgt[4]) const
//{
//	tgt[0] = SNGL(data[0]);
//	tgt[1] = SNGL(data[1]);
//	tgt[2] = SNGL(data[2]);
//	tgt[3] = SNGL(1);
//}

//void vec2d::draw_star(DBL star_size) const
//{
//	glBegin(GL_LINES);
//	glVertex3d(data[0]-star_size, data[1], data[2]);
//	glVertex3d(data[0]+star_size, data[1], data[2]);
//	glVertex3d(data[0], data[1]-star_size, data[2]);
//	glVertex3d(data[0], data[1]+star_size, data[2]);
//	glVertex3d(data[0], data[1], data[2]-star_size);
//	glVertex3d(data[0], data[1], data[2]+star_size);
//	glEnd();
//}

unsigned int vec2d::read(FILE* f)
{
	fread(data, sizeof(data), 1, f);
	return sizeof(data);
}

DBL vec2d::length() const
{
	return sqrt(data[0]*data[0] + data[1]*data[1]);
}

vec2d vec2d::normalize() const
{
	DBL l = length();
	if (l!=0)
	{
		return vec2d(data[0]/l, data[1]/l);
	}
	return vec2d(data[0], data[1]);
}

vec2d vec2d::operator^(const vec2d& tgt)	// cross product: this x tgt
{
	return vec2d( tgt.data[1], -tgt.data[0]);
}

//void vec2d::gl_color() const
//{
//	glColor3dv(data);
//}

//void vec2d::gl_normal() const
//{
//	glNormal3dv(data);
//}

//void vec2d::gl_vertex() const
//{
//	glVertex3dv(data);
//}

/** @brief operator/
  *
  * @todo: document this function
  */
vec2d vec2d::operator/(const vec2d& a)
{
	return vec2d(data[0]/a.data[0], data[1]/a.data[1]);
}

vec2d vec2d::operator/(const DBL& a) // this / (a, a, a)
{
	return vec2d(data[0]/a, data[1]/a);
}

bool vec2d::operator==(const vec2d& a) const
{
	vec2d_DEBUG_MSG_FL("vec2d::operator==(const vec2d& a) const", 0)
	if ((fabs(data[0]-a.data[0])<EPSILON) && \
			(fabs(data[1]-a.data[1])<EPSILON))
				return true;
		return false;
}

vec2d::vec2d()
{
	vec2d_DEBUG_MSG_FL("vec2d::vec2d()", 0)
	data[0] = 0;
	data[1] = 0;
}

vec2d::vec2d(const vec2d& src)
{
	vec2d_DEBUG_MSG_FL("vec2d::vec2d(const vec2d& src)", 0)
	data[0] = src.data[0];
	data[1] = src.data[1];
}

vec2d::vec2d(const DBL a)
{
	vec2d_DEBUG_MSG_FL("vec2d::vec2d(const DBL a)", 0)
	data[0] = a;
	data[1] = a;
}

vec2d::vec2d(const DBL a, const DBL b)
{
	vec2d_DEBUG_MSG_FL("vec2d::vec2d(const DBL a, const DBL b)", 0)
	data[0] = a;
	data[1] = b;
}

const vec2d &vec2d::operator=(const vec2d& src)
{
	vec2d_DEBUG_MSG_FL("&vec2d::operator=(const vec2d& src)", 0)
	if(this!=&src)
	{
		//memcpy();
		data[0] = src.data[0];
		data[1] = src.data[1];
	}
	return *this;
}

DBL vec2d::operator[](const int idx) const
{
	DBL res;
	if((idx>1)||(idx<0))
		res = 0;
	else
		res = data[idx];
	return res;
}

vec2d vec2d::operator-(const vec2d& a)
{
	vec2d_DEBUG_MSG_FL("vec2d::operator-(const vec2d& a)",0)
	vec2d tmp = vec2d(data[0]-a.data[0], data[1]-a.data[1]);
	return tmp;
}

vec2d vec2d::operator-(const DBL& a)
{
	vec2d_DEBUG_MSG_FL("vec2d::operator-(const DBL& a)", 0)
	vec2d tmp = vec2d(data[0]-a, data[1]-a);
	return tmp;
}

vec2d vec2d::operator+(const vec2d& a)
{
	vec2d_DEBUG_MSG_FL("vec2d::operator+(const vec2d& a)", 0)
	vec2d tmp;
	tmp.data[0] = data[0]+a.data[0];
	tmp.data[1] = data[1]+a.data[1];
	return tmp;
}

vec2d vec2d::operator+(const DBL& a)
{
	vec2d_DEBUG_MSG_FL("vec2d::operator+(const DBL& a)", 0)
	vec2d tmp = vec2d(data[0]+a, data[1]+a);
	return tmp;
}
/** @brief operator*
  *
  * @todo: document this function
  */
vec2d vec2d::operator*(const vec2d& a)
{
	return vec2d(data[0]*a.data[0], data[1]*a.data[1]);
}

/** @brief operator*
  *
  * @todo: document this function
  */
vec2d vec2d::operator*(const DBL& a)
{
	return vec2d(data[0]*a, data[1]*a);
}


vec2d::~vec2d()
{
	vec2d_DEBUG_MSG_FL("vec2d::~vec2d()", 0)
}

void vec2d::print(ostream& output) const
{
	output << data[0] << " " << data[1];
}

void vec2d::input(istream& is)
{
	is >> data[0] >> data[1];
}

std::ostream& operator<<( std::ostream& os, const vec2d& t )
{
	// out to stream
	t.print(os);
//	os << t.data[0] << " " << t.data[1] << " " << t.data[2];
	return os;
}

std::istream& operator>>( std::istream& is, vec2d& t )
{
	// in from stream
	t.input(is);
//	is >> t.data[0] >> t.data[1] >> t.data[2];
	return is;
}
