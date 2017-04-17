#*******************************************************
#
# Newton game dynamics 
# copy right by Julio Jerez 2003 - 2005 
#
#*******************************************************
#
# SDK Sample tutorial: 
#
#*******************************************************  


# ******************************************************
#
# project sorce files
#
# ******************************************************
#SDK_TUTORIAL_NAME = tutorial_01_GettingStarted

SDK_NEWTON = ../../sdk
SDK_COMMON = ../common
SDK_TOOLBOX = ../toolBox


SDK_INCLUDE_PATH = -I $(SDK_COMMON) -I $(SDK_TOOLBOX) -I $(SDK_NEWTON) -I ../gl
SDK_DEMO_SRC = \
	tutorial.cpp 

SDK_COMMON_SRC = \
	$(SDK_COMMON)/GraphicSystem.cpp \
	$(SDK_COMMON)/OpenGlUtil.cpp \
	$(SDK_COMMON)/RenderPrimitive.cpp \
	$(SDK_COMMON)/HiResTimer.cpp  

SDK_TOOL_BOX = \
	$(SDK_TOOLBOX)/dCRC.cpp \
	$(SDK_TOOLBOX)/dTree.cpp \
	$(SDK_TOOLBOX)/dSceneNode.cpp \
	$(SDK_TOOLBOX)/dChunkParsel.cpp \
	$(SDK_TOOLBOX)/dBaseHierarchy.cpp 


SDK_SRCS = $(SDK_DEMO_SRC) $(SDK_COMMON_SRC) $(SDK_TOOL_BOX)

SDK_OBJ_FILES = $(SDK_SRCS:.cpp=.o)

COMPILER = gcc
FLAGS = $(SDK_FLAGS) $(SDK_INCLUDE_PATH)

.SUFFIXES : .o .cpp
.cpp.o :
	$(COMPILER) $(FLAGS) -o $@ $<	

# main target
$(SDK_TUTORIAL_NAME) : $(SDK_OBJ_FILES)
	gcc $? $(SDK_NEWTON)/libNewton.a $(LIBS) -o $@
	cp $@ ../bin
	

# clean all objects target
clean :
	rm -f *.o



	
