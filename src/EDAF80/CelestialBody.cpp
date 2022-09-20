#include "CelestialBody.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "core/helpers.hpp"
#include "core/Log.h"

CelestialBody::CelestialBody(bonobo::mesh_data const& shape,
                             GLuint const* program,
                             GLuint diffuse_texture_id)
{
	_body.node.set_geometry(shape);
	_body.node.add_texture("diffuse_texture", diffuse_texture_id, GL_TEXTURE_2D);
	_body.node.set_program(program);
}

glm::mat4 CelestialBody::render(std::chrono::microseconds elapsed_time,
                                glm::mat4 const& view_projection,
                                glm::mat4 const& parent_transform,
                                bool show_basis)
{
	// Convert the duration from microseconds to seconds.
	auto const elapsed_time_s = std::chrono::duration<float>(elapsed_time).count();
	// If a different ratio was needed, for example a duration in
	// milliseconds, the following would have been used:
	// auto const elapsed_time_ms = std::chrono::duration<float, std::milli>(elapsed_time).count();

	_body.spin.rotation_angle += -elapsed_time_s * _body.spin.speed;
	_body.orbit.rotation_angle += -elapsed_time_s * _body.orbit.speed;

	glm::mat4 return_transform = parent_transform;
	glm::mat4 world = parent_transform;
	
	
	//A1:E3: Calculate the tilting matrix for the orbit
	world = glm::rotate(world, _body.orbit.inclination, glm::vec3(0.0f, 0.0f, 1.0f));
	return_transform = glm::rotate(return_transform, _body.orbit.inclination, glm::vec3(0.0f, 0.0f, 1.0f));
	//A1:E3: Calculate the orbit rotation
	world = glm::rotate(world, _body.orbit.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	return_transform = glm::rotate(return_transform, _body.orbit.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	//A1_E3: compute the translation matrix T0
	world = glm::translate(world, glm::vec3(_body.orbit.radius, 0.0f, 0.0f));
	return_transform = glm::translate(return_transform, glm::vec3(_body.orbit.radius, 0.0f, 0.0f));

	//A1_E8: rotate back to original position after creating the orbit. From here, create the spin propertiesn so that the tilt rotation will be independent
	world = glm::rotate(world, -_body.orbit.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

	//A1_E2: compute the rotation matrix R2s and overwirte the world matrix by it. This should be the 1st rotation since we want to create the tilt for the earth 
	world = glm::rotate(world, _body.spin.axial_tilt, glm::vec3(0.0f, 0.0f, 1.0f));
	//A1_E2: compute the rotation matrix R1s. This should be the 2st rotation since we want to create the earth to spin around the y-axis
	world = glm::rotate(world, _body.spin.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

	//A1_E1: Overwrite the world matrix with the scaled matrix
	world = glm::scale(world, _body.scale);

	
	if (show_basis)
	{
		bonobo::renderBasis(1.0f, 2.0f, view_projection, world);
	}

	// Note: The second argument of `node::render()` is supposed to be the
	// parent transform of the node, not the whole world matrix, as the
	// node internally manages its local transforms. However in our case we
	// manage all the local transforms ourselves, so the internal transform
	// of the node is just the identity matrix and we can forward the whole
	// world matrix.
	_body.node.render(view_projection, world);

	//render ring if it is applicable
	if (true == _ring.is_set)
	{
		//Lastly apply all the parent transformations
		glm::mat4 ringWorld = return_transform;
		//secondly rotate the ring by 90 degree around the x-axis
		ringWorld = glm::rotate(ringWorld, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//Firstly scale the ring (in programming, the order need to be reversed)
		ringWorld = glm::scale(ringWorld, glm::vec3(_ring.scale.x, _ring.scale.y, 0.0f));

		_ring.node.render(view_projection, ringWorld);
	}
	return return_transform;
}

void CelestialBody::add_child(CelestialBody* child)
{
	_children.push_back(child);
}

std::vector<CelestialBody*> const& CelestialBody::get_children() const
{
	return _children;
}

void CelestialBody::set_orbit(OrbitConfiguration const& configuration)
{
	_body.orbit.radius = configuration.radius;
	_body.orbit.inclination = configuration.inclination;
	_body.orbit.speed = configuration.speed;
	_body.orbit.rotation_angle = 0.0f;
}

void CelestialBody::set_scale(glm::vec3 const& scale)
{
	_body.scale = scale;
}

void CelestialBody::set_spin(SpinConfiguration const& configuration)
{
	_body.spin.axial_tilt = configuration.axial_tilt;
	_body.spin.speed = configuration.speed;
	_body.spin.rotation_angle = 0.0f;
}

void CelestialBody::set_ring(bonobo::mesh_data const& shape,
                             GLuint const* program,
                             GLuint diffuse_texture_id,
                             glm::vec2 const& scale)
{
	_ring.node.set_geometry(shape);
	_ring.node.add_texture("diffuse_texture", diffuse_texture_id, GL_TEXTURE_2D);
	_ring.node.set_program(program);

	_ring.scale = scale;

	_ring.is_set = true;
}
