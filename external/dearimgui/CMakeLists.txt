message(STATUS "Fetching DearImgui...")
FetchContent_MakeAvailable(dearimgui)

# So imgui-sfml can get this value
set(dearimgui_SOURCE_DIR ${dearimgui_SOURCE_DIR} PARENT_SCOPE)