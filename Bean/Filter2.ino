// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      필터로 분리해낸 LA에 SMV, SMA 유도식을 적용하는 코드
//
//  See Also
//      Filter.ino
//
//  Caution
//      출력 형태
//      { LA.x, LA.y, LA.z } SMV, SMA
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void print(float x, float y, float z)

struct Unit
{
    float x, y, z;

    Unit()
    {
        x = y = z = 0;
    }
};

// - Note
//      3-Axis Low Pass Filter
class LPF3A
{
  public:
    float alpha;

    Unit la; // Linear Acceleration
    Unit g;  // Gravity Acceleration
  public:
    LPF3A()
    {
        alpha = 0; // Caution : 0 by default
        la.x = la.y = la.z = 0;
        g.x = g.y = g.z = 0;
    }

    // - Note
    //      Receive Measured accceleration,
    //      then update Linear/Gravity acceleration.
    void separate(float mx, float my, float mz)
    {
        // G = a*G + (1-a)* MA
        g.x = mx + alpha * (g.x - mx);
        g.y = my + alpha * (g.y - my);
        g.z = mz + alpha * (g.z - mz);
        // LA = MA - G
        la.x = mx - g.x;
        la.y = my - g.y;
        la.z = mz - g.z;
    }
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

LPF3A filter;

void setup()
{
    // Attenuation Ratio
    filter.alpha = 0.8;
}

void loop()
{

    // Measure acceleration
    Unit ma;
    ma.x = Bean.getAccelerationX();
    ma.y = Bean.getAccelerationY();
    ma.z = Bean.getAccelerationZ();

    // Filtering
    filter.separate(ma.x, ma.y, ma.z);
    Unit g = filter.g;   // Gravity Acceleration
    Unit la = filter.la; // Linear Acceleration

    // Report
    print(ma.x, ma.y, ma.z);
    print(la.x, la.y, la.z);
    print(g.x, g.y, g.z);
    Serial.println();

    Bean.sleep(200); // ~5 Hz
}
void print(float x, float y, float z)
{
    Serial.print("{");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(",");
    Serial.print(z);
    Serial.print("}");
    delay(5);
}
