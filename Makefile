# List of source files
SOURCES = main.cpp game.cpp menu.cpp
OBJECTS = $(SOURCES:.cpp=.o) resource.o
TARGET = main.exe

# Compiler and flags
CXX = g++
CXXFLAGS = -I include
LDFLAGS = -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32 -lwinmm

# Default target: Build and run
all: $(TARGET)
	@echo Running the game...
	@./$(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compile resource file
resource.o: resource.rc
	windres $< -o $@

# Clean
clean:
	del /Q *.o *.exe 2>nul || true
