COBRAPATH:=/data/home/julianyang/myproject/cobra

INCLUDE += -I$(COBRAPATH)

LIB += -L$(COBRAPATH)/lib -lbase -lCobraUtil -lCobraCore

LIB += -lpthread
CC_FLAG += -g

all: server client

server : CobraServer.o
	g++ -o CobraServer CobraServer.o $(LIB)

CobraServer.o : CobraServer.cc
	g++ -c CobraServer.cc $(CC_FLAG) -o CobraServer.o $(INCLUDE)

client : CobraClient.o
	g++ -o CobraClient CobraClient.o $(LIB)

CobraClient.o : CobraClient.cc
	g++ -c CobraClient.cc $(CC_FLAG) -o CobraClient.o $(INCLUDE)

clean:
	rm CobraServer.o CobraServer CobraClient.o CobraClient
