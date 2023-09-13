# 第三方套件 websocketpp

[websocketpp](https://github.com/zaphoyd/websocketpp/tree/master)是一個C++的websocket協議的函式庫。

## 環境

這次是使用Socket，所以我們的Container要打開port，這邊我就重啟一個新的Container，也是有針對運行中的Container開port的方法，但不是重點，所以這邊就不使用了。

```shell
docker run -v D:/dvt:/home -p 9002:9002 --name=cpp_dev --tty cpp_dev
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

安裝完成後一樣執行`CMake`去設定專案的建置

```shell
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake
```

這邊的範例代碼，我就直接拿官方的[Examples](https://github.com/zaphoyd/websocketpp/tree/master/examples)來用



## 參考

[websocketpp - github](https://github.com/zaphoyd/websocketpp/tree/master)
