#include <GL/gl.h>

#include "pov_transform.hpp"
#include "vec3d.hpp"

pov_transform::pov_transform()
{
	matrix = new DBL[16];
	memset (matrix, 0, 16*sizeof(DBL));
	/*
	for (int i; i<4; i++)
	{
		matrix[i][0] = 0;
		matrix[i][1] = 0;
		matrix[i][2] = 0;
		matrix[i][3] = 0;
	}
	*/
}

pov_transform::~pov_transform()
{
	delete [] matrix;
}

void pov_transform::apply()
{
	//glPushMatrix();
	glMultMatrixd(matrix);
}

int pov_transform::read(FILE *f)
{
	fread(matrix, sizeof(DBL)*16, 1, f);
	int readden = sizeof(DBL)*16;
	return readden;
}

/** @brief get_scale
  *
  * @todo: document this function
  */
vec3d pov_transform::get_scale() const
{
	return vec3d(matrix[0], matrix[5], matrix[10]);
}

/** @brief set_translation
  *
  * @todo: document this function
  */
void pov_transform::set_translation(const vec3d v)
{
	matrix[12] = v[0];
	matrix[13] = v[1];
	matrix[14] = v[2];
}

/** @brief remove_translation
  *
  * @todo: document this function
  */
void pov_transform::remove_translation()
{
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
}

/** @brief clear
  *
  * @todo: document this function
  */
void pov_transform::clear()
{
	memset (matrix, 0, 16*sizeof(DBL));
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;
}

/** @brief transform_vec3d
  *
  * @todo: document this function
  */
vec3d pov_transform::transform_vec3d(const vec3d v)
{
	DBL a,b,c;
//	a = v[0]*matrix[0] + v[1]*matrix[1] + v[2]*matrix[2] + 1*matrix[3];
//	b = v[0]*matrix[4] + v[1]*matrix[5] + v[2]*matrix[6] + 1*matrix[7];
//	c = v[0]*matrix[8] + v[1]*matrix[9] + v[2]*matrix[10] + 1*matrix[11];
	a = v[0]*matrix[0] + v[1]*matrix[4] + v[2]*matrix[8] + 1*matrix[12];
	b = v[0]*matrix[1] + v[1]*matrix[5] + v[2]*matrix[9] + 1*matrix[13];
	c = v[0]*matrix[2] + v[1]*matrix[6] + v[2]*matrix[10] + 1*matrix[14];
	return vec3d(a, b, c);
}

/** @brief print
  *
  * @todo: document this function
  */
void pov_transform::print(std::ostream& output) const
{
	output << "\t" << matrix[0] << ",\t"<< matrix[1] << ",\t"<< matrix[2] << ",\t"<< matrix[3] << ",\t" << std::endl;
	output << "\t" << matrix[4] << ",\t"<< matrix[5] << ",\t"<< matrix[6] << ",\t"<< matrix[7] << ",\t" << std::endl;
	output << "\t" << matrix[8] << ",\t"<< matrix[9] << ",\t"<< matrix[10] << ",\t"<< matrix[11] << ",\t" << std::endl;
	output << "\t" << matrix[12] << ",\t"<< matrix[13] << ",\t"<< matrix[14] << ",\t"<< matrix[15] << ",\t" << std::endl;
}

std::ostream& operator<<( std::ostream& os, const pov_transform& t )
{
	// out to stream
	t.print(os);
//	os << t.data[0] << " " << t.data[1] << " " << t.data[2];
	return os;
}

