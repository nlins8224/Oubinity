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
    Loader loader{ verts };
    PlayerInput player_input{window.getWindow()};
    glfwSetWindowUserPointer(window.getWindow(), &player_input);

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        player_input.processInput(delta_time);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.useProgram();
        // create transformations
        // TEMPORARY CODE BEGIN
        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(player_input.getCamera().getZoom()), (float)scr_width / (float)scr_height, 0.1f, 100.0f);        // retrieve the matrix uniform locations
        view = player_input.getCamera().getViewMatrix();
        unsigned int model_loc = glGetUniformLocation(shader.getID(), "model");
        unsigned int view_loc  = glGetUniformLocation(shader.getID(), "view");

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);

        shader.setUniformMat4("projection", projection);
        // TEMPORARY CODE END
        loader.bindVAO(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();
    }

    return 0;
}

