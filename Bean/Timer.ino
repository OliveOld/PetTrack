

// - Note
//      Simple timer for Arduino
class Timer
{
    uint32_t start;

  public:
    Timer()
    {
        start = millis();
    };
    // - Note
    //      Acquire elapsed time in millisecond
    uint32_t pick() const
    {
        const uint32_t curr = millis();
        return curr - start; // elapsed
    }
    // - Note
    //      Acquire elapsed time and reset the stop watch
    uint32_t reset()
    {
        uint32_t span = pick();
        start = millis();
        return span;
    }
};

Timer timer;

void setup()
{
    timer.reset();
}

void loop()
{
    Bean.sleep(500);
    Serial.print(timer.reset());
}