from django.urls import path
from . import views

app_name = 'myapp'

urlpatterns = [
    # Web Views
    path('', views.index, name='index'),
    path('index/', views.index, name='index'),
    path('login/', views.login, name='login'),

    # API Endpoints
    path('api/data/', views.post_sensor_data, name='post_sensor_data'),
    path('api/status/update/', views.update_device_status, name='update_device_status'),
    path('api/data/latest/<str:device_id>/', views.get_latest_data, name='get_latest_data'),
    path('api/status/<str:device_id>/', views.get_device_status, name='get_device_status'),
    path('api/alerts/<str:device_id>/', views.motion_alerts, name='motion_alerts'),
]
