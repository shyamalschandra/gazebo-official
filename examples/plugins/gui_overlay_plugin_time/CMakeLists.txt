cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(Qt4 REQUIRED)
find_package(Gazebo REQUIRED)

include (${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

include_directories(SYSTEM ${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})

QT4_WRAP_CPP(headers_MOC GUIExampleTimeWidget.hh)

add_library(example SHARED ${headers_MOC}
  GUIExampleTimeWidget.cc)
target_link_libraries(example ${GAZEBO_LIBRARIES} ${QT_LIBRARIES})
