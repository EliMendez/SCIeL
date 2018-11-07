from .models import *
from rest_framework import serializers


class CultivoSerializer(serializers.ModelSerializer):
    
    class Meta:
        model = Cultivo
        fields = (
            'id_cultivo',
            'nombre_cultivo',
            'periodo_cosecha'
        )
    

class DispositivoSerializer(serializers.ModelSerializer):
    
    class Meta:
        model = Dispositivo
        fields = (
            'id_dispositivo',
            'nombre_dispositivo'
        )


class InvernaderoSerializer(serializers.ModelSerializer):
    
    class Meta:
        model = Invernadero
        fields = (
            'id_invernadero',
            'id_dispositivo',
            'id_usuario',
            'id_cultivo',
            'nombre_invernadero',
            'ubicacion'
        )


class ParametroSerializer(serializers.ModelSerializer):

    class Meta:
        model = Parametro
        fields = (
            'id_parametro',
            'id_invernadero',
            'nombre_parametro',
            'magnitud_referencia'
        )


class ActuadorSerializer(serializers.ModelSerializer):

    class Meta:
        model = Actuador
        fields = (
            'id_actuador',
            'id_dispositivo',
            'id_invernadero',
            'nombre_actuador'
        )


class SensorSerializer(serializers.ModelSerializer):

    class Meta:
        model = Sensor
        fields = (
            'id_sensor',
            'id_dispositivo',
            'id_invernadero',
            'nombre_sensor'
        )


class MedicionSerializer(serializers.ModelSerializer):

    class Meta:
        model = Medicion
        fields = (
            'id_medicion',
            'id_invernadero',
            'id_parametro',
            'id_sensor',
            'id_actuador',
            'magnitud_medicion',
            'fecha_medicion',
            'is_activo'
        )
