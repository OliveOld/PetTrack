
typedef void *PTR;
typedef PTR (*State)(void);

State state;
unsigned lag = 0;

struct Packet
{
    uint8_t prefix;
    uint8_t param;
    uint32_t value;
};
struct Packet pack;

enum Oper
{
    Discon = -1,
    Report = 1,
    Train,
    Sync,
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

PTR Monitoring();   // Monitoring

PTR Connected();    // Read Message prefix and transit to Report, Train, Sync
PTR Disconnect();   // Start disconnecting
PTR Disconnected(); // Disconnected

PTR Reporting();    // Response to App's request
PTR Training();     // Receive posture guide from App
PTR Syncing();      // Synchronize with App's data

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

PTR Monitoring()
{
    if (Serial.available())
    {
        return (PTR)Connected;
    }

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

PTR follow(uint8_t prefix)
{
    switch (prefix)
    {
    case Oper::Report:  return (PTR)Reporting;
    case Oper::Train:   return (PTR)Training;
    case Oper::Sync:    return (PTR)Syncing;
    default:            return (PTR)Disconnect;
    }
}

PTR Connected()
{
    // Wait for message
    while(Serial.available() == 0)
    {
        // Bean.sleep(200);
        delay(200);
        lag += 200;
        
        // Wait up to 3 seconds
        if(lag > 3000)
            return (PTR)Disconnected;
    }

    // Read message prefix and transit to next

    return follow(pack.prefix);
}

PTR Disconnect()
{
    // Send Disconnect Message
    Serial.write((uint8_t)Oper::Discon);

    // LED off & Manual Disconnect
    // Bean.setLed(0,0,0);
    // Bean.disconnect();

    return (PTR)Disconnected;
}

PTR Disconnected()
{
    return (PTR)Monitoring;
}


// ---- ---- ---- ---- ----

PTR Reporting()
{
    // Red
    // Bean.setLed(150, 0, 0);

    // Handle report request
    while (pack.prefix != Oper::Report)
    {
        // Recv
        {
        }
        // Send
        {
        }
        // pack.prefix;
    }

    return follow(pack.prefix);
}

PTR Training()
{
    // Green
    // Bean.setLed(0, 150, 0);

    // Handle training message
    while (pack.prefix != Oper::Train)
    {
        // Recv
        {
        }
        // Send
        {
        }
        // pack.prefix;
    }

    return follow(pack.prefix);
}

PTR Syncing()
{
    // Blue
    // Bean.setLed(0, 0, 150);
    
    // Handle sync message
    while (pack.prefix != Oper::Sync)
    {
        // Recv
        {
        }
        // Send
        {
        }
        // pack.prefix;
    }

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
