/*******************************************************************************
 * frame.h
 *
 * This header file is included by all C modules in POV-Ray. It defines all
 * globally-accessible types and constants.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/frame.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef FRAME_H
#define FRAME_H

// Generic header for all modules

#include <new>

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <vector>
#include <stack>

#ifndef MATH_H_INCLUDED
#include <math.h>
#endif

#include "base/configbase.h"
#include "base/types.h"

#include "backend/configbackend.h"
#include "backend/support/simplevector.h"
#include "backend/control/messagefactory.h"
#include "backend/colour/spectral.h"

#include "pov_mem.h"

namespace pov
{

using namespace pov_base;

// these defines affect the maximum size of some types based on FixedSimpleVector

#ifndef MEDIA_VECTOR_SIZE
#define MEDIA_VECTOR_SIZE               256
#endif

#ifndef MEDIA_INTERVAL_VECTOR_SIZE
#define MEDIA_INTERVAL_VECTOR_SIZE      256
#endif

#ifndef LIT_INTERVAL_VECTOR_SIZE
#define LIT_INTERVAL_VECTOR_SIZE        512
#endif

#ifndef LIGHT_INTERSECTION_VECTOR_SIZE
#define LIGHT_INTERSECTION_VECTOR_SIZE  512
#endif

#ifndef LIGHTSOURCE_VECTOR_SIZE
#define LIGHTSOURCE_VECTOR_SIZE         1024
#endif

#ifndef WEIGHTEDTEXTURE_VECTOR_SIZE
#define WEIGHTEDTEXTURE_VECTOR_SIZE     512
#endif

#ifndef RAYINTERIOR_VECTOR_SIZE
#define RAYINTERIOR_VECTOR_SIZE         512
#endif

/*****************************************************************************
 *
 * Typedefs that need to be known here.
 *
 *****************************************************************************/

class ObjectBase;
typedef ObjectBase * ObjectPtr;
typedef const ObjectBase * ConstObjectPtr;

class CompoundObject;


/*****************************************************************************
 *
 * Scalar, color and vector stuff.
 *
 *****************************************************************************/

typedef DBL UV_VECT[2];
typedef DBL VECTOR[3];
typedef DBL VECTOR_4D[4];
typedef DBL MATRIX[4][4];
typedef DBL EXPRESS[5];
typedef COLC COLOUR[5];
typedef COLC RGB[3];
typedef SNGL SNGL_VECT[3];

// Vector array elements.
enum
{
	U = 0,
	V = 1
};

enum
{
	X = 0,
	Y = 1,
	Z = 2,
	T = 3,
	W = 3
};

// Color array elements.
enum
{
	pRED    = 0,
	pGREEN  = 1,
	pBLUE   = 2,
	pFILTER = 3,
	pTRANSM = 4
};

// Macros to manipulate scalars, vectors, and colors.

