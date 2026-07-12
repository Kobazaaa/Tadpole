#--------------------------------------------------
#    CREATE EXECUTABLE
#--------------------------------------------------
# Source files
set(SOURCES
	# main
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/EditorHelpers.cpp"

	# imgui
	"${SOURCE_DIR}/imgui/ImGuiContext.cpp"

	# layers
	"${SOURCE_DIR}/layers/EditorLayer.cpp"

	# panels
	"${SOURCE_DIR}/panels/console/ConsolePanel.cpp"
	"${SOURCE_DIR}/panels/hierarchy/HierarchyPanel.cpp"
	"${SOURCE_DIR}/panels/inspector/InspectorPanel.cpp"
	"${SOURCE_DIR}/panels/stats/StatsPanel.cpp"
	"${SOURCE_DIR}/panels/viewport/ViewportPanel.cpp"
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
	"${SOURCE_DIR}"
	"${SOURCE_DIR}/imgui"
	"${SOURCE_DIR}/layers"
	"${SOURCE_DIR}/panels"
	"${SOURCE_DIR}/panels/console"
	"${SOURCE_DIR}/panels/hierarchy"
	"${SOURCE_DIR}/panels/inspector"
	"${SOURCE_DIR}/panels/stats"
	"${SOURCE_DIR}/panels/viewport"
)

# Max Warning Level & Warnings as Errors
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

#--------------------------------------------------
#    LIBRARIES
#--------------------------------------------------
include("../cmake/FetchAndIncludeLibraries.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE Kobengine)
