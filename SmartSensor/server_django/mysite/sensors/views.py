from django.shortcuts import render
import json
from django.http import HttpResponse
from rest_framework.views import APIView

# class info(APIView):
#     def get(self,request):
#         json_data = ""
#         with open('./sensor_data') as json_file:
#             json_data = json.load(json_file)
        

def info(request):
    json_data = ""
    with open('./sensor_data') as json_file:
        json_data = json.load(json_file)
        
    print(json.dumps(json_data))

    return HttpResponse(json.dumps(json_data), content_type="text/json-comment-filtered")