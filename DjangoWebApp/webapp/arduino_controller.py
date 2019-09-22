import queue

import time
from threading import Thread, Event

try:
    import smbus
    I2Cbus = smbus.SMBus(1)
except Exception:
    print("Could not initialize I2C Port")


ARDUINO_1_ADDRESS = 0x04  # I2C Address of Arduino 1
ARDUINO_2_ADDRESS = 0x05  # I2C Address of Arduino 2


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


class ArduinoController:
    def __init__(self, address, conn_qty, conn_layout, *args, **kwargs):
        self.name = 'Unknown'
        if address == 'receiver':
            address = ARDUINO_1_ADDRESS
            self.name = 'Receiver'
        elif address == 'sender':
            address = ARDUINO_2_ADDRESS
            self.name = 'Sender'
        self.conn_layout = conn_layout
        self.address = address
        self.conn_qty = conn_qty
        self.received_queue = queue.Queue()
        self.stop_event = Event()

    def ConvertStringToBytes(self, src):
        converted = []
        for b in src:
            converted.append(ord(b))
        return converted

    def receive(self, timeout=1):
        time.sleep(.01)
        # receive_thread = Thread(target=self._receive_data, args=[])
        # receive_thread.start()
        # receive_thread.join(timeout=timeout)
        # self.stop_event.set()
        # try:
        #     res = self.received_queue.get(block=False)
        # except Exception:
        #     res = ''
        return self._receive_data()

    def _receive_data(self):
        self.send("$status;")
        res = []
        for i in range(self.conn_qty):
            byte = I2Cbus.read_byte(self.address)
            char = chr(byte)
            if char == 'E':
                return 'Err'
            res.append(char)
        return ''.join(res)

    def send(self, data):
        time.sleep(.01)
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
            print("Could not send %s to %s" % (data, self.name))
            print(str(e))

    @property
    def comando_conn_qty(self):
        res = False
        if hasattr(self, 'conn_qty'):
            res = "$conn_qty:%s;" % str(self.conn_qty)
        return res

    @property
    def comando_conn_layout(self):
        res = False
        if hasattr(self, 'conn_layout'):
            res = "$pos:%s;" % str(self.conn_layout).\
                strip('[').strip(']').replace(' ', '')
        return res

    def start(self):
        print(self.comando_conn_qty)
        self.send(self.comando_conn_qty)
        print(self.comando_conn_layout)
        self.send(self.comando_conn_layout)
