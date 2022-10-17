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

#include <stdlib.h>


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
	enum class GameState {
		GAME_START,
		GAME_RUN,
		GAME_FINISH
	};
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.5f, 1.3f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mWorld.LookAt(glm::vec3(0.0f, 0.0f, -20.0f));
	auto camera_position = mCamera.mWorld.GetTranslation();

	// Create the shader programs
	ShaderProgramManager program_manager;
	//diffuse
	GLuint spaceshipSphere_shader = 0u;
	program_manager.CreateAndRegisterProgram("testSphere",
		{ { ShaderType::vertex, "EDAF80/A5_spaceshipSphere.vert" },
		  { ShaderType::fragment, "EDAF80/A5_spaceshipSphere.frag" } },
		spaceshipSphere_shader);
	if (spaceshipSphere_shader == 0u) {
		LogError("Failed to load spaceshipSphere_shader");
		return;
	}

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
	GLuint Mask_Engine =					bonobo::loadTexture2D(config::resources_path("ObjectModel/spaceship/textures/Mask_Translucent_Rejilla_Engine.png"), false);
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
	float skyboxRadius = 15.0f;
	auto skybox_shape = parametric_shapes::createSphere(skyboxRadius, 100u, 100u);
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


	

	///Asteroid
	//Asteroid location for the space ship
	float asteroid_acceleration = 10.0f;
	float asteroid_rotation = 0.0f;
	

	const unsigned int noOfAsteroids = 30;
	std::array<glm::vec3, noOfAsteroids>  asteroid_locations;
	std::array<glm::vec3, noOfAsteroids>  asteroid_rotateVecs;
	std::array<float, noOfAsteroids>  asteroid_scales;
	std::array<float, noOfAsteroids>  asteroid_accs;
	std::array<float, noOfAsteroids>  asteroid_rotateSpeeds;
	std::array<Node, noOfAsteroids> asteroids;

	float asteroidRadius = 1.0f;
	float spaceshipRadius = 0.05f;
	// create the spaceship sphere shape
	std::array<bonobo::mesh_data, noOfAsteroids>  test_Sphere;
	std::array<Node, noOfAsteroids> testSphere;
	for (unsigned int i = 0; i < noOfAsteroids; i++)
	{
		auto& const asteroid = asteroids[i];
		asteroid.set_geometry(asteroid_shape);
		asteroid.set_program(&asteroid_shader, phong_set_uniforms);
		asteroid.add_texture("diffTexture", asteroidDiffTex, GL_TEXTURE_2D);
		asteroid.add_texture("specTexture", asteroidSpecTex, GL_TEXTURE_2D);
		asteroid.add_texture("normTexture", asteroidNormTex, GL_TEXTURE_2D);

		float posX = -3.0f + 6.0f*std::rand() / (RAND_MAX+1.0f);
		float posZ = -skyboxRadius + 2 * skyboxRadius * std::rand() / (RAND_MAX + 1.0f);
		asteroid_locations[i] = glm::vec3(posX, 0.0f, posZ);

		asteroid_scales[i] = 0.05f + 0.05*std::rand() / (RAND_MAX+1.0f);

		asteroid_accs[i] = 30.0f + std::rand() / (RAND_MAX / 50.0f);

		asteroid_rotateSpeeds[i] = 0.1*std::rand() / (RAND_MAX +0.5f);

		float rotX = std::rand() / (RAND_MAX + 2.0f);
		float rotY = std::rand() / (RAND_MAX + 2.0f);
		float rotZ = std::rand() / (RAND_MAX + 2.0f);
		asteroid_rotateVecs[i] = glm::vec3(rotX, rotY, rotZ);

		test_Sphere[i] = parametric_shapes::createSphere(asteroidRadius, 5u, 5u);
		testSphere[i].set_geometry(test_Sphere[i]);
		testSphere[i].set_program(&spaceshipSphere_shader);
	}

	///Lock location for the space ship
	bool spaceshipUpdate = false;
	float spaceshipLocation = 0.0f;
	float spaceshipRotate = 0.0f;
	float spaceshipHorSpeed = 0.02;
	float spaceshipVerSpeed = 0.0f;
	float camLocation = 0.0f;
	spaceship.get_transform().SetScale(glm::vec3(0.05));
	/*glm::mat4 spaceshipTransform = glm::mat4(1.0f);
	spaceshipTransform = glm::scale(spaceshipTransform, );*/



	

	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	auto lastTime = std::chrono::high_resolution_clock::now();

	bool show_logs = false;
	bool show_gui = true;
	bool shader_reload_failed = false;
	bool show_basis = false;
	float basis_thickness_scale = 0.1f;
	float basis_length_scale = 0.1f;
	bool pause_bool = false;
	float safeTime;
	GameState gameState = GameState::GAME_START;
	glm::vec3 asteroidPos;
	glm::vec3 spaceshipPos;
	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();

		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);

		mWindowManager.NewImGuiFrame();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		//mCamera.Update(deltaTimeUs, inputHandler);
		switch (gameState)
		{
		case GameState::GAME_START:
			ImGui::SetNextWindowPos(ImVec2(config::resolution_x * 0.5f, config::resolution_y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(config::resolution_x * 0.23f, config::resolution_y * 0.3f), ImGuiCond_Always);

			ImGui::Begin("LOST IN THE UNIVERSE", nullptr, ImGuiWindowFlags_None);
			ImGui::Text(" ");
			ImGui::Text("		Try to go as far as you can.	   ");
			ImGui::Text("Manover to avoid colision with the asteroids");
			ImGui::Text("		    Shoot to get scores				");
			ImGui::Text(" ");
			ImGui::Separator();
			ImGui::Text("CONTROLS: ");
			ImGui::Text("	- MANOEUVER:     	W,S,A and D keys");
			ImGui::Text("	- LOOK AROUND:  	Mouse");
			ImGui::Text("	- SHOOT: 			Left mouse button");
			ImGui::Text(" ");
			ImGui::Separator();
			ImGui::Text(" ");
			if (ImGui::Button("START", ImVec2(ImGui::GetWindowSize().x * 0.98f, 0.0f)))
			{
				gameState = GameState::GAME_RUN;
				safeTime = 2.0f;
			}
			ImGui::End();
			break;
		case GameState::GAME_RUN:

			//Control the spaceship
			if (inputHandler.GetKeycodeState(GLFW_KEY_A) & PRESSED) {
				spaceshipUpdate = true;
				spaceshipLocation -= spaceshipHorSpeed;
				if (spaceshipLocation <= -1) spaceshipLocation = -1;
				spaceshipRotate += 1.0f;
				if (spaceshipRotate > 30.0f)spaceshipRotate = 30.0f;
			}

			if (inputHandler.GetKeycodeState(GLFW_KEY_D) & PRESSED) {
				spaceshipUpdate = true;
				spaceshipLocation += spaceshipHorSpeed;
				if (spaceshipLocation >= 1) spaceshipLocation = 1;
				spaceshipRotate -= 1.0f;
				if (spaceshipRotate < -30.0f)spaceshipRotate = -30.0f;

			}

			if (inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_PRESSED) {
				spaceshipVerSpeed += 80.0f;
				
			}

			if (inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_PRESSED) {
				spaceshipVerSpeed -= 20.0f;
			}

			if (inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_RELEASED) {
				spaceshipVerSpeed -= 80.0f;
			}

			if (inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_RELEASED) {
				spaceshipVerSpeed += 20.0f;
			}

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
					if (true == spaceshipUpdate)
					{
						spaceship.get_transform().SetTranslate(glm::vec3(spaceshipLocation, 0.0f, 0.0f));
						spaceship.get_transform().SetRotate(glm::radians(spaceshipRotate), glm::vec3(0.0, 0.0f, 1.0f));
						spaceshipUpdate = false;
					}				
				}
				spaceship.render(mCamera.GetWorldToClipMatrix(), glm::mat4(1.0f));
				spaceshipPos = spaceship.get_transform().GetTranslation();
				

				for (unsigned int i = 0; i < noOfAsteroids; i++)
				{
					auto& const asteroid = asteroids[i];
					if (false == pause_bool)
					{
						/// <summary>
						/// Asteroid control
						/// </summary>
						float dt = std::chrono::duration<float>(deltaTimeUs).count();
						asteroid_locations[i].z += (asteroid_accs[i] + asteroid_acceleration + spaceshipVerSpeed) * dt * dt;
						if (skyboxRadius <= asteroid_locations[i].z) asteroid_locations[i].z = -skyboxRadius;
						asteroid_rotation += asteroid_rotateSpeeds[i] * dt;
						asteroid.get_transform().ResetTransform();
						asteroid.get_transform().SetTranslate(glm::vec3(asteroid_locations[i].x, 0.0f, asteroid_locations[i].z));
						asteroid.get_transform().SetRotate(asteroid_rotation, asteroid_rotateVecs[i]);
						asteroid.get_transform().SetScale(glm::vec3(asteroid_scales[i]));															
					}
					asteroid.render(mCamera.GetWorldToClipMatrix(), glm::mat4(1.0f));

					asteroidPos = asteroid.get_transform().GetTranslation();
					glm::vec3 distance = asteroidPos - spaceshipPos;
					float distanceFloat = sqrt(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2));
					if (safeTime > 0) safeTime -= 0.01;
					else
					{
						if (distanceFloat < (spaceshipRadius + asteroid_scales[i] * asteroidRadius))
						{
							gameState = GameState::GAME_FINISH;
						}
					}
					
				}

			}
			break;
		case GameState::GAME_FINISH:
			ImGui::SetNextWindowPos(ImVec2(config::resolution_x * 0.5f, config::resolution_y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(config::resolution_x * 0.23f, config::resolution_y * 0.3f), ImGuiCond_Always);

			ImGui::Begin("GAME OVER", nullptr, ImGuiWindowFlags_None);
			ImGui::Text("Click RESTART to try again!");;
			ImGui::Separator();
			ImGui::Text(" ");
			if (ImGui::Button("RESTART", ImVec2(ImGui::GetWindowSize().x * 0.98f, 0.0f)))
			{
				gameState = GameState::GAME_RUN;
				safeTime = 3.0f;
			}
				
			ImGui::End();
			break;
		}



		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//
		bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
		if (opened) {
			ImGui::Checkbox("Show basic", &show_basis);
			ImGui::Checkbox("Pause", &pause_bool);
			ImGui::SliderFloat("Speed", &spaceshipVerSpeed, 0.0f, 400.0f);
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
