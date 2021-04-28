#include <Robot.h>
#include <BtMiniCom.h>
#include <SoftwareSerial.h>
#include <LineTrace.h>
#include <Led.h>
#include <Pressure.h>

#define BT_RXD 3 // 아두이노의 4번핀을 RX(받는 핀)로 설정
#define BT_TXD 4 // 아두이노 3번핀을 TX(보내는 핀)로 설정

// int L = digitalRead(A5); // 왼쪽 라인트레이서
// int C = digitalRead(A4); // 가운데 라인트레이서
// int R = digitalRead(A2); // 오른쪽 라인트레이서

int motor_s = 153; // 최대 속도(0~255)의 60%

int R_Motor = 0;
int L_Motor = 0;

SoftwareSerial bt(BT_RXD, BT_TXD); //블루투스 통신을 위한 설정
LineTrace lt;
Led led(0);
Pressure prs;

int RightMotor_E_pin = 5; // 오른쪽 모터의 Enable & PWM
int LeftMotor_E_pin = 6;  // 왼쪽 모터의 Enable & PWM
int RightMotor_1_pin = 8; // 오른쪽 모터 제어선 IN1
int RightMotor_2_pin = 9; // 오른쪽 모터 제어선 IN2
int LeftMotor_3_pin = 10; // 왼쪽 모터 제어선 IN3
int LeftMotor_4_pin = 11; // 왼쪽 모터 제어선 IN4

void forward(int R_motor, int L_motor, int Speed)
{
  digitalWrite(RightMotor_1_pin, R_motor);
  digitalWrite(RightMotor_2_pin, !R_motor);
  digitalWrite(LeftMotor_3_pin, L_motor);
  digitalWrite(LeftMotor_4_pin, !L_motor);

  analogWrite(RightMotor_E_pin, Speed); // 우측 모터 속도값
  analogWrite(LeftMotor_E_pin, Speed);  // 좌측 모터 속도값
}

void right(int R_motor, int L_motor, int Speed)
{
  digitalWrite(RightMotor_1_pin, R_motor);
  digitalWrite(RightMotor_2_pin, !R_motor);
  digitalWrite(LeftMotor_3_pin, L_motor);
  digitalWrite(LeftMotor_4_pin, !L_motor);

  analogWrite(RightMotor_E_pin, max(Speed * 0.6, 50)); // 우측 모터 속도값
  analogWrite(LeftMotor_E_pin, min(Speed * 1.4, 200)); // 좌측 모터 속도값
}

void left(int R_motor, int L_motor, int Speed)
{
  digitalWrite(RightMotor_1_pin, R_motor);
  digitalWrite(RightMotor_2_pin, !R_motor);
  digitalWrite(LeftMotor_3_pin, L_motor);
  digitalWrite(LeftMotor_4_pin, !L_motor);

  analogWrite(RightMotor_E_pin, min(Speed * 1.4, 200)); // 우측 모터 속도값
  analogWrite(LeftMotor_E_pin, max(Speed * 0.6, 50));   // 좌측 모터 속도값
}

void stop(int R_motor, int L_motor)
{
  digitalWrite(RightMotor_1_pin, R_motor);
  digitalWrite(RightMotor_2_pin, !R_motor);
  digitalWrite(LeftMotor_3_pin, L_motor);
  digitalWrite(LeftMotor_4_pin, !L_motor);

  analogWrite(RightMotor_E_pin, 0); // 우측 모터 속도값
  analogWrite(LeftMotor_E_pin, 0);  // 좌측 모터 속도값
}

void bt_control(char Blue_Val)
{
  if (Blue_Val == 'b')
  {
    R_Motor = HIGH;
    L_Motor = HIGH;
    motor_s = 150;
    Serial.println("Start");
    play();   // play의 while 동작
  }
  else if (Blue_Val == 's')   // s 입력 시 정지
  {
    R_Motor = LOW;
    L_Motor = LOW;
    Serial.println("stop");
    led.off();
    stop(R_Motor, L_Motor);
  }
  else
  {
    Serial.println("Unknown Command");
  }
}

