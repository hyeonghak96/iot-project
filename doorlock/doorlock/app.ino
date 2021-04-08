#include<EEPROM.h>
#include<Keypad.h>
#include<SimpleTimer.h>
#include<Servo.h>
#include<Led.h>

const byte ROWS =4;
const byte COLS =4;


const byte col_pins[COLS]={10,11,12,13};
const byte row_pins[ROWS]={9,8,7,6};

const char hex_keys[ROWS][COLS]={
    {'0','1','2','3'},
    {'4','5','6','7'},
    {'8','9','A','B'},
    {'C','D','E','F'}
};

Keypad myKeypad =Keypad(makeKeymap(hex_keys),row_pins,col_pins,ROWS,COLS);
String input="";
bool b_press = false; //첫번째 입력인지 여부 판단.
SimpleTimer timer;
int timer_id;

String PASSWORD="1234";
Servo door; 

Led led(4);

String read_password(){
    String password="";
    for(int i=0;i<20;i++)
    {char ch =EEPROM.read(i);
     if(ch=='\0'){
         break;
     }
        password +=ch;
        }
    return password;
}

void write_password(String password){
    int size =password.length();
    for(int i=0;i<size;i++){
        char c =password[i];
        Serial.println(c);
        EEPROM.write(i,c);
    }
    EEPROM.write(size,'\0');
}

//키 입력을 하면 input에 구성
// F를 누르면 완료.. 
// 이때까지 입력한 문자열을 
// 시리얼로 출력



void setup(){
    Serial.begin(9600);
    door.attach(5);
    door.write(0);
    led.off();
    PASSWORD=read_password();
}

void beep(int interval=100){
    led.on();
    delay(interval);
    led.off();
}

void reset(){
    Serial.println("time out ! reset");
    input="";
    b_press=false;
    for(int i=0;i<3;i++){
        beep();
        delay(300);
    }
}


void open_door(){
    //서브 모터 문을 열어줌
    beep(200);
    door.write(90);
    delay(5000);
    //서브모터 문을 닫음
    door.write(0);
    beep(200);
}


void loop()
{
    timer.run();
    char key =myKeypad.getKey();

    if(key!=NO_KEY){  
        beep();  //시리얼 모니터 눌린 키 이름 출력
        if(key !='F'){ //키 입력중
            if(!b_press){
                // 타이머 setTimeout
                timer_id=timer.setTimeout(3000,reset);
                b_press=true;
            }else{
                //타이머 restart
                timer.restartTimer(timer_id);
            }
            input+=key;
        } else{
            timer.deleteTimer(timer_id);
            Serial.print("Input:");
            Serial.println(input);

            //'c'로 시작하면 새로운 비밀번호 저장
            //'c0248' 이면 '0248'이 새로운 비밀번호로 저장
            if(input[0] == 'C'){
                PASSWORD=input.substring(1);
                write_password(PASSWORD);
                Serial.println("New Password:"+PASSWORD);
            } else{
                if(input == PASSWORD) {   
                //문을 열어줌
                Serial.println("Password Matched!"); 
                open_door();
            }   else{
                Serial.println("Password Mismatched!");
                beep(1000);
                }
            }
            input="";
            b_press=false;
        
        }
    }
}
