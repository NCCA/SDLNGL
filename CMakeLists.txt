cmake_minimum_required(VERSION 2.8.11)
# Name of the project
set(PROJECT_NAME SDLNGL)
project(${PROJECT_NAME})
#Bring the headers into the project (local ones)
include_directories(include $ENV{HOME}/NGL/include)

#the file(GLOB...) allows for wildcard additions of our src dir
set(SOURCES src/main.cpp  src/NGLDraw.cpp include/NGLDraw.h  )

# see what platform we are on and set platform defines
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	add_definitions(-DDARWIN)
	find_library(MACGL OpenGL)
	set ( PROJECT_LINK_LIBS -lNGL ${MACGL})

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-DLINUX)
	set ( PROJECT_LINK_LIBS -lNGL -lGL)

endif()

# now add NGL specific values
link_directories( ~/NGL/lib )
include_directories(~/NGL/include/)
# as NGL uses Qt we need to define this flag
add_definitions(-DQT5BUILD)
# As there are no proper cmake packages for SDL2 yet we use the sdl2-config script instead
# this is a good example of how to run a program / script in cmake
# exec_program exe arguments and where we place the output
exec_program(sdl2-config ARGS --cflags OUTPUT_VARIABLE SDL2INCLUDE)
# once we have the output for the include path we set this to the CXX
SET( CMAKE_CXX_FLAGS ${SDL2INCLUDE})
message("sdl2-config --cflags gives " =${SDL2INCLUDE})

exec_program(sdl2-config ARGS --libs OUTPUT_VARIABLE SDL2_LIBRARIES)
message("sdl2-config --libs gives " =${SDL2_LIBRARIES})

# NGL also needs the OpenGL framework from Qt so add it
find_package(Qt5OpenGL)

# add exe and link libs this must be after the other defines
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBS} Qt5::OpenGL ${SDL2_LIBRARIES}   )

