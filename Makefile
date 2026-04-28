CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2 -mwindows -DUNICODE -D_UNICODE
LDFLAGS = -static -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lkernel32 -lcomctl32

SRCS = main.cpp ime_core.cpp input_handler.cpp dictionary.cpp window_manager.cpp buffer_manager.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = ChineseStrokeIME.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /f /q *.o $(TARGET)
