# Redis-liteProject Structure

redis-lite/

A lightweight, high-performance in-memory key-value store inspired by Redis, built from scratch in C++17. Redis-lite provides both a command-line interface and a multithreaded TCP server with thread-safe operations.├─ src/

│  ├─ main.cpp          # Starts server and CLI

[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)](https://isocpp.org/)│  ├─ server.cpp        # TCP server & threading

[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)](https://github.com/NikSterPrime/Redis-lite)│  ├─ server.h

[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)│  ├─ datastore.cpp     # Key-value logic

│  ├─ datastore.h

---│  ├─ persistence.cpp   # Snapshot/restore

│  ├─ persistence.h

## 🚀 Features│  ├─ client.cpp        # Optional CLI client

│  └─ client.h

### Core Functionality├─ include/             # For headers if needed

- **In-Memory Key-Value Storage**: Fast hash-based lookups using `std::unordered_map`├─ benchmarks/          # Performance testing scripts

- **Thread-Safe Operations**: Read-write lock implementation with `std::shared_mutex`├─ CMakeLists.txt       # Build system

  - Concurrent reads (shared locks)└─ README.md

  - Exclusive writes (unique locks)
- **Dual Operation Modes**:
  - Interactive CLI (REPL)
  - TCP Server (port 8080)

### Supported Commands
| Command | Syntax | Description |
|---------|--------|-------------|
| `SET` | `SET <key> <value>` | Store a key-value pair |
| `GET` | `GET <key>` | Retrieve value for a key |
| `DEL` | `DEL <key>` | Delete a key from the store |
| `HELP` | `HELP` | Display available commands |
| `EXIT` | `EXIT` | Exit CLI or close TCP connection |

### Network & Concurrency
- **TCP Server**: Line-based protocol (newline-delimited commands)
- **Thread-Per-Connection Model**: Each client gets a dedicated thread
- **Robust Stream Handling**:
  - Partial read buffering
  - CRLF/LF normalization
  - Multiple commands per connection
- **Graceful Session Management**: Clean socket shutdown on `EXIT` or disconnect

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                            │
│  ┌───────────────────┐          ┌──────────────────────┐   │
│  │   CLI Mode        │          │   Server Mode        │   │
│  │  (Interactive)    │          │  (--server flag)     │   │
│  └────────┬──────────┘          └──────────┬───────────┘   │
└───────────┼─────────────────────────────────┼───────────────┘
            │                                 │
            ▼                                 ▼
    ┌───────────────┐               ┌─────────────────┐
    │  datastore.h  │◄──────────────┤   server.cpp    │
    └───────┬───────┘               │  TCP Server     │
            │                       │  (port 8080)    │
            ▼                       └────────┬────────┘
    ┌───────────────────────┐               │
    │   datastore.cpp       │               │
    │ ┌─────────────────┐   │               │
    │ │ commandExecute()│◄──┼───────────────┘
    │ └────────┬────────┘   │       Per-connection threads
    │          │            │
    │  ┌───────▼────────┐   │
    │  │   Tokenizer    │   │
    │  └───────┬────────┘   │
    │          │            │
    │  ┌───────▼────────┐   │
    │  │   Dispatcher   │   │
    │  └───────┬────────┘   │
    │          │            │
    │  ┌───────▼────────────────────────┐
    │  │  Command Handlers              │
    │  │  ┌──────┐ ┌──────┐ ┌──────┐   │
    │  │  │ SET  │ │ GET  │ │ DEL  │   │
    │  │  └──┬───┘ └──┬───┘ └──┬───┘   │
    │  └─────┼────────┼────────┼────────┘
    │        │        │        │
    │  ┌─────▼────────▼────────▼────────┐
    │  │  std::unordered_map<string>    │
    │  │  Protected by shared_mutex     │
    │  └────────────────────────────────┘
    └───────────────────────────────────┘
```

### Threading Model

```
Server Thread (accept loop)
    │
    ├─► Client Thread 1 ──► recv() ──► lineBuffer ──► commandExecute() ──► [shared_mutex]
    │                                                                            │
    ├─► Client Thread 2 ──► recv() ──► lineBuffer ──► commandExecute() ─────────┤
    │                                                                            │
    └─► Client Thread N ──► recv() ──► lineBuffer ──► commandExecute() ─────────┘
                                                                                 │
                                                                                 ▼
                                                                    ┌────────────────────┐
                                                                    │   GET: shared_lock │
                                                                    │   SET: unique_lock │
                                                                    │   DEL: unique_lock │
                                                                    └────────────────────┘
```

**Key Design Decisions**:
- **Read-Write Locks**: `GET` operations use `shared_lock` for concurrent reads; `SET`/`DEL` use `unique_lock` for exclusive writes
- **Lock Granularity**: Locks held only during datastore access, never during I/O
- **Line Buffering**: Accumulates partial TCP packets until complete newline-delimited commands are received

---

## 📋 Prerequisites

### Compiler Requirements
- **C++17** compatible compiler
- **POSIX threads** support (for `std::shared_mutex`)

### Windows (Recommended)
```bash
# MSYS2 MinGW-w64 with POSIX threads
# Install from: https://www.msys2.org/

# After installing MSYS2:
pacman -Syu
pacman -S mingw-w64-x86_64-gcc

# Add to PATH:
C:\msys64\mingw64\bin
```

Verify thread model:
```powershell
g++ -v 2>&1 | Select-String "Thread model"
# Should output: Thread model: posix
```

### Linux
```bash
# Ubuntu/Debian
sudo apt install g++ make

# Fedora/RHEL
sudo dnf install gcc-c++ make
```

---

## 🔧 Building

### Quick Build (PowerShell/Linux)
```bash
g++ -std=c++17 -pthread -o redis \
    src/main.cpp \
    src/datastore.cpp \
    src/server.cpp \
    -I include \
    -lws2_32  # Windows only; omit on Linux
```

### Windows-Specific
```powershell
# Using MinGW-w64
g++ -std=c++17 -pthread -o redis.exe src/main.cpp src/datastore.cpp src/server.cpp -I include -lws2_32
```

### Linux-Specific
```bash
g++ -std=c++17 -pthread -o redis src/main.cpp src/datastore.cpp src/server.cpp -I include
```

### Verify Build
```bash
# Should show version and thread model
g++ --version
./redis --help  # (if help flag implemented)
```

---

## 🎯 Usage

### CLI Mode (Default)
```bash
./redis
```

**Interactive Session:**
```
Welcome to the Redis-lite, here you need to use the SET, GET and DEL commands
to enter values to the Redis store for faster and efficient retrieval.
To exit the program, type EXIT
>SET name Alice
Alice stored for name
>GET name
Alice
>DEL name
Key name deleted successfully.
>EXIT
Exiting the Redis-lite. Goodbye!
```

### Server Mode
```bash
./redis --server
```

**Output:**
```
Server started on port 8080
Waiting for incoming connections...
```

---

## 🌐 Network Protocol

### Connection
```bash
# Using telnet
telnet localhost 8080

# Using netcat
nc localhost 8080

# Using PowerShell
$client = New-Object System.Net.Sockets.TcpClient("localhost", 8080)
$stream = $client.GetStream()
$writer = New-Object System.IO.StreamWriter($stream)
$writer.AutoFlush = $true
```

### Command Format
- **Line-based protocol**: Commands delimited by `\n` or `\r\n`
- **Case-sensitive**: Commands must be uppercase (`SET`, not `set`)
- **Whitespace**: Single spaces separate command and arguments

**Example Session (telnet):**
```
SET user:1000 John
GET user:1000
DEL user:1000
EXIT
```

**Current Behavior:**
- Commands are executed but **responses are printed to server stdout** (not sent to client)
- Future enhancement: Send responses back over TCP

---

## 📊 Performance Characteristics

### Time Complexity
| Operation | Average Case | Worst Case |
|-----------|--------------|------------|
| SET | O(1) | O(n)* |
| GET | O(1) | O(n)* |
| DEL | O(1) | O(n)* |

*Hash collisions (rare with good hash function)

### Concurrency
- **Read Scalability**: Multiple `GET` operations execute in parallel (shared lock)
- **Write Serialization**: `SET`/`DEL` acquire exclusive lock (one at a time)
- **Lock Contention**: Minimal—locks held only during map access, not I/O

### Memory
- **Storage**: `O(n)` where n = number of keys
- **Overhead**: ~48-64 bytes per entry (key + value + hash table overhead on 64-bit systems)

---

## 🧪 Testing

### Manual CLI Test
```bash
echo -e "SET a 1\nGET a\nDEL a\nEXIT" | ./redis
```

### TCP Server Test (PowerShell)
```powershell
# Start server in one terminal
./redis --server

# In another terminal
$client = New-Object System.Net.Sockets.TcpClient("localhost", 8080)
$stream = $client.GetStream()
$writer = New-Object System.IO.StreamWriter($stream)
$writer.WriteLine("SET test 123")
$writer.WriteLine("GET test")
$writer.WriteLine("EXIT")
$writer.Flush()
$client.Close()
```

### Concurrent Clients Test
```bash
# Terminal 1
./redis --server

# Terminal 2-5 (run simultaneously)
for i in {1..100}; do echo "SET key$i value$i"; done | nc localhost 8080 &
```

---

## 🗂️ Project Structure

```
Redis-lite/
├── src/
│   ├── main.cpp           # Entry point, CLI/server mode switch
│   ├── datastore.cpp      # Core key-value logic + threading
│   ├── server.cpp         # TCP server, connection handling
│   └── client.cpp         # (Reserved for future CLI client)
│
├── include/
│   ├── datastore.h        # Command execution API
│   ├── server.h           # TCP server API
│   └── client.h           # (Reserved)
│
├── benchmarks/            # (Reserved for perf tests)
├── .vscode/
│   └── c_cpp_properties.json  # IntelliSense config
├── CMakeLists.txt         # (Reserved for CMake build)
└── README.md
```

---

## 🔮 Roadmap

### Immediate Enhancements
- [x] Thread-safe operations with `shared_mutex`
- [x] TCP server with per-connection threading
- [x] Line-based protocol with partial read handling
- [ ] **TCP Response Protocol**: Send command results back to clients
- [ ] **Graceful Shutdown**: SIGINT handler, thread cleanup
- [ ] **Logging**: Structured logs (timestamp, client IP, command)

### Data Features
- [ ] **Expiration (TTL)**: `SETEX key seconds value`, background cleanup
- [ ] **Persistence**: Snapshot to disk (RDB-style), AOF (append-only file)
- [ ] **Data Types**:
  - [ ] Lists: `LPUSH`, `RPUSH`, `LRANGE`
  - [ ] Sets: `SADD`, `SMEMBERS`, `SISMEMBER`
  - [ ] Sorted Sets: `ZADD`, `ZRANGE`
- [ ] **Atomic Operations**: `INCR`, `DECR`, `APPEND`
- [ ] **Batch Commands**: `MGET`, `MSET`

### Performance & Scalability
- [ ] **Connection Pooling**: Limit max concurrent connections
- [ ] **Thread Pool**: Replace thread-per-connection with worker pool
- [ ] **Asynchronous I/O**: `epoll` (Linux) / `IOCP` (Windows)
- [ ] **Memory Limits**: LRU eviction policy
- [ ] **Sharding**: Consistent hashing for distributed deployments

### Protocol & Compatibility
- [ ] **RESP Protocol**: Redis-compatible binary protocol
- [ ] **Pipelining**: Batch command execution
- [ ] **Pub/Sub**: `PUBLISH`, `SUBSCRIBE` channels
- [ ] **Transactions**: `MULTI`, `EXEC`, `DISCARD`

### Tooling
- [ ] **Benchmarking**: `redis-benchmark`-like tool
- [ ] **Monitoring**: Stats endpoint (connections, ops/sec, memory)
- [ ] **CLI Client**: Standalone `redis-cli` equivalent
- [ ] **Docker Support**: Containerized deployment
- [ ] **Unit Tests**: Google Test framework

---

## 🛠️ Troubleshooting

### IntelliSense Errors (VS Code)
**Problem**: `"identifier 'mutex' is undefined"`

**Solution**:
1. Ensure `.vscode/c_cpp_properties.json` points to correct compiler:
   ```json
   {
     "configurations": [{
       "name": "mingw64-posix",
       "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
       "cppStandard": "c++17",
       "intelliSenseMode": "gcc-x64"
     }]
   }
   ```
2. Run: `C/C++: Reset IntelliSense Database` (Ctrl+Shift+P)
3. Reload VS Code

### Compilation Errors

**`undefined reference to WSAStartup`**
```bash
# Solution: Link Winsock library
g++ ... -lws2_32
```

**`mutex does not name a type`**
```bash
# Check thread model (must be posix, not win32)
g++ -v 2>&1 | grep "Thread model"

# If win32, install MinGW-w64 with POSIX threads
```

### Runtime Issues

**Port 8080 already in use**
```bash
# Windows: Find process using port
netstat -ano | findstr :8080
taskkill /PID <pid> /F

# Linux
lsof -i :8080
kill -9 <pid>
```

**Server not accepting connections**
- Check firewall rules (allow port 8080)
- Verify server is running: `netstat -an | findstr 8080`

---

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** changes (`git commit -m 'Add amazing feature'`)
4. **Push** to branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Code Style
- Follow existing formatting (4-space indentation)
- Use meaningful variable names
- Add comments for complex logic
- Keep functions focused and small

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Inspired by [Redis](https://redis.io/) by Salvatore Sanfilippo
- C++17 standard library for threading primitives
- Winsock2 API for Windows networking

---

## 📞 Contact

**Author**: Aditya  
**GitHub**: [@NikSterPrime](https://github.com/NikSterPrime)  
**Repository**: [Redis-lite](https://github.com/NikSterPrime/Redis-lite)

---

## 📈 Project Statistics

- **Language**: C++17
- **Lines of Code**: ~350 (excluding comments/whitespace)
- **Dependencies**: STL only (no external libraries)
- **Platform**: Windows (primary), Linux (compatible)

---

**Built with ❤️ and modern C++**
