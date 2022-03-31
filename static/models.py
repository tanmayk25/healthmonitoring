from django.db import models
from django.contrib.auth.models import User
from django.db.models.signals import post_save
from django.dispatch import receiver
from django.utils.timezone import now


# Create your models here.
class Profile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    first_name = models.CharField(max_length=100, blank=True)
    last_name = models.CharField(max_length=100, blank=True)
    email = models.EmailField(max_length=150)
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
    user_ID = models.ForeignKey('Health.Profile', on_delete=models.CASCADE)
    device_id = models.CharField(max_length=100, help_text="Enter the Device name", default='ESP')
    heart_rate = models.IntegerField(help_text="Enter the heart rate", default='0000000') #editable=False
    body_temperature = models.IntegerField(help_text="Enter the temperature", default='0000000')
    humidity = models.IntegerField(help_text="Enter the temperature", default='0000000')
    environment_temperature  = models.IntegerField(help_text="Enter the temperature", default='0000000')
    reading = models.TextField(help_text="Enter the value", default='0000')

    class Meta:
        ordering = [ "-created", "user_ID"]


    def __str__(self):
        return self.user_ID.user.username
