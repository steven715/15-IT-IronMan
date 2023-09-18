# 第三方套件 websocketpp

[websocketpp](https://github.com/zaphoyd/websocketpp/tree/master)是一個C++的websocket協議的函式庫。

## 環境

這次是使用websocket，所以我們的Container要打開port，這邊我就重啟一個新的Container，也是有針對運行中的Container開port的方法，但不是重點，所以這邊就不使用了。

```shell
docker run -v D:/dvt:/home -p 9002:9002 --name=cpp_dev -itd cpp_dev
```

## 安裝

安裝的部分一樣交給`vcpkg`

```shell
vcpkg install websocketpp
```

理論上，應該會遇到以下的錯誤

```shell
Can't locate IPC/Cmd.pm in @INC (@INC contains: /opt/rh/devtoolset-7/root//usr/lib64/perl5/vendor_perl /opt/rh/devtoolset-7/root/usr/lib/perl5 /opt/rh/devtoolset-7/root//usr/share/perl5/vendor_perl /usr/local/lib64/perl5 /usr/local/share/perl5 /usr/lib64/perl5/vendor_perl /usr/share/perl5/vendor_perl /usr/lib64/perl5 /usr/share/perl5 .) at -e line 1.
BEGIN failed--compilation aborted at -e line 1.
CMake Error at ports/openssl/unix/portfile.cmake:27 (message):
  

  Perl cannot find IPC::Cmd.  Please install it through your system package
  manager.
```

這時候我們把該補上的東西補上即可，然後再安裝一次

```shell
yum -y install perl-IPC-Cmd
vcpkg install websocketpp
```

安裝成功就會是以下信息

```shell
websocketpp provides CMake targets:

    # this is heuristically generated, and may not be correct
    find_package(websocketpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE websocketpp::websocketpp)
```

## 使用

這邊的範例代碼，我就直接拿官方的[Examples](https://github.com/zaphoyd/websocketpp/tree/master/examples)來用，一個簡單的`echo_server`

```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

int main() {
    // Create a server endpoint
    server echo_server;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));

        // Listen on port 9002
        echo_server.listen(9002);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}
```

然後再補上`CMakeLists.txt`跟`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake`

```cmake
cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

set(ServerSRC
    server.cpp
)

find_package(websocketpp CONFIG REQUIRED)

add_executable(server ${ServerSRC})

set(LIB
    pthread
)

target_link_libraries(server PRIVATE websocketpp::websocketpp ${LIB})
```

編譯好了之後就是實際demo了，這邊`client`的部分，改用`python`的套件[websocket-client](https://github.com/websocket-client/websocket-client)，從本機上(我是Windows)去連docker上的`server`，結果就如下囉

```shell
# 先啟動server
cd build
make
./server 
# client 連到後
[2023-09-18 14:14:20] [connect] WebSocket Connection [::ffff:172.17.0.1]:59576 v13 "" / 101
on_message called with hdl: 0x266f5d0 and message: Hello, World
[2023-09-18 14:14:20] [frame_header] Dispatching write containing 1 message(s) containing 2 header bytes and 12 payload bytes
[2023-09-18 14:14:20] [frame_header] Header Bytes: 
[0] (2) 81 0C 

[2023-09-18 14:14:20] [control] Control frame received with opcode 8
[2023-09-18 14:14:20] [frame_header] Dispatching write containing 1 message(s) containing 2 header bytes and 2 payload bytes
[2023-09-18 14:14:20] [frame_header] Header Bytes: 
[0] (2) 88 02 

[2023-09-18 14:14:20] [disconnect] Disconnect close local:[1000] remote:[1000]
```

```python
# websocket_client.py，透過pip3 install websocket-client安裝套件
from websocket import create_connection

ws = create_connection("ws://localhost:9002")
print("Sending 'Hello, World'...")
ws.send("Hello, World")
print("Sent")
print("Receiving...")
result =  ws.recv()
print("Received '%s'" % result)
ws.close()
```

```shell
python .\websocket_client.py
# output
Sending 'Hello, World'...
Sent
Receiving...
Received 'Hello, World'
```

最後本篇文章所有代碼的部分都可以從我的[Github](https://github.com/steven715/15-IT-IronMan/tree/master/Day11/cmaketest)找到唷

## 參考

[websocketpp - github](https://github.com/zaphoyd/websocketpp/tree/master)
