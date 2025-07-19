import socket

host = "localhost"
port = 8080

request = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "6\r\n"
    "Hello!\r\n"
    "7\r\n"
    "World!!\r\n"
    "3\r\n"
    "123\r\n"
    "0\r\n"
    "\r\n"
)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
s.sendall(request.encode())

response = b""
while True:
    data = s.recv(4096)
    if not data:
        break
    response += data

print(response.decode())
s.close()
