# This specifies the exe name
TARGET=SDLNGL
# where to put the .o files
OBJECTS_DIR=obj
# core Qt Libs to use add more here if needed.
QT+=gui opengl core

# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
	cache()
	DEFINES +=QT5BUILD
}
!win32:{
# this demo uses SDL so add the paths using the sdl2-config tool
QMAKE_CXXFLAGS+=$$system(sdl2-config  --cflags)
QMAKE_CXXFLAGS+=$$system(pkg-config  --cflags sdl2)

message(output from sdl2-config --cflags added to CXXFLAGS= $$QMAKE_CXXFLAGS)

LIBS+=$$system(sdl2-config  --libs)
LIBS+=$$system(pkg-config  --libs sdl2)
LIBS+=-ldl
message(output from sdl2-config --libs added to LIB=$$LIBS)
}
macx:LIBS+= $$system(sdl2-config --static-libs)
win32 :{
    message(Make sure that SDL2 is installed using vcpkg install SDL2 )

        contains(QT_ARCH, i386) {
            message("32-bit build")
			INCLUDEPATH += $$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\include\SDL2
			PRE_TARGETDEPS+=$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\SDL2.lib
			LIBS+=-L$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\ -lSDL2

        } 
        else {
            message("64-bit build")
			INCLUDEPATH += $$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x64-windows\include\SDL2
			PRE_TARGETDEPS+=$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x64-windows\lib\SDL2.lib
			LIBS+=-L$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x64-windows\lib\ -lSDL2
        }


}

# where to put moc auto generated file
MOC_DIR=moc
# on a mac we don't create a .app bundle file ( for ease of multiplatform use)
CONFIG-=app_bundle
# Auto include all .cpp files in the project src directory (can specifiy individually if required)
SOURCES+= $$PWD/src/*.cpp
# same for the .h files
HEADERS+= $$PWD/include/*.h
# and add the include dir into the search path for Qt and make
INCLUDEPATH +=./include
# where our exe is going to live (root of project)
DESTDIR=./
# add the glsl shader files
OTHER_FILES+= shaders/*.glsl \
							README.md
# were are going to default to a console app
CONFIG += console
# note each command you add needs a ; as it will be run as a single line
# first check if we are shadow building or not easiest way is to check out against current
!equals(PWD, $${OUT_PWD}){
	copydata.commands = echo "creating destination dirs" ;
	# now make a dir
	copydata.commands += mkdir -p $$OUT_PWD/shaders ;
	copydata.commands += echo "copying files" ;
	# then copy the files
	copydata.commands += $(COPY_DIR) $$PWD/shaders/* $$OUT_PWD/shaders/ ;
	# now make sure the first target is built before copy
	first.depends = $(first) copydata
	export(first.depends)
	export(copydata.commands)
	# now add it as an extra target
	QMAKE_EXTRA_TARGETS += first copydata
}
NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
        !win32:message("including $HOME/NGL")
        !win32:include($(HOME)/NGL/UseNGL.pri)
        win32:include($(HOMEDRIVE)\$(HOMEPATH)\NGL\UseNGL.pri)
}
else{ # note brace must be here
	message("Using custom NGL location")
	include($(NGLDIR)/UseNGL.pri)
}

