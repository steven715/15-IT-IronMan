# 15-IT-IronMan：C++ 開發之路 30 天

一個為期 30 天的 IT 鐵人賽系列文章，涵蓋現代 C++11/14 開發工具、第三方套件整合、資料結構與演算法、資源管理及併發程式設計等主題，搭配完整程式碼範例與實作。

## 技術棧

- **語言**：C++ (C++11/14/17)
- **建置系統**：CMake
- **編譯器**：GCC 7.3.1 (devtoolset-7)
- **套件管理**：vcpkg
- **容器化**：Docker / Docker Compose
- **編輯器**：Visual Studio Code + Dev Containers

## 目錄

### 開發工具與環境建置（Day 1–3）

| 天數 | 主題 |
|------|------|
| [Day 1](Day1/C%2B%2B開發工具介紹.md) | C++ 開發工具介紹 — VS Code、Docker、CMake 基礎 |
| [Day 2](Day2/C%2B%2B開發工具介紹2.md) | C++ 開發工具介紹 2 — CMake + vcpkg + 除錯 |
| [Day 3](Day3/代碼分析工具.md) | 代碼分析工具 — Doxygen、Graphviz |

### 第三方套件整合（Day 4–12）

| 天數 | 主題 |
|------|------|
| [Day 4](Day4/第三方套件-google-benchmark.md) | Google Benchmark — 效能測試 |
| [Day 5](Day5/第三方套件_cppjson.md) | jsoncpp & ajson — JSON 處理 |
| [Day 6](Day6/第三方套件_flatbuffers.md) | FlatBuffers — 序列化 |
| [Day 7](Day7/第三方套件_libuuid_curl.md) | libuuid & curl — UUID 產生與 HTTP 請求 |
| [Day 8](Day8/第三方套件_hiredis.md) | hiredis — Redis 客戶端 |
| [Day 9](Day9/第三方套件_mongocxx.md) | mongocxx — MongoDB C++ 驅動 |
| [Day 10](Day10/第三方套件_librdkafka.md) | librdkafka — Apache Kafka 客戶端 |
| [Day 11](Day11/第三方套件-websocketpp.md) | websocketpp — WebSocket 通訊 |
| [Day 12](Day12/第三方套件_libuv.md) | libuv — 事件迴圈 |

### 輔助函式與排序演算法（Day 13–18）

| 天數 | 主題 |
|------|------|
| [Day 13](Day13/輔助函式_chrono.md) | chrono — 時間日期處理 |
| [Day 14](Day14/輔助函式_crashlog.md) | Crash Log — 當機日誌工具 |
| [Day 15](Day15/輔助函式_容器自定義排序.md) | 容器自定義排序 |
| [Day 16](Day16/排序演算法.md) | 排序演算法 — 氣泡排序、插入排序 |
| [Day 17](Day17/排序演算法2.md) | 排序演算法 2 — 合併排序、快速排序 |
| [Day 18](Day18/排序演算法3.md) | 排序演算法 3 — 堆積排序 |

### 資源管理（Day 19–21）

| 天數 | 主題 |
|------|------|
| [Day 19](Day19/資源管理_智慧指針.md) | 智慧指標 — shared_ptr、unique_ptr、weak_ptr |
| [Day 20](Day20/資源管理_右值.md) | 右值參考與移動語意 |
| [Day 21](Day21/資源管理_內存洩漏.md) | 記憶體洩漏偵測 |

### 併發程式設計（Day 22–30）

| 天數 | 主題 |
|------|------|
| [Day 22](Day22/併發相關_mutex.md) | Mutex — 互斥鎖 |
| [Day 23](Day23/併發相關_讀寫鎖.md) | 讀寫鎖 — shared_mutex |
| [Day 24](Day24/併發相關_非同步.md) | 非同步 — async、promise、shared_future |
| [Day 25](Day25/併發相關_條件變數.md) | 條件變數 — condition_variable |
| [Day 26](Day26/併發相關_原子操作.md) | 原子操作 — atomic |
| [Day 27](Day27/併發相關_semaphore.md) | Semaphore — 信號量 |
| [Day 28](Day28/併發相關_thread_pool.md) | Thread Pool — 執行緒池 |
| [Day 29](Day29/併發相關_lock_free.md) | Lock-Free — 無鎖程式設計 |
| [Day 30](Day30/併發相關_各類鎖的比較.md) | 各類鎖的比較與使用場景 |

## 快速開始

```bash
# 使用 Docker 建立開發環境（參考 Day1）
docker build -t cpp-dev Day1/

# 編譯任一天的範例程式
cd Day4/cmaketest
mkdir build && cd build
cmake ..
make
```

## 專案結構

```
DayN/
├── *.md              # 教學文章
├── cmaketest/        # C++ 程式碼範例
│   ├── CMakeLists.txt
│   └── *.cpp
├── Dockerfile        # Docker 環境設定（部分天數）
└── *.JPG             # 截圖與說明圖片
```

## 授權

本專案為 IT 鐵人賽教學系列內容。
