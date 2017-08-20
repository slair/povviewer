/*******************************************************************************
 * isosurf.cpp
 *
 * This module implements the iso surface shapetype.
 *
 * This module was written by R.Suzuki.
 * Ported to POV-Ray 3.5 by Thorsten Froehlich.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/isosurf.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#include <limits.h>
#include <algorithm>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/scene/objects.h"
#include "backend/scene/scene.h"
#include "backend/scene/threaddata.h"
#include "backend/shape/isosurf.h"
#include "backend/shape/spheres.h" // TODO - Move sphere intersection function to math code! [trf]
#include "backend/shape/boxes.h" // TODO - Move box intersection function to math code! [trf]
#include "backend/vm/fnpovfpu.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define close(x, y) (fabs(x-y) < EPSILON ? 1 : 0)

/* Side hit. */
const int SIDE_X_0 = 1;
const int SIDE_X_1 = 2;
const int SIDE_Y_0 = 3;
const int SIDE_Y_1 = 4;
const int SIDE_Z_0 = 5;
const int SIDE_Z_1 = 6;

#define OK_X         1
#define OK_Y         2
#define OK_Z         4
#define OK_R         8
#define OK_S        16
#define OK_T        32
#define OK_U        64
#define OK_V       128


/*****************************************************************************
*
* FUNCTION
*
*   All_IsoSurface_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
	int Side1 = 0, Side2 = 0, itrace = 0, i_flg = 0;
	DBL Depth1 = 0.0, Depth2 = 0.0, len = 0.0;
	Ray New_Ray;
	VECTOR IPoint;
	VECTOR Plocal, Dlocal;
	DBL tmax = 0.0, tmin = 0.0, tmp = 0.0;
	DBL maxg = max_gradient;
	int i = 0 ; /* count of intervals in stack - 1      */
	int IFound = false;
	int begin = 0, end = 0;
	bool in_shadow_test = false;
	VECTOR VTmp;

	Thread->Stats()[Ray_IsoSurface_Bound_Tests]++;

	in_shadow_test = ray.IsShadowTestRay();

	if(container_shape)
	{
		if(Trans != NULL)
		{
			MInvTransPoint(New_Ray.Origin, ray.Origin, Trans);
			MInvTransDirection(New_Ray.Direction, ray.Direction, Trans);
			VLength(len, New_Ray.Direction);
			VInverseScaleEq(New_Ray.Direction, len);
			i_flg = Sphere::Intersect(New_Ray, container.sphere.center,
			                          (container.sphere.radius) * (container.sphere.radius),
			                          &Depth1, &Depth2);
			Depth1 = Depth1 / len;
			Depth2 = Depth2 / len;
		}
		else
		{
			i_flg = Sphere::Intersect(ray, container.sphere.center,
			                          (container.sphere.radius) * (container.sphere.radius), &Depth1, &Depth2);
		}
		Thread->Stats()[Ray_Sphere_Tests]--;
		if(i_flg)
			Thread->Stats()[Ray_Sphere_Tests_Succeeded]--;
	}
	else
	{
		i_flg = Box::Intersect(ray, Trans, container.box.corner1, container.box.corner2,
		                       &Depth1, &Depth2, &Side1, &Side2);
	}

	if(Depth1 < 0.0)
		Depth1 = 0.0;

	if(i_flg)                                   /* IsoSurface_Bound_Tests */
	{
		Thread->Stats()[Ray_IsoSurface_Bound_Tests_Succeeded]++;
		if(Trans != NULL)
		{
			MInvTransPoint(Plocal, ray.Origin, Trans);
			MInvTransDirection(Dlocal, ray.Direction, Trans);
		}
		else
		{
			Assign_Vector(Plocal, ray.Origin);
			Assign_Vector(Dlocal, ray.Direction);
		}

		Thread->isosurfaceData->Inv3 = 1;

		if(closed != false)
		{
			VEvaluateRay(VTmp, Plocal, Depth1, Dlocal);
			tmp = Vector_Function(Thread->functionContext, VTmp);
			if(Depth1 > accuracy)
			{
				if(tmp < 0.0)                   /* The ray hits the bounding shape */
				{
					VEvaluateRay(IPoint, ray.Origin, Depth1, ray.Direction);
					if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
					{
						Depth_Stack->push(Intersection(Depth1, IPoint, this, Side1));
						IFound = true;
						itrace++;
						Thread->isosurfaceData->Inv3 *= -1;
					}
				}
			}
			else
			{
				if(tmp < (maxg * accuracy * 4.0))
				{
					Depth1 = accuracy * 5.0;
					VEvaluateRay(VTmp, Plocal, Depth1, Dlocal);
					if(Vector_Function(Thread->functionContext, VTmp) < 0)
						Thread->isosurfaceData->Inv3 = -1;
					/* Change the sign of the function (IPoint is in the bounding shpae.)*/
				}
				VEvaluateRay(VTmp, Plocal, Depth2, Dlocal);
				if(Vector_Function(Thread->functionContext, VTmp) < 0.0)
				{
					VEvaluateRay(IPoint, ray.Origin, Depth2, ray.Direction);
					if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
					{
						Depth_Stack->push(Intersection(Depth2, IPoint, this, Side2));
						IFound = true;
					}
				}
			}
		}

		/*  METHOD 2   by R. Suzuki */
		tmax = Depth2 = min(Depth2, BOUND_HUGE);
		tmin = Depth1 = min(Depth2, Depth1);
		if((tmax - tmin) < accuracy)
		{
			if (IFound)
				Depth_Stack->pop(); // we added an intersection already, so we need to undo that
			return (false);
		}
		Thread->Stats()[Ray_IsoSurface_Tests]++;
		if((Depth1 < accuracy) && (Thread->isosurfaceData->Inv3 == 1))
		{
			/* IPoint is on the isosurface */
			VEvaluateRay(VTmp, Plocal, tmin, Dlocal);
			if(fabs(Vector_Function(Thread->functionContext, VTmp)) < (maxg * accuracy * 4.0))
			{
				tmin = accuracy * 5.0;
				VEvaluateRay(VTmp, Plocal, tmin, Dlocal);
				if(Vector_Function(Thread->functionContext, VTmp) < 0)
					Thread->isosurfaceData->Inv3 = -1;
				/* change the sign and go into the isosurface */
			}
		}

		Thread->isosurfaceData->ctx = Thread->functionContext;

		for (; itrace < max_trace; itrace++)
		{
			if(Function_Find_Root(*(Thread->isosurfaceData), Plocal, Dlocal, &tmin, &tmax, maxg, in_shadow_test) == false)
				break;
			else
			{
				VEvaluateRay(IPoint, ray.Origin, tmin, ray.Direction);
				if(Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
				{
					Depth_Stack->push(Intersection(tmin, IPoint, this, 0 /*Side1*/));
					IFound = true;
				}
			}
			tmin += accuracy * 5.0;
			if((tmax - tmin) < accuracy)
				break;
			Thread->isosurfaceData->Inv3 *= -1;
		}

		if(IFound)
			Thread->Stats()[Ray_IsoSurface_Tests_Succeeded]++;
	}

	if(eval == true)
	{
		DBL temp_max_gradient = max_gradient; // TODO FIXME - works around nasty gcc (found using 4.0.1) bug failing to honor casting away of volatile on pass by value on template argument lookup [trf]
		max_gradient = max((DBL)temp_max_gradient, maxg); // TODO FIXME - This is not thread-safe but should be!!! [trf]
	}

	return (IFound);
}