void play(){
 while (1)
 {
  led.on();
   
  int L = digitalRead(A5); // 왼쪽 라인트레이서
  int C = digitalRead(A4); // 가운데 라인트레이서
  int R = digitalRead(A2); // 오른쪽 라인트레이서

  Serial.print("digital : "); Serial.print(L); Serial.print(", "); Serial.print(C); Serial.print(", "); Serial.print(R); Serial.print("  \n ");

   if (lt.forward_bool(L, C, R) == true){
        Serial.println("go");
        forward(R_Motor, L_Motor, motor_s);
        led.on();
      }else if (lt.right_bool(L, C, R) == true){
        Serial.println("Right turn");
        right(R_Motor, L_Motor, motor_s);
      }else if (lt.left_bool(L, C, R) == true){
        Serial.println("Left turn");
        left(R_Motor, L_Motor, motor_s);
      }else if (lt.pause(L, C, R) == true){
        Serial.println("Pause");
        led.off();
        stop(R_Motor, L_Motor);
        if (prs.pressValue() >= 100){
          Serial.println("detected on");
          int press_value_1 = prs.pressValue();
          delay(1000);
          led.on();
          forward(R_Motor, L_Motor, motor_s);
        }else{
          delay(5000);
          Serial.println("Time Out");
          forward(R_Motor, L_Motor, motor_s);
          led.off();
        }
      }else if (lt.stop_bool(L, C, R) == true){
        Serial.println("End");
        stop(R_Motor, L_Motor);
        led.off();
        break;
      }else{
      led.off();
      Serial.println("Error...");
      }
 }
 
}
void setup()
{
  pinMode(RightMotor_E_pin, OUTPUT); // 출력모드로 설정
  pinMode(RightMotor_1_pin, OUTPUT);
  pinMode(RightMotor_2_pin, OUTPUT);
  pinMode(LeftMotor_3_pin, OUTPUT);
  pinMode(LeftMotor_4_pin, OUTPUT);
  pinMode(LeftMotor_E_pin, OUTPUT);

  // LineTrace();

  Serial.begin(9600); //PC와 아두이노간 시리얼 통신 속도를 9600bps로 설정
  bt.begin(9600);     //블루투스와 아두이노간 시리얼 통신 속도를 9600bps로 설정

  Serial.println("connected");
}

void loop(){
  if (bt.available()){
    char Blue_Val = bt.read();

    Serial.print("Input Signal : ");
    Serial.print("Bluetooth[ ");
    Serial.print(Blue_Val);
    Serial.print(" ] \n ");
    bt_control(Blue_Val);   // b를 입력하면 bt_control 실행
    
  }
}







// #include <Robot.h>
// #include <BtMiniCom.h>
// #include <SoftwareSerial.h>
// #include <LineTrace.h>
// #include <Led.h>
// #include <Pressure.h>

// #define BT_RXD 3 // 아두이노의 4번핀을 RX(받는 핀)로 설정
// #define BT_TXD 4 // 아두이노 3번핀을 TX(보내는 핀)로 설정

// int motor_s = 153; // 최대 속도(0~255)의 60%

// int R_Motor = 0;
// int L_Motor = 0;

// SoftwareSerial bt(BT_RXD, BT_TXD); //블루투스 통신을 위한 설정
// LineTrace lt;
// Led led(0);
// Pressure prs;

// int RightMotor_E_pin = 5; // 오른쪽 모터의 Enable & PWM
// int LeftMotor_E_pin = 6;  // 왼쪽 모터의 Enable & PWM
// int RightMotor_1_pin = 8; // 오른쪽 모터 제어선 IN1
// int RightMotor_2_pin = 9; // 오른쪽 모터 제어선 IN2
// int LeftMotor_3_pin = 10; // 왼쪽 모터 제어선 IN3
// int LeftMotor_4_pin = 11; // 왼쪽 모터 제어선 IN4

// void forward(int R_motor, int L_motor, int Speed)
// {
//   digitalWrite(RightMotor_1_pin, R_motor);
//   digitalWrite(RightMotor_2_pin, !R_motor);
//   digitalWrite(LeftMotor_3_pin, L_motor);
//   digitalWrite(LeftMotor_4_pin, !L_motor);

//   analogWrite(RightMotor_E_pin, Speed); // 우측 모터 속도값
//   analogWrite(LeftMotor_E_pin, Speed);  // 좌측 모터 속도값
// }

// void right(int R_motor, int L_motor, int Speed)
// {
//   digitalWrite(RightMotor_1_pin, R_motor);
//   digitalWrite(RightMotor_2_pin, !R_motor);
//   digitalWrite(LeftMotor_3_pin, L_motor);
//   digitalWrite(LeftMotor_4_pin, !L_motor);

//   analogWrite(RightMotor_E_pin, max(Speed * 0.6, 50)); // 우측 모터 속도값
//   analogWrite(LeftMotor_E_pin, min(Speed * 1.4, 200)); // 좌측 모터 속도값
// }

// void left(int R_motor, int L_motor, int Speed)
// {
//   digitalWrite(RightMotor_1_pin, R_motor);
//   digitalWrite(RightMotor_2_pin, !R_motor);
//   digitalWrite(LeftMotor_3_pin, L_motor);
//   digitalWrite(LeftMotor_4_pin, !L_motor);

//   analogWrite(RightMotor_E_pin, min(Speed * 1.4, 200)); // 우측 모터 속도값
//   analogWrite(LeftMotor_E_pin, max(Speed * 0.6, 50));   // 좌측 모터 속도값
// }

// void stop(int R_motor, int L_motor)
// {
//   digitalWrite(RightMotor_1_pin, R_motor);
//   digitalWrite(RightMotor_2_pin, !R_motor);
//   digitalWrite(LeftMotor_3_pin, L_motor);
//   digitalWrite(LeftMotor_4_pin, !L_motor);

