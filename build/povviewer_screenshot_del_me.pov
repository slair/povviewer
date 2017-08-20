#version 3.7;

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

#declare step = 2.5;

#include "colors.inc"
#include "transforms.inc"

#declare text_mat = material
{
	texture
	{
		pigment
		{
			color Red
		}
	}
}


light_source{-10*z+10*y-10*x,rgb <1, 1, 1>}
//~ #ifndef(POVVIEWER)
//~ light_source{-10*z+10*y+10*x,rgb 1}
//~ light_source{10*z+10*y-10*x,rgb 1}
//~ #end

// _!_ sphere object ==========================================================================
#declare sphere_text = text{ttf "crystal.ttf", "sphere", 0.1, 0}
#declare s_letter = text{ttf "crystal.ttf", "s", 0.1, 0 material {text_mat}}
#declare p_letter = text{ttf "crystal.ttf",	"p", 0.1, 0	material {text_mat}}
#declare h_letter = text{ttf "crystal.ttf",	"h", 0.1, 0	material {text_mat}}
#declare e_letter = text{ttf "crystal.ttf",	"e", 0.1, 0	material {text_mat}}
#declare r_letter = text{ttf "crystal.ttf",	"r", 0.1, 0	material {text_mat}}

#declare scale_text = 0.7;
#declare angle_step = 17;

