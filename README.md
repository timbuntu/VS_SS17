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
store: build stores
producer: build producers
serverClient: build an application containing all servers and clients
clean: remove build-files

Configuration file can be found in res/config relative to execution path

Configuration for netbeans:
Exclude startServer.cpp and startSensors.cpp from build, 
Compiler C++ Standard C++11, 
Additional compiler options -pthread
