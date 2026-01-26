import socket
import threading

HEADER = 64
PORT = 12345
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "!DISCONNECT"
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER, PORT)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(ADDR)


def calculate(expression):
    try:
        # Split input safely: expected format "number operator number"
        parts = expression.split()
        if len(parts) != 3:
            return "Invalid format. Use: <num1> <operator> <num2>"

        num1, op, num2 = parts
        num1, num2 = float(num1), float(num2)

        if op == '+':
            return str(num1 + num2)
        elif op == '-':
            return str(num1 - num2)
        elif op == '*':
            return str(num1 * num2)
        elif op == '/':
            if num2 == 0:
                return "Error: Division by zero"
            return str(num1 / num2)
        else:
            return "Unsupported operator"
    except Exception as e:
        return f"Error: {e}"


def handle_client(conn, addr):
    print(f"[NEW CONNECTION] {addr} connected.")
    connected = True
    while connected:
        msg_length = conn.recv(HEADER).decode(FORMAT)
        if msg_length:
            msg_length = int(msg_length)
            msg = conn.recv(msg_length).decode(FORMAT)

            if msg == DISCONNECT_MESSAGE:
                connected = False
                break

            print(f"[CLIENT {addr}] {msg}")
            result = calculate(msg)
            conn.send(result.encode(FORMAT))

    conn.close()
    print(f"[DISCONNECTED] {addr} closed connection.")


def start():
    server.listen()
    print(f"[LISTENING] Server is listening on {SERVER}:{PORT}")
    while True:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(conn, addr))
        thread.start()
        print(f"[ACTIVE CONNECTIONS] {threading.active_count() - 1}")


print("[SERVER STARTING] Calculator server is starting...")
start()
