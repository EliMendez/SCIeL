from rest_framework import status
from rest_framework.decorators import api_view
from rest_framework.response import Response as RFResponse

from django.views.generic import CreateView, TemplateView 
from django.views.generic.edit import FormView, UpdateView
from django.views.generic.detail import DetailView
from django.views.generic.list import ListView
from django.shortcuts import render, redirect, get_object_or_404
from django.forms.models import model_to_dict
from django.template import loader
from django.urls import reverse_lazy
from django.http.response import HttpResponse, HttpResponseRedirect, JsonResponse
from django.contrib.auth import authenticate, login
from django.contrib.auth.models import User
from django.contrib.auth.mixins import LoginRequiredMixin
from django.contrib.auth.forms import AuthenticationForm
from django.contrib.auth.decorators import login_required
from django.contrib.auth.views import LoginView, LogoutView
from django.utils import timezone

from .forms import LoginForm, UserUpdateForm, InvernaderoForm
from .models import *
from .serializer import *

import json

class SignUpView(LoginRequiredMixin, CreateView):
    model = User
    form_class = LoginForm


class SignInView(LoginView):
    template_name = 'invernaderos/iniciarSesion.html'

    
"""class Login(FormView):
    form_class = AuthenticationForm
    template_name = 'invernaderos/iniciarSesion.html'
    success_url = reverse_lazy('invernaderos:invernaderos')
    
    def dispatch(self, request, *args, **kwargs):
        if request.user.is_authenticated:
            return HttpResponseRedirect(self.get_success_url())
        else:
            return super(self, Login).dispatch(request, *args, **kwargs)
    
    def form_valid(self, form):
        login(self.request, form.get_user())
        return super(self, Login).form_valid(form)
"""

class SignOutView(LoginRequiredMixin, LogoutView):
    template_name = 'invernaderos/iniciarSesion.html'


class PerfilUpdateView(LoginRequiredMixin, FormView):
    form_class = UserUpdateForm
    template_name = 'invernaderos/editarPerfil.html'
    context_object_name = 'user'


class InvernaderosListView(LoginRequiredMixin, ListView):
    model = Invernadero
    template_name = 'invernaderos/gestionarInvernaderos.html'
    context_object_name = 'invernaderos'


    def get_queryset(self):
        user = self.request.user
        context = Invernadero.objects.all().filter(id_usuario=user.id).filter(is_baja=False)
        return context
    
    def get_context_data(self, **kwargs):
        context = super(InvernaderosListView, self).get_context_data(**kwargs)
        return context


class CultivosListView(LoginRequiredMixin, ListView):
    model = Cultivo
    template_name = 'invernaderos/gestionarCultivos.html'
    context_object_name = 'cultivos'


    def get_queryset(self):
        user = self.request.user
        context = Cultivo.objects.all().filter(id_usuario=user.id).filter(is_baja=False)
        return context
    
    def get_context_data(self, **kwargs):
        context = super(CultivosListView, self).get_context_data(**kwargs)
        return context


class ParametrosListView(LoginRequiredMixin, ListView):
    model = Parametro
    template_name = 'invernaderos/gestionarParametros.html'
    context_object_name = 'parametros'


    def get_queryset(self):
        user = self.request.user
        invernaderos = Invernadero.objects.all().filter(id_usuario=user.id).filter(is_baja=False)
        parametros = Parametro.objects.all().filter(is_baja=False)
        datos = []
        for invernadero in invernaderos:
            result = parametros.filter(id_invernadero=invernadero.id_invernadero)
            for parametro in result:
                datos.append(parametro)
        context = datos
        return context
    
    def get_context_data(self, **kwargs):
        context = super(ParametrosListView, self).get_context_data(**kwargs)
        return context


