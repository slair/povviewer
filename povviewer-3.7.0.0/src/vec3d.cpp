#include "GL/gl.h"
#include <GL/glx.h>

#include "common_funcs.hpp"
#include "vec3d.hpp"

void vec3d::mult_matrix(const DBL m[3][3])
{
	DBL a, b, c;
	// v[0]*mtx[0][0]+v[1]*mtx[1][0]+v[2]*mtx[2][0]
	a = data[0]*m[0][0]+data[1]*m[1][0]+data[2]*m[2][0];
	// v[0]*mtx[0][1]+v[1]*mtx[1][1]+v[2]*mtx[2][1]
	b = data[0]*m[0][1]+data[1]*m[1][1]+data[2]*m[2][1];
	// v[0]*mtx[0][2]+v[1]*mtx[1][2]+v[2]*mtx[2][2]
	c = data[0]*m[0][2]+data[1]*m[1][2]+data[2]*m[2][2];
	data[0] = a;
	data[1] = b;
	data[2] = c;
}

void vec3d::build_matrix(DBL m[3][3], const DBL angle) const
{
	DBL cosa = cos(angle);
	DBL sina = sin(angle);
	DBL x = data[0], y = data[1], z = data[2];
	// [cosa+(1.0-cosa)*x*x, (1.0-cosa)*x*y-sina*z, (1.0-cosa)*x*z+sina*y]
	// [(1.0-cosa)*y*x+sina*z, cosa+(1.0-cosa)*y*y, (1.0-cosa)*y*z-sina*x]
	// [(1.0-cosa)*z*x-sina*y, (1.0-cosa)*z*y+sina*x, cosa+(1.0-cosa)*z*z]
	m[0][0] = cosa+(1-cosa)*x*x;
	m[0][1] = (1.0-cosa)*x*y-sina*z;
	m[0][2] = (1.0-cosa)*x*z+sina*y;

	m[1][0] = (1.0-cosa)*y*x+sina*z;
	m[1][1] = cosa+(1.0-cosa)*y*y;
	m[1][2] = (1.0-cosa)*y*z-sina*x;

	m[2][0] = (1.0-cosa)*z*x-sina*y;
	m[2][1] = (1.0-cosa)*z*y+sina*x;
	m[2][2] = cosa+(1.0-cosa)*z*z;
}

void vec3d::clamp_max(const DBL max_value)
{
	if (data[0]>max_value)
		data[0] = max_value;
	if (data[1]>max_value)
		data[1] = max_value;
	if (data[2]>max_value)
		data[2] = max_value;
}

void vec3d::clamp_min(const DBL min_value)
{
	if (data[0]<min_value)
		data[0] = min_value;
	if (data[1]<min_value)
		data[1] = min_value;
	if (data[2]<min_value)
		data[2] = min_value;
}

void vec3d::gl_clear_color() const
{
	glClearColor(data[0], data[1], data[2], 0.0);
}

string vec3d::print(const char* fmt) const
{
	char buffer[2048];
	string s;
	sprintf(buffer, fmt, data[0], data[1], data[2]);
	s = buffer;
	//s += "<"+dbl2str(data[0])+", "+dbl2str(data[1])+", "+dbl2str(data[2])+">";
	return s;
}

void vec3d::draw_navigator(DBL size) const
{
	glBegin(GL_LINES);
	// x-axis
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0]-size, data[1], data[2]);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3d(data[0]+size, data[1], data[2]);
	glVertex3d(data[0]+size, data[1], data[2]);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3d(data[0], data[1]+size, data[2]);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3d(data[0]+size, data[1], data[2]);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3d(data[0], data[1], data[2]+size);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3d(data[0]+size, data[1], data[2]);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0], data[1], data[2]-size);
	glVertex3d(data[0], data[1]-size, data[2]);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3d(data[0]+size, data[1], data[2]);
	// y-axis
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0], data[1]-size, data[2]);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3d(data[0], data[1]+size, data[2]);
	glVertex3d(data[0], data[1]+size, data[2]);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3d(data[0], data[1], data[2]+size);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3d(data[0], data[1]+size, data[2]);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0]-size, data[1], data[2]);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3d(data[0], data[1]+size, data[2]);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0], data[1], data[2]-size);
	// z-axis
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0], data[1], data[2]-size);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3d(data[0], data[1], data[2]+size);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3d(data[0], data[1], data[2]+size);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0], data[1]-size, data[2]);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3d(data[0], data[1], data[2]+size);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0]-size, data[1], data[2]);
	// black-axis
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(data[0]-size, data[1], data[2]);
	glVertex3d(data[0], data[1]-size, data[2]);
	glVertex3d(data[0], data[1], data[2]-size);
	glVertex3d(data[0], data[1]-size, data[2]);
	glVertex3d(data[0], data[1], data[2]-size);
	glVertex3d(data[0]-size, data[1], data[2]);
	glEnd();
}

