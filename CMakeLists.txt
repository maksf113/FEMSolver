cmake_minimum_required(VERSION 3.20)  # Minimum CMake version

project(FEMSolver LANGUAGES CXX)  

# C++ settings
set(CMAKE_CXX_STANDARD 17)            
set(CMAKE_CXX_STANDARD_REQUIRED ON)   


# Add executable from source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")    # C++ sources
file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.hpp")  # Header files
file(GLOB_RECURSE SHADER_FILES "src/grpahics/shaders/*.vert" "src/graphics/shaders/*.frag") # shaders
add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${HEADER_FILES})

# Create source group for Visual Studio to reflect directory structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${CPP_SOURCES} ${HEADER_FILES})

# Find and link GLM, GLFW, GLEW for OpenGL
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Freetype REQUIRED)


# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    GLEW::GLEW 
    glm::glm 
    imgui::imgui
    Freetype::Freetype
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    "src"
)
# --- Resource Copying (Shaders and Fonts) ---

# 1. Find shader source files (Inputs)
#    CONFIGURE_DEPENDS tells CMake to re-run if files matching the glob are added/removed
file(GLOB_RECURSE SHADER_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/graphics/shaders/*.vert"
    "${CMAKE_SOURCE_DIR}/src/graphics/shaders/*.frag"
)

# 2. Define the destination directory for shaders (next to the executable)
#    We use a generator expression $<TARGET_FILE_DIR:...> which is evaluated at build time.
set(SHADER_DESTINATION_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders")

# 3. Define a "stamp" file. This file's timestamp indicates when the copy last happened.
set(SHADER_COPY_STAMP "${CMAKE_CURRENT_BINARY_DIR}/shader_copy_stamp.txt")

# 4. Add a custom command to perform the copy.
#    OUTPUT: The stamp file it generates.
#    COMMAND: Copies the directory, then updates the stamp file.
#    DEPENDS: The source shader files. If any source shader is newer than the stamp file, the command runs.
#    VERBATIM: Needed because the COMMAND uses generator expressions.
add_custom_command(
    OUTPUT  ${SHADER_COPY_STAMP}
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/graphics/shaders" "${SHADER_DESTINATION_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch ${SHADER_COPY_STAMP}
    DEPENDS ${SHADER_SOURCE_FILES}
    COMMENT "Copying shaders to runtime directory if changed."
    VERBATIM
)

# 5. Create a custom target that depends on the stamp file.
#    Add ALL so this target is run as part of the default build process.
add_custom_target(copy_shaders ALL DEPENDS ${SHADER_COPY_STAMP})
add_dependencies(${PROJECT_NAME} copy_shaders)

# --- Resource Copying (Fonts) ---

# 1. Find font source files (Inputs)
#    We look for .ttf and .otf files in the "fonts" directory.
file(GLOB_RECURSE FONT_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/fonts/*.ttf"
    "${CMAKE_SOURCE_DIR}/fonts/*.otf"
)

# 2. Define the destination directory for fonts (next to the executable)
set(FONT_DESTINATION_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts")

# 3. Define a "stamp" file to track when the last copy happened.
set(FONT_COPY_STAMP "${CMAKE_CURRENT_BINARY_DIR}/font_copy_stamp.txt")

# 4. Add a custom command to perform the copy if fonts have changed.
add_custom_command(
    OUTPUT  ${FONT_COPY_STAMP}
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/fonts" "${FONT_DESTINATION_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch ${FONT_COPY_STAMP}
    DEPENDS ${FONT_SOURCE_FILES}
    COMMENT "Copying fonts to runtime directory if changed."
    VERBATIM
)

# 5. Create a custom target to trigger the command and add it as a dependency.
add_custom_target(copy_fonts ALL DEPENDS ${FONT_COPY_STAMP})
add_dependencies(${PROJECT_NAME} copy_fonts)