/*****************************************************************************
*
* FUNCTION
*
*   Inside_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::Inside(const VECTOR IPoint, TraceThreadData *Thread) const
{
	VECTOR Origin_To_Center;
	VECTOR New_Point;
	DBL OCSquared;

	/* Transform the point into box space. */
	if(Trans != NULL)
		MInvTransPoint(New_Point, IPoint, Trans);
	else
		Assign_Vector(New_Point, IPoint);

	if(container_shape != 0)
	{
		/* Use ellipse method. */
		VSub(Origin_To_Center, container.sphere.center, New_Point);
		VDot(OCSquared, Origin_To_Center, Origin_To_Center);

		if(OCSquared > Sqr(container.sphere.radius))
			return (Test_Flag(this, INVERTED_FLAG));
	}
	else
	{
		/* Test to see if we are outside the box. */
		if((New_Point[X] < container.box.corner1[X]) || (New_Point[X] > container.box.corner2[X]))
			return (Test_Flag(this, INVERTED_FLAG));

		if((New_Point[Y] < container.box.corner1[Y]) || (New_Point[Y] > container.box.corner2[Y]))
			return (Test_Flag(this, INVERTED_FLAG));

		if((New_Point[Z] < container.box.corner1[Z]) || (New_Point[Z] > container.box.corner2[Z]))
			return (Test_Flag(this, INVERTED_FLAG));
	}

	if(Vector_Function(Thread->functionContext, New_Point) > 0)
		return (Test_Flag(this, INVERTED_FLAG));

	/* Inside the box. */
	return (!Test_Flag(this, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   IsoSurface_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Normal(VECTOR Result, Intersection *Inter, TraceThreadData *Thread) const
{
	VECTOR New_Point, TPoint;
	DBL funct;

	switch (Inter->i1)
	{
		case SIDE_X_0:
			Make_Vector(Result, -1.0, 0.0, 0.0);
			break;
		case SIDE_X_1:
			Make_Vector(Result, 1.0, 0.0, 0.0);
			break;
		case SIDE_Y_0:
			Make_Vector(Result, 0.0, -1.0, 0.0);
			break;
		case SIDE_Y_1:
			Make_Vector(Result, 0.0, 1.0, 0.0);
			break;
		case SIDE_Z_0:
			Make_Vector(Result, 0.0, 0.0, -1.0);
			break;
		case SIDE_Z_1:
			Make_Vector(Result, 0.0, 0.0, 1.0);
			break;

		default:

			/* Transform the point into the isosurface space */
			if(Trans != NULL)
				MInvTransPoint(New_Point, Inter->IPoint, Trans);
			else
				Assign_Vector(New_Point, Inter->IPoint);

			if(container_shape)
			{
				VSub(Result, New_Point, container.sphere.center);
				VLength(funct, Result);
				if(fabs(funct - container.sphere.radius) < EPSILON)
				{
					VInverseScaleEq(Result, container.sphere.radius);
					break;
				}
			}

			Assign_Vector(TPoint, New_Point);
			funct = Evaluate_Function(Thread->functionContext, *Function, TPoint);
			Assign_Vector(TPoint, New_Point);
			TPoint[X] += accuracy;
			Result[X] = Evaluate_Function(Thread->functionContext, *Function, TPoint) - funct;
			Assign_Vector(TPoint, New_Point);
			TPoint[Y] += accuracy;
			Result[Y] = Evaluate_Function(Thread->functionContext, *Function, TPoint) - funct;
			Assign_Vector(TPoint, New_Point);
			TPoint[Z] += accuracy;
			Result[Z] = Evaluate_Function(Thread->functionContext, *Function, TPoint) - funct;

			if((Result[X] == 0) && (Result[Y] == 0) && (Result[Z] == 0))
				Result[X] = 1.0;
			VNormalize(Result, Result);
	}


	/* Transform the point into the boxes space. */

	if(Trans != NULL)
	{
		MTransNormal(Result, Result, Trans);

		VNormalize(Result, Result);
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Translate(const VECTOR, const TRANSFORM* tr)
{
	Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Rotate(const VECTOR, const TRANSFORM* tr)
{
	Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Scale(const VECTOR, const TRANSFORM* tr)
{
	Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Invert()
{
	Invert_Flag(this, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void IsoSurface::Transform(const TRANSFORM* tr)
{
	if(Trans == NULL)
		Trans = Create_Transform();

	Compose_Transforms(Trans, tr);

	Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

IsoSurface::IsoSurface() : ObjectBase(ISOSURFACE_OBJECT)
{
	Make_Vector(container.box.corner1, -1.0, -1.0, -1.0);
	Make_Vector(container.box.corner2, 1.0, 1.0, 1.0);

	Make_BBox(BBox, -1.0, -1.0, -1.0, 2.0, 2.0, 2.0);

	Trans = Create_Transform();

	Function = NULL;
	accuracy = 0.001;
	max_trace = 1;

	eval_param[0] = 0.0; // 1.1; // not necessary
	eval_param[1] = 0.0; // 1.4; // not necessary
	eval_param[2] = 0.0; // 0.99; // not necessary
	eval = false;
	closed = true;
	container_shape = 0;
	isCopy = false;

	max_gradient = 1.1;
	gradient = 0.0;
	threshold = 0.0;

	mginfo = (ISO_Max_Gradient *)POV_MALLOC(sizeof(ISO_Max_Gradient), "isosurface max_gradient info");
	mginfo->refcnt = 1;
	mginfo->max_gradient = 0.0;
	mginfo->gradient = 0.0; // not really necessary yet [trf]
	mginfo->eval_max = 0.0;
	mginfo->eval_cnt = 0.0;
	mginfo->eval_gradient_sum = 0.0;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr IsoSurface::Copy()
{
	IsoSurface *New = new IsoSurface();
	Destroy_Transform(New->Trans);
	*New = *this;

	New->Function = Parser::Copy_Function(vm, Function);
	New->Trans = Copy_Transform(Trans);

	New->mginfo = mginfo;
	New->mginfo->refcnt++;

	// mark it as copy for use by max_gradient warning code
	New->isCopy = true;

	return (New);
}


/*****************************************************************************
*
* FUNCTION
*
*   Destroy_IsoSurface
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

IsoSurface::~IsoSurface()
{
	if(--mginfo->refcnt == 0)
		POV_FREE(mginfo);
	Parser::Destroy_Function(vm, Function);
	Destroy_Transform(Trans);
}

/*****************************************************************************
*
* FUNCTION
*
*   DispatchShutdownMessages
*
* INPUT
*
*   messageFactory: destination of messages
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Cason
*
* DESCRIPTION
*
*   If any max_gradient messages need to be sent to the user, they are dispatched
*   via the supplied MessageFactory.
*
* CHANGES
*
******************************************************************************/

void IsoSurface::DispatchShutdownMessages(MessageFactory& messageFactory)
{
	// TODO FIXME - works around nasty gcc (found using 4.0.1) bug failing to honor casting
	// away of volatile on pass by value on template argument lookup [trf]
	DBL temp_max_gradient = max_gradient;

	mginfo->gradient = max(gradient, mginfo->gradient);
	mginfo->max_gradient = max((DBL)temp_max_gradient, mginfo->max_gradient);

	if (isCopy == false)
	{
		FunctionCode *fn = vm->GetFunction(*(Function));

		if (fn != NULL)
		{
			if (eval == false)
			{
				// Only show the warning if necessary!
				// BTW, not being too picky here is a feature and not a bug ;-)  [trf]
				if ((mginfo->gradient > EPSILON) && (mginfo->max_gradient > EPSILON))
				{
					DBL diff = mginfo->max_gradient - mginfo->gradient;
					DBL prop = fabs(mginfo->max_gradient / mginfo->gradient);

					if (((prop <= 0.9) && (diff <= -0.5)) || (((prop <= 0.95) || (diff <= -0.1)) && (mginfo->max_gradient < 10.0)))
					{
						messageFactory.WarningAt(0, fn->filename, fn->filepos.lineno, 0, fn->filepos.offset,
						                         "The maximum gradient found was %0.3f, but max_gradient of the\n"
						                         "isosurface was set to %0.3f. The isosurface may contain holes!\n"
						                         "Adjust max_gradient to get a proper rendering of the isosurface.",
						                         (float)(mginfo->gradient),
						                         (float)(mginfo->max_gradient));
					}
					else if ((diff >= 10.0) || ((prop >= 1.1) && (diff >= 0.5)))
					{
						messageFactory.WarningAt(0, fn->filename, fn->filepos.lineno, 0, fn->filepos.offset,
						                         "The maximum gradient found was %0.3f, but max_gradient of\n"
						                         "the isosurface was set to %0.3f. Adjust max_gradient to\n"
						                         "get a faster rendering of the isosurface.",
						                         (float)(mginfo->gradient),
						                         (float)(mginfo->max_gradient));
					}
				}
			}
			else
			{
				DBL diff = (mginfo->eval_max / max(mginfo->eval_max - mginfo->eval_var, EPSILON));

				if ((eval_param[0] > mginfo->eval_max) || (eval_param[1] > diff))
				{
					mginfo->eval_cnt = max(mginfo->eval_cnt, 1.0); // make sure it won't be zero

					messageFactory.WarningAt(0, fn->filename, fn->filepos.lineno, 0, fn->filepos.offset,
					                         "Evaluate found a maximum gradient of %0.3f and an average\n"
					                         "gradient of %0.3f. The maximum gradient variation was %0.3f.\n",
					                         (float)(mginfo->eval_max),
					                         (float)(mginfo->eval_gradient_sum / mginfo->eval_cnt),
					                         (float)(mginfo->eval_var));
				}
			}
		}
	}

}

/*****************************************************************************
*
* FUNCTION
*
*   Compute_IsoSurface_BBox
*
* INPUT
*
*   ISOSURFACE - IsoSurface
*
* OUTPUT
*
*   ISOSURFACE
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Calculate the bounding box of an Isosurface.
*
* CHANGES
*
******************************************************************************/

void IsoSurface::Compute_BBox()
{
	if(container_shape != 0)
	{
		Make_BBox(BBox,
		          container.sphere.center[X] - container.sphere.radius,
		          container.sphere.center[Y] - container.sphere.radius,
		          container.sphere.center[Z] - container.sphere.radius,
		          container.sphere.radius * 2,
		          container.sphere.radius * 2,
		          container.sphere.radius * 2);
	}
	else
	{
		// [ABX 20.01.2004] Low_Left introduced to hide BCC 5.5 bug
		BBOX_VECT& Low_Left = BBox.Lower_Left;

		Assign_BBox_Vect(Low_Left, container.box.corner1);
		VSub(BBox.Lengths, container.box.corner2, container.box.corner1);
	}

	if(Trans != NULL)
	{
		Recompute_BBox(&BBox, Trans);
	}
}


/*****************************************************************************
*
* FUNCTION
*
*   IsoSurface_Function_Find_Root
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::Function_Find_Root(ISO_ThreadData& itd, const VECTOR PP, const VECTOR DD, DBL* Depth1, DBL* Depth2, DBL& maxg, bool in_shadow_test)
{
	DBL dt, t21, l_b, l_e, oldmg;
	ISO_Pair EP1, EP2;
	VECTOR VTmp;

	itd.ctx->threaddata->Stats()[Ray_IsoSurface_Find_Root]++;

	VLength(itd.Vlength, DD);

	if((itd.cache == true) && (itd.current == this))
	{
		itd.ctx->threaddata->Stats()[Ray_IsoSurface_Cache]++;
		VEvaluateRay(VTmp, PP, *Depth1, DD);
		VSubEq(VTmp, itd.Pglobal);
		VLength(l_b, VTmp);
		VEvaluateRay(VTmp, PP, *Depth2, DD);
		VSubEq(VTmp, itd.Dglobal);
		VLength(l_e, VTmp);
		if((itd.fmax - maxg * max(l_b, l_e)) > 0.0)
		{
			itd.ctx->threaddata->Stats()[Ray_IsoSurface_Cache_Succeeded]++;
			return false;
		}
	}

	Assign_Vector(itd.Pglobal, PP);
	Assign_Vector(itd.Dglobal, DD);

	itd.cache = false;
	EP1.t = *Depth1;
	EP1.f = Float_Function(itd, *Depth1);
	itd.fmax = EP1.f;
	if((closed == false) && (EP1.f < 0.0))
	{
		itd.Inv3 *= -1;
		EP1.f *= -1;
	}

	EP2.t = *Depth2;
	EP2.f = Float_Function(itd, *Depth2);
	itd.fmax = min(EP2.f, itd.fmax);

	oldmg = maxg;
	t21 = (*Depth2 - *Depth1);
	if((eval == true) && (oldmg > eval_param[0]))
		maxg = oldmg * eval_param[2];
	dt = maxg * itd.Vlength * t21;
	if(Function_Find_Root_R(itd, &EP1, &EP2, dt, t21, 1.0 / (itd.Vlength * t21), maxg))
	{
		if(eval == true)
		{
			DBL curvar = fabs(maxg - oldmg);

			if(curvar > mginfo->eval_var)
				mginfo->eval_var = curvar;

			mginfo->eval_cnt++;
			mginfo->eval_gradient_sum += maxg;

			if(maxg > mginfo->eval_max)
				mginfo->eval_max = maxg;
		}

		*Depth1 = itd.tl;

		return true;
	}
	else if(!in_shadow_test)
	{
		itd.cache = true;
		VEvaluateRay(itd.Pglobal, PP, EP1.t, DD);
		VEvaluateRay(itd.Dglobal, PP, EP2.t, DD);
		itd.current = this;

		return false;
	}

	return false;
}

/*****************************************************************************
*
* FUNCTION
*
*   IsoSurface_Function_Find_Root_R
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool IsoSurface::Function_Find_Root_R(ISO_ThreadData& itd, const ISO_Pair* EP1, const ISO_Pair* EP2, DBL dt, DBL t21, DBL len, DBL& maxg)
{
	ISO_Pair EPa;
	DBL temp;

	temp = fabs((EP2->f - EP1->f) * len);
	if(gradient < temp)
		gradient = temp;

	if((eval == true) && (maxg < temp * eval_param[1]))
	{
		maxg = temp * eval_param[1] * eval_param[1];
		dt = maxg * itd.Vlength * t21;
	}

	if(t21 < accuracy)
	{
		if(EP2->f < 0)
		{
			itd.tl = EP2->t;
			return true;
		}
		else
			return false;
	}

	if((EP1->f + EP2->f - dt) < 0)
	{
		t21 *= 0.5;
		dt *= 0.5;
		EPa.t = EP1->t + t21;
		EPa.f = Float_Function(itd, EPa.t);

		itd.fmax = min(EPa.f, itd.fmax);
		if(!Function_Find_Root_R(itd, EP1, &EPa, dt, t21, len * 2.0, maxg))
			return (Function_Find_Root_R(itd, &EPa, EP2, dt, t21, len * 2.0,maxg));
		else
			return true;
	}
	else
		return false;
}


/*****************************************************************************
*
* FUNCTION
*
*   Vector_IsoSurface_Function
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL IsoSurface::Vector_Function(FPUContext *ctx, const VECTOR VPos) const
{
	return Evaluate_Function(ctx, *Function, VPos) - threshold;
}


/*****************************************************************************
*
* FUNCTION
*
*   Float_IsoSurface_Function
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   R. Suzuki
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL IsoSurface::Float_Function(ISO_ThreadData& itd, DBL t) const
{
	VECTOR VTmp;

	VEvaluateRay(VTmp, itd.Pglobal, t, itd.Dglobal);

	return ((DBL)itd.Inv3 * (Evaluate_Function(itd.ctx, *Function, VTmp) - threshold));
}


/*****************************************************************************
 *
 * FUNCTION
 *
 *   Evaluate_Function
 *
 * INPUT
 *
 * OUTPUT
 *
 * RETURNS
 *
 * AUTHOR
 *
 * DESCRIPTION
 *
 *   -
 *
 * CHANGES
 *
 *   -
 *
 ******************************************************************************/

DBL IsoSurface::Evaluate_Function(FPUContext *ctx, FUNCTION funct, const VECTOR fnvec)
{
	ctx->SetLocal(X, fnvec[X]);
	ctx->SetLocal(Y, fnvec[Y]);
	ctx->SetLocal(Z, fnvec[Z]);

	return POVFPU_Run(ctx, funct);
}

// +!+ slair
void IsoSurface::Dump(FILE* df, TraceThreadData* Thread)
{
	if (df)
	{
		unsigned int MB_RESOLUTION_X, MB_RESOLUTION_Y, MB_RESOLUTION_Z;

		FILE *cfg_file;
		cfg_file = fopen(TMPCFG_NAME, "r");
		if (cfg_file!=NULL)
		{
			fread(&MB_RESOLUTION_X, sizeof(MB_RESOLUTION_X), 1, cfg_file);
			fread(&MB_RESOLUTION_Y, sizeof(MB_RESOLUTION_Y), 1, cfg_file);
			fread(&MB_RESOLUTION_Z, sizeof(MB_RESOLUTION_Z), 1, cfg_file);
			fclose(cfg_file);
		}
		else
		{
			MB_RESOLUTION_X = 8;
			MB_RESOLUTION_Y = 8;
			MB_RESOLUTION_Z = 8;
		}

		VECTOR vpos, IPoint;

		//~ FILE *dump_file;
		//~ dump_file = fopen(DUMP_FILENAME,"a");
		//~ fclose(dump_file);
		//~ dump_file = fopen(DUMP_FILENAME,"r+");
		//~ fseek(dump_file, 0L, SEEK_END);

		//~ fwrite(tag_ISOSURFACE, 4, 1, dump_file);
		//~ Dump_BASE(obj, dump_file);
		IsBaseObject=false;
		DUMPTAG(df, "ISOS")
		ObjectBase::Dump(df, Thread);
		//fwrite(obj->BBox.Lower_Left, sizeof(obj->BBox.Lower_Left), 1, dump_file);

		//~ DBL begin_x = obj->BBox.Lower_Left[0];
		//~ DBL begin_y = obj->BBox.Lower_Left[1];
		//~ DBL begin_z = obj->BBox.Lower_Left[2];
		DBL begin_x = -1;
		DBL begin_y = -1;
		DBL begin_z = -1;

		//~ DBL length_x = obj->BBox.Lengths[0];
		//~ DBL length_y = obj->BBox.Lengths[1];
		//~ DBL length_z = obj->BBox.Lengths[2];
		DBL length_x = 2;
		DBL length_y = 2;
		DBL length_z = 2;

		DBL step_x = length_x / MB_RESOLUTION_X;
		DBL step_y = length_y / MB_RESOLUTION_Y;
		DBL step_z = length_z / MB_RESOLUTION_Z;

		//~ BBOX_VECT a;
		//~ a[0] = obj->BBox.Lower_Left[0];
		//~ a[1] = obj->BBox.Lower_Left[1];
		//~ a[2] = obj->BBox.Lower_Left[2];
		//~ a[0] += obj->BBox.Lengths[0];
		//~ a[1] += obj->BBox.Lengths[1];
		//~ a[2] += obj->BBox.Lengths[2];
		//~ fwrite(a, sizeof(a), 1, dump_file);

		GRIDCELL my_grid;
		TRI *my_tri = new TRI[10];

		long int pos_iso_tri_count = ftell(df);
		//~ printf("\n\nDEBUG: pos_to_write: %d\n\n", pos_iso_tri_count);
		int iso_tri_count = 0;
		fwrite(&iso_tri_count, sizeof(iso_tri_count), 1, df);

		int num_triangles = 0;

		for (int xc=0; xc<MB_RESOLUTION_X; xc++)
		{
			for (int yc=0; yc<MB_RESOLUTION_Y; yc++)
			{
				for (int zc=0; zc<MB_RESOLUTION_Z; zc++)
				{
					my_grid.p[0].x = begin_x+xc*step_x;
					my_grid.p[0].y = begin_y+yc*step_y;
					my_grid.p[0].z = begin_z+zc*step_z;
					Assign_Vector(IPoint, my_grid.p[0]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					//~ my_grid.val[0] = IsoSurface_Function(myIsosrf, vpos);
					my_grid.val[0] = Vector_Function(Thread->functionContext, vpos);

					my_grid.p[1].x = begin_x+xc*step_x+step_x;
					my_grid.p[1].y = begin_y+yc*step_y;
					my_grid.p[1].z = begin_z+zc*step_z;
					Assign_Vector(IPoint, my_grid.p[1]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					my_grid.val[1] = Vector_Function(Thread->functionContext, vpos);

					my_grid.p[2].x = begin_x+xc*step_x+step_x;
					my_grid.p[2].y = begin_y+yc*step_y;
					my_grid.p[2].z = begin_z+zc*step_z+step_z;
					Assign_Vector(IPoint, my_grid.p[2]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					my_grid.val[2] = Vector_Function(Thread->functionContext, vpos);

					my_grid.p[3].x = begin_x+xc*step_x;
					my_grid.p[3].y = begin_y+yc*step_y;
					my_grid.p[3].z = begin_z+zc*step_z+step_z;
					Assign_Vector(IPoint, my_grid.p[3]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					my_grid.val[3] = Vector_Function(Thread->functionContext, vpos);

					my_grid.p[4].x = begin_x+xc*step_x;
					my_grid.p[4].y = begin_y+yc*step_y+step_y;
					my_grid.p[4].z = begin_z+zc*step_z;
					Assign_Vector(IPoint, my_grid.p[4]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					my_grid.val[4] = Vector_Function(Thread->functionContext, vpos);

					my_grid.p[5].x = begin_x+xc*step_x+step_x;
					my_grid.p[5].y = begin_y+yc*step_y+step_y;
					my_grid.p[5].z = begin_z+zc*step_z;
					Assign_Vector(IPoint, my_grid.p[5]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					my_grid.val[5] = Vector_Function(Thread->functionContext, vpos);

					my_grid.p[6].x = begin_x+xc*step_x+step_x;
					my_grid.p[6].y = begin_y+yc*step_y+step_y;
					my_grid.p[6].z = begin_z+zc*step_z+step_z;
					Assign_Vector(IPoint, my_grid.p[6]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					my_grid.val[6] = Vector_Function(Thread->functionContext, vpos);

					my_grid.p[7].x = begin_x+xc*step_x;
					my_grid.p[7].y = begin_y+yc*step_y+step_y;
					my_grid.p[7].z = begin_z+zc*step_z+step_z;
					Assign_Vector(IPoint, my_grid.p[7]);
					if(Trans != NULL)
					{
						//~ MInvTransPoint(vpos, IPoint, myIsosrf->Trans);
						Assign_Vector(vpos, IPoint);
					}
					else
					{
						Assign_Vector(vpos, IPoint);
					}
					my_grid.val[7] = Vector_Function(Thread->functionContext, vpos);

					num_triangles = Polygonise(my_grid, 0, my_tri);

					/*
					printf("\n\nDEBUG: values:\n%f, %f, %f, %f\n%f, %f, %f, %f\n num_triangles:%d\n\n", \
						my_grid.val[0], my_grid.val[1], my_grid.val[2], my_grid.val[3], \
						my_grid.val[4], my_grid.val[5], my_grid.val[6], my_grid.val[7], \
						num_triangles \
					);
					*/


					if (num_triangles!=0)
					{
						iso_tri_count += num_triangles;
						//~ fwrite(my_tri, sizeof(my_tri), num_triangles, dump_file);

						for (int i=0; i<num_triangles; i++)
						{
							fwrite(&(my_tri[i].p[0]), sizeof(my_tri[i].p[0]), 1, df);
							fwrite(&(my_tri[i].p[1]), sizeof(my_tri[i].p[1]), 1, df);
							fwrite(&(my_tri[i].p[2]), sizeof(my_tri[i].p[2]), 1, df);
						}

					}
				}
			}
		}

		//~ printf("\n\nDEBUG: iso_tri_count: %d cur_pos:%d\n\n", iso_tri_count, ftell(dump_file));

		fseek(df, pos_iso_tri_count, SEEK_SET);
		//~ printf("\n\nDEBUG: write_to %d\n\n", ftell(dump_file));
		fwrite(&iso_tri_count, sizeof(iso_tri_count), 1, df);

		fseek(df, 0L, SEEK_END);

		//~ printf("\n\nDEBUG: cur_pos %d\n\n", ftell(dump_file));

		//~ Dump_Trans(dump_file, obj->Trans);
		//~ Dump_Color(dump_file, obj->Texture);

		//~ fclose(dump_file);
	}
};
// -!- slair

}

