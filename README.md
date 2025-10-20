Project Structure
redis-lite/
├─ src/
│  ├─ main.cpp          # Starts server and CLI
│  ├─ server.cpp        # TCP server & threading
│  ├─ server.h
│  ├─ datastore.cpp     # Key-value logic
│  ├─ datastore.h
│  ├─ persistence.cpp   # Snapshot/restore
│  ├─ persistence.h
│  ├─ client.cpp        # Optional CLI client
│  └─ client.h
├─ include/             # For headers if needed
├─ benchmarks/          # Performance testing scripts
├─ CMakeLists.txt       # Build system
└─ README.md
