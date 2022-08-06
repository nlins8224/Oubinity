#include <vector>
#include <glad/glad.h>

class Loader 
{
public:
	void loadMesh(std::vector<float>& vertices);
	void bindVAO();
	Loader() = default;
	~Loader() = default;

private:
	GLuint m_vao;
	GLuint m_vbo;

	void createVAO();
	void storeDataInVAO(int attribute_number, std::vector<float>& vertices);
	void unbindVAO();
};