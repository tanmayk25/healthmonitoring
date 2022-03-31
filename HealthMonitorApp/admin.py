from django.contrib import admin
from .models import Profile, Monitored_Detail, Fall_Detail

# Register your models here.
admin.site.register(Profile)
admin.site.register(Monitored_Detail)
admin.site.register(Fall_Detail)
admin.site.site_header="Health Monitoring"