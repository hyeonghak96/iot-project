import bluetooth
import time
#블루투스 주소,포트 선언
serverMACAddress =  '00:20:04:32:5E:BC'
port = 1

#블루투스 연결 
s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.connect((serverMACAddress, port))


def send_host():
    print(1)  
    text = "1" # Note change to the old (Python 2) raw_input
    time.sleep(0.1)
    s.send(text)
    
def send_none():
    print(0)
    text = "0"
    time.sleep(0.05) 
    s.send(text)