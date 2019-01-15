import serial, time
import json
import sqlite3

arduino = serial.Serial('COM7', 9600, timeout=1)
print arduino.readline()

while True:
	character = arduino.readline()
	MyJson = character
	db = sqlite3.connect('db.sqlite3')
	
	if character != '\n':
		try:
		    if(character == 1): #1  --> Temperatura
				try:
					data=json.loads(character)
		            print data
		            print data['medicion']
		            curs = db.cursor()
		            curs.execute("INSERT INTO Medicion(id_invernadero, id_parametro, id_sensor, id_actuador, magnitud_medicion, fecha_medicion, is_activo) values ("1", "3", "1", "1", "+str(data['medicion'])+", now(), "False")")
		            db.commit()
		        except ValueError:
		            print ""
		    if(character == 2): #2  --> HumedadSuelo
				try:
					data=json.loads(character)
		            print data
		            print data['medicion']
		            curs = db.cursor()
		            curs.execute("INSERT INTO Medicion(id_invernadero, id_parametro, id_sensor, id_actuador, magnitud_medicion, fecha_medicion, is_activo) values ("1", "2", "2", "2", "+str(data['medicion'])+", now(), "False")")
		            db.commit()
		        except ValueError:
		            print ""
		    if(character == 3): #3  --> HumedadRelativa
				try:
					data=json.loads(character)
		            print data
		            print data['medicion']
		            curs = db.cursor()
		            curs.execute("INSERT INTO Medicion(id_invernadero, id_parametro, id_sensor, id_actuador, magnitud_medicion, fecha_medicion, is_activo) values ("1", "1", "1", "1", "+str(data['medicion'])+", now(), "False")")
		            db.commit()
		        except ValueError:
		            print ""
		    if(character == 4): #4  --> Modificación del Estandar liHs
				try:
					data=json.loads(character)
		            print data
		            print data['limite_inferior']
		            curs = db.cursor()
		            curs.execute("UPDATE Parametro SET magnitud_medicion_limite_inferior = "str(data['limite_inferior'])+" WHERE id_parametro = "2"")
		            db.commit()
		        except ValueError:
		            print ""
		    if(character == 5): #5  --> Modificación del Estandar lsHs
				try:
					data=json.loads(character)
		            print data
		            print data['limite_superior']
		            curs = db.cursor()
		            curs.execute("UPDATE Parametro SET magnitud_medicion_limite_superior = "str(data['limite_superior'])+" WHERE id_parametro = "2"")
		            db.commit()
		        except ValueError:
		            print ""
		    if(character == 6): #6  --> Modificación del Estandar liTemp
				try:
					data=json.loads(character)
		            print data
		            print data['Limite inferior']
		            curs = db.cursor()
		            curs.execute("UPDATE Parametro SET magnitud_medicion_limite_inferior = "str(data['Limite inferior'])+" WHERE id_parametro = "3"")
		            db.commit()
		        except ValueError:
		            print ""
			if(character == 7): #7  --> Modificación del Estandar lsTemp
				try:
					data=json.loads(character)
		            print data
		            print data['limite_superior']
		            curs = db.cursor()
		            curs.execute("UPDATE Parametro SET magnitud_medicion_limite_superior = "str(data['limite_superior'])+" WHERE id_parametro = "3"")
		            db.commit()
		        except ValueError:
		            print ""	
		    if(character == 8): #8  --> Modificación del Estandar liHa
				try:
					data=json.loads(character)
		            print data
		            print data['limite_inferior']
		            curs = db.cursor()
		            curs.execute("UPDATE Parametro SET magnitud_medicion_limite_inferior = "str(data['limite_inferior'])+" WHERE id_parametro = "1"")
		            db.commit()
		        except ValueError:
		            print ""  
		    if(character == 9): #9  --> Modificación del Estandar lsHa
				try:
					data=json.loads(character)
		            print data
		            print data['limite_superior']
		            curs = db.cursor()
		            curs.execute("UPDATE Parametro SET magnitud_medicion_limite_superior = "str(data['limite_superior'])+" WHERE id_parametro = "1"")
		            db.commit()
		        except ValueError:
		            print ""          
		    if(character == 10): #10  --> Modificación del estado del Rociador
				try:
					data=json.loads(character)
		            print data
		            print data['estado']
		            curs = db.cursor()
		            curs.execute("UPDATE Medicion SET is_activo = "str(data['estado'])+" WHERE id_actuador = "1" ")
		            db.commit()
		        except ValueError:
		            print ""
		    if(character == 11): #11  --> Modificación del estado del Riego por goteo
				try:
					data=json.loads(character)
		            print data
		            print data['estado']
		            curs = db.cursor()
		            curs.execute("UPDATE Medicion SET is_activo = "str(data['estado'])+" WHERE id_actuador = "2" ")
		            db.commit()
		        except ValueError:
		            print ""
		except ValueError:
			print""
arduino.close()