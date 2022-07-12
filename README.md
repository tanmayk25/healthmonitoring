The goal of the project was to create a health monitoring system. This health monitoring system calculated the user's heart rate, blood oxygen levels, body temperature, ECG, and also the room temperature and humidity. 
The device is connected to a dashboard written in python using the Django framework. The device sends the data to the backend using REST APIs.

Hardware:
• HiLetgo ESP32 controller
• MAX30102 SPO2 sensor
• DS18B20 Body Temperature sensor
• AD8232 ECG sensor
• DHT11 Surrounding Humidity and Temperature sensor
• MPU6050 Gyroscope and Accelerometer sensor
• DS18B20 Body Temperature sensor
• Touch Sensor
• Breadboard
• Jumper wires
• Adhesive Button Electrodes (for ECG)
• Lead wires (for ECG)

Back End:
• Django
• SQLite
• AWS EC2 Cloud Service
• Apache

Web App:
• HTML5
• CSS

Setting up Health Monitoring System dashboard in Linux
Open the terminal
sudo apt update
sudo apt-get update
sudo apt install python3-pip
sudo apt install python3-virtualenv
virtualenv py_env
source py_env/bin/activate
pip install django
pip install djangorestframework

Navigate to the /HealthMonitor/Healthmonitor
python3 manage.py collectstatic
python manage.py runserver 

Setting up Health Monitoring System dashboard in Windows
Install python on windows
Open cmd/Powershell
Create a virtual environment
python -m venv venv
venv\Scripts\activate
pip install django
pip install djangorestframework

Navigate to the /HealthMonitor/HealthMonitor folder
python manage.py collectstatic
python manage.py runserver 
