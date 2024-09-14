from threading import Thread,Event
import queue 
import time 
import socket 



class RelayServer(Thread): 
    def __init__(self, host,port,IMU_queue, game_engine_queue): 
        Thread.__init__(self) 
        self.host = host 
        self.port = port 
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((self.host, self.port))
        self.IMU_queue = IMU_queue 
        self.game_engine_queue = game_engine_queue
        self.server.settimeout(1.0)
        self.stop_event = Event()

    def handleClient(self, client, address):
        try:
            while not self.stop_event.is_set():
                # recv length followed by '_' followed by message
                data = b''
                while not data.endswith(b'_'):
                    _d = client.recv(1)
                    if not _d:  # Client disconnected
                        print(f"Client {address} disconnected")
                        client.close()
                        return
                    data += _d
                if len(data) == 0:
                    print("No data")
                    break

                data = data.decode("utf-8")
                length = int(data[:-1])

                data = b''
                while len(data) < length:
                    _d = client.recv(length - len(data))
                    if not _d:  # Client disconnected
                        print(f"Client {address} disconnected")
                        client.close()
                        return
                    data += _d
                if len(data) == 0:
                    print("No data")
                    break

                msg = data.decode("utf-8")
                if length != len(data):
                    print("Packet length does not match, packet dropped")
                else:
                    print(f"Received '{msg}' from client {address}")
                    self.processMessage(msg)
        finally:
            client.close()  # Ensure the connection is closed after handling
    
    def processMessage(self,msg):
        print("RelayServer: Sending message to AI") 
        self.IMU_queue.put(msg)
        print("RelayServer: Sending message to game engine")
        print("-" * 30)
        self.game_engine_queue.put(msg)
        

    def run(self): 
        self.server.listen(1)
        # time.sleep(2)
        print(f"Relay Server is listening on {self.host}:{self.port}")
        try:
            while not self.stop_event.is_set():
                try:
                    client, address = self.server.accept()
                    print(f"Relay Client connected from {address}")
                    self.handleClient(client, address)
                except socket.timeout:
                    continue 
        finally:
            self.server.close()  # Ensure the server is closed when exiting
            
    def shutdown(self):
        """Gracefully shutdown the relay server."""
        self.stop_event.set()  # Set the stop event to stop the server loop
        self.server.close()  # Close the server socket
        print("Relay server shutdown initiated")
