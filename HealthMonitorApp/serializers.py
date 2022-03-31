#Setting up the DRF Serializers for Rest API
from rest_framework import serializers
from .models import Monitored_Detail, Fall_Detail

#The serializers will convert the Monitored_data model into JSON that will be used by the API to return the data to the user.
class MonitoredSerializer(serializers.ModelSerializer):
   class Meta:
       model = Monitored_Detail
       fields = ('device_id', 'heart_rate', 'body_temperature', 'humidity', 'environment_temperature', 'blood_oxygen', 'reading')

class FallSerializer(serializers.ModelSerializer):
   class Meta:
       model = Fall_Detail
       fields = ('device_id', 'created')