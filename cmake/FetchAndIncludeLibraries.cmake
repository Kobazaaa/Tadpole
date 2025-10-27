include(FetchContent)

# Fetch ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.1-docking
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
# Fetch ImGui FileDialog
FetchContent_Declare(
    igfd
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG v0.6.7
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(imgui) #igfd)

# Add the ImGui sources to project
target_sources(${PROJECT_NAME} PRIVATE
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
  #${igfd_SOURCE_DIR}/ImGuiFileDialog.cpp
)
# don't treat warnings as errors for ImGuiFileDialog
#set_source_files_properties(${igfd_SOURCE_DIR}/ImGuiFileDialog.cpp PROPERTIES COMPILE_OPTIONS "-WX-")

target_include_directories(${PROJECT_NAME} PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    #${igfd_SOURCE_DIR}
)