class DispositivosListView(LoginRequiredMixin, ListView):
    model = Dispositivo
    template_name = 'invernaderos/gestionarDispositivos.html'
    context_object_name = 'dispositivos'


    def get_queryset(self):
        user = self.request.user
        dispositivos = Dispositivo.objects.all().filter(is_baja=False)
        invernaderos = Invernadero.objects.all().filter(id_usuario=user.id).filter(is_baja=False)
        datos = []
        for invernadero in invernaderos:
            if invernadero.id_dispositivo in dispositivos:
                if not invernadero.id_dispositivo in datos:
                    datos.append(invernadero.id_dispositivo)
        context = datos
        return context
    
    def get_context_data(self, **kwargs):
        context = super(DispositivosListView, self).get_context_data(**kwargs)
        return context


class SensoresListView(LoginRequiredMixin, ListView):
    model = Sensor
    template_name = 'invernaderos/gestionarSensores.html'
    context_object_name = 'sensores'


    def get_queryset(self):
        user = self.request.user
        sensores = Sensor.objects.all().filter(is_baja=False)
        invernaderos = Invernadero.objects.all().filter(id_usuario=user.id).filter(is_baja=False)
        datos = []
        for invernadero in invernaderos:
            result = sensores.filter(id_invernadero=invernadero.id_invernadero)
            for sensor in result:
                datos.append(sensor)
        context = datos
        return context
    
    def get_context_data(self, **kwargs):
        context = super(SensoresListView, self).get_context_data(**kwargs)
        return context


class ActuadoresListView(LoginRequiredMixin, ListView):
    model = Actuador
    template_name = 'invernaderos/gestionarActuadores.html'
    context_object_name = 'actuadores'


    def get_queryset(self):
        user = self.request.user
        invernaderos = Invernadero.objects.all().filter(id_usuario=user.id).filter(is_baja=False)
        actuadores = Actuador.objects.all().filter(is_baja=False)
        datos = []
        for invernadero in invernaderos:
            result = actuadores.filter(id_invernadero=invernadero.id_invernadero)
            for actuador in result:
                datos.append(actuador)
        context = datos
        return context
    
    def get_context_data(self, **kwargs):
        context = super(ActuadoresListView, self).get_context_data(**kwargs)
        return context

@login_required()
def monitorear_invernadero(request, id_invernadero):
    if request.method == 'GET':
        user = request.user
        invernaderos = Invernadero.objects.all().filter(id_usuario=user.id).filter(is_baja=False)
        cant_invernaderos = invernaderos.count()

        datos = []
        dispositivos = Dispositivo.objects.all().filter(is_baja=False)
        for invernadero in invernaderos:
            if invernadero.id_dispositivo in dispositivos:
                if not invernadero.id_dispositivo in datos:
                    datos.append(invernadero.id_dispositivo)
        cant_dispositivos = len(datos)
        
        actuadores = Actuador.objects.all().filter(is_baja=False)
        datos = []
        for invernadero in invernaderos:
            result = actuadores.filter(id_invernadero=invernadero.id_invernadero)
            for actuador in result:
                datos.append(actuador)
        cant_actuadores = len(datos)

        sensores = Sensor.objects.all().filter(is_baja=False)
        datos = []
        for invernadero in invernaderos:
            result = sensores.filter(id_invernadero=invernadero.id_invernadero)
            for sensor in result:
                datos.append(sensor)
        cant_sensores = len(datos)

        parametros = Parametro.objects.all().filter(is_baja=False)
        for invernadero in invernaderos:
            result = parametros.filter(id_invernadero=invernadero.id_invernadero)
            for parametro in result:
                datos.append(parametro)
        cant_parametros = len(datos)
            
        cant_cultivos = Cultivo.objects.all().filter(id_usuario=user.id).filter(is_baja=False).count()

        mediciones = Medicion.objects.all().filter(id_invernadero=id_invernadero)[:20]
        fecha = timezone.now()
            
        context = {
            'cant_cultivos': cant_cultivos,
            'cant_parametros': cant_parametros,
            'cant_sensores': cant_sensores,
            'cant_actuadores': cant_actuadores,
            'cant_dispositivos': cant_dispositivos,
            'cant_invernaderos': cant_invernaderos,
            'mediciones': mediciones,
            'fecha': fecha
        }

        temp = loader.get_template('invernaderos/monitorearInvernaderos.html')
        
        return HttpResponse(temp.render(context, request))
    elif request.method == 'RELOAD':
        mediciones = Medicion.objects.all().filter(id_invernadero=id_invernadero)[:20]
        fecha = timezone.now()
        context = {
            'mediciones': mediciones,
            'fecha': fecha
        }
        return JsonResponse(data=context, safe=True)

