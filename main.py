# Raspberry Pi to Arduino I2C Communication 
# Python Code 
 
import smbus 

# This function converts a string to an array of bytes. 
def ConvertStringToBytes(src): 
  converted = [] 
  for b in src: 
    converted.append(ord(b)) 
  return converted

# Slave Addresses for Arduinos 
ARDUINO_1_ADDRESS = 0x04 # I2C Address of Arduino 1 
ARDUINO_2_ADDRESS = 0x05 # I2C Address of Arduino 2 
 
# Create the I2C bus 
I2Cbus = smbus.SMBus(1) 
 
aSelect = input("Which Arduino (1-3): ") 

bSelect = input("On or Off (on/off): ") 

 
if aSelect == 1: 
  SlaveAddress = ARDUINO_1_ADDRESS 
elif aSelect == 2: 
  SlaveAddress = ARDUINO_2_ADDRESS 

else: 
  # quit if you messed up 
  quit()  
 
BytesToSend = ConvertStringToBytes(bSelect) 

I2Cbus.write_i2c_block_data(SlaveAddress, 0x00, BytesToSend) 

print("Sent " + SlaveAddress + " the " + bSelect + " command.") 


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

	def obtener_indice_valido(self, posiciones):
		indice = random.randint(0, largo - 1)
		if indice not in posiciones:
			return indice
		else:
			return self.obtener_indice_valido(posiciones)

	def generar_vector_posiciones(self, largo=8):
		posiciones = []
		for i in range(largo):
			indice = self.obtener_indice_valido(posiciones)
			posiciones.append(indice)


if __name__ == '__main__':
	import pdb; pdb.set_trace()
	Sender = ArduinoController(ARDUINO_1_ADDRESS)
	Receiver = ArduinoController(ARDUINO_2_ADDRESS)
	Sender.generar_vector_posiciones()