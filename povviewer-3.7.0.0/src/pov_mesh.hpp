#ifndef _POV_MESH_HPP_
#define _POV_MESH_HPP_

class pov_baseobject;
class pov_scene;

class pov_mesh : public pov_baseobject
{
	protected:
		triangle *triangles;
		unsigned int tri_count;
		void tesselate() const;
	public:
		pov_mesh(pov_scene* s);
		~pov_mesh();
		int read(FILE *f);
};

#endif
