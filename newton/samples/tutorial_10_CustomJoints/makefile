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
#SDK_TUTORIAL_NAME = 

SDK_NEWTON = ../../sdk
SDK_COMMON = ../common
SDK_TOOLBOX = ../toolBox


SDK_INCLUDE_PATH = -I $(SDK_COMMON) -I $(SDK_TOOLBOX) \
		   -I $(SDK_NEWTON) -I $(SDK_NEWTON)/customJoints -I ../gl
SDK_DEMO_SRC = \
	tutorial.cpp \
	Materials.cpp \
	JointsTutorial.cpp \
	CharaterControl.cpp

SDK_CUSTOM_JOINTS = \
	$(SDK_NEWTON)/customJoints/CustomBallAndSocket.cpp \
	$(SDK_NEWTON)/customJoints/CustomSlider.cpp \
	$(SDK_NEWTON)/customJoints/CustomConeLimitedBallAndSocket.cpp \
	$(SDK_NEWTON)/customJoints/CustomUniversal.cpp \
	$(SDK_NEWTON)/customJoints/CustomCorkScrew.cpp \
	$(SDK_NEWTON)/customJoints/CustomUpVector.cpp \
	$(SDK_NEWTON)/customJoints/CustomHinge.cpp \
	$(SDK_NEWTON)/customJoints/DryRollingFriction.cpp \
	$(SDK_NEWTON)/customJoints/CustomRayCastCar.cpp \
	$(SDK_NEWTON)/customJoints/NewtonCustomJoint.cpp


SDK_COMMON_SRC = \
	$(SDK_COMMON)/GraphicSystem.cpp \
	$(SDK_COMMON)/OpenGlUtil.cpp \
	$(SDK_COMMON)/LevelPrimitive.cpp \
	$(SDK_COMMON)/RenderPrimitive.cpp \
	$(SDK_COMMON)/HiResTimer.cpp  

SDK_TOOL_BOX = \
	$(SDK_TOOLBOX)/dCRC.cpp \
	$(SDK_TOOLBOX)/dTree.cpp \
	$(SDK_TOOLBOX)/dSceneNode.cpp \
	$(SDK_TOOLBOX)/dChunkParsel.cpp \
	$(SDK_TOOLBOX)/dBaseHierarchy.cpp 


SDK_SRCS = $(SDK_DEMO_SRC) $(SDK_COMMON_SRC) $(SDK_TOOL_BOX) $(SDK_CUSTOM_JOINTS)

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



	
