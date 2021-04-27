import mqtt_pub as mqtt
import threading 

def startTimer():
    timer = threading.Timer(5,startTimer)
    timer.start()
    # print("Timer")
    mqtt.run()
    # time.sleep(3)
    timer.cancel()
    
    
    

if __name__ == "__main__":
    startTimer()
