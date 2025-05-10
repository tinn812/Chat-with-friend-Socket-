# Chat-with-friend-Socket-
- Server 能夠同時接受多個客戶端連線，作為client間溝通的橋樑，每個 client 傳出的訊息會「廣播」到所有其他 client 之多人聊天室
- client 連線後馬上輸入使用者名稱開始聊天

檔名後綴代表適用作業系統，請用成對的 client-Server

下載執行檔即可試用

---
目前版本聊天設定說明：

### **你當 Server：**
- 你編譯並啟動 server 端 `.exe`。
- 你知道自己的 **IP 位址（內網或外網）**。
- 朋友用你提供的 IP 當作 `client.exe` 的連線目標（預設是 `127.0.0.1`，要改成你的 IP）。

## ❗還需要做：

###  1. **確認你的電腦允許網路連線**
- **防火牆可能會擋住 server 執行檔的監聽行為**，請允許執行檔通過防火牆。

###  2. **如果你們不在同一區網，需要設定「Port Forwarding」**
- 你要在家用路由器裡**設定把外部的 port（如 12345）轉發給你內部的電腦**。
- 這樣朋友才能用外部 IP 加 port 來連線你（例如：`123.123.123.123:12345`）。

###  3. **Client 程式要改 IP**
現在 client.cpp 預設是：

```cpp
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
```

這表示只能連到「自己」，要讓朋友連你，**要把這行 IP 換成你的 IP（或寫成能輸入的方式）**。

###  改成可以輸入 IP：

```cpp
std::string server_ip;
std::cout << "Enter server IP address: ";
std::getline(std::cin, server_ip);
inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
```
這樣都可以自己指定要連誰了。


###  4. **注意位元版本與 WinSock 依賴**
- 如果朋友是 Windows，用 Visual Studio 或 g++ cross-compile 出來的 `.exe` 可直接執行。
- 建議用 **`static linking`** 把必要的 DLL 靜態連進 `.exe`，或提供必要的 DLL（如 `ws2_32.dll` 是系統自帶）。


## 安全提醒
- 開放 port 有一定風險，建議只給朋友玩，並設定強密碼或使用 NAT 防護。
- 若要開發可商用或公開版本，使用 **TLS/SSL 加密、驗證機制、與現代框架（如 WebSocket）**。
(待開發)

---
筆記
## Socket 基本概念
Socket 程式是用來實現「網路通訊」的程式，它讓兩台電腦透過網路互相傳送資料。可以把它想像成在兩台電腦之間建立「電話線」，透過這條線雙方可以傳話（傳送資料）。

通常會有兩端：
  - **Server（伺服器端）**：等待連線的那一方。
  - **Client（客戶端）**：主動發起連線的那一方。


### 🧱 基本流程（以 TCP 為例）

#### Server端：
1. 建立 socket（插座）
2. 綁定 IP 和 Port（就像設定電話號碼）
3. 開始監聽（等待來電）
4. 接受連線
5. 傳送／接收資料
6. 關閉連線

#### Client端：
1. 建立 socket
2. 連線到 Server 的 IP 和 Port
3. 傳送／接收資料
4. 關閉連線


### 常見應用
- 聊天室、即時遊戲
- 網頁伺服器（像 HTTP server）
- FTP、Email 傳輸
- 資料交換

## Threading

聊天室需要「**同時處理輸入與輸出**」：

- **Server 端**要能同時接收多個 client 的訊息
- **Client 端**要能同時「發送訊息」與「接收訊息」

需要：
- 主執行緒處理發送（或主邏輯）
- 背景執行緒持續監聽對方發過來的訊息