void vec3d::to4f(SNGL tgt[4]) const
{
	tgt[0] = SNGL(data[0]);
	tgt[1] = SNGL(data[1]);
	tgt[2] = SNGL(data[2]);
	tgt[3] = SNGL(1);
}

void vec3d::draw_star(DBL star_size) const
{
	glBegin(GL_LINES);
	glVertex3d(data[0]-star_size, data[1], data[2]);
	glVertex3d(data[0]+star_size, data[1], data[2]);
	glVertex3d(data[0], data[1]-star_size, data[2]);
	glVertex3d(data[0], data[1]+star_size, data[2]);
	glVertex3d(data[0], data[1], data[2]-star_size);
	glVertex3d(data[0], data[1], data[2]+star_size);
	glEnd();
}

unsigned int vec3d::read(FILE* f)
{
	fread(data, sizeof(data), 1, f);
	return sizeof(data);
}

DBL vec3d::length() const
{
	return sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
}

vec3d vec3d::normalize() const
{
	DBL l = length();
	if (l!=0)
	{
		return vec3d(data[0]/l, data[1]/l, data[2]/l);
	}
	return vec3d(data[0], data[1], data[2]);
}

vec3d vec3d::operator^(const vec3d& tgt)	// cross product: this x tgt
{
	return vec3d( \
		data[1]*tgt.data[2] - data[2]*tgt.data[1], \
		data[2]*tgt.data[0] - data[0]*tgt.data[2], \
		data[0]*tgt.data[1] - data[1]*tgt.data[0] \
	);
}

void vec3d::gl_color() const
{
	glColor3dv(data);
}

void vec3d::gl_normal() const
{
	glNormal3dv(data);
}

void vec3d::gl_vertex() const
{
	glVertex3dv(data);
}

/** @brief operator/
  *
  * @todo: document this function
  */
vec3d vec3d::operator/(const vec3d& a) const
{
	return vec3d(data[0]/a.data[0], data[1]/a.data[1], data[2]/a.data[2]);
}

vec3d vec3d::operator/(const DBL& a) const // this / (a, a, a)
{
	return vec3d(data[0]/a, data[1]/a, data[2]/a);
}

bool vec3d::operator==(const vec3d& a) const
{
	vec3d_DEBUG_MSG_FL("vec3d::operator==(const vec3d& a) const", 0)
	if (fabs(data[0]-a.data[0])<EPSILON)
		if (fabs(data[1]-a.data[1])<EPSILON)
			if (fabs(data[2]-a.data[2])<EPSILON)
				return true;
	return false;
}

/** @brief operator<
  *
  * @todo: document this function
  */
bool vec3d::operator<(const vec3d& a) const
{
	if ((a.data[0]-data[0])>EPSILON)
		return true;
	if ((a.data[1]-data[1])>EPSILON)
		return true;
	if ((a.data[2]-data[2])>EPSILON)
		return true;
//	if ((data[0]==a.data[0])&&(data[1]<a.data[1]))
//		return true;
//	if ((data[0]==a.data[0])&&(data[1]==a.data[1])&&(data[2]<a.data[2]))
//		return true;
	return false;
}

vec3d::vec3d()
{
	vec3d_DEBUG_MSG_FL("vec3d::vec3d()", 0)
//	data[0] = 0;
//	data[1] = 0;
//	data[2] = 0;
	memset(data, 0, 3*sizeof(DBL));
}

vec3d::vec3d(const vec3d& src)
{
	vec3d_DEBUG_MSG_FL("vec3d::vec3d(const vec3d& src)", 0)
//	data[0] = src.data[0];
//	data[1] = src.data[1];
//	data[2] = src.data[2];
	memcpy(data, src.data, 3*sizeof(DBL));
}

vec3d::vec3d(const DBL a)
{
	vec3d_DEBUG_MSG_FL("vec3d::vec3d(const DBL a)", 0)
	data[0] = a;
	data[1] = a;
	data[2] = a;
}

vec3d::vec3d(const DBL a, const DBL b, const DBL c)
{
	vec3d_DEBUG_MSG_FL("vec3d::vec3d(const DBL a, const DBL b, const DBL c)", 0)
	data[0] = a;
	data[1] = b;
	data[2] = c;
}

const vec3d &vec3d::operator=(const vec3d& src)
{
	vec3d_DEBUG_MSG_FL("&vec3d::operator=(const vec3d& src)", 0)
	if(this!=&src)
	{
//		data[0] = src.data[0];
//		data[1] = src.data[1];
//		data[2] = src.data[2];
		memcpy(data, src.data, 3*sizeof(DBL));
	}
	return *this;
}

DBL vec3d::operator[](const int idx) const
{
//	DBL res;
//	if((idx>2)||(idx<0))
//		res = 0;
//	else
//		res = data[idx];
//	return res;
	return data[idx];
}

vec3d vec3d::operator-(const vec3d& a) const
{
	vec3d_DEBUG_MSG_FL("vec3d::operator-(const vec3d& a)",0)
	vec3d tmp = vec3d(data[0]-a.data[0], data[1]-a.data[1], data[2]-a.data[2]);
	return tmp;
}

