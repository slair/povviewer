#ifndef _LINE3D_HPP_
#define _LINE3D_HPP_

#include <iostream>
#include <vector>
#include <set>
#include <math.h>

#include "vec3d.hpp"

//#define DEBUG_line3d
#ifdef DEBUG_line3d
	#define line3d_DEBUG_MSG(msg)  cout << msg << endl;
	#define line3d_DEBUG_MSG_FL(msg,offset) cout << "\nFile:"	<< __FILE__ << " Line:" << dec << __LINE__+offset << endl << msg <<  endl;
#else
	#define line3d_DEBUG_MSG(msg)
	#define line3d_DEBUG_MSG_FL(msg,offset)
#endif

class line3d
{
	public:
		vec3d p1;
		vec3d p2;
		line3d();
		line3d(const vec3d a, const vec3d b);
		line3d(const line3d& src);
		const line3d &operator=(const line3d& src);
		bool operator<(const line3d& src) const;
		void dump(std::ostream& line3d_stream) const;
		void print(std::ostream& output) const;
		DBL length() const;
};
std::ostream& operator<<( std::ostream& os, const line3d& t );


typedef std::vector<line3d> vector_line3d;
//typedef std::vector<line3d>::iterator i_vector_line3d;
void add_line(vector_line3d* lines, const vec3d a, const vec3d b);

typedef std::set<line3d> set_line3d;
//typedef std::set<line3d>::iterator i_set_line3d;
void add_line(set_line3d* lines, const vec3d a, const vec3d b);


#endif
