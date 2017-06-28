classes=../Sensor.cpp ../Server.cpp ../HttpServer.cpp ../RESTManager.cpp ../Producer.cpp ../StoreManager.cpp ../MQTTClient.cpp ../gen-cpp/*
src=../main.cpp $(classes)
srcServer=../startServer.cpp $(classes)
srcClient=../startSensors.cpp $(classes)
srcStore=../startStore.cpp $(classes)
srcProducer=../startProducers.cpp $(classes)
srcTest=../tests/Test.cpp $(classes)
srcStressTest=../tests/StressTest.cpp $(classes)

compileCommand=g++ -std=c++11 -pthread -lthrift -lmosquitto

default:notest

all:runTest runStressTest notest

notest:server client serverClient store producer

run:serverClient
	./serverClient $(IP) $(PORT)

runServer:server
	./server $(IP) $(PORT)

runClient:client
	./client $(IP) $(PORT)

runStore:store
	./store

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

store:$(srcStore)
	$(compileCommand) $(srcStore) -o store

producer:$(srcProducer)
	$(compileCommand) $(srcProducer) -o producer

serverClient:$(src)
	$(compileCommand) $(src) -o serverClient

clean:
	rm -f server client store producer serverClient test stressTest res/*
