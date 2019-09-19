import random
import smbus

ARDUINO_1_ADDRESS = 0x04 # I2C Address of Arduino 1 
ARDUINO_2_ADDRESS = 0x05 # I2C Address of Arduino 2 

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
	def __init__(self, address, *args, **kwargs):
		self.address = address
		self.posiciones = []

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


if __name__ == '__main__':
	Sender = ArduinoController(ARDUINO_1_ADDRESS)
	Receiver = ArduinoController(ARDUINO_2_ADDRESS)
	Sender.generar_vector_posiciones()
	Receiver.generar_vector_posiciones()
	I2Cbus.write_i2c_block_data(Sender.address, 0x00, Sender.posiciones) 
	I2Cbus.write_i2c_block_data(Receiver.address, 0x00, Receiver.posiciones) 
	print("Posiciones del Sender: %s" % Sender.posiciones)
	print("Posiciones del Receiver: %s" % Receiver.posiciones)