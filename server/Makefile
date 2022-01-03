OBJS = ./out/main.o ./out/MyTime.o ./out/MySQLConnector.o ./out/UserService.o ./out/Online.o ./out/DataEncoder.o ./out/HeadData.o ./out/DataProcesser.o ./out/base64.o ./out/sha256.o ./out/rsacrypto.o ./out/aescrypto.o
main: $(OBJS)
	g++ -std=c++11 $(OBJS) -o main `mysql_config --cflags --libs` -ljsoncpp -I/usr/local/include/jsoncpp -L/usr/local/lib64 -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto
./out/main.o: main.cpp ./ProtocolHead/HeadData.h ./Service/DataProcesser.h ./Service/UserService.h ./Service/Online.h ./config/server_config.h ./crypto/base64.h ./crypto/sha256.h ./crypto/rsacrypto.h ./crypto/aescrypto.h
	g++ -std=c++11 -c main.cpp -o ./out/main.o -I/usr/local/include/jsoncpp -L/usr/local/lib64 -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto
./out/DataProcesser.o: ./Service/DataProcesser.cpp ./ProtocolHead/protocolmsg.h ./ProtocolHead/HeadData.h ./ProtocolHead/DataEncoder.h ./Util/MyTime.h
	g++ -std=c++11 -c ./Service/DataProcesser.cpp -o ./out/DataProcesser.o -I/usr/local/include/jsoncpp -L/usr/local/lib64
./out/HeadData.o: ./ProtocolHead/HeadData.cpp ./ProtocolHead/protocolmsg.h
	g++ -std=c++11 -c ./ProtocolHead/HeadData.cpp -o ./out/HeadData.o -I/usr/local/include/jsoncpp -L/usr/local/lib64
./out/DataEncoder.o: ./ProtocolHead/DataEncoder.cpp ./ProtocolHead/protocolmsg.h
	g++ -std=c++11 -c ./ProtocolHead/DataEncoder.cpp -o ./out/DataEncoder.o -I/usr/local/include/jsoncpp -L/usr/local/lib64
./out/Online.o: ./Service/Online.cpp ./Util/MyTime.h
	g++ -std=c++11 -c ./Service/Online.cpp -o ./out/Online.o -I/usr/local/include/jsoncpp -L/usr/local/lib64
./out/UserService.o: ./Service/UserService.cpp ./Dao/MySQLConnector.h
	g++ -std=c++11 -c ./Service/UserService.cpp -o ./out/UserService.o -I/usr/local/include/jsoncpp -L/usr/local/lib64
./out/MySQLConnector.o: ./Dao/MySQLConnector.cpp ./Util/MyTime.h ./config/mysql_config.h
	g++ -std=c++11 -c ./Dao/MySQLConnector.cpp -o ./out/MySQLConnector.o -I/usr/local/include/jsoncpp -L/usr/local/lib64
./out/MyTime.o: ./Util/MyTime.cpp
	g++ -std=c++11 -c ./Util/MyTime.cpp -o ./out/MyTime.o -I/usr/local/include/jsoncpp -L/usr/local/lib64
./out/base64.o: ./crypto/base64.cpp
	g++ -std=c++11 -c ./crypto/base64.cpp -o ./out/base64.o -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto
./out/sha256.o: ./crypto/sha256.cpp
	g++ -std=c++11 -c ./crypto/sha256.cpp -o ./out/sha256.o -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto
./out/rsacrypto.o: ./crypto/rsacrypto.cpp ./crypto/base64.h
	g++ -std=c++11 -c ./crypto/rsacrypto.cpp -o ./out/rsacrypto.o -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto
./out/aescrypto.o: ./crypto/aescrypto.cpp ./crypto/base64.h
	g++ -std=c++11 -c ./crypto/aescrypto.cpp -o ./out/aescrypto.o -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto
	
clean:
	rm -rf ./out/*.o main


