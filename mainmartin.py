import random
import queue

import time
from threading import Thread, Event

import smbus


ARDUINO_1_ADDRESS = 0x04 # I2C Address of Arduino 1 
ARDUINO_2_ADDRESS = 0x05 # I2C Address of Arduino 2 

I2Cbus = smbus.SMBus(1) 

mapeo_posiciones = {
    0: 22,
    1: 24,
    2: 26,
    3: 28,
    4: 30,
    5: 32,
    6: 34,
    7: 36,
    8: 38,
}


def _receive_data(self):
    while 1:
        try:
            data = I2Cbus.read_i2c_block_data(self.address, 32)
            if data == 0:
                break
            """nd = []
            for b in data:
                nd.append(chr(b))
            self.received_queue.put(map(chr, data))
            """
            self.received_queue.put(''.join(map(chr, filter(lambda x: x!=255, data))))
        except Exception as e:
            print(e)
        time.sleep(.3)
        if self.stop_event.is_set():
            """self.stop_event.unset()
            """
            break
    


class ArduinoController:
    def __init__(self, address, conn_qty, *args, **kwargs):
        self.address = address
        self.conn_qty = conn_qty
        self.posiciones = []
        self.received_queue = queue.Queue()
        self.stop_event = Event()

    def ConvertStringToBytes(self, src): 
        converted = [] 
        for b in src: 
            converted.append(ord(b)) 
        return converted

    def receive(self, timeout=100):
        time.sleep(.2)
        receive_thread = Thread(target=_receive_data, args=[self])
        receive_thread.start()
        receive_thread.join(timeout=timeout/100)
        self.stop_event.set()
        try:
            """
            res = ''.join(map(str, self.received_queue.queue))
            """
            res = self.received_queue.get(block=False)
        except Exception:
            res = ''
        return res

    def send(self, data):
        time.sleep(.2)
        try:
            data = str(data)
        except Exception:
            pass
        data = data
        try:
            bdata = self.ConvertStringToBytes(data) 
            res = I2Cbus.write_i2c_block_data(
                self.address, 0x00, bdata) 
            return res
        except Exception as e:
            print("Could not send %s to %s" % (data, self))
            print(str(e))

    def obtener_indice_valido(self, posiciones, largo):
        indice = random.randint(0, largo - 1)
        if indice not in posiciones:
            return indice
        else:
            return self.obtener_indice_valido(posiciones, largo)

    def generar_vector_posiciones(self, largo=8):
        posiciones = []
        for i in range(largo):
            indice = self.obtener_indice_valido(posiciones, largo)
            posiciones.append(indice)
        self.posiciones = posiciones
        return posiciones

    @property
    def comando_conn_qty(self):
        return hasattr(self, 'conn_qty') and "$conn_qty:%s;" % str(self.conn_qty)

    @property
    def comando_posiciones(self):
        return hasattr(self, 'posiciones') and "$pos:%s;" % str(self.posiciones).strip('[').strip(']').replace(' ', '')

    def start(self):
        print(self.comando_conn_qty)
        self.send(self.comando_conn_qty)
        print(self.comando_posiciones)
        self.send(self.comando_posiciones)
        

if __name__ == '__main__':
    CONN_QTY = 8

    Sender = ArduinoController(ARDUINO_2_ADDRESS, CONN_QTY)
    Receiver = ArduinoController(ARDUINO_1_ADDRESS, CONN_QTY)
    Sender.generar_vector_posiciones()
    Receiver.generar_vector_posiciones()

    Sender.start()
    Receiver.start()
    print("Posiciones del Sender: %s" % Sender.posiciones)
    print("Posiciones del Receiver: %s" % Receiver.posiciones)
    while 1:
        time.sleep(2.2)
        res = Receiver.receive()
        print(res)
