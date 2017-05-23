// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      가속도 센서의 값을 읽고, Low Pass Filter를 적용하여
//      측정값(MA), 중력 가속(G), 신체 가속(LA)를 분리한다.
//      MA = LA + G
//
//  Note
//      Low Pass Filter
//      낮은 주파수(Low Frequency)만 통과시킨다. 이는 긴 주기의 파동과 같다.
//      실제환경에서 이는 지속적으로 작용하는(따라서 주기가 긴) 중력을
//      의미하며, 따라서 LPF는 중력 성분만을 필터링하게 된다.
//
//      G = filter(MA)
//
//      이론상으로는 필터링을 통해 중력성분 만 획득하는 것이 가능하지만,
//      실제로는 중력 이외의 성분, 즉 High Frequency 성분을 희석(Attenuate)
//      시키는 것이다.
//      그 수준을 결정하기 위해 alpha값이 적절히 결정되어야 하며, 이 코드는
//      0.8을 사용한다. 따라서...
//
//      G_new = (0.8 * G_old) + (0.2 * MA)
//            = (0.8 * G_old) + (0.2 * (LA + G))
//            =  1.0 * G + 0.2 * LA
//
//      결과적으로 새로 유도한 G 에는 (1-alpha) 만큼의 LA가 포함된다.
//      이렇게 유도한 G를 MA에서 제외시킴으로써 LA역시 유도가 가능하다.
//
//      LA_new = MA - G_new
//
//  Caution
//      출력 형태
//      { MA.x, MA.y, MA.z } { G.x, G.y, G.z } { LA.x, LA.y, LA.z }
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void Println(int x, int y, int z)
{
    Serial.print(x);
    Serial.print(',');
    Serial.print(y);
    Serial.print(',');
    Serial.print(z);
    Serial.print('\n');
    delay(5);
}

struct Unit
{
    int x, y, z;

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
    int alpha;

    Unit la; // Linear Acceleration
    Unit g;  // Gravity Acceleration
  public:
    LPF3A()
    {
        alpha = 0; // Caution : 0 by default
        la.x = la.y = la.z = 0;
        g.x = g.y = g.z = 0;
    }

    int factor(int x)
    {
        return (x * alpha) / 1000;
    }
    // - Note
    //      Receive Measured accceleration,
    //      then update Linear/Gravity acceleration.
    void separate(int mx, int my, int mz)
    {
        // G = a*G + (1-a)* MA
        g.x = mx + factor(g.x - mx);
        g.y = my + factor(g.y - my);
        g.z = mz + factor(g.z - mz);
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
    filter.alpha = 759; // 0.759
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
    Println(ma.x, ma.y, ma.z);

    // Bean.sleep(200); // ~5 Hz
}