def editar_invernadero(request):
    id_invernadero = request.GET['id']
    invernadero = get_object_or_404(Invernadero, id_invernadero=id_invernadero)
    if request.method == 'POST':
        form =InvernaderoForm(request.POST, instance=invernadero)
        if form.is_valid():
            invernadero = form.save(commit=False)
            invernadero.id_usuario = request.POST['id_usuario']
            invernadero.save()
            message = 'Los cambios fueron guardados exitosamente'
        else:
            message = 'Algunos campos son inválidos'
        data = {
                'message': message
            }
        return JsonResponse(data)
    else:
        form = InvernaderoForm(instance=invernadero)
    data = {
        'form': form
    }
    return HttpResponse(form)

@api_view(['GET', 'PUT', 'DELETE'])
@login_required(login_url='/sign-in/')
def invernadero(request):  
    if request.method == 'GET':
        id_invernadero = request.GET['id']
        invernadero = Invernadero.objects.get(id_invernadero=id_invernadero)
        invernadero_serializer = InvernaderoSerializer(invernadero, many=False)
        message = 'Los datos han sido desplegados'
        data = {
            'invernadero': invernadero_serializer.data,
            'message': message
        }
        return RFResponse(data=data)
    elif request.method == 'PUT':
        id_invernadero = request.POST['id']
        invernadero = Invernadero.objects.get(id_invernadero=id_invernadero)
        invernadero_serializer = InvernaderoSerializer(invernadero, data=request.data['invernadero'])
        if invernadero_serializer.is_valid():
            result = invernadero_serializer.save()
            message = None
            if result:
                message = "El invernadero fue modificado exitosamente"
            else:
                message = "El invernadero no pudo ser modificado"
            data = {
                'message': message
            }
            return RFResponse(
                data={
                    'invernadero': invernadero_serializer.data,
                    'data':data
                }
            )
        message = "El formulario no es válido"
        data = {
            'message': message
        }
        return RFResponsedata(
            data={
                'errors': invernadero_serializer.errors,
                'data': data
            }, 
            status=status.HTTP_400_BAD_REQUEST
        )
    elif request.method == 'DELETE':
        id_invernadero = request.POST['id']
        invernadero = Invernadero.objects.get(id_invernadero=id_invernadero)
        invernadero.is_baja = True
        invernadero.save()
        message = "El invernadero fue borrado exitosamente"
        return RFResponse(data=message, status=status.HTTP_204_NO_CONTENT)

