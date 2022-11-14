#include <vector>
#include <glad/glad.h>

class Loader 
{
public:
	void loadMesh(std::vector<float>& mesh_vertices, std::vector<float>& mesh_textures, std::vector<float>& mesh_shades);
	void bindVAO();
	Loader() = default;
	~Loader() = default;

private:
	GLuint m_vao;
	GLuint m_vertex_vbo, m_texture_vbo, m_shading_vbo;

	void createVAO();
	void storeDataInVAO(int attribute_number, std::vector<float>& mesh_vertices, std::vector<float>& mesh_textures, std::vector<float>& mesh_shades);
	void unbindVAO();
};