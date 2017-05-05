classes=../Sensor.cpp ../Server.cpp
src=../main.cpp $(classes)
srcServer=../startServer.cpp $(classes)
srcClient=../startSensors.cpp $(classes)
srcTest=../tests/Test.cpp $(classes)
srcStressTest=../tests/StressTest.cpp $(classes)

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
	g++ -std=c++11 -pthread $(srcTest) -o test

stressTest:$(srcStressTest)
	g++ -std=c++11 -pthread $(srcStressTest) -o stressTest

server:$(srcServer)
	g++ -std=c++11 -pthread $(srcServer) -o server

client:$(srcClient)
	g++ -std=c++11 -pthread $(srcClient) -o client

serverClient:$(src)
	g++ -std=c++11 -pthread $(src) -o serverClient

clean:
	rm -f server client serverClient test stressTest
