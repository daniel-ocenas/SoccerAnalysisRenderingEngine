cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(Engine CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME Engine)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "graphics/icons/circle.png"
    "graphics/icons/edit.png"
    "graphics/icons/pause.png"
    "graphics/icons/play.png"
    "graphics/icons/remove.png"
    "graphics/icons/replay.png"
    "src/shaders/BackgroundTextures.frag"
    "src/shaders/BackgroundTextures.vert"
    "src/shaders/BackgroundTexturesBlend.frag"
    "src/shaders/BackgroundTexturesBlend.vert"
    "src/shaders/BackgroundTexturesShadow.frag"
    "src/shaders/BackgroundTexturesShadow.vert"
    "src/shaders/ShadowMapping.frag"
    "src/shaders/ShadowMapping.vert"
    "src/shaders/shadowMappingDebug.frag"
    "src/shaders/shadowMappingDebug.vert"
    "src/shaders/ShadowMappingDepth.frag"
    "src/shaders/ShadowMappingDepth.vert"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "Dependencies/imgui/dirent/dirent.h"
    "Dependencies/imgui/imconfig.h"
    "Dependencies/imgui/imgui.h"
    "Dependencies/imgui/imgui_impl_glfw.h"
    "Dependencies/imgui/imgui_impl_opengl3.h"
    "Dependencies/imgui/imgui_internal.h"
    "Dependencies/imgui/ImGuiFileDialog.h"
    "Dependencies/imgui/ImGuiFileDialogConfig.h"
    "Dependencies/imgui/imstb_rectpack.h"
    "Dependencies/imgui/imstb_textedit.h"
    "Dependencies/imgui/imstb_truetype.h"
    "src/Arrow.hpp"
    "src/common.hpp"
    "src/Controller.hpp"
    "src/GraphicObject.hpp"
    "src/gui.hpp"
    "src/Match.hpp"
    "src/Materials.hpp"
    "src/Mesh.hpp"
    "src/ModelLoader.hpp"
    "src/OffsideLine.hpp"
    "src/PerspectiveCamera.hpp"
    "src/Player.hpp"
    "src/settings.hpp"
    "src/Shader.hpp"
    "src/Team.hpp"
    "src/Videoloader.hpp"
    "src/View.hpp"
)
source_group("Header Files" FILES ${Header_Files})

set(Resource_Files
    "graphics/icons/arrow.png"
    "graphics/icons/flag.png"
    "ResTempl.rct"
)
source_group("Resource Files" FILES ${Resource_Files})

set(Source_Files
    "Dependencies/imgui/imgui.cpp"
    "Dependencies/imgui/imgui_demo.cpp"
    "Dependencies/imgui/imgui_draw.cpp"
    "Dependencies/imgui/imgui_impl_glfw.cpp"
    "Dependencies/imgui/imgui_impl_opengl3.cpp"
    "Dependencies/imgui/imgui_tables.cpp"
    "Dependencies/imgui/imgui_widgets.cpp"
    "Dependencies/imgui/ImGuiFileDialog.cpp"
    "src/Arrow.cpp"
    "src/Controller.cpp"
    "src/GraphicObject.cpp"
    "src/gui.cpp"
    "src/main.cpp"
    "src/Match.cpp"
    "src/ModelLoader.cpp"
    "src/OffsideLine.cpp"
    "src/PerspectiveCamera.cpp"
    "src/Player.cpp"
    "src/Shader.cpp"
    "src/Team.cpp"
    "src/Videoloader.cpp"
    "src/View.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Resource_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Engine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_RELEASE "${PROJECT_NAME}"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION         "TRUE"
)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    MultiThreadedDLL
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/imgui;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src;"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glm-0.9.9.8;"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/opencv/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glfw/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/include"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "NDEBUG;"
    "_CONSOLE;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /permissive-;
        /Od;
        /Oi;
        /sdl;
        /Gy;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        /wd4996;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        /DEBUG;
        /SUBSYSTEM:CONSOLE;
        /OPT:REF;
        /LTCG;
        /OPT:ICF;
        /INCREMENTAL:NO
    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "glfw3;"
    "glfw3dll;"
    "glew32;"
    "assimp-vc142-mtd;"
    "opengl32;"
    "opencv_aruco450;"
    "opencv_bgsegm450;"
    "opencv_bioinspired450;"
    "opencv_calib3d450;"
    "opencv_ccalib450;"
    "opencv_core450;"
    "opencv_datasets450;"
    "opencv_dnn450;"
    "opencv_dnn_objdetect450;"
    "opencv_dnn_superres450;"
    "opencv_dpm450;"
    "opencv_face450;"
    "opencv_features2d450;"
    "opencv_flann450;"
    "opencv_fuzzy450;"
    "opencv_gapi450;"
    "opencv_hfs450;"
    "opencv_highgui450;"
    "opencv_imgcodecs450;"
    "opencv_imgproc450;"
    "opencv_img_hash450;"
    "opencv_intensity_transform450;"
    "opencv_line_descriptor450;"
    "opencv_mcc450;"
    "opencv_ml450;"
    "opencv_objdetect450;"
    "opencv_optflow450;"
    "opencv_phase_unwrapping450;"
    "opencv_photo450;"
    "opencv_plot450;"
    "opencv_quality450;"
    "opencv_rapid450;"
    "opencv_reg450;"
    "opencv_rgbd450;"
    "opencv_saliency450;"
    "opencv_shape450;"
    "opencv_stereo450;"
    "opencv_stitching450;"
    "opencv_structured_light450;"
    "opencv_superres450;"
    "opencv_surface_matching450;"
    "opencv_text450;"
    "opencv_tracking450;"
    "opencv_video450;"
    "opencv_videoio450;"
    "opencv_videostab450;"
    "opencv_xfeatures2d450;"
    "opencv_ximgproc450;"
    "opencv_xobjdetect450;"
    "opencv_xphoto450"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/graphics;"
    "${CMAKE_SOURCE_DIR}/Dependencies/assimp/lib;"
    "${CMAKE_SOURCE_DIR}/Dependencies/glew/lib/Release/x64;"
    "${CMAKE_SOURCE_DIR}/Dependencies/opencv/x64/vc16/lib;"
    "${CMAKE_SOURCE_DIR}/Dependencies/glfw/lib"
)

