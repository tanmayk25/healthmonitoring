from django.urls import path, include
from . import views
from django.urls import re_path
from . import views as core_views
from django.contrib import admin
from django.contrib.auth import views as auth_views
from django.views.generic.base import TemplateView
from rest_framework import routers
from .views import MonitoredViewSet, FallDetailViewSet

#After the viewsets are defined we can now use the router functionality provided by DRF to route a desired API endpoint to the given viewset
router = routers.DefaultRouter()
router.register(r'sensor_data', MonitoredViewSet)
router.register(r'fall_details', FallDetailViewSet)

urlpatterns = [
	path('', views.index, name='index'),
	path('signup/', views.signup, name="signup"),
    re_path(r'^login/$', auth_views.LoginView.as_view(), {'template_name': 'login.html'}, name='login'),
    re_path(r'^logout/$', auth_views.LogoutView.as_view(), {'next_page': 'login.html'}, name='logout'),
    path('dashboard/', views.dashboard, name="dashboard"),
    path('details/', views.details, name="details"),
    path('', include(router.urls)),
    re_path(r'^readings/$', views.readings),


]