from django.db import models
from django.contrib.auth.models import User
from django.db.models.signals import post_save
from django.dispatch import receiver
from django.utils.timezone import now
import os
from twilio.rest import Client


# Create your models here.
class Profile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    first_name = models.CharField(max_length=100, blank=True)
    last_name = models.CharField(max_length=100, blank=True)
    email = models.EmailField(max_length=150)
    device_id = models.CharField(max_length=100, help_text="Enter the Device name", blank = True)
    bio = models.TextField()

    def __str__(self):
        return self.user.username

@receiver(post_save, sender=User)
def update_profile_signal(sender, instance, created, **kwargs):
    if created:
        Profile.objects.create(user=instance)
    instance.profile.save()

class Monitored_Detail(models.Model):
    created = models.DateTimeField(default=now, editable=False)
    device_id = models.CharField(max_length=100, help_text="Enter the Device name", blank = True)
    heart_rate = models.IntegerField(help_text="Enter the heart rate", default='0000000') #editable=False
    body_temperature = models.IntegerField(help_text="Enter the temperature", default='0000000')
    humidity = models.IntegerField(help_text="Enter the temperature", default='0000000')
    environment_temperature  = models.IntegerField(help_text="Enter the temperature", default='0000000')
    blood_oxygen = models.IntegerField(help_text="Enter the oxygen", default='0000000')
    reading = models.TextField(help_text="Enter the value", default='0000')

    class Meta:
        ordering = [ "created"]


    def __str__(self):
        return self.device_id

class Fall_Detail(models.Model):
    device_id = models.CharField(max_length=100, help_text="Enter the Device name", blank = True)
    created  = models.DateTimeField(default=now, editable=False)
    def save(self, silent=False, *args, **kwargs):
        user_ID = Profile.objects.get(device_id = self.device_id)
        name = user_ID.first_name + " " + user_ID.last_name
        account_sid = "AC03422bf48df78e4d312e1c48b47ba1f8"
        auth_token = "8a78a33e649ca1a8c99a1718e5697f77"
        client = Client(account_sid, auth_token)
        message = client.messages \
                .create(
                     body="Fall detected for user " + name,
                     from_='+15076974447',
                     to='+14086465104'
                 )
        print(message.sid)
        return super(Fall_Detail, self).save(*args, **kwargs)
