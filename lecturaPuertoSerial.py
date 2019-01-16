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
		if character == '1':
			try: 
				character= arduino.readline() # leemos hasta que encontramos el final de la linea

				data=json.loads(character)
				print(data)
				print(data['invernadero'])
				curs = db.cursor()
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
		elif character == '2':
			try:
				character= arduino.readline() # leemos hasta que encontramos el final de la linea
				data=json.loads(character)
				print(data)
				curs = db.cursor()
				curs.execute(
					'UPDATE invernaderos_parametro SET '
					+'magnitud_referencia_limite_inferior='
					+data['li']
					+' AND '
					+'magnitud_referencia_limite_superior='
					+data['ls']
					+' WHERE id_parametro='
					+data['id']
				)
arduino.close()