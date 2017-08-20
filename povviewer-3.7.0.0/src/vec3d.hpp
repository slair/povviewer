#ifndef _VEC3D_HPP_
#define _VEC3D_HPP_

#include <iostream>
#include <vector>
#include <math.h>

#include "global_defs.hpp"

//#define DEBUG_vec3d
#ifdef DEBUG_vec3d
	#define vec3d_DEBUG_MSG(msg)  cout << msg << endl;
	#define vec3d_DEBUG_MSG_FL(msg,offset) cout << "\nFile:"	<< __FILE__ << " Line:" << dec << __LINE__+offset << endl << msg <<  endl;
#else
	#define vec3d_DEBUG_MSG(msg)
	#define vec3d_DEBUG_MSG_FL(msg,offset)
#endif

class vec3d
{
	protected:
		DBL data[3];
	public:
		~vec3d();
		vec3d();
		vec3d(const DBL a);
		vec3d(const DBL a, const DBL b, const DBL c);
		vec3d(const std::string val);
		vec3d(const vec3d& src);	// copy constructor
		const vec3d &operator=(const vec3d& src);	//
		DBL operator[](const int idx) const;
		// math operations
		vec3d operator-(const vec3d& a) const;	// this - a
		vec3d operator-(const DBL& a) const;	// this - (a, a, a)
		vec3d operator+(const vec3d& a) const;	// this + a
		vec3d operator+(const DBL& a) const;	// this + (a, a, a)
		vec3d operator*(const vec3d& a) const;	// this * a
		vec3d operator*(const DBL& a) const; // this * (a, a, a)
		vec3d operator/(const vec3d& a) const; // this / a
		vec3d operator/(const DBL& a) const; // this / (a, a, a)
		// vector operations
		DBL length() const;
		vec3d normalize() const;
		vec3d operator^(const vec3d& tgt);	// cross product: this x tgt
		DBL vdot(const vec3d& tgt); 	// dot product: this (dot) tgt
		// compare operations
		bool operator==(const vec3d& a) const;
		bool operator<(const vec3d& a) const;
		// io subsystem
		void print(std::ostream& output) const;
		void input(std::istream& is);
		unsigned int read(FILE* f);
		void gl_normal() const;
		void gl_vertex() const;
		void gl_color() const;
		void gl_clear_color() const;
		void draw_star(DBL star_size) const;
		void draw_vector(vec3d loc, vec3d color) const;
		void draw_vector_a(vec3d loc, vec3d color) const;
		void to4f(SNGL tgt[4]) const;
		void draw_navigator(DBL size) const;
		std::string print(const char* fmt) const;
		void clamp_max(const DBL max_value);
		void clamp_min(const DBL min_value);
		void build_matrix(DBL m[3][3], const DBL angle) const;
		void mult_matrix(const DBL m[3][3]);
		void gl_rasterpos() const;
};

bool compare_ex(const vec3d a, const vec3d b);

typedef vec3d triangle[3];

DBL cos_alpha(const vec3d a, const vec3d b);

vec3d vec3d_from_vec3s(const vec3s src);

std::ostream& operator<<( std::ostream& os, const vec3d& t );

#endif
