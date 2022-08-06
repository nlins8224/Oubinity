#include <vector>
#include <glad/glad.h>

class Loader 
{
public:
	void loadMesh(std::vector<float>& mesh_vertices, std::vector<float>& mesh_textures);
	void bindVAO();
	Loader() = default;
	~Loader() = default;

private:
	GLuint m_vao;
	GLuint m_vertex_vbo, m_texture_vbo;

	void createVAO();
	void storeDataInVAO(int attribute_number, std::vector<float>& mesh_vertices, std::vector<float>& mesh_textures);
	void unbindVAO();
};