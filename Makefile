SOURCES = main.cpp

OBJECTS = $(SOURCES:.cpp=.o)

INCLUDE = include/
LIB = lib/

CXXFLAGS = -I $(INCLUDE)
LDFLAGS = -L $(LIB) -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32 -lwinmm 

TARGET = main.exe

default: $(TARGET)
	@./$(TARGET)

$(TARGET): $(OBJECTS)
	@g++ $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	@g++ -c $< -o $@ $(CXXFLAGS)

clean:
	@del $(OBJECTS) $(TARGET)
