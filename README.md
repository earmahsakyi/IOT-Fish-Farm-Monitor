# 🛰️ IoT Smart Monitoring System with MQTT and Django

This project provides a secure and scalable Internet of Things (IoT) system that allows remote monitoring and control of sensors and devices such as:

- Temperature sensor (DS18B20)
- Turbidity sensor
- Water level sensor
- PIR motion detector
- Relay-controlled devices (e.g., water pump(part of the hardware))
- Buzzer for alerts

Sensor data is published over MQTT (with SSL/TLS) to a Django backend that stores the data and provides REST APIs for real-time monitoring, alert tracking, and device control.

---

## 🚀 Features

- Publish secure sensor data using MQTT over SSL
- Store and analyze:
  - Temperature readings
  - Turbidity in NTU
  - Water level status (Empty, Low, Medium, Full)
  - Motion alerts
- Control relay and buzzer remotely via MQTT or Web
- RESTful API endpoints for integration with dashboards and mobile apps
- Automatically reconnect to WiFi and MQTT on disconnect

## Preview 
Homepage

![WhatsApp Image 2025-06-12 at 14 08 44_8b48b3d6](https://github.com/user-attachments/assets/0f308586-792d-434d-8b97-dc9f0ade7d8a)

hardwares(examples)

![water level](https://github.com/user-attachments/assets/32d17032-ca45-4ca8-8d3c-a90e81789b0e)
![temperature](https://github.com/user-attachments/assets/af4c0ffb-e76c-4c08-aa10-93561c6719d2)
![arduino](https://github.com/user-attachments/assets/26f304b0-9745-48f6-ad48-fa85aba84e74)



## Demo

![setup](https://github.com/user-attachments/assets/f7fd6ec3-c5e0-48fb-b19f-46e9f7666ed7)



## 🔧 Technologies Used

| Component        | Technology                    |
|------------------|-------------------------------|
| Hardware         | Arduino Uno R4 , BreadBoard |
| Backend          | Django, Django REST Framework |
| Messaging        | MQTT over SSL (Mosquitto broker) |
| Sensors          | DS18B20, analog turbidity, PIR, water level sensor |
| Data Format      | JSON                          |
| Database         | SQLite (default)  |

---

## 📂 Project Structure
┣ 📁myapp
┃ ┣ 📄models.py
┃ ┣ 📄views.py
┃ ┣ 📄serializers.py
┃ ┣ 📄urls.py
┣ 📁templates/
┣ 📁static/
┣ 📄manage.py
┣ 📄README.md

🙋‍♂️ Author
Emmanuel Armah Sakyi
