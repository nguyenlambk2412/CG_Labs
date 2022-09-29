#include "parametric_shapes.hpp"
#include "core/Log.h"

#include <glm/glm.hpp>

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

bonobo::mesh_data
parametric_shapes::createQuad(float const width, float const height,
                              unsigned int const horizontal_split_count,
                              unsigned int const vertical_split_count)
{
	bonobo::mesh_data data;
	/*auto const vertices = std::array<glm::vec3, 4>{
		glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(width, 0.0f, 0.0f),
			glm::vec3(width, height, 0.0f),
			glm::vec3(0.0f, height, 0.0f)
	};
	data.vertices_nb = 4;
	auto const index_sets = std::array<glm::uvec3, 2>{
		glm::uvec3(0u, 1u, 2u),
			glm::uvec3(0u, 2u, 3u)
	};
	data.indices_nb = 6;*/
	//A4_E1: create the quad on XZ plane
	// calculate the number of edges from the number of split
	auto const horizontal_slice_edges_count = horizontal_split_count + 1u;
	auto const vertical_slice_edges_count = vertical_split_count + 1u;
	// calculate the number of vertices from the number of edges
	auto const horizontal_slice_vertices_count = horizontal_slice_edges_count + 1u;
	auto const vertical_slice_vertices_count = vertical_slice_edges_count + 1u;
	data.vertices_nb = horizontal_slice_vertices_count * vertical_slice_vertices_count;
	
	//create buffer for position, texture coordinates, normals, tangents and binormals.
	auto vertices = std::vector<glm::vec3>(data.vertices_nb);
	auto texcoords = std::vector<glm::vec3>(data.vertices_nb);
	data.indices_nb = horizontal_slice_edges_count * vertical_slice_edges_count * 2*3;
	auto index_sets = std::vector<glm::uvec3>(horizontal_slice_edges_count * vertical_slice_edges_count * 2);
	
	
	//tesselation
	unsigned int index = 0;
	for (unsigned int i = 0; i < vertical_slice_vertices_count; ++i)
	{
		for (unsigned int j = 0; j < horizontal_slice_vertices_count; ++j)
		{
			vertices[index] = glm::vec3(width*j/ horizontal_slice_edges_count, 0.0f, height*i/ vertical_slice_edges_count);

			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(horizontal_slice_edges_count)),
				static_cast<float>(i) / (static_cast<float>(vertical_slice_edges_count)),
				0.0f);
			index++;	// Next vertex
		}
	}
	// generate indices iteratively
	
	index = 0u;
	for (unsigned int i = 0u; i < vertical_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < horizontal_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(vertical_slice_vertices_count * (i + 0u) + (j + 0u),
				vertical_slice_vertices_count * (i + 1u) + (j + 1u),
				vertical_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
	
			index_sets[index] = glm::uvec3(vertical_slice_vertices_count * (i + 0u) + (j + 0u),
				vertical_slice_vertices_count * (i + 0u) + (j + 1u),
				vertical_slice_vertices_count * (i + 1u) + (j + 1u));
			++index;
		}
	}

	// calculate offset and size for each data buffer: position, normal vector, texture coordinate, tangent and binormal
	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	
	auto const texcoords_offset = vertices_offset + vertices_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	
	// the size of the data buffer will be the summary of all buffers
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size + texcoords_size);

	/*if (horizontal_split_count > 0u || vertical_split_count > 0u)
	{
		LogError("parametric_shapes::createQuad() does not support tesselation.");
		return data;
	}*/

	//
	// NOTE:
	//
	// Only the values preceeded by a `\todo` tag should be changed, the
	// other ones are correct!
	//

	// Create a Vertex Array Object: it will remember where we stored the
	// data on the GPU, and  which part corresponds to the vertices, which
	// one for the normals, etc.
	//
	// The following function will create new Vertex Arrays, and pass their
	// name in the given array (second argument). Since we only need one,
	// pass a pointer to `data.vao`.
	glGenVertexArrays(1, /*! \todo fill me */&data.vao);

	// To be able to store information, the Vertex Array has to be bound
	// first.
	glBindVertexArray(/*! \todo bind the previously generated Vertex Array */data.vao);

	// To store the data, we need to allocate buffers on the GPU. Let's
	// allocate a first one for the vertices.
	//
	// The following function's syntax is similar to `glGenVertexArray()`:
	// it will create multiple OpenGL objects, in this case buffers, and
	// return their names in an array. Have the buffer's name stored into
	// `data.bo`.
	glGenBuffers(1, /*! \todo fill me */&data.bo);

	// Similar to the Vertex Array, we need to bind it first before storing
	// anything in it. The data stored in it can be interpreted in
	// different ways. Here, we will say that it is just a simple 1D-array
	// and therefore bind the buffer to the corresponding target.
	glBindBuffer(GL_ARRAY_BUFFER, /*! \todo bind the previously generated Buffer */data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);
	//put the vertex data into the buffer.
	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	//put the textures coordinates into the buffer.
	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));
	
	// Now, let's allocate a second one for the indices.
	//
	// Have the buffer's name stored into `data.ibo`.
	glGenBuffers(1, /*! \todo fill me */&data.ibo);

	// We still want a 1D-array, but this time it should be a 1D-array of
	// elements, aka. indices!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, /*! \todo bind the previously generated Buffer */data.ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, /*! \todo how many bytes should the buffer contain? */static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)),
	             /* where is the data stored on the CPU? */index_sets.data(),
	             /* inform OpenGL that the data is modified once, but used often */GL_STATIC_DRAW);
	
	// All the data has been recorded, we can unbind them.
	glBindVertexArray(0u);
	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}

