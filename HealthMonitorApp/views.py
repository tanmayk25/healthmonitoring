from django.shortcuts import render
from django.contrib.auth import login, authenticate
from .forms import SignUpForm
from django.shortcuts import render, redirect
from django.contrib.auth.forms import UserCreationForm
from .models import Monitored_Detail, Profile, Fall_Detail
from rest_framework import viewsets
from .serializers import MonitoredSerializer, FallSerializer
from rest_framework.decorators import api_view
from rest_framework.response import Response

# Create your views here.
def index(request):
	context = {

	}
	return render(request, 'index.html', context=context)

def details(request):
    profile = Profile.objects.get(user=request.user)
    ecgReading = "0000"
    device = profile.device_id
    context = {'created':[], 'heart_rate':[], 'body_temperature':[], 'humidity':[], 'environment_temperature':[], 'labels':[], 'list_val':[], 'blood_oxygen':[]}
    try:
        readings = Monitored_Detail.objects.filter(device_id=device)
        latestECGReading = Monitored_Detail.objects.filter(device_id=device).exclude(reading = '0000').latest('created')
        if latestECGReading:
            ecgReading = latestECGReading.reading
        for obj in readings:
            context['created'].append(obj.created)
            context['heart_rate'].append(obj.heart_rate)
            context['body_temperature'].append(obj.body_temperature)
            context['humidity'].append(obj.humidity)
            context['environment_temperature'].append(obj.environment_temperature)
            context['blood_oxygen'].append(obj.blood_oxygen)
        if ecgReading[-1] == ",":
            ecgReading = ecgReading[:-1]
        ecgReading = ecgReading.split(",")
        newecgReading = []
        for i in range(len(ecgReading)):
            if int(ecgReading[i]) < 5000:
                newecgReading.append(ecgReading[i])
        context['list_val'] = newecgReading
        for i in range(len(context['list_val'])):
            context['labels'].append(i)
    except Monitored_Detail.DoesNotExist:
        pass
    # Details_all=Monitored_Detail.objects.all()[:50]
    # val1 = Details_all[0].reading
    # if val1[-1] == ",":
    #     val1 = val1[:-1]
    # list_val = val1.split(",")
    # labels = []
    # for i in range(len(list_val)):
    #     labels.append(i)
    # heart_rate = []
    # body_temperature = []
    # humidity = []
    # environment_temperature  = []
    # created = []
    # result = []
    # line = []
    # for obj in Details_all:
    #     if request.user == obj.user_ID.user:
    #         created.append(obj.created)
    #         heart_rate.append(obj.heart_rate)
    #         body_temperature.append(obj.body_temperature)
    #         humidity.append(obj.humidity)
    #         environment_temperature.append(obj.environment_temperature)

    return render(
        request,
        'details.html',
        context=context
    )

def dashboard(request):
    profile = Profile.objects.get(user=request.user)
    device = profile.device_id
    context = {'heart_rate':0, 'body_temperature':0, 'humidity':0, 'environment_temperature':0, 'blood_oxygen':0}
    try:
        readings = Monitored_Detail.objects.filter(device_id=device).latest('created')
        context['heart_rate'] = readings.heart_rate
        context['body_temperature'] = readings.body_temperature
        context['humidity'] = readings.humidity
        context['environment_temperature'] = readings.environment_temperature
        context['blood_oxygen'] = readings.blood_oxygen
    except Monitored_Detail.DoesNotExist:
        pass

    return render(
        request,
        'dashboard.html',
        context=context
    )    

@api_view(['GET'])
def readings(request):
    profile = Profile.objects.get(user=request.user)
    device = profile.device_id
    context = {'heart_rate':0, 'body_temperature':0, 'humidity':0, 'environment_temperature':0, 'blood_oxygen':0, 'hrThreshold' : 0, 'boThreshold' : 0, 'btThreshold':0}
    try:
        readings = Monitored_Detail.objects.filter(device_id=device).latest('created')
        if int(readings.heart_rate) > 100 or int(readings.heart_rate) < 60:
            context['hrThreshold'] = 1
        if int(readings.blood_oxygen) < 90:
            context['boThreshold'] = 1
        if int(readings.body_temperature) > 99:
            context['btThreshold'] = 1
        context['heart_rate'] = readings.heart_rate
        context['body_temperature'] = readings.body_temperature
        context['humidity'] = readings.humidity
        context['environment_temperature'] = readings.environment_temperature
        context['blood_oxygen'] = readings.blood_oxygen        
    except Monitored_Detail.DoesNotExist:
        pass
    return Response(context)

def signup(request):
    if request.method == 'POST':
        form = SignUpForm(request.POST)
        if form.is_valid():
            user = form.save()
            user.refresh_from_db()  # load the profile instance created by the signal
            user.profile.first_name = form.cleaned_data.get('first_name')
            user.profile.last_name = form.cleaned_data.get('last_name')
            user.profile.email = form.cleaned_data.get('email')
            user.profile.device_id = form.cleaned_data.get('device_id')
            user.save()
            raw_password = form.cleaned_data.get('password1')
            user = authenticate(username=user.username, password=raw_password)
            login(request, user)
            return redirect('login')
    else:
        form = SignUpForm()
    return render(request, 'signup.html', {'form': form})

#After the serializers are created we need to create a view to the API and connect it to the Django URLs.
#Viewsets provide the advantage of combining multiple sets of logic into a single class.
class MonitoredViewSet(viewsets.ModelViewSet):
   queryset = Monitored_Detail.objects.all()
   serializer_class = MonitoredSerializer

class FallDetailViewSet(viewsets.ModelViewSet):
   queryset = Fall_Detail.objects.all()
   serializer_class = FallSerializer

def history(request):
    Details_all=Monitored_Detail.objects.all()[:50]
    UserD=Profile.objects.all()[:50]
    heart_rate = []
    body_temperature = []
    humidity = []
    environment_temperature  = []
    created = []
    result = []
    line = []
    for obj in Details_all:
        if request.user == obj.user_ID.user:
            created.append(obj.created)
            heart_rate.append(obj.heart_rate)
            body_temperature.append(obj.body_temperature)
            humidity.append(obj.humidity)
            environment_temperature.append(obj.environment_temperature)

    for i in range(len(heart_rate)):
        line.append(created[i])
        line.append(heart_rate[i])
        line.append(body_temperature[i])
        line.append(humidity[i])
        line.append(environment_temperature[i])
        result.append(line)
        line = []
    return render(
        request,
        'history.html',
        context={'result':result}
    )