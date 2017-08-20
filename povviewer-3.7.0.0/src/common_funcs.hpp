#ifndef _COMMON_FUNCS_HPP_
#define _COMMON_FUNCS_HPP_

#include "GL/gl.h"
#include <GL/glx.h>
#include <GL/glu.h>

#include <string>
using namespace std;

#include "global_defs.hpp"
#include "gl_object.hpp"

class vec3d;

bool v_pool_init();
int v_pool_find(const vec3d v);
bool v_pool_grow();
unsigned int v_pool_add(const vec3d v);
bool v_pool_clear();
void v_pool_destroy();

string get_current_folder();
string get_folder_from_path(const string path);
string get_file_from_path(const string path);
string get_config_dir();

string smart_format(const unsigned int num);

void print_usage_and_help();

string dbl2str(const DBL n);
string flt2str(const SNGL n);
string int2str(const int n);

void normal_with_vertex(DBL vertex[6]);

void tessError(GLenum error);

void tessCombine(GLdouble coords[3], void* vertex_data[4], GLfloat weight[4], \
	void** dataOut);

unsigned int get_vmsize_kb();

string get_param_name(const char* line);
string get_param_value(const char* line);

void create_sphere(const vec3d center, const DBL radius, \
	const int numlongitudes, const int numlatitudes, gl_object *globj);

string tr(const string msg);

#endif
