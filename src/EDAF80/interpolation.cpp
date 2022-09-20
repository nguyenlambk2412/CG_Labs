#include "interpolation.hpp"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x)
{
	//! \todo Implement this function
	//! Apply the equation in the lecture3
	glm::vec2 t_transpose(1.0f, x);	// matrix 1 row and 2 columns
	glm::mat2 M_inverse(1.0f, -1.0f, 0.0f, 1.0f); // matrix 2 rows and 2 columns
	glm::mat3x2 q(p0.x, p1.x, p0.y, p1.y, p0.z, p1.z); // matrix 2 rows and 3 columns
	glm::vec3 p = t_transpose * M_inverse * q;
	return p;
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
                              glm::vec3 const& p2, glm::vec3 const& p3,
                              float const t, float const x)
{
	//! \todo Implement this function
	//! Apply the equation in the lecture3
	glm::vec4 t_transpose(1.0f, x, glm::pow(x,2), glm::pow(x, 3));
	glm::mat4 M_inverse(0, -t, 2 * t, -t,
						1, 0, t - 3, 2 - t,
						0, t, 3 - 2 * t, t - 2,
						0, 0, -t, t);
	glm::mat3x4 q(p0.x, p1.x, p2.x, p3.x, p0.y, p1.y, p2.y, p3.y, p0.z, p1.z, p2.z, p3.z);

	glm::vec3 p = t_transpose * M_inverse * q;
	return p;
	return glm::vec3();
}
