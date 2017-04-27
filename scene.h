#pragma once

#include "opengl.h"
#include "packed_freelist.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>

struct DiffuseMap
{
    GLuint DiffuseMapTO;
};

struct Material
{
    std::string Name;

    float Ambient[3];
    float Diffuse[3];
    float Specular[3];
    float Shininess;
    uint32_t DiffuseMapID;
};

struct Mesh
{
    std::string Name;

    GLuint MeshVAO;
    GLuint PositionBO;
    GLuint TexCoordBO;
    GLuint NormalBO;
	GLuint ProjectionBO;
    GLuint IndexBO;

    GLuint IndexCount;
    GLuint VertexCount;

    std::vector<GLDrawElementsIndirectCommand> DrawCommands;
    std::vector<uint32_t> MaterialIDs;
};

struct Transform
{
	int ParentID;

    glm::vec3 Scale;
    glm::vec3 RotationOrigin;
    glm::quat Rotation;
    glm::vec3 Translation;
};

struct Instance
{
    uint32_t MeshID;
    uint32_t TransformID;
};

struct Camera
{
    // View
    glm::vec3 Eye;
    glm::vec3 Look;
    glm::vec3 Up;

	bool rollercoaster = false;
	bool renderSpline = false;
	int render_location = 1;
	int roller_location = 1;
	std::vector<glm::vec3> Pi_renderSpline = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(4.0f, 4.0f, 0.0f),  glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)};
	std::vector<glm::vec3> Pi_rollercoaster = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(4.0f, 4.0f, 0.0f),  glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };
	glm::mat4 Mcr = glm::mat4(0.0f, -1.0f, 2.0f, -1.0f, 2.0f, 0.0f, -5.0f, 3.0f, 0.0f, 1.0f, 4.0f, -3.0f, 0.0f, 0.0f, -1.0f, 1.0f)*0.5f;
	float t_renderSpline = 0.0f;
	float t_rollercoaster = 0.0f;
	float render_speed = 0.5f;
	float roller_speed = 0.5f;
	bool render_reverse = false;
	bool roller_reverse = false;
	int render_spline_curr = 1;
	int roller_spline_curr = 1;

	int render_spline_count = Pi_renderSpline.size();
	int roller_spline_count = Pi_rollercoaster.size();

    // Projection
    float FovY;
};

struct Light {
	glm::vec3 Up;
	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec3 Shoot;

	float FovY;
};

class Scene
{
public:
    packed_freelist<DiffuseMap> DiffuseMaps;
    packed_freelist<Material> Materials;
    packed_freelist<Mesh> Meshes;
    packed_freelist<Transform> Transforms;
    packed_freelist<Instance> Instances;

    Camera MainCamera;
	Light MainLight;

    void Init();
};

void LoadMeshesFromFile(
    Scene* scene,
    const std::string& filename,
    std::vector<uint32_t>* loadedMeshIDs);

void AddMeshInstance(
    Scene* scene,
    uint32_t meshID,
    uint32_t* newInstanceID);
