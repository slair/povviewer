#ifndef _VEC2D_HPP_
#define _VEC2D_HPP_

#include <iostream>
#include <math.h>
using namespace std;

#include "global_defs.hpp"

//#define DEBUG_vec2d
#ifdef DEBUG_vec2d
	#define vec2d_DEBUG_MSG(msg)  cout << msg << endl;
	#define vec2d_DEBUG_MSG_FL(msg,offset) cout << "\nFile:"	<< __FILE__ << " Line:" << dec << __LINE__+offset << endl << msg <<  endl;
#else
	#define vec2d_DEBUG_MSG(msg)
	#define vec2d_DEBUG_MSG_FL(msg,offset)
#endif

class vec2d
{
	protected:
		DBL data[2];
	public:
		~vec2d();
		vec2d();
		vec2d(const DBL a);
		vec2d(const DBL a, const DBL b);
		vec2d(const vec2d& src);	// copy constructor
		const vec2d &operator=(const vec2d& src);	//
		DBL operator[](const int idx) const;
		// math operations
		vec2d operator-(const vec2d& a);	// this - a
		vec2d operator-(const DBL& a);	// this - (a, a)
		vec2d operator+(const vec2d& a);	// this + a
		vec2d operator+(const DBL& a);	// this + (a, a)
		vec2d operator*(const vec2d& a);	// this * a
		vec2d operator*(const DBL& a); // this * (a, a)
		vec2d operator/(const vec2d& a); // this / a
		vec2d operator/(const DBL& a); // this / (a, a)
		// vector operations
		DBL length() const;
		vec2d normalize() const;
		vec2d operator^(const vec2d& tgt);	// cross product: this x tgt
		vec2d operator,(const vec2d& tgt); 	// dot product: this (dot) tgt
		// compare operations
		bool operator==(const vec2d& a) const;
		// io subsystem
		void print(ostream& output) const;
		void input(istream& is);
		unsigned int read(FILE* f);
//		void gl_normal() const;
		void gl_vertex(DBL z) const;
//		void gl_color() const;
//		void gl_clear_color() const;
//		void draw_star(DBL star_size) const;
//		void to4f(SNGL tgt[4]) const;
//		void draw_navigator(DBL size) const;
		string print(const char* fmt) const;
		void clamp_max(const DBL max_value);
		void clamp_min(const DBL min_value);
//		void build_matrix(DBL m[3][3], const DBL angle) const;
//		void mult_matrix(const DBL m[3][3]);
};
#endif
