#include "assignment5.hpp"

#include "parametric_shapes.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <tinyfiledialogs.h>

#include <clocale>
#include <stdexcept>



edaf80::Assignment5::Assignment5(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment5::~Assignment5()
{
	bonobo::deinit();
}

void
edaf80::Assignment5::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	mCamera.mWorld.LookAt(glm::vec3(0.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(3.0f); // 3 m/s => 10.8 km/h
	auto camera_position = mCamera.mWorld.GetTranslation();

	// Create the shader programs
	ShaderProgramManager program_manager;
	//skybox shader
	GLuint skybox_shader = 0u;
	program_manager.CreateAndRegisterProgram("skybox",
	                                         { { ShaderType::vertex, "EDAF80/skybox.vert" },
	                                           { ShaderType::fragment, "EDAF80/skybox.frag" } },
											skybox_shader);
	if (skybox_shader == 0u) {
		LogError("Failed to load skybox shader");
		return;
	}

	//spaceShip shader
	GLuint spaceship_shader = 0u;
	program_manager.CreateAndRegisterProgram("spaceship",
		{ { ShaderType::vertex, "EDAF80/A5_SpaceShip.vert" },
		  { ShaderType::fragment, "EDAF80/A5_SpaceShip.frag" } },
		spaceship_shader);
	if (spaceship_shader == 0u) {
		LogError("Failed to load spaceship shader");
		return;
	}

	//asteriod shader
	GLuint asteroid_shader = 0u;
	program_manager.CreateAndRegisterProgram("asteroid",
		{ { ShaderType::vertex, "EDAF80/A5_Object.vert" },
		  { ShaderType::fragment, "EDAF80/A5_Object.frag" } },
		asteroid_shader);
	if (asteroid_shader == 0u) {
		LogError("Failed to load asteroid shader");
		return;
	}

	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	//

	//// load textures
	//for the skybox
	GLuint skyboxTexture = bonobo::loadTextureCubeMap(config::resources_path("cubemaps/Universe/right.png"),
		config::resources_path("cubemaps/Universe/left.png"),
		config::resources_path("cubemaps/Universe/front.png"),
		config::resources_path("cubemaps/Universe/back.png"),
		config::resources_path("cubemaps/Universe/bottom.png"), 
		config::resources_path("cubemaps/Universe/top.png"), false);

	GLuint asteroidDiffTex = bonobo::loadTexture2D(config::resources_path("scenes/asteroid_1/textures/asteroid_baked_diffuse.png"), false);
	GLuint asteroidSpecTex = bonobo::loadTexture2D(config::resources_path("scenes/asteroid_1/textures/asteroid_baked_ao.png"), false);
	GLuint asteroidNormTex = bonobo::loadTexture2D(config::resources_path("scenes/asteroid_1/textures/asteroid_baked_normals.png"), false);

	GLuint Albedo_Engine					= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Albedo_Engine.png"), false);
	GLuint Albedo_Ship						= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Albedo_Ship.png"), false);
	GLuint AmbientOclussion_Engine			= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/AmbientOclussion_Engine.png"), false);
	GLuint AmbientOclussion_Ship			= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/AmbientOclussion_Ship.png"), false);
	GLuint Emissive_Engine					= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Emissive_Engine.png"), false);
	GLuint Emissive_Ship					= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Emissive_Ship.png"), false);
	GLuint Mask_Engine = bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Mask_Translucent_Rejilla_Engine.png"), false);
	GLuint Metallic_Engine					= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Metallic_Engine.png"), false);
	GLuint Metallic_Ship					= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Metallic_Ship.png"), false);
	GLuint Normal_Engine_PostProdc			= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Normal_Engine_PostProdc.png"), false);
	GLuint Normal_Ship_PostProdc			= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Normal_Ship_PostProdc.png"), false);
	GLuint Roughness_Engine					= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Roughness_Engine.png"), false);
	GLuint Roughness_Ship					= bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Roughness_Ship.png"), false);


	////uniforms
	//lights
	int meshID = 0;
	auto light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
	auto const phong_set_uniforms = [&meshID, &light_position, &camera_position](GLuint program) {
		glUniform1i(glGetUniformLocation(program, "mesh_id"), meshID);
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
	};

	//
	// Todo: Load your geometry
	//
	// create the skybox shape
	auto skybox_shape = parametric_shapes::createSphere(20.0f, 100u, 100u);
	if (skybox_shape.vao == 0u) {
		LogError("Failed to retrieve the mesh for the skybox");
		return;
	}

	// create the spaceship
	std::vector<bonobo::mesh_data> const spaceShipObj = bonobo::loadObjects(config::resources_path("ObjectModel/spaceship/source/Ship.obj"));
	if (spaceShipObj.empty()) {
		LogError("Failed to load the sphere geometry: exiting.");
		return;
	}

	// create the asteriod
	std::vector<bonobo::mesh_data> const asteriodObj = bonobo::loadObjects(config::resources_path("scenes/asteroid_1/source/asteroid.obj"));
	if (asteriodObj.empty()) {
		LogError("Failed to load the sphere geometry: exiting.");
		return;
	}
	bonobo::mesh_data const& asteroid_shape = asteriodObj.front();


	//// Object Node
	// Skybox
	Node skybox;
	skybox.set_geometry(skybox_shape);
	skybox.set_program(&skybox_shader);
	skybox.add_texture("skyboxTexture", skyboxTexture, GL_TEXTURE_CUBE_MAP);

	//SpaceShip
	Node spaceship;
	
	spaceship.set_program(&spaceship_shader, phong_set_uniforms);
	spaceship.add_texture("albedoEngTex", Albedo_Engine, GL_TEXTURE_2D);
	spaceship.add_texture("ambientEngTex", AmbientOclussion_Engine, GL_TEXTURE_2D);
	spaceship.add_texture("metalicEngTex", Metallic_Engine, GL_TEXTURE_2D);
	spaceship.add_texture("normalEngTex", Normal_Engine_PostProdc, GL_TEXTURE_2D);
	spaceship.add_texture("roughnesssEngTex", Roughness_Engine, GL_TEXTURE_2D);
	spaceship.add_texture("albedoBodyTex", Albedo_Ship, GL_TEXTURE_2D);
	spaceship.add_texture("ambientBodyTex", AmbientOclussion_Ship, GL_TEXTURE_2D);
	spaceship.add_texture("metalicBodyTex", Metallic_Ship, GL_TEXTURE_2D);
	spaceship.add_texture("normalBodyTex", Normal_Ship_PostProdc, GL_TEXTURE_2D);
	spaceship.add_texture("roughnesssBodyTex", Roughness_Ship, GL_TEXTURE_2D);

	spaceship.add_texture("emissEngTex", Emissive_Engine, GL_TEXTURE_2D);
	spaceship.add_texture("emissBodyTex", Emissive_Ship, GL_TEXTURE_2D);
	spaceship.add_texture("maskEngTex", Mask_Engine, GL_TEXTURE_2D);

	
	//Asteroid
	Node asteroid;
	asteroid.set_geometry(asteroid_shape);
	asteroid.set_program(&asteroid_shader, phong_set_uniforms);
	asteroid.add_texture("diffTexture", asteroidDiffTex, GL_TEXTURE_2D);
	asteroid.add_texture("specTexture", asteroidSpecTex, GL_TEXTURE_2D);
	asteroid.add_texture("normTexture", asteroidNormTex, GL_TEXTURE_2D);


	///Lock location for the space ship
	glm::mat4 spaceshipTransform = glm::mat4(1.0f);
	spaceshipTransform = glm::translate(spaceshipTransform, glm::vec3(0.0f, 0.0f, 0.0f));
	spaceshipTransform = glm::scale(spaceshipTransform, glm::vec3(0.1));

	///Asteroid location for the space ship
	glm::mat4 asteroidTransform = glm::mat4(1.0f);
	asteroidTransform = glm::translate(asteroidTransform, glm::vec3(0.0f, 0.0f, -15.0f));
	asteroidTransform = glm::scale(asteroidTransform, glm::vec3(0.3));
	

	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	auto lastTime = std::chrono::high_resolution_clock::now();

	bool show_logs = true;
	bool show_gui = true;
	bool shader_reload_failed = false;
	bool show_basis = false;
	float basis_thickness_scale = 1.0f;
	float basis_length_scale = 1.0f;

	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(deltaTimeUs, inputHandler);

		if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
			shader_reload_failed = !program_manager.ReloadAllPrograms();
			if (shader_reload_failed)
				tinyfd_notifyPopup("Shader Program Reload Error",
				                   "An error occurred while reloading shader programs; see the logs for details.\n"
				                   "Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
				                   "error");
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);


		// Retrieve the actual framebuffer size: for HiDPI monitors,
		// you might end up with a framebuffer larger than what you
		// actually asked for. For example, if you ask for a 1920x1080
		// framebuffer, you might get a 3840x2160 one instead.
		// Also it might change as the user drags the window between
		// monitors with different DPIs, or if the fullscreen status is
		// being toggled.
		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);


		//
		// Todo: If you need to handle inputs, you can do it here
		//


		mWindowManager.NewImGuiFrame();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


		if (!shader_reload_failed) {
			//
			// Todo: Render all your geometry here.
			//
			skybox.render(mCamera.GetWorldToClipMatrix());

			///render the spaceship
			//Render mesh by mesh
			
			for (unsigned int i = 0; i < spaceShipObj.size(); i++)
			{
				spaceship.set_geometry(spaceShipObj[i]);
				if ("Low_Vehicle_Circle.014" == spaceShipObj[i].name)
				{
					meshID = 0;
				}
				else if ("Low_Engine_Circle.030" == spaceShipObj[i].name)
				{
					meshID = 1;
				}
				else
				{
					meshID = 2;
				}
				spaceship.render(mCamera.GetWorldToClipMatrix(), spaceshipTransform);
			}
			asteroid.render(mCamera.GetWorldToClipMatrix(), asteroidTransform);
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//
		bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
		if (opened) {
			ImGui::Checkbox("Show basis", &show_basis);
			ImGui::SliderFloat("Basis thickness scale", &basis_thickness_scale, 0.0f, 100.0f);
			ImGui::SliderFloat("Basis length scale", &basis_length_scale, 0.0f, 100.0f);
		}
		ImGui::End();

		if (show_basis)
			bonobo::renderBasis(basis_thickness_scale, basis_length_scale, mCamera.GetWorldToClipMatrix());
		if (show_logs)
			Log::View::Render();
		mWindowManager.RenderImGuiFrame(show_gui);

		glfwSwapBuffers(window);
	}
}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment5 assignment5(framework.GetWindowManager());
		assignment5.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
