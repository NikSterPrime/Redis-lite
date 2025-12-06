import socket
import threading

def spam():
    s = socket.socket()
    s.connect(("127.0.0.1",8080))
    for _ in range(200):
        s.sendall(b"SET x 1\n")
        s.recv(1024)       
    s.close()

threads = [threading.Thread(target = spam) for _ in range (50)]
for t in threads: t.start()
for t in threads: t.join()
