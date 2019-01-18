import serial # Importamos la libreira de PySerial
import json
import sqlite3
arduino = serial.Serial('COM7', 9600, timeout=1) # Abrimos el puerto del arduino a 9600 

# Creamos un buble sin fin
while True:
	character= arduino.readline() # leemos el tipo de operación a realizar
	print(character)
	MyJson = character

	db = sqlite3.connect('db.sqlite3')
	if character != '\n' :
		try: 
			data=json.loads(character)
			print(data)
			print(data['invernadero'])
			curs = db.cursor()
			print('INSERT INTO invernaderos_medicion (id_invernadero_id, id_parametro_id, id_sensor_id, id_actuador_id, magnitud_medicion, fecha_medicion, is_activo) values ('
				+str(data['invernadero'])
				+', '
				+str(data['parametro'])
				+', '
				+str(data['sensor'])
				+', '
				+str(data['actuador'])
				+', '
				+str(data['medicion'])
				+', datetime(), '
				+str(data['estado'])
				+')')
			curs.execute(
				'INSERT INTO invernaderos_medicion ('
					+'id_invernadero_id,'
					+'id_parametro_id,'
					+'id_sensor_id,'
					+'id_actuador_id,'
					+'magnitud_medicion,'
					+'fecha_medicion,'
					+'is_activo'
				+') values ('
				+str(data['invernadero'])
				+', '
				+str(data['parametro'])
				+', '
				+str(data['sensor'])
				+', '
				+str(data['actuador'])
				+', '
				+str(data['medicion'])
				+', datetime(), '
				+str(data['estado'])
				+')')
			db.commit() 
			print("Se registro con exito")
		except ValueError:
			print("")
			
arduino.close()