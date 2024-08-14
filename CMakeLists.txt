cmake_minimum_required(VERSION 3.28)
project(imgv)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(WebP REQUIRED)
find_package(Lua REQUIRED)
qt_standard_project_setup()

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++20")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

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
    src/PixAnalyser.cpp
    src/Minimap.cpp
    src/MinimapRect.cpp

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
    src/PixAnalyser.hpp
    src/Minimap.hpp
    src/MinimapRect.hpp
)

qt_add_resources(${PROJECT_NAME}
    PREFIX "resources"
    FILES
        resources/images/logo.svg
        resources/fonts/ShureTechMono.ttf
)

qt_add_plugin(qtiff
    STATIC
    MANUAL_FINALIZATION
    PLUGIN_TYPE imageformats
)

# qt_add_plugin(${PROJECT_NAME} STATIC ImageFormats MANUAL_FINALIZATION)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${CMAKE_SOURCE_DIR}/lib/liblua.a
    ${CMAKE_SOURCE_DIR}/lib/libwebp.a)


target_include_directories(${PROJECT_NAME} PRIVATE 
    ${WebP_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR})

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)

#Install .desktop file
install(FILES ${CMAKE_SOURCE_DIR}/imgv.desktop DESTINATION share/applications)

# Install icons
install(DIRECTORY ${CMAKE_SOURCE_DIR}/icons/ DESTINATION share/icons/hicolor)
