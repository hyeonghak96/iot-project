#!/usr/bin/python
import spidev
import time
import json
import WeatherAPI
import re
import RPi.GPIO as GPIO

from MQTT_Publisher import pubish
from MQTT_Subscriber import subscribe 


android_ctrl = False

led_pin = 13
fire_led_pin = 19

GPIO.setmode(GPIO.BCM)
# LED 핀의 IN/OUT 설정
GPIO.setup(led_pin, GPIO.OUT)
GPIO.setup(fire_led_pin, GPIO.OUT)




##window_pin
#서보모터를 PWM으로 제어할 핀 번호 설정
window_pin = 26
# GPIO핀의 번호 모드 설정
GPIO.setmode(GPIO.BCM)
# 서보핀의 출력 설정
GPIO.setup(window_pin, GPIO.OUT)
# PWM 인스턴스 servo 생성, 주파수 50으로 설정
servo = GPIO.PWM(window_pin,50)
# PWM 듀티비 0 으로 시작
servo.start(0)


# 딜레이 시간(센서 측정 간격)
delay = 2
# MCP3008 채널 중 센서에 연결한 채널 설정
light_sensor = 0
flame_sensor = 1
# SPI 인스턴스 spi 생성
spi = spidev.SpiDev()
# SPI 통신 시작하기
spi.open(0, 0)
# SPI 통신 속도 설정
spi.max_speed_hz = 100000

# 0 ~7 까지 8개의 채널에서 SPI 데이터 읽기
def readadc(adcnum):
    if adcnum < 0 or adcnum > 7:
        return -1
    r = spi.xfer2([1, 8+adcnum <<4, 0])
    data = ((r[1] & 3) << 8) + r[2]
    return data

def get_seoul_weather():
    result = WeatherAPI.check_weather()
    # print(result)
    result = json.loads(result)
    # print("RE")
    # print(type(result))
    # print(weather_Grade)

    weather_Grade = result["response"]["body"]["items"][0]["informGrade"]
    seoul = re.findall(r'서울 :([\W\w]*),제',weather_Grade)[0].strip()
    # print(seoul)
    return seoul


def recv_mqtt(client,userdata,msg):
    topic = msg.topic
    #print(msg.topic)
    message = msg.payload.decode('ascii')
    #print(message)

    if topic == "iot/control/servo":
        print(message)
        if message.isnumeric():
            angle = -1.0 + float(message)*0.02
            print(f"set angle {angle}")
            servo.value = angle



# print("Ready MQTT")

# subscribe("localhost","iot/control/servo",recv_mqtt)


seoul_weather = get_seoul_weather()
print(seoul_weather)

try:
    while True:
        if android_ctrl:
            pass
        else:
            sensor_dict = dict()
            # readadc 함수로 light_sensor SPI 데이터를 읽기
            pot_value0 = readadc(light_sensor)
            pot_value1 = readadc(flame_sensor)
            if pot_value0 > 400:
                sensor_dict['light'] = 1
                GPIO.output(led_pin,0) 
                servo.ChangeDutyCycle(2.5) # 미세먼지 적으면 문열음
            else:
                sensor_dict['light'] = 0
                GPIO.output(led_pin,1) # 어두우면 불켜짐
                servo.ChangeDutyCycle(10) # 미세먼지 많으면 문닫음

            if pot_value1 < 5:
                sensor_dict['fire'] = 0
                GPIO.output(fire_led_pin,0) 
            else:
                sensor_dict['fire'] = 1
                GPIO.output(fire_led_pin,1) # 불나면 불켜짐
            if seoul_weather == "보통":
                sensor_dict['munzi'] = "Good"
                # servo.ChangeDutyCycle(2.5) # 미세먼지 적으면 문열음
            else:
                sensor_dict['munzi'] = "Bad"
                # servo.ChangeDutyCycle(10) # 미세먼지 많으면 문닫음
            
            print("---------------------------")
            print("POT0 value: %d" % pot_value0) # 1이면 밝음 0이면 어두움
            print("POT1 value: %d" % pot_value1) # 1이면 불남 0이면 안남




            with open('./sensor_data', 'w') as outfile:
                json.dump(sensor_dict, outfile)
            
            with open('./server_django/mysite/sensor_data', 'w') as outfile:
                json.dump(sensor_dict, outfile)

            time.sleep(delay)
        
finally:
    GPIO.cleanup()