This is the simplest SDL / NGL demo using a basic SDL2.0 window and core profile OpenGL context

Note that this .pro file contains the calls to add the output of sdl2-config to the build using

# this demo uses SDL so add the paths using the sdl2-config tool
QMAKE_CXXFLAGS+=$$system(sdl2-config  --cflags)
message(output from sdl2-config --cflags added to CXXFLAGS= $$QMAKE_CXXFLAGS)

LIBS+=$$system(sdl2-config  --libs)
message(output from sdl2-config --libs added to LIB=$$LIBS)

This is different from the Qt versions of the .pro file
