# Day 1 C++開發工具介紹

第一天是工具介紹，主要是分享一下目前工作上常用的工具，也會是將來30天使用的工具

## Visual Studio Code

第一個介紹的是之後用來寫C++的工具`VS Code`，相比於`Visual Studio`，個人還是更熟悉VS Code的一些快捷鍵跟使用，而VS更多的時候，會被我拿來當一個快速測試小專案，新開一個專案，做簡單驗證某些函數的使用。

### 快捷鍵分享

- 單行註解：`ctrl + /`
- 範圍註解：`alt + shit + A`
- 返回上一個程式段：`alt + left`
- 返回下一個程式段：`alt + right`
- 複製該行文字：`shift + alt + down`
- 整行文字刪除：`shift + delete`
- 移動該行文字上下：`alt + up(down)`
- 打開terminal：`ctrl + ~`
- 多行模式：`ctrl + alt + down`
- 查詢文字：`ctrl + f`
- 取代文字：`ctrl + h`，使用正則表達式可以更有效率

```shell
# 判斷數字
^[0-9]*$

# 刪除空白行
^\s*(?=\r?$)\n

# 刪除帶有關鍵字的行
.*(word).*

# 不包含關鍵字的行
^((?!test).)*$
```

### Extensions

再來是分享一下`VS Code`上面常用的套件，若是有人有不錯的套件要分享，也歡迎唷~

- `C/C++ Extension Pack`：C/C++算必備工具，有提供語法支援、路徑解析、主題顏色等功能
- `CMake`、`CMake Tools`：CMake在VS Code上的套件，透過VS Code可以用CMake去編譯C++的程式，下面會單獨介紹CMake
- `Bookmarks`：標記特別行數，並可以加上註解
- `Todo Tree`：標記未來需要做的事情，可以透過GUI快速檢視
- `GitLens`：比較Git之間不同版本的差異、檢查某段程式碼的Git版本
- `Docker`：這次開發會以Linux系統來做開發，所以也會使用docker這工具來建置開發環境
- `Dev Containers`：可以在docker container裡面使用VS Code，但要注意有些套件會需要重新安裝
- `Remote - Explorer`：該工具會結合`Dev Containers`跟`Remote - SSH`，去連到docker container裡面
- `Remote - SSH`：透過VS Code去ssh到其他的機器上開發

## Docker

Docker是個開源的容器生成軟體，容器跟虛擬機有一點差距，這邊就不做贅述了。

使用Docker目的是用來快速建一個Linux系統(`CentOS 7`)當作開發環境，選擇`CentOS 7`系統的原因是工作上主要是以這個系統為主，而且也是免費開源的。

```shel
# 測試的Centos 7
docker run -v D:/dvt:/home --name=centos7 --tty centos:7 
```

### Dockerfile

單單建置一個容器是無法用來開發C++的，還需要安裝很多工具才能做到，這些工具在每次建容器的時候都要重裝會太麻煩，所以用`Dockerfile`，簡化每次安裝的過程。

```docker
FROM centos:7

RUN yum -y update
RUN yum -y install gcc
RUN yum -y install vim

# CentOS devtoolset
RUN yum -y install centos-release-scl
RUN yum -y install devtoolset-7
RUN scl enable devtoolset-7 bash
RUN echo 'source scl_source enable devtoolset-7' >> ~/.bashrc

# CMake Insatll
RUN cd /usr/local/src
RUN curl -LO https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.tar.gz
RUN tar -xvf cmake-3.22.2-linux-x86_64.tar.gz
RUN mv cmake-3.22.2-linux-x86_64 /usr/local/cmake
RUN echo 'export PATH="/usr/local/cmake/bin:$PATH"' >> ~/.bashrc
RUN source ~/.bashrc

# git update version
RUN yum -y remove git
RUN yum -y install https://packages.endpointdev.com/rhel/7/os/x86_64/endpoint-repo.x86_64.rpm
RUN yum -y install git
```

[RedHat Dev Toolset Components](https://access.redhat.com/documentation/zh-tw/red_hat_developer_toolset/11/html-single/user_guide/index)是一個工具集合，裡面有GCC, GDB, Valgrind, Make等開發用的工具。

Dockerfile檔案有了之後，就可以透過該檔案建所要的容器環境，然後啟動起來。

```shell
docker build -t cpp_dev .
docker run -v D:/dvt:/home --name=cpp_dev --tty cpp_dev 
```

### VS Code Remote Explorer

在上面透過dockerfile建好開發環境後，就能利用vs code套件的remote explorer連到開發環境上面了。

## CMake

CMake是一個跨平台的編譯輔助軟體，一般用C++的編譯器`g++`，會需要很長的指令來編譯，但透過CMake提供的語法，能用參數化的形式簡化指令。

CMake的核心是`CMakeLists.txt`，這個檔案能讓使用者搭配CMake語法，建置執行檔或lib，以下是簡單範本。

```CMake
cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_COMPILER "/opt/rh/devtoolset-7/root/usr/bin/g++") # 指定編譯器的路徑
set(CMAKE_CXX_FLAGS "-std=c++14") # 給編譯器的參數

# CMake中的變數使用
set(SRC
    smain.cpp
)

add_executable(${PROJECT_NAME} ${SRC}) # 建置執行檔
```

`CMakeLists.txt`可以當作是Visual Studio裡面專案檔的存在，定義相關依賴檔案、建置參數等設定，所以用CMake都會搭配類似下面的檔案目錄，src是放源代碼的地方，build是放編譯出來的檔案。

```file
- 專案根目錄
    - build
    - src
    - CMakeLists.txt
```

依照上面目錄架構，再用剛剛的範本建立一個用CMake編譯的執行檔，相關指令如下

```shell
cd 專案根目錄
mkdir build
cmake ..
make
```

這樣就能看到建置出來的執行檔在build的目錄下了

## End of Day 1

第一天就先介紹到CMake的最簡單版本，明天會接著補充CMake其他語法、以及如何偵錯，再來是分享如何搭配`vcpkg`(套件管理工具)，展示使用第三方套件的場景。

最後，以上內容(Dockerfile、CMakeLists專案)也可以從我的[Github](https://github.com/steven715/15-IT-IronMan)上去取得唷!
