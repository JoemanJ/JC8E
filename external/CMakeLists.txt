include(FetchContent)

# Dear Imgui
FetchContent_Declare(
    dearimgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.0
)
add_subdirectory(dearimgui)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v2.6
)
add_subdirectory(imgui-sfml)