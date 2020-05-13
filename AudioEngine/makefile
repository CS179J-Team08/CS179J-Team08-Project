
NAME = audio_test
PARSE = parse_test
EXE = cloud_sound

ifndef CPU
    $(error Specify CPU=[x86|x86_64|arm|armhf])
endif
ifndef CONFIG
    $(error Specify CONFIG=[Debug|Release])
endif

ifeq (${CPU}, armhf)
    FLAGS += -marm -march=armv6 -mfpu=vfp -mfloat-abi=hard
else ifeq (${CPU}, arm)
    FLAGS += -marm -march=armv6 -mfpu=vfp -mfloat-abi=softfp
else ifeq (${CPU}, x86)
    FLAGS += -m32
else
    override CPU = x86_64
    FLAGS += -m64
endif

ifeq (${CONFIG}, Debug)
    FLAGS += -g 
    SUFFIX = L
else
    override CONFIG = Release
    FLAGS += -O2
    SUFFIX =
endif

SOURCE_FILES = \
    FMOD_tests.cpp \
    AudioEngine.cpp \

PACKET_FILES = \
    Packet_Parser_tests.cpp \
    PacketParser.cpp \
    AudioEngine.cpp \

BUILD_FILES = \
    s3_playback.cpp \
    AudioEngine.cpp \
    PacketParser.cpp \

INCLUDE_DIRS = \
    -I fmodstudioapi20100linux/api/core/inc

LOWLEVEL_LIB = fmodstudioapi20100linux/api/core/lib/${CPU}/libfmod${SUFFIX}.so

test:
	g++ -std=c++11 -pthread ${FLAGS} -o ${NAME} ${SOURCE_FILES} -Wl,-rpath=\$$ORIGIN/$(dir ${LOWLEVEL_LIB}) ${LOWLEVEL_LIB} ${INCLUDE_DIRS}
	#g++ -std=c++11 -pthread ${FLAGS} -o ${PARSE} ${PACKET_FILES} -Wl,-rpath=\$$ORIGIN/$(dir ${LOWLEVEL_LIB}) ${LOWLEVEL_LIB} ${INCLUDE_DIRS}

build:
	g++ -std=c++11 -pthread ${FLAGS} -o ${EXE} ${BUILD_FILES} -Wl,-rpath=\$$ORIGIN/$(dir ${LOWLEVEL_LIB}) ${LOWLEVEL_LIB} ${INCLUDE_DIRS}
