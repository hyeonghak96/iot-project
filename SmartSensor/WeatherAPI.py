#!/usr/bin/python
import json
import requests
import time

def check_weather():
    t = time.time()
    today = time.strftime("%Y-%m-%d",time.gmtime(t))

    AIR_KOREA_URL = "http://apis.data.go.kr/B552584/ArpltnInforInqireSvc"

    AIR_KOREA_SERVICE = [
                            "/getMinuDustFrcstDspth",
                            ""
                        ]

    AIR_KOREA_APIKEY = "IM2bG69oxmgZydP+Qto2w2GYeIeigZR8ZYQrVHuBHY8GvJoTThLkN+1qd68GdsBCo6EYjizeJFBha2KRp1Jxqg=="

    air_params_dict = {
        "serviceKey" : AIR_KOREA_APIKEY,
        "returnType" : "json",
        "numOfRows" : 1,
        "pageNo" : 1,
        "searchDate" : today,
        "InformCode" : "PM10"
    }

    response = requests.get(AIR_KOREA_URL+AIR_KOREA_SERVICE[0],params = air_params_dict)
    result = response.content.decode('UTF-8')
    return result

if __name__ == "__main__":
    check_weather()