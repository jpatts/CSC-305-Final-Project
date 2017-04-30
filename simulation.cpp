#include "simulation.h"

#include "scene.h"

#include "imgui.h"

#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "flythrough_camera.h"

#include <glm/gtc/type_ptr.hpp>

#include <SDL.h>

#include <vector>

#include <tuple>

#include <string>

#include <math.h>

void Simulation::Init(Scene* scene)
{
    mScene = scene;

    std::vector<uint32_t> loadedMeshIDs;

    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/cube/cube.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
        uint32_t newInstanceID;
        AddMeshInstance(mScene, loadedMeshID, &newInstanceID);

        // scale up the cube
        uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
        scene->Transforms[newTransformID].Scale = glm::vec3(2.0f);
    }

    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/teapot/teapot.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
        // place a teapot on top of the cube
        {
            uint32_t newInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
            uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
            scene->Transforms[newTransformID].Translation += glm::vec3(0.0f, 2.0f, 0.0f);
        }

		//spline
		{
			uint32_t newInstanceID;
			AddMeshInstance(mScene, 0, &newInstanceID);
			uint32_t newTransformID = mScene->Instances[newInstanceID].TransformID;
			//mScene->Transforms[newTransformID].Translation += glm::vec3(0.0f, 4.0f, 0.0f);
			mSplineTransformID = newInstanceID;
		}
        // place a teapot on the side
        {
            uint32_t newInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
            uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
            scene->Transforms[newTransformID].Translation += glm::vec3(3.0f, 1.0f, 4.0f);
        }

        // place another teapot on the side
        {
            uint32_t newInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
            uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
            scene->Transforms[newTransformID].Translation += glm::vec3(3.0f, 1.0f, -4.0f);
        }
		
		{
			uint32_t parentInstanceID;
			uint32_t childInstanceID;
			AddMeshInstance(scene, loadedMeshID, &parentInstanceID);
			scene->Transforms[scene->Instances[parentInstanceID].TransformID].ParentID = -1;
			AddMeshInstance(scene, loadedMeshID, &childInstanceID);
			uint32_t childTransformID = scene->Instances[childInstanceID].TransformID;
			scene->Transforms[scene->Instances[childInstanceID].TransformID].ParentID =
				scene->Instances[parentInstanceID].TransformID;
			scene->Transforms[childTransformID].Translation = glm::vec3(3.5f, 2.0f, 0.0f);
			uint32_t parentTransformID = scene->Instances[parentInstanceID].TransformID;
			scene->Transforms[parentTransformID].Translation = glm::vec3(0.0f, -1.0f, 0.0f);
			scene->Transforms[childTransformID].RotationOrigin =
				-scene->Transforms[childTransformID].Translation;
			mSpinningTransformID = childInstanceID;
		}
		
    }
	/*
	loadedMeshIDs.clear();
	LoadMeshesFromFile(mScene, "assets/sponza/sponza.obj", &loadedMeshIDs);
	for (uint32_t loadedMeshID : loadedMeshIDs)
	{
		if (scene->Meshes[loadedMeshID].Name == "sponza_04")
		{
			continue;
		}
		uint32_t newInstanceID;
		AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
		uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
		scene->Transforms[newTransformID].Scale = glm::vec3(1.0f / 50.0f);
	}
	*/
    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/floor/floor.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
        AddMeshInstance(mScene, loadedMeshID, nullptr);
    }

    Camera mainCamera;
    mainCamera.Eye = glm::vec3(5.0f);
    glm::vec3 target = glm::vec3(0.0f);
    mainCamera.Look = normalize(target - mainCamera.Eye);
    mainCamera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
    mainCamera.FovY = glm::radians(70.0f);
    mScene->MainCamera = mainCamera;

	Light mainLight;
	mainLight.Shoot = glm::vec3(3.0f);
	mainLight.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	mainLight.Position = glm::vec3(10.0f, 10.0f, 10.0f);
	mainLight.Direction = normalize(target - mainLight.Shoot);

	mainLight.FovY = glm::radians(70.0f);
	mScene->MainLight = mainLight;

}

void Simulation::HandleEvent(const SDL_Event& ev)
{
    if (ev.type == SDL_MOUSEMOTION)
    {
        mDeltaMouseX += ev.motion.xrel;
        mDeltaMouseY += ev.motion.yrel;
    }
}

