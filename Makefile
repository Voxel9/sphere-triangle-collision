TARGET	    = sphere-triangle-collision

BUILD       = bin
SRC_DIR     = src

SOURCES     = $(wildcard src/*.cpp)

OFILES      = $(patsubst $(SRC_DIR)/%, $(BUILD)/%, $(SOURCES:.cpp=.o))

RESFILES    = res/icon.res

FLAGS       = -O3 -Wall -std=c++11 -static -DGLEW_STATIC

LIBS        = -lglfw3 -lglew32 -lglu32 -lopengl32 -lgdi32

INCLUDES    = -I./src


all: $(TARGET)

$(TARGET): $(OFILES)
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) -o $(BUILD)/$(TARGET) $(OFILES) $(RESFILES) $(LIBS)

$(BUILD)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDES)

clean:
	@echo clean...
	@rm -fr $(BUILD)
