from django.db import models

class Device(models.Model):
    device_id = models.CharField(max_length=64, unique=True)
    name = models.CharField(max_length=128, default="Unnamed Device")
    location = models.CharField(max_length=128, blank=True, null=True)
    is_active = models.BooleanField(default=True)
    last_seen = models.DateTimeField(auto_now=True)

    def __str__(self):
        return f"{self.name} ({self.device_id})"


class SensorData(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE)
    timestamp = models.DateTimeField(auto_now_add=True)
    
    turbidity = models.FloatField(help_text="Turbidity in NTU")
    water_level = models.CharField(max_length=10, choices=[('Empty', 'Empty'), ('Low', 'Low'), ('Medium', 'Medium'), ('Full', 'Full')])
    temperature = models.FloatField(help_text="Temperature in Celsius")
    motion_detected = models.BooleanField()

    def __str__(self):
        return f"Data @ {self.timestamp} for {self.device}"


class DeviceStatus(models.Model):
    device = models.OneToOneField(Device, on_delete=models.CASCADE)
    relay_status = models.CharField(max_length=3, choices=[('ON', 'ON'), ('OFF', 'OFF')], default='OFF')
    buzzer_status = models.CharField(max_length=3, choices=[('ON', 'ON'), ('OFF', 'OFF')], default='OFF')

    def __str__(self):
        return f"Status of {self.device}"


class MotionAlert(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE)
    timestamp = models.DateTimeField(auto_now_add=True)
    message = models.TextField(default="Motion Detected")

    def __str__(self):
        return f"Alert on {self.timestamp} - {self.device}"
