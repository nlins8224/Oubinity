#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "chunk/ChunkManager.h"
#include "PlayerInput.h"
#include "io/Window.h"
#include "shader/Shader.h"
#include "Camera.h"
#include "renderer/MasterRenderer.h"
#include "optick.h"

const int scr_width = 1200;
const int scr_height = 1600;


float fov = 90.0f;

int main()
{
    Window window{"Minecraft" };
    window.windowInit();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetError() << std::endl;
    Camera camera{ glm::vec3(0.0f, 128.0f, 3.0f) };
    TextureManager m_texture_manager{ 16, 16, 256 };
    WorldGenerator world_generator{ 1234, 8 };
    ChunkManager chunk_manager(camera, world_generator);
    PlayerInput player_input{window.getWindow(), chunk_manager, camera};
    MasterRenderer master_renderer(chunk_manager.getChunksMap(), chunk_manager.getChunksMapMutex(), chunk_manager.getShouldProcessChunks(), chunk_manager.getIsReadyToProcessChunks());

    master_renderer.initConfig();
    master_renderer.getChunkRenderer().launchChunkProcessingTask();
    chunk_manager.launchHandleTasks();
    glfwSetWindowUserPointer(window.getWindow(), &player_input);
    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    float delta_time = 0.0f;
    float last_frame = 0.0f;
    float seconds_elapsed = 0.0f;
    int frames_per_second = 0;

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        OPTICK_FRAME("MainThread");
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        frames_per_second++;
        // Once per second
        if (current_frame - seconds_elapsed >= 1.0f)
        {
            std::cout << "FPS: " << frames_per_second << std::endl;
            seconds_elapsed += 1.0f;
            frames_per_second = 0;
        }


        player_input.processInput(delta_time);       
        master_renderer.clear();
        master_renderer.render(camera);
        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();
    }

    return 0;
}

