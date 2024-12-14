cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(MimosaEngine VERSION 0.3.1 LANGUAGES C CXX)

set(STEAM_APPID 480)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Source_Files
    "gen_docs.bat"
    "src/core/accurate_timer.cpp"
    "src/core/input_handler.cpp"
    "src/core/log.cpp"
    "src/core/main.cpp"
    "src/debug/detail/obj_writer.cpp"
    "src/math/detail/basis.cpp"
    "src/math/detail/conversion.cpp"
    "src/math/detail/formatting_util.cpp"
    "src/math/detail/mat3x3.cpp"
    "src/math/detail/mat4x4.cpp"
    "src/math/detail/quaternion.cpp"
    "src/math/detail/transform3d.cpp"
    "src/math/detail/vec2.cpp"
    "src/math/detail/vec3.cpp"
    "src/math/detail/vec_math.cpp"
    "src/networking/m_net/connection.cpp"
    "src/networking/m_net/device_interface.cpp"
    "src/networking/m_net/message.cpp"
    "src/physics/physics_manager.cpp"
    "src/physics/types/constraints/collision_constraint.cpp"
    "src/physics/types/constraints/constraint.cpp"
    "src/physics/types/detail/collision_handler.cpp"
    "src/physics/types/detail/collision_mesh.cpp"
    "src/physics/types/detail/ode_solver.cpp"
    "src/physics/types/detail/phys_obj.cpp"
    "src/physics/types/detail/sle_solver.cpp"
    "src/rendering/post_processing_manager.cpp"
    "src/rendering/renderer.cpp"
    "src/rendering/types/detail/camera.cpp"
    "src/rendering/types/detail/image.cpp"
    "src/rendering/types/detail/material.cpp"
    "src/rendering/types/detail/mesh.cpp"
    "src/rendering/types/detail/obj_loader.cpp"
    "src/rendering/types/detail/polygon.cpp"
    "src/rendering/types/detail/shader.cpp"
    "src/rendering/types/detail/texture.cpp"
    "src/resource/material_manager.cpp"
    "src/resource/resource_manager.cpp"
    "src/resource/texture_manager.cpp"
    "src/resource/types/resource_reference.cpp"
    "src/scene/scene_manager.cpp"
    "src/scene/types/detail/node_path.cpp"
    "src/scene/types/detail/node_type.cpp"
    "src/scene/types/nodes/detail/node.cpp"
    "src/ui/font_manager.cpp"
    "src/ui/types/button.cpp"
    "src/ui/types/font.cpp"
    "src/ui/types/text.cpp"
    "src/ui/types/text_box.cpp"
    "src/ui/ui_manager.cpp"
    "src/vendor/glad/glad.c"
)
source_group("Source Files" FILES ${Source_Files})

add_executable(MimosaEngine ${Source_Files})
set_target_properties(MimosaEngine PROPERTIES LINK_FLAGS -Wl,-stack-size=41943040)

target_include_directories(MimosaEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/asio;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_definitions(MimosaEngine PRIVATE
    "$<$<CONFIG:Debug>:"
    "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
    "NDEBUG"
    ">"
    "_CONSOLE;"
    "UNICODE;"
    "_UNICODE"
)

if (MSVC)
target_compile_options(MimosaEngine PRIVATE
    /W4
    /MP
    /Oi
    /Ot
    /O2
)
else()
target_compile_options(MimosaEngine PRIVATE
    -Wall -Wextra -Wpedantic
    -Wno-language-extension-token
    -O3 -ffast-math
    -msse
    -msse2
    -msse3
    -mavx
    -mavx2
    -mfma
)
endif()

add_custom_command(
    TARGET MimosaEngine POST_BUILD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}../
    COMMAND "doxygen"
)

add_custom_command(
    TARGET MimosaEngine POST_BUILD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}../
    COMMAND "doxygen"
)

add_custom_command(
    TARGET MimosaEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:MimosaEngine>/assets
)

add_custom_command(
    TARGET MimosaEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/vendor/lib/steam_api64.dll $<TARGET_FILE_DIR:MimosaEngine>
)

file(WRITE build/steam_appid.txt ${STEAM_APPID})

set(LIBRARIES
"steam_api64;"
"freetype;"
"opengl32;"
"glfw3;"
"kernel32;"
"user32;"
"gdi32;"
"winspool;"
"comdlg32;"
"advapi32;"
"shell32;"
"ole32;"
"oleaut32;"
"uuid;"
"odbc32;"
"odbccp32"
)

target_link_libraries(MimosaEngine PRIVATE "${LIBRARIES}")

target_link_directories(MimosaEngine PRIVATE
    "${CMAKE_SOURCE_DIR}/src/vendor/lib"
)