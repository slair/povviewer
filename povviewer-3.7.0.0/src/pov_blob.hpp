#ifndef _POV_BLOB_HPP_
#define _POV_BLOB_HPP_

class pov_baseobject;
class pov_scene;

class pov_blob : public pov_baseobject
{
	protected:
		int tri_count;
		triangle* triangles;
		void tesselate() const;
	public:
		pov_blob(pov_scene* s);
		~pov_blob();
		int read(FILE *f);
};

#endif
