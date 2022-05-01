#include <vector>
#include <glad/glad.h>

class Loader {
public:
	void loadToVAO(std::vector<float> vertices);
	void bindVAO(int idx);
	Loader() = default;
	Loader(std::vector<float> vertices);

private:
	std::vector<int> m_vaos{};
	std::vector<int> m_vbos{};

	void createVAO();
	void storeDataInVAO(int attribute_number, std::vector<float> vertices);
	void unbindVAO();
};