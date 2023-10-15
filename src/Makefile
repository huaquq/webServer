INCLUDES:=./
SRC = srpc/
SRC1 = srpc/user.pb.cc
OBJ1 = user.pb.cc
SRCS:=$(wildcard *.cc)
CXXFLAGS:= -g
LIBS:= -lwfrest -lsrpc -lprotobuf -lworkflow  -lssl -lcrypto -lcrypt -lSimpleAmqpClient -lpthread -llog4cpp -ll8w8jwt
ALIBABALIBS:= -fno-rtti -lalibabacloud-oss-cpp-sdk -lcurl -lpthread
OBJS:=$(patsubst %.cc, %.o, $(SRCS))
SERVER:=wfrest_server
CONSUMER=consumer

$(SERVER): main.o hash.o token.o configuration.o amqp.o gateway.o mylogger.o user.pb.o
	g++ $^ -o $@ $(CXXFLAGS)  $(LIBS)   $(addprefix -I, $(INCLUDES))


$(CONSUMER): consumer.cc oss.cc amqp.cc
	g++ $^ -o $@ $(CXXFLAGS) $(ALIBABALIBS) $(LIBS)   

%.o:%.cc
	g++ -c $^ -o $@ $(CXXFLAGS) $(LIBS)  $(addprefix -I, $(INCLUDES))
%.o:$(SRC)%.cc
	g++ -c $^ -o $@ $(CXXFLAGS) $(LIBS)  $(addprefix -I, $(INCLUDES))

clean:
	rm -rf $(OBJS) $(SERVER) $(CONSUMER)

