
// - Note
//      Measure current acceleration
void Measure()
{
    AccelerationReading a = Bean.getAcceleration();
    ma.x = a.x;
    ma.y = a.y;
    ma.z = a.z;
}

