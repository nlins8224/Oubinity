#include "VAOBinder.h"

void VAOBinder::bindVAO(GLuint vao) const { glBindVertexArray(vao); }

void VAOBinder::unbindVAO() const { glBindVertexArray(0); }
