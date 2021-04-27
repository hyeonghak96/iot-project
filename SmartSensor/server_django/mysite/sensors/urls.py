from django.urls import path
from sensors import views

app_name = 'sensors'

urlpatterns = [
    path('inform/',views.info,name='info'),
]