@api_view(['GET', 'PUT', 'DELETE'])
@login_required(login_url='/sign-in/')
def dispositivo(request):
    if request.method == 'GET':
        id_dispositivo = request.GET['id']
        dispositivo = Dispositivo.objects.get(id_dispositivo=id_dispositivo)
        dispositivo_serializer = DispositivoSerializer(dispositivo, many=False)
        message = 'Los datos han sido desplegados'
        data = {
            'dispositivo': dispositivo_serializer.data,
            'message': message
        }
        return RFResponse(data=data)
    elif request.method == 'PUT':
        id_dispositivo = request.POST['id']
        dispositivo = Dispositivo.objects.get(id_dispositivo=id_dispositivo)
        dispositivo_serializer = DispositivoSerializer(dispositivo, data=request.data['dispositivo'])
        if dispositivo_serializer.is_valid():
            result = dispositivo_serializer.save()
            message = None
            if result:
                message = "El dispositivo fue modificado exitosamente"
            else:
                message = "El dispositivo no pudo ser modificado"
            data = {
                'message': message
            }
            return RFResponse(
                data={
                    'dispositivo': dispositivo_serializer.data,
                    'data':data
                }
            )
        message = "El formulario no es válido"
        data = {
            'message': message
        }
        return RFResponsedata(
            data={
                'errors': dispositivo_serializer.errors,
                'data': data
            }, 
            status=status.HTTP_400_BAD_REQUEST
        )
    elif request.method == 'DELETE':
        id_dispositivo = request.POST['id']
        dispositivo = Dispositivo.objects.get(id_dispositivo=id_dispositivo)
        dispositivo.is_baja = True
        dispositivo.save()
        message = "El dispositivo fue borrado exitosamente"
        return RFResponse(data={'message': message}, status=status.HTTP_204_NO_CONTENT)        

@api_view(['GET', 'PUT', 'DELETE'])
@login_required(login_url='/sign-in/')
def sensor(request):
    if request.method == 'GET':
        id_sensor = request.GET['id']
        sensor = Sensor.objects.get(id_sensor=id_sensor)
        sensor_serializer = SensorSerializer(sensor, many=False)
        message = 'Los datos han sido desplegados'
        data = {
            'sensor': sensor_serializer.data,
            'message': message
        }
        return RFResponse(data=data)
    elif request.method == 'PUT':
        id_sensor = request.POST['id']
        sensor = Sensor.objects.get(id_sensor=id_sensor)
        sensor_serializer = SensorSerializer(sensor, data=request.data['sensor'])
        if sensor_serializer.is_valid():
            result = sensor_serializer.save()
            message = None
            if result:
                message = "El sensor fue modificado exitosamente"
            else:
                message = "El sensor no pudo ser modificado"
            data = {
                'message': message
            }
            return RFResponse(
                data={
                    'sensor': sensor_serializer.data,
                    'data':data
                }
            )
        message = "El formulario no es válido"
        data = {
            'message': message
        }
        return RFResponsedata(
            data={
                'errors': sensor_serializer.errors,
                'data': data
            }, 
            status=status.HTTP_400_BAD_REQUEST
        )
    elif request.method == 'DELETE':
        id_sensor = request.POST['id']
        sensor = Sensor.objects.get(id_sensor=id_sensor)
        sensor.is_baja = True
        sensor.save()
        message = "El sensor fue borrado exitosamente"
        return RFResponse(data={'message': message}, status=status.HTTP_204_NO_CONTENT)

@api_view(['GET', 'PUT', 'DELETE'])
@login_required(login_url='/sign-in/')
def actuador(request):  
    if request.method == 'GET':
        id_actuador = request.GET['id']
        actuador = Actuador.objects.get(id_actuador=id_actuador)
        actuador_serializer = ActuadorSerializer(actuador, many=False)
        message = 'Los datos han sido desplegados'
        data = {
            'actuador': actuador_serializer.data,
            'message': message
        }
        return RFResponse(data=data)
    elif request.method == 'PUT':
        id_actuador = request.POST['id']
        actuador = Actuador.objects.get(id_actuador=id_actuador)
        actuador_serializer = ActuadorSerializer(actuador, data=request.data['actuador'])
        if actuador_serializer.is_valid():
            result = actuador_serializer.save()
            message = None
            if result:
                message = "El actuador fue modificado exitosamente"
            else:
                message = "El actuador no pudo ser modificado"
            data = {
                'message': message
            }
            return RFResponse(
                data={
                    'actuador': actuador_serializer.data,
                    'data':data
                }
            )
        message = "El formulario no es válido"
        data = {
            'message': message
        }
        return RFResponsedata(
            data={
                'errors': actuador_serializer.errors,
                'data': data
            }, 
            status=status.HTTP_400_BAD_REQUEST
        )
    elif request.method == 'DELETE':
        id_actuador = request.POST['id']
        actuador = Actuador.objects.get(id_actuador=id_actuador)
        actuador.is_baja = True
        actuador.save()
        message = "El actuador fue borrado exitosamente"
        return RFResponse(data={'message': message}, status=status.HTTP_204_NO_CONTENT)

