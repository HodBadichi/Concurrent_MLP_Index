CXX := g++
FLAGS := -O3 -std=c++14 -march=native -pthread -isystem -mavx2 -g

# Uncomment the following line to enable huge pages usage
#FLAGS += -DUSE_HUGE_PAGES

FLAGS += -DNDEBUG
#SRCS := $(wildcard *.cpp)
#OBJS := $(SRCS)
#OBJS := $(subst .cpp,.o,$(OBJS))

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))


INC_DIRS := .
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA
INC := $(addprefix -I,$(INC_DIRS))

TARGET := main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(FLAGS) $(INC) $(OBJS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(FLAGS) $(INC) -c $< -o $@

clean:
	rm -f *.o $(TARGET)