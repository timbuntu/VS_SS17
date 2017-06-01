classes=../Sensor.cpp ../Server.cpp ../HttpServer.cpp ../RESTManager.cpp ../gen-cpp/*
src=../main.cpp $(classes)
srcServer=../startServer.cpp $(classes)
srcClient=../startSensors.cpp $(classes)
srcTest=../tests/Test.cpp $(classes)
srcStressTest=../tests/StressTest.cpp $(classes)

compileCommand=g++ -std=c++11 -pthread -lthrift

all:runTest server client serverClient

notest:server client serverClient

run:serverClient
	./serverClient $(IP) $(PORT)

runServer:server
	./server $(IP) $(PORT)

runClient:client
	./client $(IP) $(PORT)

runTest:test
	./test $(IP) $(PORT)

runStressTest:stressTest
	./stressTest $(IP) $(PORT)

test:$(srcTest)
	$(compileCommand) $(srcTest) -o test

stressTest:$(srcStressTest)
	$(compileCommand) $(srcStressTest) -o stressTest

server:$(srcServer)
	$(compileCommand) $(srcServer) -o server

client:$(srcClient)
	$(compileCommand) $(srcClient) -o client

serverClient:$(src)
	$(compileCommand) $(src) -o serverClient

clean:
	rm -f server client serverClient test stressTest res/*
