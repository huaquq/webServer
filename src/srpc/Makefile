CXX = g++
CXXFLAGS = -std=c++11 -Wall
SRC = ../
SRC1 = ../token.cc ../configuration.cc ../mylogger.cc
SRC2 = myconsul.cc user.pb.cc 
SRC3 = signup.cc
SRC4 = signin.cc
OBJ1 = token.o configuration.o mylogger.o
OBJ2 = myconsul.o user.pb.o
OBJ3 = signup.o 
OBJ4 = signin.o 
OBJ5 = userinfo.o
TARGET3 = signup
TARGET4 = signin
TARGET5 = userinfo
LIBS = -lsrpc -lprotobuf -lworkflow -lcrypt -lppconsul -lcurl -lpthread -llog4cpp -ll8w8jwt

all: $(TARGET3) $(TARGET4)

$(TARGET3): $(OBJ1) $(OBJ2) $(OBJ3)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
$(TARGET4): $(OBJ1) $(OBJ2) $(OBJ4)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
$(TARGET5): $(OBJ1) $(OBJ2) $(OBJ5)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o:$(SRC)%.cc
	$(CXX) $(CXXFLAGS) -c $<
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET3) $(TARGET4) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)

