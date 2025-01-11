set(PROJECT_NAME Oubinity)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(no_group_source_files
    "shaders/chunkFragment.glsl"
    "shaders/chunkVertex.glsl"
    "shaders/gradientFragment.glsl"
    "shaders/gradientVertex.glsl"
    "shaders/sceneFragment.glsl"
    "shaders/sceneVertex.glsl"
    "shaders/skyboxFragment.glsl"
    "shaders/skyboxVertex.glsl"
    "shaders/skyFragment.glsl"
    "shaders/skyVertex.glsl"
    "shaders/waterFragment.glsl"
    "shaders/waterVertex.glsl"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "src/block/Block.h"
    "src/block/BlockArray.h"
    "src/block/BlockMesh.h"
    "src/block/BlockStorage.h"
    "src/Camera.h"
    "src/chunk/Chunk.h"
    "src/chunk/ChunksMap.h"
    "src/chunk/Face.h"
    "src/chunk/Vertex.h"
    "src/dynamic_bitset.hpp"
    "src/FrameBuffer.h"
    "src/frustum/AABox.h"
    "src/frustum/Frustum.h"
    "src/frustum/Plane.h"
    "src/gpu_loader/SkyboxLoader.h"
    "src/gpu_loader/SkyLoader.h"
    "src/gpu_loader/StaticVertexPool.h"
    "src/gpu_loader/VAOBinder.h"
    "src/gpu_loader/WaterLoader.h"
    "src/gpu_loader/Zone.h"
    "src/gpu_loader/ZoneVertexPool.h"
    "src/gui/GuiLayout.h"
    "src/gui/GuiSceneWindow.h"
    "src/gui/GuiUIManager.h"
    "src/gui/GuiWindow.h"
    "src/io/Keyboard.h"
    "src/io/Mouse.h"
    "src/io/Window.h"
    "src/level_of_detail/LevelOfDetail.h"
    "src/loguru.hpp"
    "src/PlayerInput.h"
    "src/Ray.h"
    "src/renderer/ChunkBorder.h"
    "src/renderer/ChunkRenderer.h"
    "src/renderer/ChunkSlidingWindow.h"
    "src/renderer/GradientRenderer.h"
    "src/renderer/MasterRenderer.h"
    "src/renderer/Renderer.h"
    "src/renderer/SkyboxRenderer.h"
    "src/renderer/SkyRenderer.h"
    "src/renderer/WaterRenderer.h"
    "src/Settings.h"
    "src/shader/ChunkShader.h"
    "src/shader/GradientShader.h"
    "src/shader/SceneShader.h"
    "src/shader/Shader.h"
    "src/shader/SkyboxShader.h"
    "src/shader/SkyShader.h"
    "src/shader/WaterShader.h"
    "src/skybox/SkyboxMesh.h"
    "src/terrain_generation/preloaded_generation/FileMapParser.h"
    "src/terrain_generation/preloaded_generation/PreloadedGenerator.h"
    "src/terrain_generation/procedural_generation/DecorationGenerator.h"
    "src/terrain_generation/procedural_generation/ProceduralGenerator.h"
    "src/terrain_generation/TerrainGenerationTypes.h"
    "src/terrain_generation/TerrainGenerator.h"
    "src/TextureManager.h"
    "src/third_party/BS_thread_pool.hpp"
    "src/third_party/parallel_hashmap/btree.h"
    "src/third_party/parallel_hashmap/meminfo.h"
    "src/third_party/parallel_hashmap/phmap.h"
    "src/third_party/parallel_hashmap/phmap_base.h"
    "src/third_party/parallel_hashmap/phmap_bits.h"
    "src/third_party/parallel_hashmap/phmap_config.h"
    "src/third_party/parallel_hashmap/phmap_dump.h"
    "src/third_party/parallel_hashmap/phmap_fwd_decl.h"
    "src/third_party/parallel_hashmap/phmap_utils.h"
    "src/third_party/stb_image.h"
    "src/third_party/timer.h"
    "src/tree/BranchGenerator.h"
    "src/tree/Tree.h"
    "src/Util.h"
    "src/water/WaterMesh.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Header_Files__imgui
    "imgui/imconfig.h"
    "imgui/imgui.h"
    "imgui/imgui_impl_glfw.h"
    "imgui/imgui_impl_opengl3.h"
    "imgui/imgui_impl_opengl3_loader.h"
    "imgui/imgui_internal.h"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h"
)
source_group("Header Files\\imgui" FILES ${Header_Files__imgui})

