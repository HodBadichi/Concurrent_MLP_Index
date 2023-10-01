FLAGS := -O3 -std=c++14 -march=native -isystem -pthread -mavx2 -g

# Uncomment the following line to enable huge pages usage
#FLAGS += -DUSE_HUGE_PAGES

FLAGS += -DNDEBUG
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS)
OBJS := $(subst .cpp,.o,$(OBJS))

INC_DIRS := .
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA
INC := $(addprefix -I,$(INC_DIRS))

all: $(OBJS)
	g++ $(FLAGS) $(OBJS) $(INC) -o main


%.o : %.cpp
	g++ $(FLAGS) $(INC) -c $< -o $@

clean:
	rm *.o main
