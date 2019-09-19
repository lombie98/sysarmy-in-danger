import random
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


class ArduinoController:
    def __init__(self, address, conn_qty, *args, **kwargs):
        self.address = address
        self.conn_qty = conn_qty
        self.posiciones = []

    def ConvertStringToBytes(self, src): 
        converted = [] 
        for b in src: 
            converted.append(ord(b)) 
        return converted

    def send(self, data):
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
        except Exception:
            pass

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
        self.send(self.comando_conn_qty)
        self.send(self.comando_posiciones)



if __name__ == '__main__':
    CONN_QTY = 8

    Sender = ArduinoController(ARDUINO_1_ADDRESS, CONN_QTY)
    Receiver = ArduinoController(ARDUINO_2_ADDRESS, CONN_QTY)
    Sender.generar_vector_posiciones()
    Receiver.generar_vector_posiciones()

    Sender.start()
    Receiver.start()
    print("Posiciones del Sender: %s" % Sender.posiciones)
    print("Posiciones del Receiver: %s" % Receiver.posiciones)
