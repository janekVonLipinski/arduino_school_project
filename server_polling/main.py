import socket
import json

print("hallo")
socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect(('192.168.172.19', 80))

def mysend():
    socket.sendall(b'GET / HTTP/1.1\r\nHost: 192.168.172.19\r\n\r\n')


def myreceive():
    receivedMessage = socket.recv(2048)
    return receivedMessage.decode("utf-8")

def poll_server():
    mysend()
    receivedMessage = myreceive()
    splitMessage = receivedMessage.split("{")
    endOfMessage = "{"+splitMessage[1]
    gasSensorVoltageValues = json.loads(endOfMessage)
    return gasSensorVoltageValues



print(poll_server())
