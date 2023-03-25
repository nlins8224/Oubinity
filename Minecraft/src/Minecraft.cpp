#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "chunk/ChunkManager.h"
#include "terrain_generation/TerrainGenerator.h"
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
    Camera camera{ glm::vec3(156.0f, 128.0f, 3.0f) };
    TextureManager m_texture_manager{ 16, 16, 256 };
    TerrainGenerator terrain_generator{ 1337 };
    ChunkManager chunk_manager(camera, terrain_generator);
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
            glm::vec3 player_pos = player_input.getCamera().getCameraPos();
            glm::ivec3 current_chunk_pos = chunk_manager.getChunkPosition(player_pos);
            glm::ivec3 current_block_pos = chunk_manager.getChunkBlockPosition(player_pos);
            float surface_height = chunk_manager.getTerrainGenerator().getSurfaceHeight({ current_chunk_pos.x, current_chunk_pos.z }, { current_block_pos.x, current_block_pos.z });
            float basic_noise_value = chunk_manager.getTerrainGenerator().getShapeGenerator().getBasicNoiseValue({ current_chunk_pos.x, current_chunk_pos.z }, { current_block_pos.x, current_block_pos.z });
            std::cout << "FPS: " << frames_per_second << std::endl;
            std::cout << "CURRENT CHUNK XZ: " << current_chunk_pos.x << " " << current_chunk_pos.z << std::endl;
            std::cout << "CURRENT BLOCK XZ: " << current_block_pos.x << " " << current_block_pos.z << std::endl;
            std::cout << "PLAYER POS XZ: " << player_pos.x << " " << player_pos.z << std::endl;
            std::cout << "BASIC NOISE VALUE: " << basic_noise_value << std::endl;
            std::cout << "SURFACE HEIGHT: " << surface_height << std::endl;

            seconds_elapsed += 1.0f;
            frames_per_second = 0;
        }

        chunk_manager.deleteFromChunksMap();
        player_input.processInput(delta_time);       
        master_renderer.clear();
        master_renderer.render(camera);
        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();
    }

    return 0;
}

