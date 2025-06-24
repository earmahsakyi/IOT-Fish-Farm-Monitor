from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login as auth_login, logout as auth_logout
from django.contrib import messages
from django.contrib.auth.decorators import login_required

from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status

from .models import Device, SensorData, DeviceStatus, MotionAlert
from .serializers import SensorDataSerializer, DeviceStatusSerializer, MotionAlertSerializer


def index(request):
    return render(request, 'index.html')


# Web login view using Django auth
def login(request):
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')
        user = authenticate(request, username=username, password=password)

        if user:
            auth_login(request, user)
            return redirect('myapp:index')
        else:
            messages.error(request, 'Invalid username or password.')

    return render(request, 'login.html')


def logout_view(request):
    auth_logout(request)
    return redirect('myapp:login')


# ================= API VIEWS ====================

@api_view(['POST'])
def post_sensor_data(request):
    try:
        device_id = request.data.get('device_id')
        turbidity = request.data.get('turbidity')
        water_level = request.data.get('water_level')
        temperature = request.data.get('temperature')
        motion_detected = request.data.get('motion_detected')

        device, created = Device.objects.get_or_create(device_id=device_id)

        data = SensorData.objects.create(
            device=device,
            turbidity=turbidity,
            water_level=water_level,
            temperature=temperature,
            motion_detected=motion_detected
        )

        if motion_detected:
            MotionAlert.objects.create(device=device, message="Motion Detected")

        return Response({"message": "Data saved successfully"}, status=status.HTTP_201_CREATED)
    except Exception as e:
        return Response({"error": str(e)}, status=status.HTTP_400_BAD_REQUEST)


@api_view(['POST'])
def update_device_status(request):
    try:
        device_id = request.data.get('device_id')
        relay_status = request.data.get('relay_status')
        buzzer_status = request.data.get('buzzer_status')

        device, created = Device.objects.get_or_create(device_id=device_id)
        status_obj, created = DeviceStatus.objects.get_or_create(device=device)

        if relay_status:
            status_obj.relay_status = relay_status
        if buzzer_status:
            status_obj.buzzer_status = buzzer_status

        status_obj.save()
        return Response({"message": "Device status updated"}, status=status.HTTP_200_OK)
    except Exception as e:
        return Response({"error": str(e)}, status=status.HTTP_400_BAD_REQUEST)


@api_view(['GET'])
def get_latest_data(request, device_id):
    try:
        device = Device.objects.get(device_id=device_id)
        data = SensorData.objects.filter(device=device).latest('timestamp')
        serializer = SensorDataSerializer(data)
        return Response(serializer.data, status=status.HTTP_200_OK)
    except Device.DoesNotExist:
        return Response({"error": "Device not found"}, status=status.HTTP_404_NOT_FOUND)
    except SensorData.DoesNotExist:
        return Response({"error": "No data found"}, status=status.HTTP_404_NOT_FOUND)


@api_view(['GET'])
def get_device_status(request, device_id):
    try:
        device = Device.objects.get(device_id=device_id)
        status_obj = DeviceStatus.objects.get(device=device)
        serializer = DeviceStatusSerializer(status_obj)
        return Response(serializer.data, status=status.HTTP_200_OK)
    except (Device.DoesNotExist, DeviceStatus.DoesNotExist):
        return Response({"error": "Device or status not found"}, status=status.HTTP_404_NOT_FOUND)


@api_view(['GET'])
def motion_alerts(request, device_id):
    try:
        device = Device.objects.get(device_id=device_id)
        alerts = MotionAlert.objects.filter(device=device).order_by('-timestamp')
        serializer = MotionAlertSerializer(alerts, many=True)
        return Response(serializer.data, status=status.HTTP_200_OK)
    except Device.DoesNotExist:
        return Response({"error": "Device not found"}, status=status.HTTP_404_NOT_FOUND)
