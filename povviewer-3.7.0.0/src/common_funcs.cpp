#include "GL/gl.h"
#include <GL/glx.h>
#include <GL/glu.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h>
using namespace std;

#include "global_defs.hpp"
#include "common_funcs.hpp"
#include "vec3d.hpp"

extern unsigned int my_pid;

extern vec3d* v_pool;
extern unsigned int v_pool_size, v_pool_capacity;

void tessCombine(GLdouble coords[3], void* vertex_data[4], GLfloat weight[4], void** dataOut)
{
	(void)vertex_data;
	(void)weight;
	DBL* v = new DBL[3];
	v[0] = coords[0];
	v[1] = coords[1];
	v[2] = coords[2];
	*dataOut=v;
}

void tessError(GLenum error)
{
	const GLubyte *szError = gluErrorString(error);
	cout << "Tesselation ERROR! - " << szError << endl;
}

void normal_with_vertex(DBL vertex[6])
{
	glNormal3d(vertex[3], vertex[4], vertex[5]);
	glVertex3d(vertex[0], vertex[1], vertex[2]);
}

string get_current_folder()
{
	char buff[PATH_MAX];
	if (getcwd(buff, PATH_MAX-1)==NULL)
		perror("Dumb error in getcwd!");
	return string(buff);
}

string get_folder_from_path(const string path)
{
	int last_slash = path.rfind("/");
	if (last_slash==-1) last_slash = path.rfind("\\");
	return path.substr(0, last_slash);
}

string get_file_from_path(const string path)
{
	int last_slash = path.rfind("/")+1;
	if (last_slash==-1)
		last_slash = path.rfind("\\")+1;
	return path.substr(last_slash, path.length()-last_slash);
}

void print_usage_and_help()
{
	cout << "Need scene name as parameter!" << endl;
}

/** @brief get_config_dir
  *
  * @todo: document this function
  */
string get_config_dir()
{
	string cfg_dir = getenv("HOME");
	cfg_dir += "/.povviewer";
	return cfg_dir;
}

string dbl2str(const DBL n)
{
	std::string str;
	std::stringstream conv;
	conv<<n;
	conv>>str;
	return str;
}

string flt2str(const SNGL n)
{
	std::string str;
	std::stringstream conv;
	conv<<n;
	conv>>str;
	return str;
}

string int2str(const int n)
{
	std::string str;
	std::stringstream conv;
	conv<<n;
	conv>>str;
	return str;
}

/** @brief smart_format
  *
  * @todo: document this function
  */
string smart_format(const unsigned int num)
{
	char buffer[2048];
	string s;
	sprintf(buffer, "%10d", num);
	s = buffer;
	return s;
}

/** @brief v_pool_find
  *
  * @todo: document this function
  */
