from rest_framework import serializers
from .models import SensorData, DeviceStatus, MotionAlert

class SensorDataSerializer(serializers.ModelSerializer):
    class Meta:
        model = SensorData
        fields = '__all__'


class DeviceStatusSerializer(serializers.ModelSerializer):
    class Meta:
        model = DeviceStatus
        fields = '__all__'


class MotionAlertSerializer(serializers.ModelSerializer):
    class Meta:
        model = MotionAlert
        fields = '__all__'
