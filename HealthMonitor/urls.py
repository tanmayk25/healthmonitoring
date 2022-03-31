"""HealthMonitor URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from django.urls import include
from django.views.generic import RedirectView
from django.conf import settings
from django.conf.urls.static import static
from HealthMonitorApp.views import signup
urlpatterns = [
    path('admin/', admin.site.urls),
    path('HealthMonitorApp/', include('HealthMonitorApp.urls')),
    path('', RedirectView.as_view(url='HealthMonitorApp/', permanent=True)),
    #connecting the main Django URL at HealthMonitor/urls.py to point to the app’s URL file.
    path('ping-of-things/', include('HealthMonitorApp.urls')),

] + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
