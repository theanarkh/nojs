build:
	g++ -g -Ideps/v8/include -Ideps/libuv/include -Ideps/llhttp/build No.cc core/*.cc -o No -lv8_monolith -luv -lllhttp -Llib -pthread -std=c++17 -DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX