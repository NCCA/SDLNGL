cmake_minimum_required(VERSION 2.8.11)
# Name of the project
project(SDLNGL)
#Bring the headers into the project (local ones)
include_directories(include $ENV{HOME}/NGL/include)

#the file(GLOB...) allows for wildcard additions of our src dir
set(SOURCES src/main.cpp  src/NGLDraw.cpp  
			include/NGLDraw.h  

)

# see what platform we are on and set platform defines
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	add_definitions(-DDARWIN)

elif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-DLINUX)
endif()



# now add NGL specific values
set ( PROJECT_LINK_LIBS -lNGL -lGL)
link_directories( ~/NGL/lib )
include_directories(~/NGL/include/)
# as NGL uses Qt we need to define this flag
add_definitions(-DQT5BUILD)

exec_program(sdl2-config ARGS --cflags OUTPUT_VARIABLE SDL2INCLUDE)
SET( CMAKE_CXX_FLAGS ${SDL2INCLUDE})
message("sdl2-config --cflags gives " =${SDL2INCLUDE})

exec_program(sdl2-config ARGS --libs OUTPUT_VARIABLE SDL2_LIBRARIES)
message("sdl2-config --libs gives " =${SDL2_LIBRARIES})

# NGL also needs the OpenGL framework from Qt so add it
find_package(Qt5OpenGL)



# add exe and link libs this must be after the other defines
add_executable(SDLNGL ${SOURCES})

target_link_libraries(SDLNGL ${PROJECT_LINK_LIBS} Qt5::OpenGL ${SDL2_LIBRARIES}   )

