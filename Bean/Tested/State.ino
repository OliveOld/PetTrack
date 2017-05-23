// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      Skeleton code of state machine
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

typedef void *(*State)(void);

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
    OP_Train, // 2
    OP_Sync,  // 3
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void *Monitoring(); // Monitoring

void *Connected();    // Read Message prefix and transit to Report, Train, Sync
void *Disconnect();   // Start disconnecting
void *Disconnected(); // Disconnected

void *follow(int prefix); 
void *Reporting(); // Response to App's request
void *Training();  // Receive posture guide from App
void *Syncing();   // Synchronize with App's data

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void *Monitoring()
{
    if (Serial.available())
    {
        return (void *)Connected;
    }

    // - Measure
    // - Pre-process

    // - Posture
    // - if static
    //      Orientation
    // - Posture = Posture + Orientation

    // - Pick Timer
    // - Record Posture/Time

    return (void *)Monitoring;
}

// ---- ---- ---- ---- ----

void *follow(int prefix)
{
    // report -> Reporting
    if (prefix == OP_Report)
        return (void *)Reporting;
    // train -> Training
    else if (prefix == OP_Train)
        return (void *)Training;
    // sync -> Syncing
    else if (prefix == OP_Sync)
        return (void *)Syncing;
    // else -> Disconnect
    else
        return (void *)Disconnect;
}

unsigned lag = 0;
void *Connected()
{
    // Wait for message
    while (Serial.available() == 0)
    {
        // Bean.sleep(200);
        lag += 200;

        // Wait up to 3 seconds
        if (lag > 3000)
            return (void *)Disconnected;
    }

    // Read message prefix and transit to next
    return (void *)follow(pack.prefix);
}

void *Disconnect()
{
    // Send Disconnect Message
    Serial.write((uint8_t)OP_Discon);

    // LED off & Manual Disconnect
    Bean.setLed(0,0,0);
    Bean.disconnect();

    return (void *)Disconnected;
}

void *Disconnected()
{
    // Do stuff...
    return (void *)Monitoring;
}

// ---- ---- ---- ---- ----

void *Reporting()
{
    // Red
    Bean.setLed(150, 0, 0);

    // Handle report request
    while (pack.prefix != OP_Report)
    {
        // Recv
        {
        }
        // Send
        {
        }
        // pack.prefix;
    }
    return (void *)follow(pack.prefix);
}

void *Training()
{
    // Green
    Bean.setLed(0, 150, 0);

    // Handle training message
    while (pack.prefix != OP_Train)
    {
        // Recv
        {
        }
        // Send
        {
        }
        // pack.prefix;
    }
    return (void *)follow(pack.prefix);
}

void *Syncing()
{
    // Blue
    Bean.setLed(0, 0, 150);

    // Handle sync message
    while (pack.prefix != OP_Sync)
    {
        // Recv
        {
        }
        // Send
        {
        }
        // pack.prefix;
    }
    return (void *)follow(pack.prefix);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    state = Monitoring;
    // Update sensor in a 100ms period
    Bean.setAccelerometerPowerMode(0x5A);
}

void loop()
{
    void *next = (*state)();
    state = (State)next;
}