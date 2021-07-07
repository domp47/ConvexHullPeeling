cmake_minimum_required(VERSION 3.19)
project(kMeansPeel)

set(CMAKE_CXX_STANDARD 14)

set(project_sources
        main.cpp
        glImage.cpp
        glPixel.cpp
        coordinate.cpp
        kMeans.cpp
        convexHull.cpp
        )

set(headers
        glImage.h
        glPixel.h
        coordinate.h
        kMeans.h
        convexHull.h
        )

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(kMeansPeel ${project_sources})
target_link_libraries(kMeansPeel pthread OpenGL::GL GLUT::GLUT)