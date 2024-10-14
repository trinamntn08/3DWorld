cmake_minimum_required(VERSION 3.10)  # Set the minimum CMake version
project(3DWorld)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the directories for the dependencies
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")
set(DEPS_DIR "${CMAKE_SOURCE_DIR}/deps")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(SHADER_DIR "${SOURCE_DIR}/shaders")
set(IMGUI_DIR "${DEPS_DIR}/imgui")

set(BIN_DIR "${CMAKE_SOURCE_DIR}/bin")

# Include the directories for the dependencies
include_directories(
    ${DEPS_DIR}/glad
    ${DEPS_DIR}/glfw
    ${DEPS_DIR}/glm
	${DEPS_DIR}/imgui
	${DEPS_DIR}/assimp/include
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Source and shader file management
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h")
file(GLOB GLAD_SOURCES "${SOURCE_DIR}/glad.c")
file(GLOB SHADER_FILES "${SHADER_DIR}/*.*")
file(GLOB IMGUI_SOURCES "${IMGUI_DIR}/*.cpp" "${IMGUI_DIR}/*.h")

# Organize files in Visual Studio
source_group("Shaders" FILES ${SHADER_FILES})
source_group("IMGUI" FILES ${IMGUI_SOURCES})

# Add the executable target
add_executable(3DWorld ${SOURCES} ${GLAD_SOURCES} ${SHADER_FILES} ${IMGUI_SOURCES})

# Specify the path to the GLFW and NFD libraries
set(GLFW_LIB_DIR "${DEPS_DIR}/glfw")
set(ASSIMP_LIB_DIR "${DEPS_DIR}/assimp")

# Add the GLFW library directory to the target
target_link_directories(3DWorld PRIVATE ${GLFW_LIB_DIR} ${ASSIMP_LIB_DIR})

# Link the libraries
target_link_libraries(3DWorld
    glfw3
    OpenGL::GL
)

# Link the assimp-vc143-mt library depending on the build configuration
target_link_libraries(3DWorld
    $<$<CONFIG:Debug>:assimp-vc143-mtd>     # Link assimp-vc143-mtd.lib for Debug
    $<$<CONFIG:Release>:assimp-vc143-mt>     # Link assimp-vc143-mt.lib for Release
)

# Add the ASSIMP include directory to the target
#set(ASSIMP_INCLUDE_DIR "${DEPS_DIR}/assimp/include")
#target_include_directories(3DWorld PRIVATE ${ASSIMP_INCLUDE_DIR})

# Set environment variable for debugging in Visual Studio
set(VS_ENVIRONMENT_STRING "SHADER_DIR=${SHADER_DIR}\nRESOURCES_DIR=${RESOURCES_DIR}")
# Set the environment variables for debugging in Visual Studio
set_property(TARGET 3DWorld PROPERTY VS_DEBUGGER_ENVIRONMENT "${VS_ENVIRONMENT_STRING}")

# Copy shader files to the output directory after building
add_custom_command(TARGET 3DWorld POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SHADER_DIR}" $<TARGET_FILE_DIR:3DWorld>/shaders
    
    COMMENT "Copying shaders to the output directory..."
)

# Copy Assimp DLL for Debug configuration
add_custom_command(TARGET 3DWorld POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ASSIMP_LIB_DIR}/assimp-vc143-mtd.dll" $<TARGET_FILE_DIR:3DWorld>
    COMMENT "Copying Assimp-vc143-mtd.dll for Debug configuration..."
    VERBATIM
    CONFIGURATIONS Debug
)

# Copy Assimp DLL for Release configuration
add_custom_command(TARGET 3DWorld POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ASSIMP_LIB_DIR}/assimp-vc143-mt.dll" $<TARGET_FILE_DIR:3DWorld>
    COMMENT "Copying Assimp-vc143-mt.dll for Release configuration..."
    VERBATIM
    CONFIGURATIONS Release
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT 3DWorld)