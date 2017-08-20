#version 3.7;

#include "colors.inc"

global_settings{
	assumed_gamma 1.0
}

#default{
	pigment
	{
		checker
		color rgb 1
		color rgb 0.7
	}
}

camera
{
	location <3, 3, -12>
	look_at <0, 0, 0>
	up y
	right 1.33*x
	angle 67.38
}

light_source{-10*z+10*y-10*x,rgb <0.2, 0.8, 0.5>}


#declare axis_size=100;
//~ cylinder{-x*axis_size,x*axis_size,0.05 texture{pigment{color Red}}}
//~ cylinder{-y*axis_size,y*axis_size,0.05 texture{pigment{color Green}}}
//~ cylinder{-z*axis_size,z*axis_size,0.05 texture{pigment{color Blue}}}

merge{
	box{
		-1,1
		rotate y*10
	}

	sphere{
		0,1
		translate y*2
	}
	rotate z*30
}

sphere_sweep{
//~ 	linear_spline
	cubic_spline
//~ 	b_spline
	4,
	x*5, 0.5,
	x*10-y, 0.3,
	x*15, 0.5,
	x*20+y*2, 0.2
//~ 	rotate -y*45
	translate y*2
}
/*sphere_sweep{
//~ 	linear_spline
	cubic_spline
//~ 	b_spline
	4,
	x*5, 0.5,
	y*5, 1,
	z*0, 0.5,
	z*0+x*5, 1
	translate 0*x
}
sphere_sweep{
//~ 	linear_spline
//~ 	cubic_spline
	b_spline
	4,
	x*5, 0.5,
	y*5, 1,
	z*0, 0.5,
	z*0+x*5, 1
	translate x*10
}

#declare Eye_Right = //-----------
union{
 sphere{ <0,0,0>, 0.10
    scale<1,1,0.3>
    texture{
     pigment{ color rgb<1,1,1>}
     finish { phong 1 }
    } // end of texture
 } //---------------
 sphere{<0,0,0>, 0.05
    texture{
     pigment{ color rgb<1,1,1>*.1}
     finish { phong 0.1}
    } // end of texture
    translate<0,0,-0.025>
 } //---------------
} //------ end Eye_Right

#declare P_00 = < 1.50, 4.00, 0>;
#declare P_01 = <-0.90, 2.00, 0>;
#declare P_02 = < 0.20, 1.00, 0>;
#declare P_03 = <-0.50, 1.00, 0>;
#declare P_04 = <-0.50, 0.00, 0>;
#declare P_05 = < 0.50, 0.00, 0>;
#declare P_06 = < 0.50, 1.00, 0>;
#declare P_07 = < 1.00, 1.50, 0>;
#declare P_08 = < 1.60, 1.50, 0>;
#declare P_09 = < 1.50, 0.30, 0>;
#declare P_10 = < 2.40, 0.00, 0>;
#declare P_11 = < 2.70, 1.80, 0>;
#declare P_12 = < 3.70, 1.50, 0>;
#declare P_13 = < 3.00, 0.50, 0>;
#declare P_14 = < 4.00, 0.00, 0>;
#declare P_15 = < 4.50, 1.50, 0>;
#declare P_16 = < 4.20, 3.50, 0>;
#declare P_17 = < 6.50, 2.50, 0>;

union{ // --  Little_Dragon

difference{

sphere_sweep { // Body
  b_spline
  18,
  P_00, 0.01,
  P_01, 0.07,
  P_02, 0.25,
  P_03, 0.30,
  P_04, 0.15,
  P_05, 0.45,
  P_06, 0.15,
  P_07, 0.20,
  P_08, 0.35,
  P_09, 0.30,
  P_10, 0.20,
  P_11, 0.40,
  P_12, 0.15,
  P_13, 0.30,
  P_14, 0.20,
  P_15, 0.30,
  P_16, 0.50,
  P_17, 0.50
  tolerance 0.1
  texture{
   pigment{ color rgb<0.7,1,0.15>}
   finish { diffuse 0.9 phong 1}
  } // end of texture
} //  end of sphere_sweep

// Mouth
sphere{ <0,0,0>, 0.50
   scale<1,0.2,2>
   rotate<0,0,-30>
   translate<5 ,2.85,0>
   pigment{ color rgb<1,0.7,0.15>}
  } // end of sphere

}  // end of
  //difference Body minus Mouth

// adding eyes:
object{ Eye_Right
  translate<4.5,3.10,-0.43>
  }  // end of object
object{ Eye_Right
  translate<4.5,3.10,-0.43>
  scale<1,1,-1>
  }  // end of object

scale<1,1,1>
rotate<0,0,0>
translate<0,0.5,10>
} // end Little_Dragon
*/
