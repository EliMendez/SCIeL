import sqlite3

db = sqlite3.connect('db.sqlite3')
curs = db.cursor()
curs.execute('SELECT (id_medicion, id_invernadero, id_parametro, id_sensor, id_actuador, magnitud_medicion, fecha_medicion, is_activo) FROM "medicion"')
for registro in curs:
    print(registro)
db.close()