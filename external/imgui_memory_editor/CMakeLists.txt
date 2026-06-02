add_library(imgui_hex_editor INTERFACE)

target_include_directories(imgui_hex_editor 
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# target_link_libraries(imgui_hex_editor
#     INTERFACE
#         ImGui-SFML::ImGui-SFML
# )