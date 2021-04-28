#pragma once

#include <iostream>
#include <Arduino.h>

int RightMotor_E_pin = 5;      // 오른쪽 모터의 Enable & PWM
int LeftMotor_E_pin = 6;       // 왼쪽 모터의 Enable & PWM
int RightMotor_1_pin = 8;      // 오른쪽 모터 IN1
int RightMotor_2_pin = 9;      // 오른쪽 모터 IN2
int LeftMotor_3_pin = 10;      // 왼쪽 모터 IN3
int LeftMotor_4_pin = 11;      // 왼쪽 모터 IN4

int L_Line = A5; // 왼쪽 라인트레이서
int C_Line = A4; // 가운데 라인트레이서
int R_Line = A3; // 오른쪽 라인트레이서

int motor_s = 120;     // 모터 속도 최대 255

int SL = 1;
int SC = 1;
int SR = 1;

void motor_role(int R_motor, int L_motor, int Speed) {
  digitalWrite(RightMotor_1_pin, R_motor);
  digitalWrite(RightMotor_2_pin, !R_motor);
  digitalWrite(LeftMotor_3_pin, L_motor);
  digitalWrite(LeftMotor_4_pin, !L_motor);

  analogWrite(RightMotor_E_pin, Speed);  // 우측 모터 속도값
  analogWrite(LeftMotor_E_pin, Speed);   // 좌측 모터 속도값
}

void Right_role(int R_motor, int L_motor, int Speed) {
  digitalWrite(RightMotor_1_pin, R_motor);
  digitalWrite(RightMotor_2_pin, !R_motor);
  digitalWrite(LeftMotor_3_pin, L_motor);
  digitalWrite(LeftMotor_4_pin, !L_motor);

  analogWrite(RightMotor_E_pin, max(Speed * 0.4, 50)); // 우측 모터 속도값
  analogWrite(LeftMotor_E_pin, min(Speed * 1.4, 255)); // 좌측 모터 속도값
}
void Left_role(int R_motor, int L_motor, int Speed) {
  digitalWrite(RightMotor_1_pin, R_motor);
  digitalWrite(RightMotor_2_pin, !R_motor);
  digitalWrite(LeftMotor_3_pin, L_motor);
  digitalWrite(LeftMotor_4_pin, !L_motor);

  analogWrite(RightMotor_E_pin, min(Speed * 1.4, 255)); // 우측 모터 속도값
  analogWrite(LeftMotor_E_pin, max(Speed * 0.2, 50)); // 좌측 모터 속도값
}
