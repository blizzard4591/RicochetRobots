cmake_minimum_required(VERSION 3.9)
project(RicochetRobots)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/json")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/l3pp")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/tap/include")

set(ricochet_files
	src/BarrierType.h 
	src/Color.h
	src/Defines.h
	src/GoalType.h
	src/Map.h 
	src/MapBuilder.h 
	src/MapBuilder.cpp
	src/MapTile.h 
	src/ObstacleType.h
	src/OccupationData.h 
	src/OccupationData.cpp 
	src/Position.h
	src/ReachabilityAnalysis.h 
	src/RelativeWallLocation.h
	src/Robot.h 
	src/TileOccupation.h
)

add_library(ricochet ${ricochet_files})
SET_TARGET_PROPERTIES(ricochet PROPERTIES LINKER_LANGUAGE CXX)

add_executable(rrobot src/RicochetRobots.cpp)
add_dependencies(rrobot ricochet)
target_link_libraries(rrobot ricochet)
