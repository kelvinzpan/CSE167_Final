#ifndef _GEODE_H_
#define _GEODE_H_

#include "Node.h"
#include "MatrixTransform.h"
#include "ParticleSpawn.h"

// Used for realistic lighting
struct Material
{
	glm::vec3 color_diff;
	glm::vec3 color_spec;
	glm::vec3 color_ambi;
	float spec_shine;
};

struct Light
{
	glm::vec3 light_color;
	glm::vec3 light_dir;
};

// Group for creating groups to transform together
class Geode : public Node
{
public:
	Geode();
	Geode(const char * filepath);
	~Geode();

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	MatrixTransform * parentMT;
	glm::vec3 maxCoord;
	glm::vec3 minCoord;
	bool particleActive = false;
	ParticleSpawn * particleEffect;

	bool dontDraw;

	// These variables are needed for the shader program
	GLuint VBO_v, VBO_n, VAO, EBO;
	Material material;
	Light light;

	void draw(GLuint program, glm::mat4 C);
	void update();
	void setParticleEffect();
	void activeParticleEffect();
	void parse(const char * filepath);
	void initSize(float scale, bool centeredOnFloor);
	void setParentMT(MatrixTransform * parent);
};

#endif