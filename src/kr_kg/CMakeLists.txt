cmake_minimum_required(VERSION 3.16)

project(kr_kg VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets Sql)
find_package(OpenGL REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(kr_kg
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        point3d.h point3d.cpp
        edge.h edge.cpp
        section.h section.cpp
        segment.h segment.cpp
        tube.h tube.cpp
        tubeviewer.h tubeviewer.cpp
        grideditor.h grideditor.cpp
        sectionframe.h sectionframe.cpp
        sectionview.h sectionview.cpp
        deformationpoint.h deformationpoint.cpp
        deformationengine.h deformationengine.cpp
        databasemanager.h databasemanager.cpp
        tuberepository.h tuberepository.cpp
    )
else()
    if(ANDROID)
        add_library(kr_kg SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(kr_kg
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(kr_kg PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::Sql
    OpenGL::GL
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.kr_kg)
endif()

set_target_properties(kr_kg PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS kr_kg
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(kr_kg)
endif()
