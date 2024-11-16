set -e
# # get depot_tools
# mkdir -p deps && cd deps
# if [ ! -d "depot_tools" ]; then
#     git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
# fi
# export PATH=$PATH:`pwd`/depot_tools

# # get and compile v8
# gclient config https://github.com/v8/v8.git
# gclient sync

# compile v8 
cd deps/v8 && tools/dev/v8gen.py x64.release.sample 
../depot_tools/ninja -C out.gn/x64.release.sample v8_monolith && cd ../..

# compile llhttp 
cd deps/llhttp && npm i && make generate && make all && cd ../..

# compile libuv 
cd deps/libuv && sh autogen.sh && ./configure && make && cd ../..

# compile c-ares 
cd deps/c-ares && autoreconf -fi && ./configure && make && cd ../..

# copy static library to the lib dir
mkdir -p lib
cp -n deps/v8/out.gn/x64.release.sample/obj/libv8_monolith.a lib/libv8_monolith.a 
cp -n deps/llhttp/build/libllhttp.a lib/libllhttp.a 
cp -n deps/libuv/.libs/libuv.a lib/libuv.a
cp -n deps/c-ares/src/lib/.libs/libcares.a lib/libcares.a 

# compile and run the demo
make && ./No test/demo/index.js