@api_view(['GET', 'PUT', 'DELETE'])
@login_required(login_url='/sign-in/')
def cultivo(request):
    if request.method == 'GET':
        id_cultivo = request.GET['id']
        cultivo = Cultivo.objects.get(id_cultivo=id_cultivo)
        cultivo_serializer = CultivoSerializer(cultivo, many=False)
        message = 'Los datos han sido desplegados'
        data = {
            'cultivo': cultivo_serializer.data,
            'message': message
        }
        return RFResponse(data=data)
    
    elif request.method == 'PUT':
        id_cultivo = request.POST['id']
        cultivo = Cultivo.objects.get(id_cultivo=id_cultivo)
        cultivo_serializer = CultivoSerializer(cultivo, data=request.data['cultivo'])
        if cultivo_serializer.is_valid():
            result = cultivo_serializer.save()
            message = None
            if result:
                message = "El cultivo fue modificado exitosamente"
            else:
                message = "El cultivo no pudo ser modificado"
            data = {
                'message': message
            }
            return RFResponse(
                data={
                    'cultivo': cultivo_serializer.data,
                    'data':data
                }
            )
        message = "El formulario no es válido"
        data = {
            'message': message
        }
        return RFResponsedata(
            data={
                'errors': cultivo_serializer.errors,
                'data': data
            }, 
            status=status.HTTP_400_BAD_REQUEST
        )
    elif request.method == 'DELETE':
        id_cultivo = request.POST['id']
        cultivo = Cultivo.objects.get(id_cultivo=id_cultivo)
        cultivo.is_baja = True
        cultivo.save()
        message = "El cultivo fue borrado exitosamente"
        return RFResponse(data={'message': message}, status=status.HTTP_204_NO_CONTENT)

@api_view(['GET', 'PUT', 'DELETE'])
@login_required(login_url='/sign-in/')
def parametro(request):   
    if request.method == 'GET':
        id_parametro = request.GET['id']
        parametro = Parametro.objects.get(id_parametro=id_parametro)
        parametro_serializer = ParametroSerializer(parametro, many=False)
        message = 'Los datos han sido desplegados'
        data = {
            'parametro': parametro_serializer.data,
            'message': message
        }
        return RFResponse(data=data)    
    elif request.method == 'PUT':
        id_parametro = request.POST['id']
        parametro = Parametro.objects.get(id_parametro=id_parametro)
        parametro_serializer = ParametroSerializer(parametro, data=request.data['parametro'])
        if parametro_serializer.is_valid():
            result = parametro_serializer.save()
            message = None
            if result:
                message = "El parametro fue modificado exitosamente"
            else:
                message = "El parametro no pudo ser modificado"
            data = {
                'message': message
            }
            return RFResponse(
                data={
                    'parametro': parametro_serializer.data,
                    'data':data
                }
            )
        message = "El formulario no es válido"
        data = {
            'message': message
        }
        return RFResponsedata(
            data={
                'errors': parametro_serializer.errors,
                'data': data
            }, 
            status=status.HTTP_400_BAD_REQUEST
        )
    elif request.method == 'DELETE':
        id_parametro = request.POST['id']
        parametro = Parametro.objects.get(id_parametro=id_parametro)
        parametro.is_baja = True
        parametro.save()
        message = "El parametro fue borrado exitosamente"
        return RFResponse(data={'message': message}, status=status.HTTP_204_NO_CONTENT)