#declare sphere_obj = merge
{
	sphere{0, 1}
	object{s_letter Center_Trans(sphere_text, y+z) Center_Trans(s_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*2.5}
	object{p_letter Center_Trans(sphere_text, y+z) Center_Trans(p_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*1.5}
	object{h_letter Center_Trans(sphere_text, y+z) Center_Trans(h_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*0.5}
	object{e_letter Center_Trans(sphere_text, y+z) Center_Trans(e_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*0.5}
	object{r_letter Center_Trans(sphere_text, y+z) Center_Trans(r_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*1.5}
	object{e_letter Center_Trans(sphere_text, y+z) Center_Trans(e_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*2.5}
}
// end sphere object ==========================================================================
// _!_ box object =============================================================================
#declare action = 0.0;
#declare box_text = text{ ttf "crystal.ttf", "box", 0.1, 0 material{text_mat}}
#declare box_obj = merge
{
	box{-1, 1}
	object{box_text Center_Trans(box_text, x+y+z) scale scale_text translate -z*1.1}
	rotate action*y
}
// _!_ cone object =============================================================================
#declare cone_text = text{ ttf "crystal.ttf", "cone", 0.1, 0}
#declare c_letter = text{ ttf "crystal.ttf", "c", 0.1, 0 material{text_mat}}
#declare o_letter = text{ ttf "crystal.ttf", "o", 0.1, 0 material{text_mat}}
#declare n_letter = text{ ttf "crystal.ttf", "n", 0.1, 0 material{text_mat}}
#declare cone_obj = merge
{
	cone{-y , 1, y, 0.1}
	object{c_letter Center_Trans(cone_text, y+z) Center_Trans(c_letter, x) scale scale_text rotate x*30 translate -z*0.56 rotate y*2*angle_step*1.5}
	object{o_letter Center_Trans(cone_text, y+z) Center_Trans(o_letter, x) scale scale_text rotate x*30 translate -z*0.56 rotate y*2*angle_step*0.5}
	object{n_letter Center_Trans(cone_text, y+z) Center_Trans(n_letter, x) scale scale_text rotate x*30 translate -z*0.56 rotate -y*2*angle_step*0.5}
	object{e_letter Center_Trans(cone_text, y+z) Center_Trans(e_letter, x) scale scale_text rotate x*30 translate -z*0.56 rotate -y*2*angle_step*1.5}
}
// end box object =============================================================================
// _!_ isosurface object ======================================================================
#declare isosurface_text = text{ ttf "crystal.ttf", "isosurface", 0.1, 0}
#declare i_letter = text{ttf "crystal.ttf", "i", 0.1, 0 material{text_mat}}
#declare u_letter = text{ttf "crystal.ttf", "u", 0.1, 0 material{text_mat}}
#declare f_letter = text{ttf "crystal.ttf", "f", 0.1, 0 material{text_mat}}
#declare a_letter = text{ttf "crystal.ttf", "a", 0.1, 0 material{text_mat}}
#declare isosurface_obj = merge
{
	object{i_letter Center_Trans(isosurface_text, y+z) Center_Trans(i_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*4.5}
	object{s_letter Center_Trans(isosurface_text, y+z) Center_Trans(s_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*3.5}
	object{o_letter Center_Trans(isosurface_text, y+z) Center_Trans(o_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*2.5}
	object{s_letter Center_Trans(isosurface_text, y+z) Center_Trans(s_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*1.5}
	object{u_letter Center_Trans(isosurface_text, y+z) Center_Trans(u_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*0.5}
	object{r_letter Center_Trans(isosurface_text, y+z) Center_Trans(r_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*0.5}
	object{f_letter Center_Trans(isosurface_text, y+z) Center_Trans(f_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*1.5}
	object{a_letter Center_Trans(isosurface_text, y+z) Center_Trans(a_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*2.5}
	object{c_letter Center_Trans(isosurface_text, y+z) Center_Trans(c_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*3.5}
	object{e_letter Center_Trans(isosurface_text, y+z) Center_Trans(e_letter, x) scale scale_text translate -z*1.1 rotate -y*angle_step*4.5}

	isosurface {
		function {
			sqrt(x*x + y*y + z*z) - 0.9
			+ sin(12*atan2(x, z))*sin(8*atan2(y, sqrt(x*x + z*z)))*0.1
		}
		threshold 0
		max_gradient 8.5
		contained_by {sphere {< 0, 0, 0>, 1}}
		texture {
			//pigment {color Blue}
			finish {
				ambient 0 diffuse 1
				specular 1 roughness 0.02
				brilliance 2
			}
		}
		scale 1
		//rotate <-36, 24, 0>
		//translate <-1,-2, 0>
	}

}
// end isosurface object ======================================================================
#declare lathe_text = text{ ttf "crystal.ttf", "lathe", 0.1, 0}
#declare l_letter = text{ttf "crystal.ttf", "l", 0.1, 0 material{text_mat}}
#declare t_letter = text{ttf "crystal.ttf", "t", 0.1, 0 material{text_mat}}
//#declare t_letter = text{ttf "crystal.ttf", "h", 0.1, 0 material{text_mat}}
#declare zrad = -z*0.6;
#declare lathe_obj = merge
{
	object{l_letter Center_Trans(lathe_text, y+z) Center_Trans(l_letter, x) scale scale_text translate zrad rotate y*angle_step*4}
	object{a_letter Center_Trans(lathe_text, y+z) Center_Trans(a_letter, x) scale scale_text translate zrad rotate y*angle_step*2}
	object{t_letter Center_Trans(lathe_text, y+z) Center_Trans(t_letter, x) scale scale_text translate zrad rotate y*angle_step*0}
	object{h_letter Center_Trans(lathe_text, y+z) Center_Trans(h_letter, x) scale scale_text translate zrad rotate -y*angle_step*2}
	object{e_letter Center_Trans(lathe_text, y+z) Center_Trans(e_letter, x) scale scale_text translate zrad rotate -y*angle_step*4}
	/*lathe {
		linear_spline
		5, <2, 0>, <3, 0>, <3, 5>, <2, 5>, <2, 0>
		translate -y*2.5
		scale 0.35
 }*/
lathe {
  cubic_spline
  12,
  <0.000000, 0.000000>,
  <0.000000, 0.000000>,
  <0.460606, 0.036364>,
  <0.515152, 0.303030>,
  <0.157576, 0.660606>,
  <0.248485, 1.000000>,
  <0.230303, 1.000000>,
  <0.139394, 0.660606>,
  <0.496970, 0.296970>,
  <0.448485, 0.054545>,
  <0.000000, 0.018182>,
  <0.000000, 0.018182>

  scale <1.5, 1.5, 1.5>
	translate -y
}
}
// end lathe object =============================================================================
#declare prism_text = text{ ttf "crystal.ttf", "prism", 0.1, 0 material{text_mat}}
#declare prism_obj = merge
{
	object{prism_text Center_Trans(prism_text, x+y+z) scale scale_text translate -z*1.2}
	prism {
		cubic_spline
		0.0,
		1.0,
		35,
		<-0.228311, 0.228311>,  // Outer prism
		<-1.000000, -1.000000>,
		<1.000000, -1.000000>,
		<0.228311, 0.228311>,
		<-0.228311, 0.228311>,
		<-1.000000, -1.000000>,
		<1.000000, -1.000000>
		<-0.228311, 0.000000>,  // 1st inner prism
		<-0.228311, -0.456621>,
		<0.228311, -0.456621>,
		<0.228311, 0.000000>,
		<-0.228311, 0.000000>,
		<-0.228311, -0.456621>,
		<0.228311, -0.456621>
		<-0.479452, -0.273973>, // 2nd inner prism
		<-0.479452, -0.730594>,
		<-0.022831, -0.730594>,
		<-0.022831, -0.273973>,
		<-0.479452, -0.273973>,
		<-0.479452, -0.730594>,
		<-0.022831, -0.730594>
		<0.027397, -0.273973>, // 3rd inner prism
		<0.027397, -0.730594>,
		<0.484018, -0.730594>,
		<0.484018, -0.273973>,
		<0.027397, -0.273973>,
		<0.027397, -0.730594>,
		<0.484018, -0.730594>
		<-0.228311, -0.502283>, // 4th inner prism
		<-0.228311, -0.958904>,
		<0.228311, -0.958904>,
		<0.228311, -0.502283>,
		<-0.228311, -0.502283>,
		<-0.228311, -0.958904>,
		<0.228311, -0.958904>
		translate -.5*y
	}
}

#declare plane_text = text{ttf "crystal.ttf", "plane", 0.1, 0 material{text_mat}}
#declare plane_obj = merge
{
	object {plane_text Center_Trans(prism_text, x+y+z) rotate x*90 scale scale_text translate y*0.1}
	plane{y,0}
}

#declare torus_text = text{ttf "crystal.ttf", "torus", 0.1, 0 material{text_mat}}
#declare torus_obj = merge
{
	//object {torus_text Center_Trans(prism_text, x+y+z) scale scale_text translate -z*1.1}
	object{t_letter Center_Trans(lathe_text, y+z) Center_Trans(l_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*2}
	object{o_letter Center_Trans(lathe_text, y+z) Center_Trans(l_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*1}
	object{r_letter Center_Trans(lathe_text, y+z) Center_Trans(l_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*0}
	object{u_letter Center_Trans(lathe_text, y+z) Center_Trans(l_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*-1}
	object{s_letter Center_Trans(lathe_text, y+z) Center_Trans(l_letter, x) scale scale_text translate -z*1.1 rotate y*angle_step*-2}
	torus{0.6, 0.4}
}

#declare bicubic_text = text{ttf "crystal.ttf", "bicubic", 0.1, 0 material{text_mat}}
#declare patch_text = text{ttf "crystal.ttf", "patch", 0.1, 0 material{text_mat}}
#declare bicubicpatch_obj = merge
{
	object{bicubic_text Center_Trans(bicubic_text, y+z) Center_Trans(bicubic_text, x) scale scale_text translate -z*0.7+y*0.3}
	object{patch_text Center_Trans(patch_text, y+z) Center_Trans(patch_text, x) scale scale_text translate -z*0.7-y*0.3}
	bicubic_patch { type 1 flatness 0.1  u_steps 8  v_steps 8
		 < 0.0, 0.0, 2.0>, < 1.0, 0.0, 0.0>, < 2.0, 0.0, 0.0>, < 3.0, 0.0, -2.0>,
		 < 0.0, 1.0, 0.0>, < 1.0, 1.0, 0.0>, < 2.0, 1.0, 0.0>, < 3.0, 1.0,  0.0>,
		 < 0.0, 2.0, 0.0>, < 1.0, 2.0, 0.0>, < 2.0, 2.0, 0.0>, < 3.0, 2.0,  0.0>,
		 < 0.0, 3.0, 2.0>, < 1.0, 3.0, 0.0>, < 2.0, 3.0, 0.0>, < 3.0, 3.0, -2.0>
		 translate <-1.5, -1.5, 0>
		 scale 0.5
		 rotate -y*45+z*180
		 //rotate <30, -60, 0>
	}
}

#declare blob_text = text{ttf "crystal.ttf", "blob", 0.1, 0 material{text_mat}}
#declare blob_obj = merge
{
	object{blob_text Center_Trans(blob_text, y+z) Center_Trans(blob_text, x) scale scale_text translate -z*0.6}
	blob {
		 threshold 0.6
		 component 1.0, 1.0, <0.75, 0, 0>
		 component 1.0, 1.0, <-0.375, 0.64952, 0>
		 component 1.0, 1.0, <-0.375, -0.64952, 0>

		 finish {
				ambient 0.2
				diffuse 0.8
				phong 1
		 }
	//~    rotate 30*y
	}
}

#declare mesh_text = text{ttf "crystal.ttf", "mesh", 0.1, 0 material{text_mat}}
#declare mesh_obj = merge
{
	object{mesh_text Center_Trans(mesh_text, y+z) Center_Trans(mesh_text, x) scale scale_text translate -z*0.6}
	object{
		#include "chess.inc"
		scale 0.2
		translate z*1
		rotate y*45
	}
}


#declare height_text = text{ttf "crystal.ttf", "height", 0.1, 0 material{text_mat}}
#declare field_text = text{ttf "crystal.ttf", "field", 0.1, 0 material{text_mat}}
#declare hf_obj = merge
{
	object{height_text Center_Trans(height_text, y+z) Center_Trans(height_text, x) scale scale_text*0.8 rotate 45*y translate -z*0.6-x*0.6+0.3*y}
	object{field_text Center_Trans(field_text, y+z) Center_Trans(field_text, x) scale scale_text*0.8 rotate -45*y translate -z*0.6+x*0.6+0.365*y}
	height_field {
		"crater_dat.png" smooth
		translate <-.5, 0, -.5>
		scale <2, 0.5, 2>
		rotate 45*y
	}
}


#declare polygon_text = text{ttf "crystal.ttf", "polygon", 0.1, 0 material{text_mat}}
#declare P = polygon
{
  12,
  <0, 0>, <0, 6>, <4, 6>, <4, 3>, <1, 3>, <1, 0>, <0, 0>,
  <1, 4>, <1, 5>, <3, 5>, <3, 4>, <1, 4>
}
#declare O = polygon
{
  10,
  <0, 0>, <0, 6>, <4, 6>, <4, 0>, <0, 0>,
  <1, 1>, <1, 5>, <3, 5>, <3, 1>, <1, 1>
}
#declare V = polygon
{
  8,
  <1, 0>, <0, 6>, <1, 6>, <2, 1>, <3, 6>, <4, 6>, <3, 0>, <1, 0>
}
#declare poly_obj = merge
{
  object { P translate -5*x }
  object { O translate 0*x }
  object { V translate 5*x }
  translate -2*x-3*y
	scale 0.2
}
#declare polygon_obj = merge
{
	object{polygon_text Center_Trans(polygon_text, y+z) Center_Trans(polygon_text, x) scale scale_text translate -0.2*z}
	object{poly_obj}
}



#declare disc_text = text{ttf "crystal.ttf", "disc", 0.1, 0 material{text_mat}}
#declare disc_obj = merge
{
	object{disc_text Center_Trans(disc_text, y+z) Center_Trans(disc_text, x) scale scale_text translate y*0.3}
	disc{0, y, 1, 0.1}
}

//~ error in scene

object{sphere_obj translate step*x}
object{cone_obj translate step*y+step*x}
object{isosurface_obj rotate z*45 translate -step*x+step*y}
object{lathe_obj translate -y*step-x*step}
object{prism_obj translate -step*x/*-step*y*/}
object{box_obj translate step*x-step*y}
object{plane_obj translate -step*y*2}
object{torus_obj translate -step*y*0-2*x*step+y*step}
object{bicubicpatch_obj translate -step*x*2}
object{blob_obj translate step*x*2+step*y }
object{mesh_obj translate 2*step*x-y*step}
object{hf_obj translate 2*step*x}
object{polygon_obj translate -x*2*step-y*step}
object{disc_obj translate 2*y*step+x*step}


camera{
	perspective
	location <5.9944, -1.27523, -5.69074>
	look_at <5.56319, -2.4678, -0.600009>
	sky <0, 1, 0>
	right x*1.33333
	up y
	angle 63.5
}