bonobo::mesh_data
parametric_shapes::createSphere(float const radius,
                                unsigned int const longitude_split_count,
                                unsigned int const latitude_split_count)
{

	//! \todo Implement this function
	// Take reference from the createCircleRing
	// 
	// calculate the number of edges from the number of split
	auto const longitude_slice_edges_count = longitude_split_count + 1u;
	auto const latitude_slice_edges_count = latitude_split_count + 1u;
	// calculate the number of vertices from the number of edges
	auto const longitude_slice_vertices_count = longitude_slice_edges_count + 1u;
	auto const latitude_slice_vertices_count = latitude_slice_edges_count + 1u;
	auto const vertices_nb = longitude_slice_vertices_count * latitude_slice_vertices_count;
	//create buffer for position, texture coordinates, normals, tangents and binormals.
	auto vertices = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto normals = std::vector<glm::vec3>(vertices_nb);
	auto tangents = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);

	//calculate the theta and phi for each iteration
	float const d_theta = glm::two_pi<float>() / (static_cast<float>(longitude_slice_edges_count));
	float const d_phi = glm::pi<float>() / (static_cast<float>(latitude_slice_edges_count));

	size_t index = 0u;
	float theta = 0.0f;
	// calculate value for each vertex, texture coordinate, normal, tangent, binormal using the equation
	for (unsigned int i = 0u; i < longitude_slice_vertices_count; ++i) {
		float const cos_theta = std::cos(theta);
		float const sin_theta = std::sin(theta);

		float phi = 0.0f;
		for (unsigned int j = 0u; j < latitude_slice_vertices_count; ++j) {
			float const cos_phi = std::cos(phi);
			float const sin_phi = std::sin(phi);

			// vertex
			vertices[index] = glm::vec3(radius * sin_theta * sin_phi,
				-radius * cos_phi,
				radius * cos_theta * sin_phi);
			//A3_E1: Texture coordinates
			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(latitude_slice_vertices_count)),
										static_cast<float>(i) / (static_cast<float>(longitude_slice_vertices_count)),
										0.0f);

			// tangent
			// Simplify
			auto const t = glm::vec3(cos_theta,
				0.0f,
				-sin_theta);
			tangents[index] =  glm::normalize(t);

			// binormal
			// Simplify
			auto const b = glm::vec3(sin_theta * cos_phi,
				sin_phi,
				cos_theta * cos_phi);
			binormals[index] = glm::normalize(b);

			// normalis the cross product of tangent and binormal
			auto const n = glm::cross(t, b);
			normals[index] = glm::normalize(n);

			phi += d_phi;
			++index;
		}

		theta += d_theta;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * longitude_slice_edges_count * latitude_slice_edges_count);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < longitude_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < latitude_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(latitude_slice_vertices_count * (i + 1u) + (j + 0u),
				latitude_slice_vertices_count * (i + 1u) + (j + 1u),
				latitude_slice_vertices_count * (i + 0u) + (j + 0u));
			++index;

			index_sets[index] = glm::uvec3(latitude_slice_vertices_count * (i + 1u) + (j + 1u),
				latitude_slice_vertices_count * (i + 0u) + (j + 1u),
				latitude_slice_vertices_count * (i + 0u) + (j + 0u));
			++index;
		}
	}

	bonobo::mesh_data data;
	//create the vertex array object
	glGenVertexArrays(1, &data.vao);
	// check if the created VAO is valid
	assert(data.vao != 0u);
	//bind the VAO for the below configuration
	glBindVertexArray(data.vao);

	// calculate offset and size for each data buffer: position, normal vector, texture coordinate, tangent and binormal
	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	// the size of the data buffer will be the summary of all buffers
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
		+ normals_size
		+ texcoords_size
		+ tangents_size
		+ binormals_size
		);
	//create a data buffer object
	glGenBuffers(1, &data.bo);
	//check if the created buffer object is valid
	assert(data.bo != 0u);
	//Bind the current data buffer for below configurations
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	//configure the data buffer as the array buffer with the size is the total size, the specific data will be put later.
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);
	//put the vertex data into the buffer.
	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));
	//put the normal data into the buffer.
	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));
	//put the textures coordinates into the buffer.
	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));
	//put the tangenet data into the buffer.
	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));
	//put the binormal data into the buffer.
	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));
	//unbind the data buffer object to avoid changing by mistake
	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	//calculate the number of index
	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u); // each set is the vec3
	//create new data buffer for the indices
	glGenBuffers(1, &data.ibo);
	//check validity
	assert(data.ibo != 0u);
	//bind the indices buffer for below configuration
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	//put the data into the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	//finish configurating. Unbild all VAO and VBO
	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}