set(Source_Files
    "src/block/BlockArray.cpp"
    "src/block/BlockStorage.cpp"
    "src/Camera.cpp"
    "src/chunk/Chunk.cpp"
    "src/FrameBuffer.cpp"
    "src/frustum/AABox.cpp"
    "src/frustum/Frustum.cpp"
    "src/frustum/Plane.cpp"
    "src/gpu_loader/SkyboxLoader.cpp"
    "src/gpu_loader/SkyLoader.cpp"
    "src/gpu_loader/StaticVertexPool.cpp"
    "src/gpu_loader/VAOBinder.cpp"
    "src/gpu_loader/WaterLoader.cpp"
    "src/gpu_loader/ZoneVertexPool.cpp"
    "src/gui/GuiLayout.cpp"
    "src/gui/GuiSceneWindow.cpp"
    "src/gui/GuiUIManager.cpp"
    "src/gui/GuiWindow.cpp"
    "src/io/Keyboard.cpp"
    "src/io/Mouse.cpp"
    "src/io/Window.cpp"
    "src/loguru.cpp"
    "src/Oubinity.cpp"
    "src/PlayerInput.cpp"
    "src/Ray.cpp"
    "src/renderer/ChunkRenderer.cpp"
    "src/renderer/ChunkSlidingWindow.cpp"
    "src/renderer/GradientRenderer.cpp"
    "src/renderer/MasterRenderer.cpp"
    "src/renderer/Renderer.cpp"
    "src/renderer/SkyboxRenderer.cpp"
    "src/renderer/SkyRenderer.cpp"
    "src/renderer/WaterRenderer.cpp"
    "src/shader/ChunkShader.cpp"
    "src/shader/GradientShader.cpp"
    "src/shader/SceneShader.cpp"
    "src/shader/Shader.cpp"
    "src/shader/SkyboxShader.cpp"
    "src/shader/SkyShader.cpp"
    "src/shader/WaterShader.cpp"
    "src/terrain_generation/preloaded_generation/FileMapParser.cpp"
    "src/terrain_generation/preloaded_generation/PreloadedGenerator.cpp"
    "src/terrain_generation/procedural_generation/DecorationGenerator.cpp"
    "src/terrain_generation/procedural_generation/ProceduralGenerator.cpp"
    "src/terrain_generation/TerrainGenerator.cpp"
    "src/TextureManager.cpp"
    "src/third_party/glad.cpp"
    "src/third_party/stb_image.cpp"
    "src/tree/BranchGenerator.cpp"
    "src/tree/Tree.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(Source_Files__imgui
    "imgui/imgui.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_impl_glfw.cpp"
    "imgui/imgui_impl_opengl3.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
)
source_group("Source Files\\imgui" FILES ${Source_Files__imgui})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Header_Files__imgui}
    ${Source_Files}
    ${Source_Files__imgui}
)

add_executable(${PROJECT_NAME} ${ALL_FILES})

set(ROOT_NAMESPACE Oubinity)

target_include_directories(${PROJECT_NAME} PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm;"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui;"
"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLAD/"
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)

set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE) #example if don't need the graph tool
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/FastNoise2/)
target_link_libraries(${PROJECT_NAME} FastNoise)

if (MSVC)
    add_compile_options(/W2)
    # Avoid Release or Debug directories to align config with Linux
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_SOURCE_DIR}/bin>)
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui.ini DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
