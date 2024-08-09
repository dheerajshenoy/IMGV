cmake_minimum_required(VERSION 3.30)
project(imgv)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
qt_standard_project_setup()

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++20")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(LUA_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/libs/liblua.a")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

qt_add_executable(imgv
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

target_link_libraries(imgv PRIVATE
    "${LIB_DIR}/libwebp.a"
    "${LIB_DIR}/liblua.a"
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)
