# 第三方套件 faltbuffers

[FlatBuffers](https://github.com/google/flatbuffers)是一種跨平台資料序列化且最大記憶體效率的函式庫。

## 環境

環境就老樣子，用前一天的`Dockerfile`，然後專案也可以把Day1的`cmaketest`資料夾拿過來用。

```shell
docker run -v D:/dvt:/home --name=cpp_dev --tty cpp_dev
```

## 安裝

安裝方面一樣交給`vcpkg`。

```shell
vcpkg install flatbuffers
# 安裝完成信息
flatbuffers provides CMake targets:

    # this is heuristically generated, and may not be correct
    find_package(flatbuffers CONFIG REQUIRED)
    target_link_libraries(main PRIVATE flatbuffers::flatbuffers)
```

## 使用

那這邊還需要先準備flatbuffer schema檔案(`.fbs`)，定義要序列化資料格式以及可以支援不同的語言，然後使用`flatc`這個工具去將`.fbs`生成對應語言的檔案。

範例的`.fbs`檔案一樣就用官方提供的`monster.fbs`。

```fbs
// Example IDL file for our monster's schema.

namespace MyGame.Sample;

enum Color:byte { Red = 0, Green, Blue = 2 }

union Equipment { Weapon } // Optionally add more tables.

struct Vec3 {
  x:float;
  y:float;
  z:float;
}

table Monster {
  pos:Vec3;
  mana:short = 150;
  hp:short = 100;
  name:string;
  friendly:bool = false (deprecated);
  inventory:[ubyte];
  color:Color = Blue;
  weapons:[Weapon];
  equipped:Equipment;
  path:[Vec3];
}

table Weapon {
  name:string;
  damage:short;
}

root_type Monster;
```

使用以下指令去生成需要的檔案。

```shell
# 用vcpkg安裝的話，flatc應該會在以下目錄，這邊就直接放環境變數下使用
ln -s /opt/vcpkg/installed/x64-linux/tools/flatbuffers/flatc /usr/local/bin/flatc
flatc --cpp monster.fbs # output monster_generated.h
```

代碼的部分，一樣使用官方的Example，[sample_binary.cpp](https://github.com/google/flatbuffers/blob/master/samples/sample_binary.cpp#L24-L56)，這邊就只展示`CMakeLists.txt`。

`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake`，將專案設定好，然後編譯起來。

```cmake
cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

include_directories(${PROJECT_SOURCE_DIR})

set(SRC
    smain.cpp
)

find_package(flatbuffers CONFIG REQUIRED)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE flatbuffers::flatbuffers)
```

一切就緒後，就來到編譯並看結果的部分，Example中我有加上讓序列化的資料存成檔案，之後有機會再來展示透過網路傳輸的部分。

```shell
cd build
make
./cmaketest
```

## 結語

今年2023的鐵人賽，我意外的中箭落馬了，被自己射的箭，這周實在太累，竟然在Day5，明明已經在晚上11點出頭就寫好文章，但最後只是儲存文章，卻忘了發表，導致今天一早就看到已經失去資格了，真是大意失荊州呀!

不過我還是會按表操課，持續將剩餘的天數照時程補完，雖然IT鐵人賽我已經輸了，但我自己的鐵人賽還再繼續!