inline void Assign_Vector(VECTOR d, const VECTOR s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Assign_Vector(VECTOR d, const SNGL_VECT s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Assign_Vector(SNGL_VECT d, const VECTOR s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Assign_Vector(SNGL_VECT d, const SNGL_VECT s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Assign_UV_Vect(UV_VECT d, const UV_VECT s)
{
	d[X] = s[X];
	d[Y] = s[Y];
}

inline void Assign_Vector_4D(VECTOR_4D d, const VECTOR_4D s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
	d[T] = s[T];
}

inline void Assign_Colour(COLOUR d, const COLOUR s)
{
	d[pRED] = s[pRED];
	d[pGREEN] = s[pGREEN];
	d[pBLUE] = s[pBLUE];
	d[pFILTER] = s[pFILTER];
	d[pTRANSM] = s[pTRANSM];
}

inline void Assign_Colour_Express(COLOUR d, const EXPRESS s)
{
	d[pRED] = s[pRED];
	d[pGREEN] = s[pGREEN];
	d[pBLUE] = s[pBLUE];
	d[pFILTER] = s[pFILTER];
	d[pTRANSM] = s[pTRANSM];
}

inline void Assign_Express(EXPRESS d, const EXPRESS s)
{
	d[pRED] = s[pRED];
	d[pGREEN] = s[pGREEN];
	d[pBLUE] = s[pBLUE];
	d[pFILTER] = s[pFILTER];
	d[pTRANSM] = s[pTRANSM];
}

inline void Make_ColourA(COLOUR c, COLC r, COLC g, COLC b, COLC a, COLC t)
{
	c[pRED] = r;
	c[pGREEN] = g;
	c[pBLUE] = b;
	c[pFILTER] = a;
	c[pTRANSM] = t;
}

inline void Make_Vector(VECTOR v, DBL a, DBL b, DBL c)
{
	v[X] = a;
	v[Y] = b;
	v[Z] = c;
}

inline void Make_Vector(SNGL_VECT v, SNGL a, SNGL b, SNGL c)
{
	v[X] = a;
	v[Y] = b;
	v[Z] = c;
}

inline void Make_UV_Vector(UV_VECT v, DBL a, DBL b)
{
	v[U] = a;
	v[V] = b;
}

inline void Destroy_Float(DBL *x)
{
	if(x != NULL)
		POV_FREE(x);
}

inline void Destroy_Vector(VECTOR *x)
{
	if(x != NULL)
		POV_FREE(x);
}

inline void Destroy_UV_Vect(UV_VECT *x)
{
	if(x != NULL)
		POV_FREE(x);
}

inline void Destroy_Vector_4D(VECTOR_4D *x)
{
	if(x != NULL)
		POV_FREE(x);
}

inline void Destroy_Colour(COLOUR *x)
{
	if(x != NULL)
		POV_FREE(x);
}

#if 0
#pragma mark * Vector2d
#endif

class Vector2d
{
	public:
		Vector2d()
		{
			vect[X] = 0.0;
			vect[Y] = 0.0;
		}

		explicit Vector2d(DBL d)
		{
			vect[X] = d;
			vect[Y] = d;
		}

		Vector2d(DBL x, DBL y)
		{
			vect[X] = x;
			vect[Y] = y;
		}

		explicit Vector2d(const UV_VECT vi)
		{
			vect[X] = vi[X];
			vect[Y] = vi[Y];
		}

		explicit Vector2d(const SNGL_VECT vi)
		{
			vect[X] = DBL(vi[X]);
			vect[Y] = DBL(vi[Y]);
		}

		Vector2d(const Vector2d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
		}

		Vector2d& operator=(const Vector2d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
			return *this;
		}

		DBL operator[](int idx) const { return vect[idx]; }
		DBL& operator[](int idx) { return vect[idx]; }

		Vector2d operator+(const Vector2d& b) const
		{
			return Vector2d(vect[X] + b[X], vect[Y] + b[Y]);
		}

		Vector2d operator-(const Vector2d& b) const
		{
			return Vector2d(vect[X] - b[X], vect[Y] - b[Y]);
		}

		Vector2d operator*(const Vector2d& b) const
		{
			return Vector2d(vect[X] * b[X], vect[Y] * b[Y]);
		}

		Vector2d operator/(const Vector2d& b) const
		{
			return Vector2d(vect[X] / b[X], vect[Y] / b[Y]);
		}

		Vector2d& operator+=(const Vector2d& b)
		{
			vect[X] += b[X];
			vect[Y] += b[Y];
			return *this;
		}

		Vector2d& operator-=(const Vector2d& b)
		{
			vect[X] -= b[X];
			vect[Y] -= b[Y];
			return *this;
		}

		Vector2d& operator*=(const Vector2d& b)
		{
			vect[X] *= b[X];
			vect[Y] *= b[Y];
			return *this;
		}

		Vector2d& operator/=(const Vector2d& b)
		{
			vect[X] /= b[X];
			vect[Y] /= b[Y];
			return *this;
		}

		Vector2d operator-() const
		{
			return Vector2d(-vect[X], -vect[Y]);
		}

		Vector2d operator+(DBL b) const
		{
			return Vector2d(vect[X] + b, vect[Y] + b);
		}

		Vector2d operator-(DBL b) const
		{
			return Vector2d(vect[X] - b, vect[Y] - b);
		}

		Vector2d operator*(DBL b) const
		{
			return Vector2d(vect[X] * b, vect[Y] * b);
		}

		Vector2d operator/(DBL b) const
		{
			return Vector2d(vect[X] / b, vect[Y] / b);
		}

		Vector2d& operator+=(DBL b)
		{
			vect[X] += b;
			vect[Y] += b;
			return *this;
		}

		Vector2d& operator-=(DBL b)
		{
			vect[X] -= b;
			vect[Y] -= b;
			return *this;
		}

		Vector2d& operator*=(DBL b)
		{
			vect[X] *= b;
			vect[Y] *= b;
			return *this;
		}

		Vector2d& operator/=(DBL b)
		{
			vect[X] /= b;
			vect[Y] /= b;
			return *this;
		}

		const UV_VECT& operator*() const { return vect; }
		UV_VECT& operator*() { return vect; }

		DBL x() const { return vect[X]; }
		DBL& x() { return vect[X]; }

		DBL y() const { return vect[Y]; }
		DBL& y() { return vect[Y]; }

		DBL u() const { return vect[X]; }
		DBL& u() { return vect[X]; }

		DBL v() const { return vect[Y]; }
		DBL& v() { return vect[Y]; }

		DBL length() const
		{
			return sqrt(vect[X] * vect[X] + vect[Y] * vect[Y]);
		}
		DBL lengthSqr() const
		{
			return vect[X] * vect[X] + vect[Y] * vect[Y];
		}
		Vector2d normalized() const
		{
			DBL l = length();
			if (l != 0)
				return *this / l;
			else
				return *this;
		}
		void normalize()
		{
			DBL l = length();
			if (l != 0)
				*this /= l;
			// no else
		}

	private:
		DBL vect[2];
};

#if 0
#pragma mark * Vector3d
#endif

class Vector3d
{
	public:

		Vector3d()
		{
			vect[X] = 0.0;
			vect[Y] = 0.0;
			vect[Z] = 0.0;
		}

		explicit Vector3d(DBL d)
		{
			vect[X] = d;
			vect[Y] = d;
			vect[Z] = d;
		}

		Vector3d(DBL x, DBL y, DBL z)
		{
			vect[X] = x;
			vect[Y] = y;
			vect[Z] = z;
		}

		explicit Vector3d(const VECTOR vi)
		{
			vect[X] = vi[X];
			vect[Y] = vi[Y];
			vect[Z] = vi[Z];
		}

		explicit Vector3d(const SNGL_VECT vi)
		{
			vect[X] = DBL(vi[X]);
			vect[Y] = DBL(vi[Y]);
			vect[Z] = DBL(vi[Z]);
		}

		Vector3d(const Vector3d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
			vect[Z] = b[Z];
		}

		Vector3d& operator=(const Vector3d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
			vect[Z] = b[Z];
			return *this;
		}

		DBL operator[](int idx) const { return vect[idx]; }
		DBL& operator[](int idx) { return vect[idx]; }

		Vector3d operator+(const Vector3d& b) const
		{
			return Vector3d(vect[X] + b[X], vect[Y] + b[Y], vect[Z] + b[Z]);
		}

		Vector3d operator-(const Vector3d& b) const
		{
			return Vector3d(vect[X] - b[X], vect[Y] - b[Y], vect[Z] - b[Z]);
		}

		Vector3d operator*(const Vector3d& b) const
		{
			return Vector3d(vect[X] * b[X], vect[Y] * b[Y], vect[Z] * b[Z]);
		}

		Vector3d operator/(const Vector3d& b) const
		{
			return Vector3d(vect[X] / b[X], vect[Y] / b[Y], vect[Z] / b[Z]);
		}

		Vector3d& operator+=(const Vector3d& b)
		{
			vect[X] += b[X];
			vect[Y] += b[Y];
			vect[Z] += b[Z];
			return *this;
		}

		Vector3d& operator-=(const Vector3d& b)
		{
			vect[X] -= b[X];
			vect[Y] -= b[Y];
			vect[Z] -= b[Z];
			return *this;
		}

		Vector3d& operator*=(const Vector3d& b)
		{
			vect[X] *= b[X];
			vect[Y] *= b[Y];
			vect[Z] *= b[Z];
			return *this;
		}

		Vector3d& operator/=(const Vector3d& b)
		{
			vect[X] /= b[X];
			vect[Y] /= b[Y];
			vect[Z] /= b[Z];
			return *this;
		}

		Vector3d operator-() const
		{
			return Vector3d(-vect[X], -vect[Y], -vect[Z]);
		}

		Vector3d operator+(DBL b) const
		{
			return Vector3d(vect[X] + b, vect[Y] + b, vect[Z] + b);
		}

		Vector3d operator-(DBL b) const
		{
			return Vector3d(vect[X] - b, vect[Y] - b, vect[Z] - b);
		}

		Vector3d operator*(DBL b) const
		{
			return Vector3d(vect[X] * b, vect[Y] * b, vect[Z] * b);
		}

		Vector3d operator/(DBL b) const
		{
			return Vector3d(vect[X] / b, vect[Y] / b, vect[Z] / b);
		}

		Vector3d& operator+=(DBL b)
		{
			vect[X] += b;
			vect[Y] += b;
			vect[Z] += b;
			return *this;
		}

		Vector3d& operator-=(DBL b)
		{
			vect[X] -= b;
			vect[Y] -= b;
			vect[Z] -= b;
			return *this;
		}

		Vector3d& operator*=(DBL b)
		{
			vect[X] *= b;
			vect[Y] *= b;
			vect[Z] *= b;
			return *this;
		}

		Vector3d& operator/=(DBL b)
		{
			vect[X] /= b;
			vect[Y] /= b;
			vect[Z] /= b;
			return *this;
		}

		const VECTOR& operator*() const { return vect; }
		VECTOR& operator*() { return vect; }

		DBL x() const { return vect[X]; }
		DBL& x() { return vect[X]; }

		DBL y() const { return vect[Y]; }
		DBL& y() { return vect[Y]; }

		DBL z() const { return vect[Z]; }
		DBL& z() { return vect[Z]; }

		DBL length() const
		{
			return sqrt(vect[X] * vect[X] + vect[Y] * vect[Y] + vect[Z] * vect[Z]);
		}
		DBL lengthSqr() const
		{
			return vect[X] * vect[X] + vect[Y] * vect[Y] + vect[Z] * vect[Z];
		}
		Vector3d normalized() const
		{
			DBL l = length();
			if (l != 0)
				return *this / l;
			else
				return *this;
		}
		void normalize()
		{
			DBL l = length();
			if (l != 0)
				*this /= l;
			// no else
		}

	private:
		DBL vect[3];
};

inline DBL dot(const Vector2d& a, const Vector2d& b)
{
	return (a.x() * b.x()) + (a.y() * b.y());
}

inline DBL dot(const Vector3d& a, const Vector3d& b)
{
	return ((a.x() * b.x()) + (a.y() * b.y()) + (a.z() * b.z()));
}

inline Vector3d cross(const Vector3d& a, const Vector3d& b)
{
	return Vector3d( ((a.y() * b.z()) - (a.z() * b.y())),
	                 ((a.z() * b.x()) - (a.x() * b.z())),
	                 ((a.x() * b.y()) - (a.y() * b.x())) );
}

inline bool similar(const Vector3d& a, const Vector3d& b)
{
	return ( fabs(a.x()-b.x()) + fabs(a.y()-b.y()) + fabs(a.z()-b.z()) < EPSILON );
}

inline Vector3d operator* (double a, const Vector3d& b) { return b * a; }

typedef pov_base::Colour Colour;

/*****************************************************************************
 *
 * Bounding box stuff (see also BOUND.H).
 *
 *****************************************************************************/

#if 0
#pragma mark * Bounding
#endif

typedef SNGL BBOX_VAL;

typedef BBOX_VAL BBOX_VECT[3];

typedef struct Bounding_Box_Struct BBOX;

struct Bounding_Box_Struct
{
	union
	{
		BBOX_VECT Lower_Left;
		BBOX_VECT pmin;
		BBOX_VECT lowerleft;
	};
	union
	{
		BBOX_VECT Lengths;
		BBOX_VECT pmax;
		BBOX_VECT length;
	};

	SNGL GetMinX() const { return Lower_Left[X]; }
	SNGL GetMinY() const { return Lower_Left[Y]; }
	SNGL GetMinZ() const { return Lower_Left[Z]; }

	SNGL GetMaxX() const { return Lower_Left[X] + Lengths[X]; }
	SNGL GetMaxY() const { return Lower_Left[Y] + Lengths[Y]; }
	SNGL GetMaxZ() const { return Lower_Left[Z] + Lengths[Z]; }
};

inline void Assign_BBox_Vect(BBOX_VECT& d, const BBOX_VECT s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Assign_BBox_Vect(BBOX_VECT& d, const VECTOR s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Assign_BBox_Vect(VECTOR& d, const BBOX_VECT s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Make_BBox(BBOX& BBox, const BBOX_VAL llx, const BBOX_VAL lly, const BBOX_VAL llz, const BBOX_VAL lex, const BBOX_VAL ley, const BBOX_VAL lez)
{
	BBox.Lower_Left[X] = (BBOX_VAL)(llx);
	BBox.Lower_Left[Y] = (BBOX_VAL)(lly);
	BBox.Lower_Left[Z] = (BBOX_VAL)(llz);
	BBox.Lengths[X] = (BBOX_VAL)(lex);
	BBox.Lengths[Y] = (BBOX_VAL)(ley);
	BBox.Lengths[Z] = (BBOX_VAL)(lez);
}

inline void Make_BBox_from_min_max(BBOX& BBox, const BBOX_VECT mins, const BBOX_VECT maxs)
{
	BBox.Lower_Left[X] = (BBOX_VAL)(mins[X]);
	BBox.Lower_Left[Y] = (BBOX_VAL)(mins[Y]);
	BBox.Lower_Left[Z] = (BBOX_VAL)(mins[Z]);
	BBox.Lengths[X] = (BBOX_VAL)(maxs[X]-mins[X]);
	BBox.Lengths[Y] = (BBOX_VAL)(maxs[Y]-mins[Y]);
	BBox.Lengths[Z] = (BBOX_VAL)(maxs[Z]-mins[Z]);
}

inline void Make_BBox_from_min_max(BBOX& BBox, const VECTOR mins, const VECTOR maxs)
{
	BBox.Lower_Left[X] = (BBOX_VAL)(mins[X]);
	BBox.Lower_Left[Y] = (BBOX_VAL)(mins[Y]);
	BBox.Lower_Left[Z] = (BBOX_VAL)(mins[Z]);
	BBox.Lengths[X] = (BBOX_VAL)(maxs[X]-mins[X]);
	BBox.Lengths[Y] = (BBOX_VAL)(maxs[Y]-mins[Y]);
	BBox.Lengths[Z] = (BBOX_VAL)(maxs[Z]-mins[Z]);
}

inline void Make_min_max_from_BBox(BBOX_VECT& mins, BBOX_VECT& maxs, const BBOX& BBox)
{
	mins[X] = BBox.Lower_Left[X];
	mins[Y] = BBox.Lower_Left[Y];
	mins[Z] = BBox.Lower_Left[Z];
	maxs[X] = mins[X] + BBox.Lengths[X];
	maxs[Y] = mins[Y] + BBox.Lengths[Y];
	maxs[Z] = mins[Z] + BBox.Lengths[Z];
}

inline void Make_min_max_from_BBox(VECTOR& mins, VECTOR& maxs, const BBOX& BBox)
{
	mins[X] = BBox.Lower_Left[X];
	mins[Y] = BBox.Lower_Left[Y];
	mins[Z] = BBox.Lower_Left[Z];
	maxs[X] = mins[X] + BBox.Lengths[X];
	maxs[Y] = mins[Y] + BBox.Lengths[Y];
	maxs[Z] = mins[Z] + BBox.Lengths[Z];
}

inline bool Inside_BBox(const VECTOR point, const BBOX& bbox)
{
	if (point[X] < (DBL)bbox.Lower_Left[X])
		return(false);
	if (point[Y] < (DBL)bbox.Lower_Left[Y])
		return(false);
	if (point[Z] < (DBL)bbox.Lower_Left[Z])
		return(false);
	if (point[X] > (DBL)bbox.Lower_Left[X] + (DBL)bbox.Lengths[X])
		return(false);
	if (point[Y] > (DBL)bbox.Lower_Left[Y] + (DBL)bbox.Lengths[Y])
		return(false);
	if (point[Z] > (DBL)bbox.Lower_Left[Z] + (DBL)bbox.Lengths[Z])
		return(false);

	return(true);
}

/*****************************************************************************
 *
 * Transformation stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Transform
#endif

typedef struct Transform_Struct TRANSFORM;

struct Transform_Struct
{
	MATRIX matrix;
	MATRIX inverse;
};



/*****************************************************************************
 *
 * Color map stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Blend Map
#endif

const int MAX_BLEND_MAP_ENTRIES = 256;

typedef struct Blend_Map_Entry BLEND_MAP_ENTRY;
typedef struct Blend_Map_Struct BLEND_MAP;
typedef struct Pattern_Struct TPATTERN;
typedef struct Texture_Struct TEXTURE;
typedef struct Pigment_Struct PIGMENT;
typedef struct Tnormal_Struct TNORMAL;
typedef struct Finish_Struct FINISH;
typedef struct Turb_Struct TURB;
typedef struct Warps_Struct WARP;
typedef struct Spline_Entry SPLINE_ENTRY;
typedef struct Spline_Struct SPLINE;

struct Blend_Map_Entry
{
	SNGL value;
	unsigned char Same;
	union
	{
		COLOUR colour;
		PIGMENT *Pigment;
		TNORMAL *Tnormal;
		TEXTURE *Texture;
		UV_VECT Point_Slope;
	} Vals;
};

struct Blend_Map_Struct
{
	int Users;
	short Number_Of_Entries;
	char Transparency_Flag, Type;
	BLEND_MAP_ENTRY *Blend_Map_Entries;
};

inline void Make_Blend_Map_Entry(BLEND_MAP_ENTRY& entry, SNGL v, unsigned char s, COLC r, COLC g, COLC b, COLC a, COLC t)
{
	entry.value = v;
	entry.Same = s;
	Make_ColourA(entry.Vals.colour, r, g, b, a, t);
}


/*****************************************************************************
 *
 * Media and Interior stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Media, Interior
#endif

class Media
{
	public:
		int Type;
		int Intervals;
		int Min_Samples;
		int Max_Samples;
		unsigned Sample_Method : 8;
		bool is_constant : 1;
		bool use_absorption : 1;
		bool use_emission : 1;
		bool use_extinction : 1;
		bool use_scattering : 1;
		bool ignore_photons : 1;
		DBL Jitter;
		DBL Eccentricity;
		DBL sc_ext;
		RGBColour Absorption;
		RGBColour Emission;
		RGBColour Extinction;
		RGBColour Scattering;

		DBL Ratio;
		DBL Confidence;
		DBL Variance;
		DBL *Sample_Threshold;

		DBL AA_Threshold;
		int AA_Level;

		PIGMENT *Density;

		Media();
		Media(const Media&);
		~Media();

		Media& operator=(const Media&);

		void Transform(const TRANSFORM *trans);

		void PostProcess();
};

class SubsurfaceInterior;
class Interior
{
	public:
		int References;
		int  hollow, Disp_NElems;
		SNGL IOR, Dispersion;
		SNGL Caustics, Old_Refract;
		SNGL Fade_Distance, Fade_Power;
		RGBColour Fade_Colour;
		vector<Media> media;
		shared_ptr<SubsurfaceInterior> subsurface;

		Interior();
		Interior(const Interior&);
		~Interior();

		void Transform(const TRANSFORM *trans);

		void PostProcess();
	private:
		Interior& operator=(const Interior&);
};



/*****************************************************************************
 *
 * Spline stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Spline
#endif

struct Spline_Entry
{
	DBL par;      // Parameter
	DBL vec[5];   // Value at the parameter
	DBL coeff[5]; // Interpolating coefficients at the parameter
};

struct Spline_Struct
{
	int Number_Of_Entries, Type;
	int Max_Entries;
	SPLINE_ENTRY *SplineEntries;
	int Coeffs_Computed;
	int Terms;
/* [JG] flyspray #294 : cache is not thread-safe
 * (and seems useless, as slower than without it)
	bool Cache_Valid;
	int Cache_Type;
	DBL Cache_Point;
	EXPRESS Cache_Data;
 */
	int ref_count;
};

typedef struct Spline_Entry SPLINE_ENTRY;


/*****************************************************************************
 *
 * Image stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Image
#endif

// Legal image attributes.

#define NO_FILE         0x00000000
#define GIF_FILE        0x00000001
#define POT_FILE        0x00000002
#define SYS_FILE        0x00000004
#define IFF_FILE        0x00000008
#define TGA_FILE        0x00000010
#define GRAD_FILE       0x00000020
#define PGM_FILE        0x00000040
#define PPM_FILE        0x00000080
#define PNG_FILE        0x00000100
#define JPEG_FILE       0x00000200
#define TIFF_FILE       0x00000400
#define BMP_FILE        0x00000800
#define EXR_FILE        0x00001000
#define HDR_FILE        0x00002000

// Image types.

#define IMAGE_FILE    GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define NORMAL_FILE   GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define MATERIAL_FILE GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define HF_FILE       GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+POT_FILE

#define PIGMENT_TYPE  0
#define NORMAL_TYPE   1
#define PATTERN_TYPE  2
#define TEXTURE_TYPE  4
#define COLOUR_TYPE   5
#define SLOPE_TYPE    6
#define DENSITY_TYPE  7

#define DEFAULT_FRACTAL_EXTERIOR_TYPE 1
#define DEFAULT_FRACTAL_INTERIOR_TYPE 0
#define DEFAULT_FRACTAL_EXTERIOR_FACTOR 1
#define DEFAULT_FRACTAL_INTERIOR_FACTOR 1


/*****************************************************************************
 *
 * Pigment, Tnormal, Finish, Texture & Warps stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Pigment, Normal, Finish, Texture, Warp
#endif

typedef struct Density_file_Struct DENSITY_FILE;
typedef struct Density_file_Data_Struct DENSITY_FILE_DATA;

struct Density_file_Struct
{
	int Interpolation;
	DENSITY_FILE_DATA *Data;
};

struct Density_file_Data_Struct
{
	int References;
	char *Name;
	size_t Sx, Sy, Sz;
	int Type;
	union
	{
		unsigned char *Density8;
		unsigned short *Density16;
		unsigned int *Density32;
	};
};

class ImageData;
class FunctionVM;

struct Crackle_Cache_Struct
{
	VECTOR data [125];
	bool valid [125];
	int lastSeed;
};

struct Pattern_Struct
{
	unsigned short Type, Wave_Type, Flags;
	int References;
	SNGL Frequency, Phase;
	SNGL Exponent;
	WARP *Warps;
	TPATTERN *Next;
	BLEND_MAP *Blend_Map;
	union {
		DENSITY_FILE *Density_File;
		ImageData *image;
		VECTOR Gradient;
		SNGL Agate_Turb_Scale;
		short Num_of_Waves;
		short Iterations;
		short Arms;
		struct { SNGL Mortar; VECTOR Size; } Brick;
		struct { SNGL Control0, Control1; } Quilted;
		struct { DBL Size, UseCoords, Metric; } Facets;
		struct { VECTOR Form; DBL Metric; DBL Offset; DBL Dim;
		         short IsSolid; } Crackle;
		struct { VECTOR Slope_Vector, Altit_Vector;
		         short Slope_Base, Altit_Base; DBL Slope_Len,
		         Altit_Len; UV_VECT Slope_Mod, Altit_Mod;
		         bool Point_At; } Slope;
		struct { UV_VECT Coord; DBL efactor, ifactor;
		         unsigned int Iterations; int Exponent;
		         unsigned char interior_type, exterior_type; } Fractal;
		struct { void *Fn; unsigned int Data; FunctionVM *vm; } Function;
		struct { unsigned char Side,Tile,Number,Exterior,Interior,Form;} Pavement;
		struct { unsigned char Pattern; } Tiling;
		PIGMENT *Pigment;
		ObjectBase *Object;
	} Vals;
};

struct Pigment_Struct : public Pattern_Struct
{
	Colour colour;       // may have a filter/transmit component
	Colour Quick_Colour; // may have a filter/transmit component
};

struct Tnormal_Struct : public Pattern_Struct
{
	SNGL Amount;
	SNGL Delta; // NK delta
};

struct Texture_Struct : public Pattern_Struct
{
	TEXTURE *Next_Material;
	PIGMENT *Pigment;
	TNORMAL *Tnormal;
	FINISH *Finish;
	TEXTURE *Materials;
	int Num_Of_Mats;

};

struct Finish_Struct
{
	SNGL Diffuse, DiffuseBack, RawDiffuse, RawDiffuseBack, Brilliance;
	SNGL Specular, Roughness;
	SNGL Phong, Phong_Size;
	SNGL Irid, Irid_Film_Thickness, Irid_Turb;
	SNGL Temp_Caustics, Temp_IOR, Temp_Dispersion, Temp_Refract, Reflect_Exp;
	SNGL Crand, Metallic;
	RGBColour Ambient, Emission, Reflection_Max, Reflection_Min;
	RGBColour SubsurfaceTranslucency, SubsurfaceAnisotropy;
	//RGBColour SigmaPrimeS, SigmaA;
	SNGL Reflection_Falloff;  // Added by MBP 8/27/98
	int Reflection_Type;  // Added by MBP 9/5/98
	SNGL Reflect_Metallic; // MBP
	int Conserve_Energy;  // added by NK Dec 19 1999
	bool UseSubsurface;   // whether to use subsurface light transport
};

struct Warps_Struct
{
	unsigned short Warp_Type;
	WARP *Prev_Warp;
	WARP *Next_Warp;
};

struct Turb_Struct : public Warps_Struct
{
	VECTOR Turbulence;
	int Octaves;
	SNGL Lambda, Omega;
};

#define Destroy_Finish(x) if ((x)!=NULL) POV_FREE(x)

typedef struct Material_Struct MATERIAL;

struct Material_Struct
{
   TEXTURE *Texture;
   TEXTURE * Interior_Texture;
   Interior *interior;
};

/*****************************************************************************
 *
 * Object stuff (see also OBJECTS.H and primitive include files).
 *
 *****************************************************************************/

#if 0
#pragma mark * Object
#endif

#ifndef DUMP_OBJECT_DATA
#define DUMP_OBJECT_DATA 0
#endif

// TODO FIXME
class SceneThreadData;
#define TraceThreadData SceneThreadData

// These fields are common to all objects.
class LightSource;
class Ray;
class Intersection;

template<typename T>
class RefPool
{
	public:
		RefPool() { }
		~RefPool() { for(typename vector<T*>::iterator i(pool.begin()); i != pool.end(); i++) delete *i; pool.clear(); }

		T *alloc() { if(pool.empty()) return new T(); T *ptr(pool.back()); pool.pop_back(); return ptr; }
		void release(T *ptr) { pool.push_back(ptr); }
	private:
		vector<T*> pool;

		RefPool(const RefPool&);
		RefPool& operator=(RefPool&);
};

template<typename T>
struct RefClearDefault
{
	void operator()(T&) { }
};

template<typename T>
struct RefClearContainer
{
	void operator()(T& p) { p.clear(); }
};

template<typename T, class C = RefClearDefault<T> >
class Ref
{
	public:
		Ref(RefPool<T>& p) : pool(p), ptr(p.alloc()) { }
		~Ref() { C c; c(*ptr); pool.release(ptr); }

		T& operator*() { return *ptr; }
		const T& operator*() const { return *ptr; }

		T *operator->() { return ptr; }
		const T *operator->() const { return ptr; }
	private:
		RefPool<T>& pool;
		T *ptr;

		Ref();
		Ref(const Ref&);
		Ref& operator=(Ref&);
};

typedef stack<Intersection, vector<Intersection> > IStackData;
typedef RefPool<IStackData> IStackPool;
typedef Ref<IStackData> IStack;

struct WeightedTexture
{
	COLC weight;
	TEXTURE *texture;

	WeightedTexture(COLC w, TEXTURE *t) :
		weight(w), texture(t) { }
};

// TODO: these sizes will need tweaking.
typedef FixedSimpleVector<WeightedTexture, WEIGHTEDTEXTURE_VECTOR_SIZE> WeightedTextureVector;
typedef FixedSimpleVector<Interior *, RAYINTERIOR_VECTOR_SIZE> RayInteriorVector;

class ObjectDebugHelper
{
	public:
		int Index;
		bool IsCopy;
		std::string Tag;

		ObjectDebugHelper() { Index = ObjectIndex++; IsCopy = false; }
		ObjectDebugHelper& operator=(const ObjectDebugHelper& src) { Index = ObjectIndex++; IsCopy = true; Tag = src.Tag; return *this; }

		std::string& SimpleDesc (std::string& result);
	private:
		static int ObjectIndex;
		ObjectDebugHelper(const ObjectDebugHelper& src) { Index = ObjectIndex++; IsCopy = true; Tag = src.Tag; }
};

class ObjectBase
{
	public:
		int Type;
		TEXTURE *Texture;
		TEXTURE *Interior_Texture;
		Interior *interior;
		vector<ObjectPtr> Bound;
		vector<ObjectPtr> Clip;
		vector<LightSource *> LLights;
		BBOX BBox;
		TRANSFORM *Trans;
		TRANSFORM *UV_Trans;
		SNGL Ph_Density;
		FloatSetting RadiosityImportance;
		unsigned int Flags;

#ifdef OBJECT_DEBUG_HELPER
		ObjectDebugHelper Debug;
#endif
		enum BBoxDirection
		{
			BBOX_DIR_X0Y0Z0 = 0,
			BBOX_DIR_X0Y0Z1 = 1,
			BBOX_DIR_X0Y1Z0 = 2,
			BBOX_DIR_X0Y1Z1 = 3,
			BBOX_DIR_X1Y0Z0 = 4,
			BBOX_DIR_X1Y0Z1 = 5,
			BBOX_DIR_X1Y1Z0 = 6,
			BBOX_DIR_X1Y1Z1 = 7
		};

		ObjectBase(int t) :
			Type(t),
			Texture(NULL), Interior_Texture(NULL), interior(NULL), Trans(NULL), UV_Trans(NULL),
			Ph_Density(0), RadiosityImportance(0.0), Flags(0)
		{
			Make_BBox(BBox, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
			// +!+ slair
			IsBaseObject=true;
			// -!- slair
		}

		ObjectBase(int t, ObjectBase& o, bool transplant) :
			Type(t),
			Texture(o.Texture), Interior_Texture(o.Interior_Texture), interior(o.interior), Trans(o.Trans), UV_Trans(o.UV_Trans),
			Ph_Density(o.Ph_Density), RadiosityImportance(o.RadiosityImportance), Flags(o.Flags),
			Bound(o.Bound), Clip(o.Clip), LLights(o.LLights), BBox(o.BBox)
		{
			if (transplant)
			{
				o.Texture = NULL;
				o.Interior_Texture = NULL;
				o.interior = NULL;
				o.Trans = NULL;
				o.UV_Trans = NULL;
				o.Bound.clear();
				o.Clip.clear();
				o.LLights.clear();
			}
			// +!+ slair
			IsBaseObject=true;
			// -!- slair
		}
		virtual ~ObjectBase() { }

		virtual ObjectPtr Copy() = 0;

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) = 0; // could be "const", if it wasn't for isosurface max_gradient estimation stuff
		virtual bool Inside(const VECTOR, TraceThreadData *) const = 0;
		virtual void Normal(VECTOR, Intersection *, TraceThreadData *) const = 0;
		virtual void UVCoord(UV_VECT, const Intersection *, TraceThreadData *) const;
		virtual void Translate(const VECTOR, const TRANSFORM *) = 0;
		virtual void Rotate(const VECTOR, const TRANSFORM *) = 0;
		virtual void Scale(const VECTOR, const TRANSFORM *) = 0;
		virtual void Transform(const TRANSFORM *) = 0;
		virtual void Invert() = 0;
		virtual void Compute_BBox() = 0;
		virtual void Determine_Textures(Intersection *, bool, WeightedTextureVector&, TraceThreadData *Thread); // could be "(const Intersection*...) const" if it wasn't for blob specials
		virtual bool Intersect_BBox(BBoxDirection, const BBOX_VECT&, const BBOX_VECT&, BBOX_VAL = HUGE_VAL) const;

		// optional post-render message dispatcher; will be called upon completion
		// of rendering a view. this is the appropriate place to send messages that
		// would traditionally have been sent at the end of a render or at object
		// destruction - e.g. IsoSurface max_gradient warnings. (object destruction
		// isn't the place to do that anymore since a scene may persist between views).
		virtual void DispatchShutdownMessages(MessageFactory& messageFactory) {};

		// +!+ slair
		bool IsBaseObject;
		virtual void Dump(FILE* df, TraceThreadData *Thread)
		{
			if (df)
			{
				if (IsBaseObject)
				{
					fwrite("BASE",4,1,df);
				}
				if (Trans)
				{
					fwrite("TRNS",4,1,df);
					fwrite(Trans->matrix, sizeof(Trans->matrix), 1, df);
				}
				else
				{
					fwrite("\x00\x00\x00\x00",4,1,df);
				}
				fwrite("BBOX",4,1,df);
				fwrite(BBox.Lower_Left, sizeof(BBox.Lower_Left), 1, df);
				BBOX_VECT a;
				a[0] = BBox.Lower_Left[0] + BBox.Lengths[0];
				a[1] = BBox.Lower_Left[1] + BBox.Lengths[1];
				a[2] = BBox.Lower_Left[2] + BBox.Lengths[2];
				fwrite(a, sizeof(a), 1, df);
				if (Texture)
				{
					fwrite("COLR", 4, 1, df);
					fwrite(&(Texture->Pigment->colour), sizeof(Texture->Pigment->colour), 1, df);
				}
				else
				{
					fwrite("\x00\x00\x00\x00",4,1,df);
				}
			}
		};
		// -!- slair
	protected:
		explicit ObjectBase(const ObjectBase&) { }
};

/* This is an abstract structure that is never actually used.
   All other objects are descendents of this primitive type */

class CompoundObject : public ObjectBase
{
	public:
		CompoundObject(int t) : ObjectBase(t) {}
		CompoundObject(int t, CompoundObject& o, bool transplant) : ObjectBase(t, o, transplant), children(o.children) { if (transplant) o.children.clear(); }
		vector<ObjectPtr> children;
};

typedef struct BBox_Tree_Struct BBOX_TREE;

struct BBox_Tree_Struct
{
	short Infinite;   // Flag if node is infinite
	short Entries;    // Number of sub-nodes in this node
	BBOX BBox;        // Bounding box of this node
	BBOX_TREE **Node; // If node: children; if leaf: element
};

typedef struct Project_Struct PROJECT;
typedef struct Project_Tree_Node_Struct PROJECT_TREE_NODE;

struct Project_Struct
{
	int x1, y1, x2, y2;
};

/*
 * The following structure represent the bounding box hierarchy in 2d space.
 * Because is_leaf, Object and Project are the first elements in both
 * structures they can be accessed without knowing at which structure
 * a pointer is pointing.
 */

struct Project_Tree_Node_Struct
{
	unsigned short is_leaf;
	BBOX_TREE *Node;
	PROJECT Project;
	unsigned short Entries;
	PROJECT_TREE_NODE **Entry;
};

#if 0
#pragma mark * Light Source
#endif

class LightSource : public CompoundObject
{
	public:
		size_t index;
		RGBColour colour;
		VECTOR Direction, Center, Points_At, Axis1, Axis2;
		DBL Coeff, Radius, Falloff;
		DBL Fade_Distance, Fade_Power;
		int Area_Size1, Area_Size2;
		int Adaptive_Level;
//		ObjectBase *Shadow_Cached_Object;
		ObjectBase *Projected_Through_Object;
		BLEND_MAP *blend_map;// NK for dispersion
//		PROJECT_TREE_NODE *Light_Buffer[6]; // Light buffers for the six general directions in space. [DB 9/94]

		unsigned Light_Type : 8;
		bool Area_Light : 1;
		bool Use_Full_Area_Lighting : 1; // JN2007: Full area lighting
		bool Jitter : 1;
		bool Orient : 1;
		bool Circular : 1;
//		bool Track : 1;
		bool Parallel : 1;
		bool Photon_Area_Light : 1;
		bool Media_Attenuation : 1;
		bool Media_Interaction : 1;
		bool lightGroupLight : 1;

		LightSource();
		virtual ~LightSource();

		virtual ObjectPtr Copy();

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
		virtual bool Inside(const VECTOR, TraceThreadData *) const;
		virtual void Normal(VECTOR, Intersection *, TraceThreadData *) const;
		virtual void UVCoord(UV_VECT, const Intersection *, TraceThreadData *) const;
		virtual void Translate(const VECTOR, const TRANSFORM *);
		virtual void Rotate(const VECTOR, const TRANSFORM *);
		virtual void Scale(const VECTOR, const TRANSFORM *);
		virtual void Transform(const TRANSFORM *);
		virtual void Invert();
		virtual void Compute_BBox() { }
		// +!+ slair
		virtual void Dump(FILE* df, TraceThreadData* Thread);
		// -!- slair
};

typedef unsigned short HF_VAL;


/*****************************************************************************
 *
 * Intersection stack stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Intersection
#endif

/**
 *  Ray-object intersection data.
 *  This class holds various information on a ray-object intersection.
 */
class Intersection
{
	public:
		/// Distance from the intersecting ray's origin.
		DBL Depth;
		/// Point of the intersection in global coordinate space.
		VECTOR IPoint;
		/// Unpertubed surface normal at the intersection point.
		/// @note This is not necessarily the true geometric surface normal, as it may include fake smoothing.
		/// @note This value is invalid if haveNormal is false.
		/// @todo We should have two distinct vectors: A true geometric one, and a separate one for faked smoothing.
		VECTOR INormal;
		/// Perturbed normal vector (set during texture evaluation).
		VECTOR PNormal;
		/// UV texture coordinate.
		UV_VECT Iuv;
		/// Intersected object.
		ObjectBase *Object;

		/// @name Object-Specific Auxiliary Data
		/// These members hold information specific to particular object types, typically generated during
		/// intersection testing, to be re-used later for normal and/or UV coordinate computation.
		/// @note These values may be invalid or meaningless depending on the type of object intersected.
		//@{
		/// Point of the intersection in local coordinate space (used by Blob)
		/// @note This value is invalid if haveLocalIPoint is false.
		VECTOR LocalIPoint;
		/// Flag to indicate whether INormal was computed during intersection testing (used by HField)
		/// @note Objects either always or never computing INormal during intersection testing don't use this flag.
		bool haveNormal;
		/// Flag to indicate whether LocalIPoint has already been computed.
		bool haveLocalIPoint;
		/// Generic auxiliary integer data #1 (used by Sor, Prism, Lathe, Cones, Boxes)
		int i1;
		/// Generic auxiliary integer data #2 (used by Sor, Prism)
		int i2;
		/// Generic auxiliary float data #1 (used by Prism, Lathe)
		DBL d1;
		/// Generic auxiliary pointer data (used by Mesh)
		const void *Pointer;
		//@}

		/// Root-level parent CSG object for cutaway textures.
		ObjectBase *Csg;

		Intersection() { Depth = BOUND_HUGE; Object = NULL; Csg = NULL; }

		Intersection(DBL d, const VECTOR& v, ObjectBase *o)
		{
			Depth  = d;
			Object = o;
			Assign_Vector(IPoint, v);
			Assign_UV_Vect(Iuv, v);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, const VECTOR& n, ObjectBase *o)
		{
			Depth  = d;
			Object = o;
			Assign_Vector(IPoint, v);
			Assign_UV_Vect(Iuv, v);
			Assign_Vector(INormal, n);
			haveNormal = true;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, const UV_VECT& uv, ObjectBase *o)
		{
			Depth  = d;
			Object = o;
			Assign_Vector(IPoint, v);
			Assign_UV_Vect(Iuv, uv);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, const VECTOR& n, const UV_VECT& uv, ObjectBase *o)
		{
			Depth  = d;
			Object = o;
			Assign_Vector(IPoint, v);
			Assign_Vector(INormal, n);
			Assign_UV_Vect(Iuv, uv);
			haveNormal = true;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, ObjectBase *o, const void *a)
		{
			Depth  = d;
			Object = o;
			Pointer = a;
			Assign_Vector(IPoint, v);
			Assign_UV_Vect(Iuv, v);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, const UV_VECT& uv, ObjectBase *o, const void *a)
		{
			Depth  = d;
			Object = o;
			Pointer = a;
			Assign_Vector(IPoint, v);
			Assign_UV_Vect(Iuv, uv);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, ObjectBase *o, int a)
		{
			Depth  = d;
			Object = o;
			i1 = a;
			Assign_Vector(IPoint, v);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, ObjectBase *o, DBL a)
		{
			Depth  = d;
			Object = o;
			d1 = a;
			Assign_Vector(IPoint, v);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, ObjectBase *o, int a, int b)
		{
			Depth  = d;
			Object = o;
			i1 = a;
			i2 = b;
			Assign_Vector(IPoint, v);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, ObjectBase *o, int a, DBL b)
		{
			Depth  = d;
			Object = o;
			i1 = a;
			d1 = b;
			Assign_Vector(IPoint, v);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		Intersection(DBL d, const VECTOR& v, ObjectBase *o, int a, int b, DBL c)
		{
			Depth  = d;
			Object = o;
			i1 = a;
			i2 = b;
			d1 = c;
			Assign_Vector(IPoint, v);
			haveNormal = false;
			haveLocalIPoint = false;
			Csg = NULL;
		}

		~Intersection() { }
};

/*****************************************************************************
 *
 * Ray stuff (see also RAY.H).
 *
 *****************************************************************************/

#if 0
#pragma mark * Ray
#endif

class Ray
{
	public:
		enum RayType
		{
			OtherRay = 0,
			PrimaryRay = 1,
			ReflectionRay = 2,
			RefractionRay = 3,
			SubsurfaceRay = 4,  ///< Ray is shot from just below a surface; very close intersections shall not be suppressed.
		};

		VECTOR Origin;
		VECTOR Direction;

		Ray(RayType rt = PrimaryRay, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
		Ray(const VECTOR ov, const VECTOR dv, RayType rt = PrimaryRay, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
		~Ray();

		const VECTOR& GetOrigin() const { return Origin; }
		const VECTOR& GetDirection() const { return Direction; }

		void AppendInterior(Interior *i);
		void AppendInteriors(RayInteriorVector&);
		bool RemoveInterior(const Interior *i);
		void ClearInteriors() { interiors.clear(); }

		bool IsInterior(const Interior *i) const;
		const RayInteriorVector& GetInteriors() const { return interiors; }
		RayInteriorVector& GetInteriors() { return interiors; }

		void SetSpectralBand(const SpectralBand&);
		const SpectralBand& GetSpectralBand() const;

		void SetFlags(RayType rt, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
		void SetFlags(RayType rt, const Ray& other);

		bool IsPrimaryRay() const { return primaryRay; }
		bool IsImageRay() const { return primaryRay || (refractionRay && !reflectionRay); }
		bool IsReflectionRay() const { return reflectionRay; }
		bool IsRefractionRay() const { return refractionRay; }
		bool IsSubsurfaceRay() const { return subsurfaceRay; }
		bool IsShadowTestRay() const { return shadowTestRay; }
		bool IsPhotonRay() const { return photonRay; }
		bool IsRadiosityRay() const { return radiosityRay; }
		bool IsMonochromaticRay() const { return monochromaticRay; }
		bool IsHollowRay() const { return hollowRay; }
		bool IsPretraceRay() const { return pretraceRay; }

		bool Inside(const BBOX& bbox) const { return Inside_BBox(Origin, bbox); }
	private:
		RayInteriorVector interiors;
		SpectralBand spectralBand;

		bool primaryRay : 1;
		bool reflectionRay : 1;
		bool refractionRay : 1;
		bool subsurfaceRay : 1;
		bool shadowTestRay : 1;
		bool photonRay : 1;
		bool radiosityRay : 1;
		bool monochromaticRay : 1;
		bool hollowRay : 1;
		bool pretraceRay : 1;
};

struct RayObjectCondition
{
	virtual bool operator()(const Ray& ray, const ObjectBase* object, DBL data) const = 0;
};

struct TrueRayObjectCondition : public RayObjectCondition
{
	virtual bool operator()(const Ray&, const ObjectBase*, DBL) const { return true; }
};

struct PointObjectCondition
{
	virtual bool operator()(const Vector3d& point, const ObjectBase* object) const = 0;
};

struct TruePointObjectCondition : public PointObjectCondition
{
	virtual bool operator()(const Vector3d&, const ObjectBase*) const { return true; }
};


/*****************************************************************************
 *
 * Frame tracking information
 *
 *****************************************************************************/

enum FRAMETYPE
{
	FT_SINGLE_FRAME,
	FT_MULTIPLE_FRAME
};

#define INT_VALUE_UNSET (-1)
#define DBL_VALUE_UNSET (-1.0)

struct FrameSettings
{
	FRAMETYPE FrameType;
	DBL Clock_Value;      // May change between frames of an animation
	int FrameNumber;      // May change between frames of an animation

	int InitialFrame;
	DBL InitialClock;

	int FinalFrame;
	int FrameNumWidth;
	DBL FinalClock;

	int SubsetStartFrame;
	DBL SubsetStartPercent;
	int SubsetEndFrame;
	DBL SubsetEndPercent;

	bool Field_Render_Flag;
	bool Odd_Field_Flag;
};


/*****************************************************************************
 *
 * Miscellaneous stuff.
 *
 *****************************************************************************/

typedef struct complex_block complex;

struct Chunk_Header_Struct
{
	int name;
	int size;
};

struct complex_block
{
	DBL r, c;
};

struct BYTE_XYZ
{
	unsigned char x, y, z;
};

inline void VUnpack(Vector3d& dest_vec, const BYTE_XYZ * pack_vec)
{
	dest_vec[X] = ((double)pack_vec->x * (1.0 / 255.0)) * 2.0 - 1.0;
	dest_vec[Y] = ((double)pack_vec->y * (1.0 / 255.0));
	dest_vec[Z] = ((double)pack_vec->z * (1.0 / 255.0)) * 2.0 - 1.0;

	dest_vec.normalize(); // already good to about 1%, but we can do better
}

// platform-specific headers should have provided DECLARE_THREAD_LOCAL_PTR.
// if not, we default to using the boost thread_specific_ptr class.
#ifndef DECLARE_THREAD_LOCAL_PTR
#define DECLARE_THREAD_LOCAL_PTR(ptrType, ptrName)                boost::thread_specific_ptr<ptrType> ptrName
#define IMPLEMENT_THREAD_LOCAL_PTR(ptrType, ptrName, ptrCleanup)  boost::thread_specific_ptr<ptrType> ptrName(ptrCleanup)
#define GET_THREAD_LOCAL_PTR(ptrName)                             (ptrName.get())
#define SET_THREAD_LOCAL_PTR(ptrName, ptrValue)                   (ptrName.reset(ptrValue))
#endif
// +!+ slair
#define DUMP(b,a) fwrite(&(a),sizeof(a),1,(b));
#define DUMPTAG(b,a) fwrite(&(a),4,1,(b));
#define TMPCFG_NAME "/dev/shm/scene.cfg"
typedef struct {
	DBL x;
	DBL y;
	DBL z;
} XYZ;

typedef struct {
   XYZ p[3];
} TRI;

typedef struct {
   XYZ p[8];
   double val[8];
} GRIDCELL;

static void Assign_Vector(VECTOR d, XYZ s)
{
	d[X] = s.x;
	d[Y] = s.y;
	d[Z] = s.z;
}

#define ABS fabs
/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
static XYZ VertexInterp(DBL isolevel, XYZ p1, XYZ p2, DBL valp1, DBL valp2)
{
   DBL mu;
   XYZ p;

   if (ABS(isolevel-valp1) < 0.00001)
      return(p1);
   if (ABS(isolevel-valp2) < 0.00001)
      return(p2);
   if (ABS(valp1-valp2) < 0.00001)
      return(p1);
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}

/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
static int Polygonise(GRIDCELL grid,double isolevel,TRI *triangles)
{
   int i,ntriang;
   int cubeindex;
   XYZ vertlist[12];

int edgeTable[256]={
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };
int triTable[256][16] =
{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   /*
      Determine the index into the edge table which
      tells us which vertices are inside of the surface
   */
   cubeindex = 0;
   if (grid.val[0] < isolevel) cubeindex |= 1;
   if (grid.val[1] < isolevel) cubeindex |= 2;
   if (grid.val[2] < isolevel) cubeindex |= 4;
   if (grid.val[3] < isolevel) cubeindex |= 8;
   if (grid.val[4] < isolevel) cubeindex |= 16;
   if (grid.val[5] < isolevel) cubeindex |= 32;
   if (grid.val[6] < isolevel) cubeindex |= 64;
   if (grid.val[7] < isolevel) cubeindex |= 128;

   /* Cube is entirely in/out of the surface */
   if (edgeTable[cubeindex] == 0)
      return(0);

   /* Find the vertices where the surface intersects the cube */
   if (edgeTable[cubeindex] & 1)
      vertlist[0] =
         VertexInterp(isolevel,grid.p[0],grid.p[1],grid.val[0],grid.val[1]);
   if (edgeTable[cubeindex] & 2)
      vertlist[1] =
         VertexInterp(isolevel,grid.p[1],grid.p[2],grid.val[1],grid.val[2]);
   if (edgeTable[cubeindex] & 4)
      vertlist[2] =
         VertexInterp(isolevel,grid.p[2],grid.p[3],grid.val[2],grid.val[3]);
   if (edgeTable[cubeindex] & 8)
      vertlist[3] =
         VertexInterp(isolevel,grid.p[3],grid.p[0],grid.val[3],grid.val[0]);
   if (edgeTable[cubeindex] & 16)
      vertlist[4] =
         VertexInterp(isolevel,grid.p[4],grid.p[5],grid.val[4],grid.val[5]);
   if (edgeTable[cubeindex] & 32)
      vertlist[5] =
         VertexInterp(isolevel,grid.p[5],grid.p[6],grid.val[5],grid.val[6]);
   if (edgeTable[cubeindex] & 64)
      vertlist[6] =
         VertexInterp(isolevel,grid.p[6],grid.p[7],grid.val[6],grid.val[7]);
   if (edgeTable[cubeindex] & 128)
      vertlist[7] =
         VertexInterp(isolevel,grid.p[7],grid.p[4],grid.val[7],grid.val[4]);
   if (edgeTable[cubeindex] & 256)
      vertlist[8] =
         VertexInterp(isolevel,grid.p[0],grid.p[4],grid.val[0],grid.val[4]);
   if (edgeTable[cubeindex] & 512)
      vertlist[9] =
         VertexInterp(isolevel,grid.p[1],grid.p[5],grid.val[1],grid.val[5]);
   if (edgeTable[cubeindex] & 1024)
      vertlist[10] =
         VertexInterp(isolevel,grid.p[2],grid.p[6],grid.val[2],grid.val[6]);
   if (edgeTable[cubeindex] & 2048)
      vertlist[11] =
         VertexInterp(isolevel,grid.p[3],grid.p[7],grid.val[3],grid.val[7]);

   /* Create the triangle */
   ntriang = 0;
   for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
      triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i  ]];
      triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i+1]];
      triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i+2]];
      ntriang++;
   }

   return(ntriang);
}
// -!- slair
}

#endif