vec3d vec3d::operator-(const DBL& a) const
{
	vec3d_DEBUG_MSG_FL("vec3d::operator-(const DBL& a)", 0)
	vec3d tmp = vec3d(data[0]-a, data[1]-a, data[2]-a);
	return tmp;
}

vec3d vec3d::operator+(const vec3d& a) const
{
	vec3d_DEBUG_MSG_FL("vec3d::operator+(const vec3d& a)", 0)
	vec3d tmp;
	tmp.data[0] = data[0]+a.data[0];
	tmp.data[1] = data[1]+a.data[1];
	tmp.data[2] = data[2]+a.data[2];
	return tmp;
}

vec3d vec3d::operator+(const DBL& a) const
{
	vec3d_DEBUG_MSG_FL("vec3d::operator+(const DBL& a)", 0)
	vec3d tmp = vec3d(data[0]+a, data[1]+a, data[2]+a);
	return tmp;
}
/** @brief operator*
  *
  * @todo: document this function
  */
vec3d vec3d::operator*(const vec3d& a) const
{
	return vec3d(data[0]*a.data[0], data[1]*a.data[1], data[2]*a.data[2]);
}

/** @brief operator*
  *
  * @todo: document this function
  */
vec3d vec3d::operator*(const DBL& a) const
{
	return vec3d(data[0]*a, data[1]*a, data[2]*a);
}


vec3d::~vec3d()
{
	vec3d_DEBUG_MSG_FL("vec3d::~vec3d()", 0)
}

void vec3d::print(ostream& output) const
{
	output << data[0] << ", " << data[1] << ", " << data[2];
}

void vec3d::input(istream& is)
{
	is >> data[0] >> data[1] >> data[2];
}

std::ostream& operator<<( std::ostream& os, const vec3d& t )
{
	// out to stream
	t.print(os);
//	os << t.data[0] << " " << t.data[1] << " " << t.data[2];
	return os;
}

std::istream& operator>>( std::istream& is, vec3d& t )
{
	// in from stream
	t.input(is);
//	is >> t.data[0] >> t.data[1] >> t.data[2];
	return is;
}

/** @brief vdot
  *
  * @todo: document this function
  */
DBL vec3d::vdot(const vec3d& tgt)
{
	return data[0]*tgt.data[0] + data[1]*tgt.data[1] + data[2]*tgt.data[2];
}

/** @brief cos_alpha
  *
  * @todo: document this function
  */
DBL cos_alpha(const vec3d a, const vec3d b)
{
	return ((a[0]*b[0]+a[1]*b[1]+a[2]*b[2])/(a.length()*b.length()));
}

/** @brief vec3d_from_vec3s
  *
  * @todo: document this function
  */
vec3d vec3d_from_vec3s(const vec3s src)
{
	return vec3d(DBL(src[0])/DBL(32767.0), DBL(src[1])/DBL(32767.0), DBL(src[2])/DBL(32767.0));
}

/** @brief gl_rasterpos
  *
  * @todo: document this function
  */
void vec3d::gl_rasterpos() const
{
	glRasterPos3dv(data);
}

/** @brief vec3d
  *
  * @todo: document this function
  */
 vec3d::vec3d(const string val)
{
	size_t first_comma, last_comma;
	first_comma = val.find_first_of(',');
	last_comma = val.find_last_of(',');
	if((first_comma!=string::npos)&&(last_comma!=string::npos))
	{
		data[0] = atof(val.substr(0,first_comma).c_str());
		data[1] = atof(val.substr(first_comma+1,last_comma - first_comma-1).c_str());
		data[2] = atof(val.substr(last_comma+1,val.size() - last_comma-1).c_str());
	}
	else
	{
		DEBUG_MSG("Bad string '"<<val<<"' in vec3d::vec3d(const string val)")
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
	}
}

/** @brief draw_vector
  *
  * @todo: document this function
  */
void vec3d::draw_vector(vec3d loc, vec3d color) const
{
	vec3d head = loc + vec3d(data[0], data[1], data[2]);
	color.gl_color();
	glBegin(GL_LINES);
	loc.gl_vertex();
	head.gl_vertex();
	glEnd();
}

/** @brief draw_vector_a
  *
  * @todo: document this function
  */
void vec3d::draw_vector_a(vec3d loc, vec3d color) const
{
	vec3d self = vec3d(data[0], data[1], data[2]);
	vec3d head = loc + self;
	color.gl_color();
	glBegin(GL_LINES);
	loc.gl_vertex();
	head.gl_vertex();
	//head.gl_vertex();
	glEnd();
}

/** @brief compare_ex
  *
  * @todo: document this function
  */
bool compare_ex(const vec3d a, const vec3d b)
{
	if ((a[0]==b[0])&&(a[1]==b[1])&&(a[2]==b[2]))
		return true;
	return false;
}
