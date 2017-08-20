#ifndef _GLOBAL_DEFS_HPP_
#define _GLOBAL_DEFS_HPP_

#include <SDL.h>

#define PI 3.141592653589793238
#define PI2 6.283185307179586477
#define RAD 0.01745329251994329577
#define EPSILON 1.0e-3
#define DECEL 1.0e-1

#define PATH_MAX 1024

#define KEY_DELAY 100
#define KEY_REPEAT 100

#define MIN_FPS 200.0
#define DELAY_FPS 1000.0 / MIN_FPS

//#define DEFAULT_SCENE_NAME "scenes/rcube_test.pov"
#define DEFAULT_SCENE_NAME "scenes/test.pov"
//#define DEFAULT_SCENE_NAME "scenes/broken_sky/teeter.pov"
//#define DEFAULT_SCENE_NAME "~/work/povdump/scenes/advanced/landscape.pov"

#define TMPCFG_NAME "/dev/shm/scene.cfg"
#define DUMP_FILENAME "/dev/shm/scene.dump"
#define CFG_DIR "/.config/povviewer/"
#define CFG_FILE_SCENE "scene.conf"
#define CFG_FILE_CAMERA "camera.conf"

#define MAX_ERR_STRINGS 500

typedef double DBL;
typedef float SNGL;
typedef unsigned short USHORT;
typedef short SHORT;
typedef unsigned char BYTE;
typedef short vec3s[3];

#define START_POOL_SIZE 500000
#define GROW_POOL_SIZE 100000

typedef DBL vertex[3];

typedef enum
{
	ALL			= 100,
	BBOX		= 0,
	DOTS		= 1,
	LINES		= 2,
	SOLID		= 3,
	NORMALS	= 4,
	PLANES	= 5
} view_mode;

struct KEY_ACTION
{
	SDLKey sdl_key;
	bool *bool_var;
};

class pov_scene;
class view_camera;

typedef void (pov_scene::* pfunc_pov_scene) ();
typedef void (view_camera::* pfunc_view_camera) ();

struct KEY_TOGGLES
{
	SDLKey sdl_key;
	pfunc_pov_scene pov_scene_toggle_func;
	pfunc_view_camera view_camera_toggle_func;
};

#define RESERVED_VEC3D 10000

#define SS_FILE_NAME "povviewer_screenshot_del_me.pov"

#ifdef DEBUG
	#define DEBUG_MSG(msg)  std::cout << msg << std::endl;
	#define DEBUG_ASSERT(val,msg) if (val) { std::cout << "DEBUG_ASSERT("<<#val<<") "<<msg<<endl<< " File:"	<< __FILE__ << " Line:" <<__LINE__<<endl;exit(10);}
	#define DEBUG_MSG_FL(msg,offset) cout << "\nFile:"	<< __FILE__ << " Line:" << dec << __LINE__+offset << endl << msg <<  endl;
#else
	#define DEBUG_MSG(msg)
	#define DEBUG_MSG_FL(msg,offset)
	#define DEBUG_ASSERT(val,msg)
#endif

#define SAFE_DELETE(p)       { delete   (p); (p)=NULL; }
#define SAFE_DELETE_ARRAY(p) { delete[] (p); (p)=NULL; }

#define PERSPECTIVE_CAMERA      1
#define ORTHOGRAPHIC_CAMERA     2
#define FISHEYE_CAMERA          3
#define ULTRA_WIDE_ANGLE_CAMERA 4
#define OMNIMAX_CAMERA          5
#define PANORAMIC_CAMERA        6
#define CYL_1_CAMERA            7
#define CYL_2_CAMERA            8
#define CYL_3_CAMERA            9
#define CYL_4_CAMERA           10
#define SPHERICAL_CAMERA       11

//#define VECTOR_LINES 1 // ifdef line3d stored in vector, another in set

#endif
