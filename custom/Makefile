classes=../Sensor.cpp ../Server.cpp
src=../main.cpp $(classes)
srcServer=../startServer.cpp $(classes)
srcClient=../startSensors.cpp $(classes)
srcTest=../tests/Test.cpp $(classes)

all:runTest server client serverClient

notest:server client serverClient

run:serverClient
	./serverClient

runServer:server
	./server

runClient:client
	./client

runTest:test
	./test

test:$(srcTest)
	g++ -std=c++11 -pthread $(srcTest) -o test

server:$(srcServer)
	g++ -std=c++11 -pthread $(srcServer) -o server

client:$(srcClient)
	g++ -std=c++11 -pthread $(srcClient) -o client

serverClient:$(src)
	g++ -std=c++11 -pthread $(src) -o serverClient

clean:
	rm -f server client serverClient test
