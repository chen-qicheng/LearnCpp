###########################
#vsc : vsc_main.cpp vsc_func.cpp
#	g++ -o vsc vsc_func.cpp vsc_main.cpp

###########################
# CXX = g++
# TARGET = vsc
# OBJ = vsc_main.o vsc_func.o

# $(TARGET): $(OBJ)
# 	$(CXX) -o $(TARGET) $(OBJ)

# vsc_main.o :vsc_main.cpp
# 	$(CXX) -c vsc_main.cpp

# vsc_func.o :vsc_func.cpp
# 	$(CXX) -c vsc_func.cpp

###########################
# CXX = g++
# TARGET = vsc
# OBJ = vsc_main.o vsc_func.o

# CXXFLAGS = -c -Wall #-Wall: Warning all

# $(TARGET): $(OBJ)
# 	$(CXX) -o $(TARGET) $(OBJ)

# %.o :%.cpp
# 	$(CXX) $(CXXFLAGS) $< -o $@

###########################
CXX = g++
TARGET = main
SRC = $(wildcard *.cpp) #wildcard:通配符
OBJ = $(patsubst %.cpp, %.o, $(SRC)) #patsubst：模式转换

CXXFLAGS = -c -Wall #-Wall: Warning all

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ)

%.o :%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean #.PHONY代表伪目标，防止有余clean同名文件
clean:
	rm -f *.o $(TARGET)