import serial, time
import json
import sqlite3

arduino = serial.Serial('COM3', baudrate=9600, timeout=1.0)
print arduino.readline()

while True:
	character = arduino.readline()
	MyJson = character
	db = sqlite3.connect('db.sqlite3')

	    if(character == 1): #1  --> Temperatura
			try:
				data=json.loads(character)
	            print data
	            print data['medicion']
	            curs = db.cursor()
	            curs.execute("INSERT INTO Medicion(id_invernadero, id_parametro, id_sensor, id_actuador, magnitud_medicion, fecha_medicion, is_activo) values ("1", "3", "1", "1", "+str(data['medicion'])+", now(), "True")")
	            db.commit()
	        except ValueError:
	            print ""
	    if(character == 2): #2  --> HumedadSuelo
			try:
				data=json.loads(character)
	            print data
	            print data['medicion']
	            curs = db.cursor()
	            curs.execute("INSERT INTO Medicion(id_invernadero, id_parametro, id_sensor, id_actuador, magnitud_medicion, fecha_medicion, is_activo) values ("1", "2", "2", "2", "+str(data['medicion'])+", now(), "True")")
	            db.commit()
	        except ValueError:
	            print ""
	    if(character == 3): #3  --> HumedadRelativa
			try:
				data=json.loads(character)
	            print data
	            print data['medicion']
	            curs = db.cursor()
	            curs.execute("INSERT INTO Medicion(id_invernadero, id_parametro, id_sensor, id_actuador, magnitud_medicion, fecha_medicion, is_activo) values ("1", "1", "1", "1", "+str(data['medicion'])+", now(), "True")")
	            db.commit()
	        except ValueError:
	            print ""



	    if(character == 10): #10  --> Modificación del estado del Rociador
			try:
				data=json.loads(character)
	            print data
	            print data['actuador']
	            curs = db.cursor()
	            curs.execute("UPDATE Actuador SET is_baja = "str(data['actuador'])+" WHERE id_actuador = "1" ")
	            db.commit()
	        except ValueError:
	            print ""
	    if(character == 11): #11  --> Modificación del estado del Riego por goteo
			try:
				data=json.loads(character)
	            print data
	            print data['actuador']
	            curs = db.cursor()
	            curs.execute("UPDATE Actuador SET is_baja = "str(data['actuador'])+" WHERE id_actuador = "2" ")
	            db.commit()
	        except ValueError:
	            print ""
arduino.close()