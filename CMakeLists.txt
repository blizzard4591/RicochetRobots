cmake_minimum_required(VERSION 3.9)
project(RicochetRobots)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/json")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/l3pp")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/tap/include")

add_definitions(/D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

set(ricochet_files
	src/BarrierType.h 
	src/Color.h
	src/Defines.h
    src/Direction.h
    src/GoalType.h
	src/Map.h 
	src/Map.cpp 
	src/MapBuilder.h 
	src/MapBuilder.cpp
	src/MapTile.h 
	src/ObstacleType.h
	src/OccupationData.h 
	src/OccupationData.cpp 
	src/Position.h
	src/ReachabilityAnalysis.h 
	src/Robot.h
	src/TileOccupation.h
        src/MoveSequence.cpp src/MoveSequence.h src/Game.cpp src/Game.h src/Goal.h)

add_library(ricochet ${ricochet_files})
SET_TARGET_PROPERTIES(ricochet PROPERTIES LINKER_LANGUAGE CXX)

add_executable(rrobot src/RicochetRobots.cpp)
add_dependencies(rrobot ricochet)
target_link_libraries(rrobot ricochet)

add_executable(unicodetest src/UnicodeTest.cpp)
