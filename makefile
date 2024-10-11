# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Libraries
LIBS = -lmysqlcppconn -lhiredis -lboost_system -lbcrypt

# Include directories
INCLUDES = -I/usr/include/mysql -I/usr/local/include

# Source files
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp

# Output files
SERVER_OUT = server
CLIENT_OUT = client

# Default target
all: $(SERVER_OUT) $(CLIENT_OUT)

# Compile server
$(SERVER_OUT): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile client
$(CLIENT_OUT): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Clean up
clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT)

# Phony targets
.PHONY: all clean
