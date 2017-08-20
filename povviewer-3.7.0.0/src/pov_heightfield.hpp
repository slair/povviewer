#ifndef _POV_HEIGHTFIELD_HPP_
#define _POV_HEIGHTFIELD_HPP_

class pov_baseobject;
class pov_scene;

class pov_heightfield : public pov_baseobject
{
	protected:
		vec3d bounding_corners[2];
		int max_x, max_z;
		SHORT min_y, max_y;
		int block_max_x, block_max_z, block_width_x, block_width_z;
		DBL* map;
		vec3s* normals;
		void tesselate() const;
	public:
		pov_heightfield(pov_scene* s);
		~pov_heightfield();
		int read(FILE *f);
};

#endif
