cmake_minimum_required(VERSION 3.9)
project(RicochetRobots)

set(CMAKE_CXX_STANDARD 14)

add_library(ricochet src/Map.h src/MapTile.h src/OccupationData.h src/ReachabilityAnalysis.h src/Robot.h src/TileOccupation.h)
SET_TARGET_PROPERTIES(ricochet PROPERTIES LINKER_LANGUAGE CXX)

add_executable(rrobot src/RicochetRobots.cpp)
add_dependencies(rrobot ricochet)