int v_pool_find(const vec3d v)
{
	for (unsigned int i=0; i<v_pool_size; i++)
	{
		if (fabs(v[0]-v_pool[i][0])<EPSILON)
		{
			if (fabs(v[1]-v_pool[i][1])<EPSILON)
			{
				if (fabs(v[2]-v_pool[i][2])<EPSILON)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

/** @brief v_pool_add
  *
  * @todo: document this function
  */
unsigned int v_pool_add(const vec3d v)
{
	v_pool[v_pool_size++] = v;
	if (v_pool_size==v_pool_capacity)
		v_pool_grow();
	return (v_pool_size-1);
}

/** @brief v_pool_grow
  *
  * @todo: document this function
  */
bool v_pool_grow()
{
	vec3d* new_pool = new vec3d[v_pool_capacity+GROW_POOL_SIZE];
	memcpy(new_pool, v_pool, v_pool_size*sizeof(vec3d));
	delete [] v_pool;
	v_pool = new_pool;
	v_pool_capacity += GROW_POOL_SIZE;
	DEBUG_MSG("v_pool_grow() v_pool_capacity = "<<v_pool_capacity)
	return true;
}

/** @brief v_pool_init
  *
  * @todo: document this function
  */
bool v_pool_init()
{
	v_pool_capacity = 0;
	v_pool_size = 0;
	v_pool = new vec3d[START_POOL_SIZE];
	if (v_pool==NULL)
	{
		DEBUG_MSG("Cannot allocate v_pool["<<START_POOL_SIZE<<"]")
		exit(234);
	}
	v_pool_capacity = START_POOL_SIZE;
	v_pool_size = 0;
	return true;
}

/** @brief v_pool_clear
  *
  * @todo: document this function
  */
bool v_pool_clear()
{
	delete [] v_pool;
	v_pool = NULL;
	v_pool_init();
	return true;
}

/** @brief v_pool_destroy
  *
  * @todo: document this function
  */
void v_pool_destroy()
{
	delete [] v_pool;
}

/** @brief get_vmsize_kb
  *
  * @todo: document this function
  */
unsigned int get_vmsize_kb()
{
	#define MAX_NUM_BUFFER 40
	unsigned int res = 0;
	string status_file_name = "/proc/"+int2str(my_pid)+"/status";
	char buffer[1024];
	//char num_buffer[MAX_NUM_BUFFER];

	FILE *status_file = fopen(status_file_name.c_str(), "r");

	if (!status_file)
		return res;

	while (fgets(buffer, sizeof(buffer), status_file))
	{
		if (strncmp(buffer, "VmSize:", 7)==0)
		{
			for(unsigned int i=0; i<MAX_NUM_BUFFER; i++)
			{
				if (buffer[i]=='k')
				{
					buffer[i]=0;
					break;
				}
				else
				{
					if ((buffer[i]<'0')||(buffer[i]>'9'))
					{
						buffer[i]=' ';
					}
				}
			}
			res = atoi(buffer);
			break;
		}
	}
	fclose(status_file);
	return res;
}

/** @brief get_param_value
  *
  * @todo: document this function
  */
string get_param_value(const char* line)
{
	string res = "";
	unsigned int line_length = strlen(line);
	if (line_length>13)
	{
		unsigned int i = 13;
		for (i=13; i<line_length; i++)
		{
			if ((line[i]!=':')&&(line[i]!='\n')&&(line[i]!='\r')&&(line[i]!='='))
			{

			}
			else
			{
				break;
			}
		}
		for(unsigned int j=(i+1);j<line_length;j++)
		{
			if ((line[j]!='\r')&&(line[j]!='\n'))
			{
				res += line[j];
			}
			else
			{
				break;
			}
		}
	}
	return res;
}

/** @brief get_param_name
  *
  * @todo: document this function
  */
string get_param_name(const char* line)
{
	string res = "";
	unsigned int line_length = strlen(line);
	if (line_length>13)
	{
		for (unsigned int i=13; i<line_length; i++)
		{
			if ((line[i]!=':')&&(line[i]!='\r')&&(line[i]!='\n')&&(line[i]!='='))
			{
				res += line[i];
			}
			else
			{
				break;
			}
		}
	}
	return res;
}

void create_sphere(const vec3d center, const DBL radius, const int numlongitudes, const int numlatitudes, gl_object *globj)
{
	DBL u, u2, v, v1, sinv, sinv1;
	vec3d point, n;
	vec3d point0, point1, point2, point3;
	int i,j;

	// top triangle fan
	point0 = vec3d(0, radius, 0);
	v = double(1) / numlatitudes;
	sinv = sin(v*PI)*radius;
	for(i=0; i<numlongitudes; i++)
	{
		u = double(i) / numlongitudes;
		point1 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		u = double(i+1) / numlongitudes;
		point2 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		globj->append_face(point2 + center, point1 + center, point0 + center);
	}

	// bottom triangle fan
	point0 = vec3d(0, -radius, 0);
	v = double(numlatitudes-1) / numlatitudes;
	sinv = sin(v*PI)*radius;
	for(i=0; i<numlongitudes; i++)
	{
		u = double(i) / numlongitudes;
		point1 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		u = double(i+1) / numlongitudes;
		point2 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
		globj->append_face(point0 + center, point1 + center, point2 + center);
	}

	// quad strips
	for(i=0; i<numlatitudes-2; i++)
	{
		v = double(i+1) / numlatitudes;
		v1 = double(i+2) / numlatitudes;
		sinv = sin(v*PI)*radius;
		sinv1 = sin(v1*PI)*radius;
		for(j=0; j<numlongitudes; j++)
		{
			u = double(j) / numlongitudes;
			u2 = double(j+1) / numlongitudes;
			point0 = vec3d(-cos(u*PI2)*sinv, cos(v*PI)*radius, sin(u*PI2)*sinv);
			point1 = vec3d(-cos(u*PI2)*sinv1, cos(v1*PI)*radius, sin(u*PI2)*sinv1);
			point2 = vec3d(-cos(u2*PI2)*sinv1, cos(v1*PI)*radius, sin(u2*PI2)*sinv1);
			point3 = vec3d(-cos(u2*PI2)*sinv, cos(v*PI)*radius, sin(u2*PI2)*sinv);
			globj->append_face(point3 + center, point2 + center, point1 + center, point0 + center);
		}
	}
}

string tr(const string msg)
{
	return msg;
}
