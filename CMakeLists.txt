cmake_minimum_required(VERSION 3.29)
project(imgv)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(WebP REQUIRED)
find_package(Lua REQUIRED)
qt_standard_project_setup()

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++20")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
# set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Installation Directory")

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
    src/ImageWidget.cpp
    src/imgv.cpp
    src/StatusBar.cpp
    src/Thumbnail.cpp
    src/AboutDialog.cpp
    src/utils.cpp
    src/exif.cpp
    src/ThumbnailView.cpp
    src/ThumbnailModel.cpp
    src/NoteWidget.cpp
    src/ImagePropertiesDialog.cpp
    src/ManageSessionsDialog.cpp
    src/ImageWidget.hpp
    src/imgv.hpp
    src/StatusBar.hpp
    src/Thumbnail.hpp
    src/sol/sol.hpp
    src/AboutDialog.hpp
    src/MovieItem.hpp
    src/utils.hpp
    src/exif.hpp
    src/ThumbnailView.hpp
    src/ThumbnailModel.hpp
    src/ThumbnailFilterProxy.hpp
    src/NoteWidget.hpp
    src/ImagePropertiesDialog.hpp
    src/ManageSessionsDialog.hpp
    src/qmarkdowntextedit/markdownhighlighter.cpp
    src/qmarkdowntextedit/qmarkdowntextedit.cpp
    src/qmarkdowntextedit/qownlanguagedata.cpp
    src/qmarkdowntextedit/qplaintexteditsearchwidget.cpp
    src/qmarkdowntextedit/linenumberarea.h
    src/qmarkdowntextedit/markdownhighlighter.h
    src/qmarkdowntextedit/qmarkdowntextedit.h
    src/qmarkdowntextedit/qownlanguagedata.h
    src/qmarkdowntextedit/qplaintexteditsearchwidget.h
    src/sol/sol.hpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    webp
    lua)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${WebP_INCLUDE_DIRS}
    ${Lua_INCLUDE_DIRS})

# install(TARGETS ${PROJECT_NAME}
#         RUNTIME DESTINATION bin)
