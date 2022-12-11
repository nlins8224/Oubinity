#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "ChunkManager.h"
#include "PlayerInput.h"
#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "optick.h"

const int scr_width = 1200;
const int scr_height = 1600;

float delta_time = 0.0f;
float last_frame = 0.0f;

float fov = 90.0f;

int main()
{
    OPTICK_FRAME("MainThread");
    Window window{"Minecraft" };
    window.windowInit();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader("shaders/blockVertex.glsl", "shaders/blockFragment.glsl");

    std::cout << glGetError() << std::endl;
    Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f) };
    WorldGenerator world_generator{ 1234 };
    //TODO: should shader, camera and world_generator be properties of chunk_manager?
    ChunkManager chunk_manager(shader, camera, world_generator);
    PlayerInput player_input{window.getWindow(), chunk_manager, camera};

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSetWindowUserPointer(window.getWindow(), &player_input);
    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        OPTICK_FRAME("MainThread");
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        player_input.processInput(delta_time);

        glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();
       
        glm::mat4 model = glm::mat4(1.0f);
        shader.setUniformMat4("model", model);
        glm::mat4 view  = glm::mat4(1.0f);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)scr_width / (float)scr_height, 0.1f, 200.0f);  
        shader.setUniformMat4("projection", projection); 

        view = player_input.getCamera().getViewMatrix();
        shader.setUniformMat4("view", view);

        chunk_manager.refreshChunks();
        chunk_manager.renderChunks();
        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();

        // 1. Player is updated
        // 2. ChunkManager decides what to load
        // 3. Renderer renders mesh data

    }

    return 0;
}

