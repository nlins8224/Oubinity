#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "PlayerInput.h"
#include "Window.h"
#include "Shader.h"
#include "Loader.h"
#include "Camera.h"

const int scr_width = 1200;
const int scr_height = 1600;


float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool firstMouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

int main()
{
    Window window{ scr_width, scr_height, "Minecraft" };
    window.windowInit();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::vector<float> verts{
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };
      
    Shader shader("shaders/blockVertex.glsl", "shaders/blockFragment.glsl");
    Loader loader{verts};
    PlayerInput playerInput{window.getWindow()};

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        playerInput.processInput(delta_time);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.useProgram();
        // create transformations
        // TEMPORARY CODE BEGIN
        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(Camera::m_default_camera.m_zoom), (float)scr_width / (float)scr_height, 0.1f, 100.0f);        // retrieve the matrix uniform locations
        view = Camera::m_default_camera.getViewMatrix();
        unsigned int modelLoc = glGetUniformLocation(shader.getID(), "model");
        unsigned int viewLoc  = glGetUniformLocation(shader.getID(), "view");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        shader.setUniformMat4("projection", projection);
        // TEMPORARY CODE END
        loader.bindVAO(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();
    }

    return 0;
}