//   analogWrite(RightMotor_E_pin, 0); // 우측 모터 속도값
//   analogWrite(LeftMotor_E_pin, 0);  // 좌측 모터 속도값
// }

// void bt_control(char Blue_Val)
// {
//   if (Blue_Val == 'b')
//   {
//     R_Motor = HIGH;
//     L_Motor = HIGH;
//     motor_s = 150;
//     Serial.println("Start");
//     forward(R_Motor, L_Motor, motor_s);
//     led.on();
//   }
//   else if (Blue_Val == 's')
//   {
//     R_Motor = LOW;
//     L_Motor = LOW;
//     Serial.println("stop");
//     stop(R_Motor, L_Motor);
//     led.off();
//   }
//   else
//   {
//     Serial.println("Unknown Command");
//   }
// }

// void setup()
// {
//   pinMode(RightMotor_E_pin, OUTPUT); // 출력모드로 설정
//   pinMode(RightMotor_1_pin, OUTPUT);
//   pinMode(RightMotor_2_pin, OUTPUT);
//   pinMode(LeftMotor_3_pin, OUTPUT);
//   pinMode(LeftMotor_4_pin, OUTPUT);
//   pinMode(LeftMotor_E_pin, OUTPUT);

//   LineTrace();

//   Serial.begin(9600); //PC와 아두이노간 시리얼 통신 속도를 9600bps로 설정
//   bt.begin(9600);     //블루투스와 아두이노간 시리얼 통신 속도를 9600bps로 설정

//   Serial.println("connected");
// }

// void loop(){
//   if (bt.available()){
//     char Blue_Val = bt.read();

//     Serial.print("Input Signal : ");
//     Serial.print("Bluetooth[ ");
//     Serial.print(Blue_Val);
//     Serial.print(" ] \n ");
//     bt_control(Blue_Val);
//     while (true) {
//       // char Blue_Val = bt.read();
      
//       if (Blue_Val == 's') break;
//       if (lt.forward_bool() == true){
//         Serial.println("go");
//         forward(R_Motor, L_Motor, motor_s);
//         led.on();
//       }else if (lt.right_bool() == true){
//         Serial.println("Right turn");
//         right(R_Motor, L_Motor, motor_s);
//       }else if (lt.left_bool() == true){
//         Serial.println("Left turn");
//         left(R_Motor, L_Motor, motor_s);
//       }else if (lt.pause() == true){
//         Serial.println("Pause");
//         led.off();
//         stop(R_Motor, L_Motor);
//         if (prs.pressValue() >= 100){
//           Serial.println("detected on");
//           int press_value_1 = prs.pressValue();
//           delay(1000);
//           led.on();
//           forward(R_Motor, L_Motor, motor_s);
//         }else{
//           delay(5000);
//           Serial.println("Time Out");
//           forward(R_Motor, L_Motor, motor_s);
//           led.off();
//         }
//       }else if (lt.stop_bool() == true){
//         Serial.println("End");
//         stop(R_Motor, L_Motor);
//         break;
//       }else{
//       led.off();
//       Serial.println("Error...");
//       }
//     }
//   } 
//   else {
//     Serial.println("connected fail ");
//   }
// }

// #include <Arduino.h>
// #include <LineTrace.h>
// #include <DcMotor_4.h>
// #include <Pressure.h>
// #include <BtMiniCom.h>
// #include <Led.h>

// void receive(String msg);
// BtMiniCom bt(3, 4, receive);
// LineTrace lt;
// DcMotor_4 car;
// Pressure prs;
// Led led(0);

// void receive(String msg) {
//   Serial.print("msg Command : ");
//   Serial.println(msg.c_str());
//   led.off();

//   if(msg.c_str()=="b") {
//     // while (0) {
//       Serial.print("start");
//       if (lt.forward_bool() == true) {
//         Serial.print("go");
//         car.forward();
//         led.on();
//       } else if (lt.right_bool() == true) {
//         Serial.print("Right turn");
//         car.right();
//       } else if (lt.left_bool() == true) {
//         Serial.print("Left turn");
//         car.left();
//       } else if (lt.pause() == true) {
//         Serial.print("Pause");
//         led.off();
//         car.stop();
//         if (prs.pressValue() >= 100){
//           led.off();
//           Serial.println("detect on");
//           int press_value_1 = prs.pressValue();
//           delay(1000);
//           led.on();
//           car.forward();

//         } else{
//           delay(5000);
//           Serial.println("Time Out");
//           car.forward();
//           led.off();

//         }
//       } else if(lt.stop_bool() == true) {
//         Serial.println("End");
//         car.stop();
//         // break;
//       }
//     }
//    else {
//     led.off();
//     Serial.println("Unknown Command");
//   }
// }

// void setup() {
//   Serial.begin(9600);    // PC와의 시리얼 통신 9600bps로 설정
//   lt.setSpeed(50);
//   // lt.LineValuePrint();
//   lt.ErrFix();
//   bt.init();
// }

// void loop() {
//   // lt.LineValuePrint();
//   bt.run();
// }