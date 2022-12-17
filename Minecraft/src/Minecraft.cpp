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
#include "renderer/MasterRenderer.h"
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

    std::cout << glGetError() << std::endl;
    Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f) };
    TextureManager m_texture_manager{ 16, 16, 256 };
    WorldGenerator world_generator{ 1234 };
    ChunkManager chunk_manager(camera, world_generator);
    PlayerInput player_input{window.getWindow(), chunk_manager, camera};
    MasterRenderer master_renderer;

    master_renderer.initConfig();
    glfwSetWindowUserPointer(window.getWindow(), &player_input);
    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        OPTICK_FRAME("MainThread");
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        master_renderer.clear();
        player_input.processInput(delta_time);       
        chunk_manager.refreshChunks();
        master_renderer.render(camera, chunk_manager.getChunksMap());
        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();
    }

    return 0;
}