bonobo::mesh_data
parametric_shapes::createTorus(float const major_radius,
                               float const minor_radius,
                               unsigned int const major_split_count,
                               unsigned int const minor_split_count)
{
	//! \todo (Optional) Implement this function
	return bonobo::mesh_data();
}

bonobo::mesh_data
parametric_shapes::createCircleRing(float const radius,
                                    float const spread_length,
                                    unsigned int const circle_split_count,
                                    unsigned int const spread_split_count)
{
	auto const circle_slice_edges_count = circle_split_count + 1u;
	auto const spread_slice_edges_count = spread_split_count + 1u;
	auto const circle_slice_vertices_count = circle_slice_edges_count + 1u;
	auto const spread_slice_vertices_count = spread_slice_edges_count + 1u;
	auto const vertices_nb = circle_slice_vertices_count * spread_slice_vertices_count;

	auto vertices  = std::vector<glm::vec3>(vertices_nb);
	auto normals   = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto tangents  = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);

	float const spread_start = radius - 0.5f * spread_length;
	float const d_theta = glm::two_pi<float>() / (static_cast<float>(circle_slice_edges_count));
	float const d_spread = spread_length / (static_cast<float>(spread_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float theta = 0.0f;
	for (unsigned int i = 0u; i < circle_slice_vertices_count; ++i) {
		float const cos_theta = std::cos(theta);
		float const sin_theta = std::sin(theta);

		float distance_to_centre = spread_start;
		for (unsigned int j = 0u; j < spread_slice_vertices_count; ++j) {
			// vertex
			vertices[index] = glm::vec3(distance_to_centre * cos_theta,
			                            distance_to_centre * sin_theta,
			                            0.0f);

			// texture coordinates
			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(spread_slice_vertices_count)),
			                             static_cast<float>(i) / (static_cast<float>(circle_slice_vertices_count)),
			                             0.0f);

			// tangent
			auto const t = glm::vec3(cos_theta, sin_theta, 0.0f);
			tangents[index] = t;

			// binormal
			auto const b = glm::vec3(-sin_theta, cos_theta, 0.0f);
			binormals[index] = b;

			// normal
			auto const n = glm::cross(t, b);
			normals[index] = n;

			distance_to_centre += d_spread;
			++index;
		}

		theta += d_theta;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * circle_slice_edges_count * spread_slice_edges_count);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < circle_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < spread_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
			                               spread_slice_vertices_count * (i + 0u) + (j + 1u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 1u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	                                            +normals_size
	                                            +texcoords_size
	                                            +tangents_size
	                                            +binormals_size
	                                            );
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}
