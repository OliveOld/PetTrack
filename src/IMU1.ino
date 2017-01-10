// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  File    : IMU1.ino
//  Updated : 17/01/07
//  Author  : 
//      Team OliveOld
//      - Kim Min Ji
//      - Yoon Tae Yeon
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      Test code for unit, integration testing
//
//  Status
//      - Program size  :  26  of 30,720 byte
//      - Memory usage  :  29  of  2,048 byte
//
//  To Do
//      - Bluetooth reliablility Test
//      - Filtering for Raw values
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

#include "Wire.h"

#include "./alias.h"
#include "./utils/stop_watch.hpp"

#include "./module/led.h"

#include "./module/bluetooth.h"
#include "./module/mpu9250.h"
#include "./Sample.h"


// Global Constants
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

static constexpr auto baud     = 9600;
static constexpr auto sec      = 1000;     // 1 second is 1000 ms
static constexpr auto hz       = 30;       // sampling in 1 sec
static constexpr u32  looptime = sec / hz; // milisec time for 1 loop

static constexpr auto buf_size = hz;       // Sampling capacity.

// D4 : Arduino(recv) << Bluetooth(Tx)
// D3 : Arduino(send) >> Bluetooth(Rx)
static constexpr u8 rxPin = 4;
static constexpr u8 txPin = 3;


// Global Variables
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

namespace pets
{
    // modules
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

    olive::bluetooth    bth{ txPin, rxPin };
    olive::mpu9250      mpu{};

    // Classifier
    //classifier      cl{};

    stop_watch  watch{};

    // Built-in LED
    led         led13{ LED_BUILTIN };

}
using namespace pets;

//static i8          ctr = 0;
//static SampleData  buffer[buf_size]{};




// Function Signature
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup();
void loop();



//  Function Definition
//      - setup
//      - loop
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
	Wire.begin();
    
    bth.begin(baud);
    Serial.begin(baud);

    led13.init();
    mpu.init();

    delay(1000);
    Serial.println("Setup Done");
}

// - Pattern
//      Game Loop 
void loop()
{
    // 1. Process Input
    // ---- ---- ---- ---- ---- ---- ---- ---- ----

    //while (bth.available()) {
    //    // Read a message byte-by-byte
    //    char ch = static_cast<char>(bth.read());
    //    Serial.print(ch);
    //    //bth.print(ch);
    //    delay(5);
    //}


    // 2. Update Moduels & Variables
    //      Process input from sensor + buffering
    // ---- ---- ---- ---- ---- ---- ---- ---- ----

    //ctr = (ctr + 1) % buf_size;
    //u32 elapsed = watch.reset();
    mpu.update();   // Refresh MPU9250 
	//buffer[ctr] = sanpshot(mpu); // Save to buffer
    SampleData sample = sanpshot(mpu); 


    // 3. Render(Draw) 
    //      Transmit sampling data
    // ---- ---- ---- ---- ---- ---- ---- ---- ----

    //if (ctr == buf_size - 1)
    //{
        watch.reset();

        //led13.on();
        //for (int i = 0; i < buf_size; ++i) {
            print(bth, sample);
            bth.print('\n');
            //bth.println();
            //print(Serial, sample);
            //Serial.println();
        //}
        delay(1);
        bth.flush();
        //Serial.flush();
        //led13.off();

        Serial.println(watch.reset());
    //}

    // Adjust speed
    // ---- ---- ---- ---- ----
    //if (elapsed < looptime) {
    //    delay(looptime - elapsed);
    //}

}





