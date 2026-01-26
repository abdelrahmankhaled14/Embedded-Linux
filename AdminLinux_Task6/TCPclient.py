import socket

HEADER = 64
PORT = 12345
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "!DISCONNECT"
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER, PORT)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(ADDR)


def send(msg):
    message = msg.encode(FORMAT)
    msg_length = len(message)
    send_length = str(msg_length).encode(FORMAT)
    send_length += b' ' * (HEADER - len(send_length))
    client.send(send_length)
    client.send(message)
    response = client.recv(2048).decode(FORMAT)
    print(f"[SERVER RESPONSE] {response}")


print("Connected to calculator server. Type expressions like '5 + 3' or '!DISCONNECT' to exit.\n")

while True:
    expression = input("Enter calculation: ")
    if expression == DISCONNECT_MESSAGE:
        send(DISCONNECT_MESSAGE)
        break
    send(expression)
