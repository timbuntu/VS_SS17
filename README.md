# VS_SS17

Build in the custom folder using make.
Targets:
all: (default target) run the tests and build all files
notest: build all files without running tests
run: build and run server and client in the same application
runServer: build and run the server
runClient: build and run the client
runTest: build and run the tests
test: build the tests
server: build the server
client: build the client
serverClient: build an application containing server and client
clean: remove build-files

Configuration for netbeans:
Exclude startServer.cpp and startSensors.cpp from build, 
Compiler C++ Standard C++11, 
Additional compiler options -pthread
