// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      Skeleton code of state machine
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

typedef void *PTR;
typedef PTR (*State)(void);

State state;

struct Packet
{
    uint8_t prefix;
    uint8_t param;
    uint32_t value;
};
struct Packet pack;

enum Oper
{
    OP_Discon = -1,
    OP_Report = 1,
    OP_Train,
    OP_Sync,
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

PTR Monitoring(); // Monitoring

PTR Connected();    // Read Message prefix and transit to Report, Train, Sync
PTR Disconnect();   // Start disconnecting
PTR Disconnected(); // Disconnected

PTR Reporting(); // Response to App's request
PTR Training();  // Receive posture guide from App
PTR Syncing();   // Synchronize with App's data

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

PTR Monitoring()
{
    if (Serial.available()) return (PTR)Connected;

    // Measure
    // Pre-process

    // Posture
    // if static
    //      Orientation
    // Posture = Posture + Orientation

    // Pick Timer
    // Record Posture/Time

    return (PTR)Monitoring;
}

// ---- ---- ---- ---- ----

PTR follow(int prefix)
{
    if(prefix == OP_Report){
        return (PTR)Reporting;
    }
    else if(prefix == OP_Train)
    {
        return (PTR)Training;
    }
    return (PTR)Monitoring;
    // switch (prefix)
    // {
    // case (int)OP_Report:
    //     return (PTR)Reporting;
    // case (int)OP_Train:
    //     return (PTR)Training;
    // case (int)OP_Sync:
    //     return (PTR)Syncing;
    // default:
    //     return (PTR)Disconnect;
    // }
}

unsigned lag = 0;
PTR Connected()
{
    // Wait for message
    while (Serial.available() == 0)
    {
        // Bean.sleep(200);
        delay(200);
        lag += 200;

        // Wait up to 3 seconds
        if (lag > 3000)
            return (PTR)Disconnected;
    }

    // Read message prefix and transit to next

    return follow(pack.prefix);
}

PTR Disconnect()
{
    // Send Disconnect Message
    Serial.write((uint8_t)OP_Discon);

    // LED off & Manual Disconnect
    // Bean.setLed(0,0,0);
    // Bean.disconnect();

    return (PTR)Disconnected;
}

PTR Disconnected()
{
    // Do stuff...
    return (PTR)Monitoring;
}

// ---- ---- ---- ---- ----

PTR Reporting()
{
    // Red
    // Bean.setLed(150, 0, 0);

    // Handle report request
    // while (pack.prefix != OP_Report)
    // {
    //     // Recv
    //     {
    //     }
    //     // Send
    //     {
    //     }
    //     // pack.prefix;
    // }

    return follow(pack.prefix);
}

PTR Training()
{
    // Green
    // Bean.setLed(0, 150, 0);

    // Handle training message
    // while (pack.prefix != OP_Train)
    // {
    //     // Recv
    //     {
    //     }
    //     // Send
    //     {
    //     }
    //     // pack.prefix;
    // }

    return follow(pack.prefix);
}

PTR Syncing()
{
    // Blue
    // Bean.setLed(0, 0, 150);

    // Handle sync message
    // while (pack.prefix != OP_Sync)
    // {
    //     // Recv
    //     {
    //     }
    //     // Send
    //     {
    //     }
    //     // pack.prefix;
    // }

    return follow(pack.prefix);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    state = Monitoring;
    // Update sensor in a 100ms period
    // Bean.setAccelerometerPowerMode(0x5A);
}

void loop()
{
    PTR next = (*state)();
    state = (State)next;
}