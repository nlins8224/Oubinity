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

const int scr_width = 1200;
const int scr_height = 1600;

float delta_time = 0.0f;
float last_frame = 0.0f;

float fov = 90.0f;

int main()
{
    Window window{ scr_width, scr_height, "Minecraft" };
    window.windowInit();


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader("shaders/blockVertex.glsl", "shaders/blockFragment.glsl");

    std::cout << glGetError() << std::endl;
    ChunkManager chunk_manager(shader);
    PlayerInput player_input{window.getWindow(), chunk_manager};

    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glsl_ver = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("%s : %s (%s)\n >> GLSL: %s\n",
        vendor,
        renderer,
        version,
        glsl_ver);

    glEnable(GL_DEPTH_TEST);
    glfwSetWindowUserPointer(window.getWindow(), &player_input);
    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    while (!glfwWindowShouldClose(window.getWindow()))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        player_input.processInput(delta_time);

        glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.useProgram();
       
        glm::mat4 model = glm::mat4(1.0f);
        shader.setUniformMat4("model", model);
        glm::mat4 view  = glm::mat4(1.0f);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)scr_width / (float)scr_height, 0.1f, 100.0f);  
        shader.setUniformMat4("projection", projection); 

        view = player_input.getCamera().getViewMatrix();
        shader.setUniformMat4("view", view);

        for (auto& chunk : chunk_manager.getChunks())
        {
            chunk.second.renderChunk();
        }
        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();

        glm::vec3 last_player_pos{ -1, -1, -1 };
        glm::vec3 player_pos = player_input.getCamera().getCameraPos();

        if (player_pos != last_player_pos)
        {
     /*       std::cout << "Player pos x: " << 
                player_pos.x << " y: " <<
                player_pos.y << " z: " <<
                player_pos.z << std::endl;*/
        /*    std::cout << "Chunk position x: " <<
                chunk_manager.getChunkPosition(player_pos).x << " y: " <<
                chunk_manager.getChunkPosition(player_pos).y << " z: " <<
                chunk_manager.getChunkPosition(player_pos).z << std::endl;*/

           /* std::cout << "Chunk local position x: " <<
                chunk_manager.getLocalChunkPosition(player_pos).x << " y: " <<
                chunk_manager.getLocalChunkPosition(player_pos).y << " z: " <<
                chunk_manager.getLocalChunkPosition(player_pos).z << std::endl;*/
        }

        last_player_pos = player_pos;
        //chunk_manager.updateBlock(player_pos, Block::PLANKS);
    }

    return 0;
}

