CC=g++
LD=ld
AR=ar

#export CC LD AR

DEVDIR=$(shell pwd)/..
INCDIR=$(DEVDIR)/../

CFG_INC=-I$(INCDIR)

CFG_OPT= -fPIC -Wall -g

#export CFG_INC CFG_OPT

LIBDIR=$(DEVDIR)/../lib
SLIB=$(LIBDIR)/libCobraCore.a
DLIB=$(LIBDIR)/libCobraCore.so

TARGET= $(SLIB) $(DLIB)

SRC=Connection.cc \
	ConnectionList.cc \
	NetworkThread.cc \
	OneWorkThreadProxy.cc \
	PortManager.cc \
	Protocol.cc \
	RequestContext.cc \
	RequestHandleThread.cc \
	ServerNetContext.cc \
	CallbackThread.cc \
	ClientNetContext.cc \
	ClientNetworkThread.cc \
	ServerProxy.cc \
	Message.cc \
	ClientConnection.cc \
	RouteManager.cc

OBJ=$(SRC:%.cc=%.o)

all:$(TARGET)

$(DLIB):$(OBJ)
	$(CC) -shared -Wall -O2 -o $@ $(OBJ)

$(SLIB):$(OBJ)
	$(AR) -r $@ $(OBJ)

$(OBJ):%.o:%.cc
	$(CC) -c $(CFG_OPT) $^ -o $@ $(CFG_INC)

clean:
	$(RM) $(OBJ) $(SLIB) $(DLIB)
