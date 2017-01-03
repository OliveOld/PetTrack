#include "Wire.h"
#include "I2Cdev.h"
#include "MPU9250.h"
#include <SoftwareSerial.h>

/* 블루투스 */
#define sample_num_mdate  5000
#define rxPin 3 // 받는핀
#define txPin 2  // 보내는핀
#define ledPin 13 // 내장 LED
#define BUFF_SIZE 30 // 데이터(문자열)을 받을 버퍼의 크기.

uint8_t index = 0;
uint8_t data;
SoftwareSerial swSerial(txPin, rxPin); //시리얼 통신을 위한 객체선언
void setup_bluetooth(void);
bool ready_BT(void);


/* MPU센서 */
MPU9250 accelgyro;
I2Cdev   I2C_M;
uint8_t buffer_m[6];
struct Sample_data
{
  float s_ax, s_ay, s_az;
  float s_gx, s_gy, s_gz;
};
Sample_data buffer[30] = {0, };

// for raw data
int16_t r_ax, r_ay, r_az;
int16_t r_gx, r_gy, r_gz;

int ctrl = 0;

void setup()
{
    Wire.begin();

    /* MPU 센서 셋업 */
    accelgyro.initialize(); 

    /* 블루투스 셋업 */
    setup_bluetooth();
    //delay(1000);
}

void setup_bluetooth(void)
{
    Serial.begin(9600);   //시리얼모니터 
    swSerial.begin(9600);  //  115200 
    pinMode(ledPin, OUTPUT); // ledPin = 13
}

bool ready_BT(void)
{
  if (swSerial.available())
    return true;
  else
    return false;
}

Sample_data make_Sample(void)
{
    Sample_data smp;
    
    accelgyro.getMotion6(&r_ax, &r_ay, &r_az, &r_gx, &r_gy, &r_gz);
    
    smp.s_ax = (double) r_ax / 16384;
    smp.s_ay = (double) r_ay / 16384;
    smp.s_az = (double) r_az / 16384;

    smp.s_gx = (double) r_gx * 250 / 32768;
    smp.s_gy = (double) r_gy * 250 / 32768;
    smp.s_gz = (double) r_gz * 250 / 32768;

    return smp;  
}

void print_swSerial(Sample_data bf[])
{
  swSerial.println("Accel");
  for(int i=0; i<30; i++) {
    swSerial.print(bf[i].s_ax);
    swSerial.print(", ");
    swSerial.print(bf[i].s_ay);
    swSerial.print(", ");
    swSerial.print(bf[i].s_az);
    swSerial.print("\n");
  }
  
  swSerial.println("Gyro");
  for(int i=0; i<30; i++) {
    swSerial.print(bf[i].s_gx);
    swSerial.print(", ");
    swSerial.print(bf[i].s_gy);
    swSerial.print(", ");
    swSerial.print(bf[i].s_gz);
    swSerial.print("\n");
  }
}

void print_Serial(Sample_data bf[])
{
  for(int i=0; i<30; i++) {
    Serial.print(bf[i].s_ax);
    Serial.print(", ");
    Serial.print(bf[i].s_ay);
    Serial.print(", ");
    Serial.print(bf[i].s_az);
    Serial.print("\n");
  }
  
  for(int i=0; i<30; i++) {
    Serial.print(bf[i].s_gx);
    Serial.print(", ");
    Serial.print(bf[i].s_gy);
    Serial.print(", ");
    Serial.print(bf[i].s_gz);
    Serial.print("\n");
  }
}
void loop() 
{
    // Process input from sensor + buffering
    Sample_data smp = make_Sample();
    buffer[ctrl] = smp; // Save to buffer
    
    if( ctrl++ == 29 )
    {
        print_swSerial(buffer);
        print_Serial(buffer);
        delay(1);     // 9600bps 기준으로 delay 를 1ms 을 줘야 한다, 수신 문자열 끊김 방지
        ctrl=0;
    }
}