void Simulation::Update(float deltaTime)
{
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);
    
	float angularVelocity = 30.0f; // rotating at 30 degrees per second
	mScene->Transforms[mSpinningTransformID].Rotation *=
		glm::angleAxis(
			glm::radians(angularVelocity * deltaTime),
			glm::vec3(0.0f, 1.0f, 0.0f));

	angularVelocity = -90.0f;
	mScene->Transforms[mSpinningTransformID2].Rotation *=
		glm::angleAxis(
			glm::radians(angularVelocity * deltaTime),
			glm::vec3(0.0f, 1.0f, 0.0f));

    int mx, my;
    Uint32 mouse = SDL_GetMouseState(&mx, &my);

    if ((mouse & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0 && mScene->MainCamera.rollercoaster == false)
    {
        flythrough_camera_update(
            value_ptr(mScene->MainCamera.Eye),
            value_ptr(mScene->MainCamera.Look),
            value_ptr(mScene->MainCamera.Up),
            NULL,
            deltaTime,
            5.0f, // eye_speed
            0.1f, // degrees_per_cursor_move
            80.0f, // max_pitch_rotation_degrees
            mDeltaMouseX, mDeltaMouseY,
            keyboard[SDL_SCANCODE_W], keyboard[SDL_SCANCODE_A], keyboard[SDL_SCANCODE_S], keyboard[SDL_SCANCODE_D],
            keyboard[SDL_SCANCODE_SPACE], keyboard[SDL_SCANCODE_LCTRL],
            0);
    }
	//A Catmull Rom spline rendered by a floating Cube that traverses the Spline, 
	//and once the end is reached, turns around and does it again, repeating this until stopped by user
	if (mScene->MainCamera.renderSpline == true) {
		//Standard math behind Catmull Rom Splines
		int j = mScene->MainCamera.render_location;
		bool reverse = mScene->MainCamera.render_reverse;
		std::vector <glm::vec3> Pi = mScene->MainCamera.Pi_renderSpline;
		glm::mat4 Mcr = mScene->MainCamera.Mcr;
		mScene->MainCamera.t_renderSpline += mScene->MainCamera.render_speed * deltaTime;
		float t = mScene->MainCamera.t_renderSpline;
		float t2 = t*t;
		float t3 = t*t*t;
		
		glm::vec4 T = glm::vec4(1, t, t2, t3);
		glm::vec4 Cix = glm::vec4(Pi[j-1].x, Pi[j].x, Pi[j+1].x, Pi[j+2].x);
		glm::vec4 Ciy = glm::vec4(Pi[j-1].y, Pi[j].y, Pi[j+1].y, Pi[j+2].y);
		glm::vec4 Ciz = glm::vec4(Pi[j-1].z, Pi[j].z, Pi[j+1].z, Pi[j+2].z);
		glm::mat3x4 Ci = glm::mat3x4(Cix, Ciy, Ciz);
		glm::vec3 CiT = T * Mcr * Ci;

		mScene->Transforms[mSplineTransformID].Translation = CiT;

		//perform arc length check for current spline by taking dx/ds, dy/ds, and dz/ds
		if (mScene->MainCamera.arc_check == false) {
			float x0 = 0.0f, y0 = 0.0f, z0 = 0.0f;
			float arc = 0.0f;
			int length = 100;
			for (int i = 1; i < length; i++) {
				float x = i*0.5f, y = i*0.5f, z = i*0.5f;
				float dx = x0 - x, dy = y0 - y, dz = z0 - z;
				arc += sqrt((dx*dx) + (dy*dy) + (dz*dz));
				mScene->MainCamera.arc_lengths.push_back(glm::vec2(float(i/length), arc));
				x0 = x;
				y0 = y;
				//mScene->MainCamera.arc_speed = mScene->MainCamera.render_speed / sqrt((dxds*dxds) + (dyds*dyds) + (dzds*dzds));
			}
			mScene->MainCamera.total_length = arc;
			mScene->MainCamera.arc_check = true;
		}

		//Checks if we are on next spline; if so move on, if at end turn around
		if (mScene->MainCamera.t_renderSpline > 1.0 && reverse == false) {
			if (j == Pi.size() - 3 && reverse == false) {
				mScene->MainCamera.render_reverse = true;
				mScene->MainCamera.render_speed = -1 * mScene->MainCamera.render_speed;
			}
			else {
				mScene->MainCamera.t_renderSpline = 0.0;
				mScene->MainCamera.render_location++;
				mScene->MainCamera.render_spline_curr++;
				mScene->MainCamera.arc_check = false;
			}
		}
		//Checks if we are on previous spline, if so go back, if at end turn around again
		else if (mScene->MainCamera.t_renderSpline < 0.0 && reverse == true) {
			if (j == 1 && reverse == true) {
				mScene->MainCamera.render_reverse = false;
				mScene->MainCamera.render_speed = -1 * mScene->MainCamera.render_speed;
			}
			else {
				mScene->MainCamera.t_renderSpline = 1.0;
				mScene->MainCamera.render_location--;
				mScene->MainCamera.render_spline_curr--;
				mScene->MainCamera.arc_check = false;
			}
		}
	}
	//Same idea as rendering a cube to traverse the spline, but this incorporates camera
	if (mScene->MainCamera.rollercoaster == true) {
		int j = mScene->MainCamera.roller_location;
		bool reverse = mScene->MainCamera.roller_reverse;
		std::vector <glm::vec3> Pi = mScene->MainCamera.Pi_rollercoaster;
		glm::mat4 Mcr = mScene->MainCamera.Mcr;
		mScene->MainCamera.t_rollercoaster += mScene->MainCamera.roller_speed * deltaTime;
		float t = mScene->MainCamera.t_rollercoaster;
		float t2 = t*t;
		float t3 = t*t*t;

		glm::vec4 T = glm::vec4(1, t, t2, t3);
		glm::vec4 Cix = glm::vec4(Pi[j - 1].x, Pi[j].x, Pi[j + 1].x, Pi[j + 2].x);
		glm::vec4 Ciy = glm::vec4(Pi[j - 1].y, Pi[j].y, Pi[j + 1].y, Pi[j + 2].y);
		glm::vec4 Ciz = glm::vec4(Pi[j - 1].z, Pi[j].z, Pi[j + 1].z, Pi[j + 2].z);
		glm::mat3x4 Ci = glm::mat3x4(Cix, Ciy, Ciz);
		glm::vec3 CiT = T * Mcr * Ci;

		//always points camera at origin to view our scene rather than looking in some random direction
		mScene->MainCamera.Eye = CiT;
		mScene->MainCamera.Look = glm::normalize(0.0f - mScene->MainCamera.Eye);

		if (mScene->MainCamera.t_rollercoaster > 1.0 && reverse == false) {
			if (j == Pi.size() - 3 && reverse == false) {
				mScene->MainCamera.roller_reverse = true;
				mScene->MainCamera.roller_speed = -1 * mScene->MainCamera.roller_speed;
			}
			else {
				mScene->MainCamera.t_rollercoaster = 0.0;
				mScene->MainCamera.roller_location++;
				mScene->MainCamera.roller_spline_curr++;
			}
		}
		else if (mScene->MainCamera.t_rollercoaster < 0.0 && reverse == true) {
			if (j == 1 && reverse == true) {
				mScene->MainCamera.roller_reverse = false;
				mScene->MainCamera.roller_speed = -1 * mScene->MainCamera.roller_speed;
			}
			else {
				mScene->MainCamera.t_rollercoaster = 1.0;
				mScene->MainCamera.roller_location--;
				mScene->MainCamera.roller_spline_curr--;
			}
		}
	}

    mDeltaMouseX = 0;
    mDeltaMouseY = 0;
	
	//GUI code
	if (ImGui::Begin("Catmull-Rom Spline"))
	{
		//Starts the camera catmull rom spline
		ImGui::Checkbox("Rollercoaster", &mScene->MainCamera.rollercoaster);
		//Adjusts speed of spline depending on direction vector
		if (*&mScene->MainCamera.roller_speed < 0.0f) {
			ImGui::SliderFloat("Speed Roller", &mScene->MainCamera.roller_speed, -1, -0.001f);
		}
		else if (*&mScene->MainCamera.roller_speed >= 0.0f) {
			ImGui::SliderFloat("Speed Roller", &mScene->MainCamera.roller_speed, 0.001, 1);
		}
		//Adds or removes points from spline in real time, while making sure that we never remove current spline
		//10 is just an arbitrary number I chose, this can theoretically have infinite splines, 
		//although you will run out of space to modify the coordinates with the GUI unless
		//you only look at a certain set of splines we are near
		ImGui::SliderInt("+/- Roller Splines", &mScene->MainCamera.roller_spline_count, 4, 10);
		if (mScene->MainCamera.roller_spline_curr + 3 > mScene->MainCamera.roller_spline_count) {
			mScene->MainCamera.roller_spline_count = mScene->MainCamera.roller_spline_curr + 3;
			printf("%s\n", "Dont Interrupt The Current Spline You Pleb");
		}
		if (mScene->MainCamera.roller_spline_count > mScene->MainCamera.Pi_rollercoaster.size()) {
			for (int i = mScene->MainCamera.Pi_rollercoaster.size(); i < *&mScene->MainCamera.roller_spline_count; i++) {
				mScene->MainCamera.Pi_rollercoaster.push_back(glm::vec3(0, 0, 0));
			}
		}
		else if (mScene->MainCamera.roller_spline_count < mScene->MainCamera.Pi_rollercoaster.size()) {
			for (int i = mScene->MainCamera.Pi_rollercoaster.size() - 1; i >= *&mScene->MainCamera.roller_spline_count; i--) {
				mScene->MainCamera.Pi_rollercoaster.erase(mScene->MainCamera.Pi_rollercoaster.begin() + i);
			}
		}
		ImGui::PushItemWidth(87.5);
		//Adds GUI Labels and Boxes to modify coordinates of Splines
		for (int i = 0; i < *&mScene->MainCamera.roller_spline_count; i++) {
			std::string x = "P" + std::to_string(i) + "x Roller";
			std::string y = "P" + std::to_string(i) + "y Roller";
			std::string z = "P" + std::to_string(i) + "z Roller";
			const char *Pix = x.c_str();
			const char *Piy = y.c_str();
			const char *Piz = z.c_str();
			ImGui::InputFloat(Pix, &mScene->MainCamera.Pi_rollercoaster[i].x);
			ImGui::SameLine();
			ImGui::InputFloat(Piy, &mScene->MainCamera.Pi_rollercoaster[i].y);
			ImGui::SameLine();
			ImGui::InputFloat(Piz, &mScene->MainCamera.Pi_rollercoaster[i].z);
		}
		ImGui::PopItemWidth();
		
		//Does all the same stuff as the above code, however for the rendered cube rather tha camera
		ImGui::Checkbox("Render Spline", &mScene->MainCamera.renderSpline);
		if (*&mScene->MainCamera.render_speed < 0.0f) {
			ImGui::SliderFloat("Speed Render", &mScene->MainCamera.render_speed, -1, -0.001f);
		}
		else if (*&mScene->MainCamera.render_speed >= 0.0f) {
			ImGui::SliderFloat("Speed Render", &mScene->MainCamera.render_speed, 0.001, 1);
		}
		ImGui::SliderInt("+/- Render Splines", &mScene->MainCamera.render_spline_count, 4, 10);
		if (mScene->MainCamera.render_spline_curr + 3 > mScene->MainCamera.render_spline_count) {
			mScene->MainCamera.render_spline_count = mScene->MainCamera.render_spline_curr + 3;
			printf("%s\n", "Dont Interrupt The Current Spline You Pleb");
		}
		if (mScene->MainCamera.render_spline_count > mScene->MainCamera.Pi_renderSpline.size()) {
			for (int i = mScene->MainCamera.Pi_renderSpline.size(); i < *&mScene->MainCamera.render_spline_count; i++) {
				mScene->MainCamera.Pi_renderSpline.push_back(glm::vec3(0, 0, 0));
			}
		}
		else if (mScene->MainCamera.render_spline_count < mScene->MainCamera.Pi_renderSpline.size()) {
			for (int i = mScene->MainCamera.Pi_renderSpline.size() - 1; i >= *&mScene->MainCamera.render_spline_count; i--) {
				mScene->MainCamera.Pi_renderSpline.erase(mScene->MainCamera.Pi_renderSpline.begin() + i);
			}
		}
		ImGui::PushItemWidth(87.5);
		for (int i = 0; i < *&mScene->MainCamera.render_spline_count; i++) {
			std::string x = "P" + std::to_string(i) + "x Render";
			std::string y = "P" + std::to_string(i) + "y Render";
			std::string z = "P" + std::to_string(i) + "z Render";
			const char *Pix = x.c_str();
			const char *Piy = y.c_str();
			const char *Piz = z.c_str();
			ImGui::InputFloat(Pix, &mScene->MainCamera.Pi_renderSpline[i].x);
			ImGui::SameLine();
			ImGui::InputFloat(Piy, &mScene->MainCamera.Pi_renderSpline[i].y);
			ImGui::SameLine();
			ImGui::InputFloat(Piz, &mScene->MainCamera.Pi_renderSpline[i].z);
		}
		ImGui::PopItemWidth();
	}
	ImGui::End();
}

void* Simulation::operator new(size_t sz)
{
    // zero out the memory initially, for convenience.
    void* mem = ::operator new(sz);
    memset(mem, 0, sz);
    return mem;
}
    