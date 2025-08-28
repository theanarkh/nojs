build: # -g
	g++ -fno-rtti -Ideps/v8/include -Ideps/libuv/include -Ideps/llhttp/build -Ideps/c-ares/include No.cc core/*.cc -o No -lv8_monolith -luv -lllhttp -lcares -Llib -pthread -std=c++17 -DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX
build-debug:
	g++ -fno-rtti -Ideps/v8/include -Ideps/libuv/include -Ideps/llhttp/build -Ideps/c-ares/include No.cc core/*.cc -o No -lv8_monolith -luv -lllhttp -lcares -Ldeps/v8/out.gn/arm64.debug/obj -Llib -lpthread -std=c++17 -DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX