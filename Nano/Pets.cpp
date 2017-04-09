// 
// 
// 

#include "Pets.h"

void LPF3A::separate(const Unit & ma) noexcept
{
    // G = a*G + (1-a)* MA
    g.x = ma.x + alpha * (g.x - ma.x);
    g.y = ma.y + alpha * (g.y - ma.y);
    g.z = ma.z + alpha * (g.z - ma.z);
    // LA = MA - G
    la.x = ma.x - g.x;
    la.y = ma.y - g.y;
    la.y = ma.y - g.y;
}

void Pets::init() noexcept
{
    this->reset();
}

void Pets::reset() noexcept
{
    this->Hz = 10;
    connected = false;
    for(auto i = 0; i< plength; ++i){
        time[i] = 0;
    }
}


void Pets::sleep(duration ms) noexcept
{
    delay(ms);
}

bool Pets::report(Stream & out) noexcept
{
    out.print("Hz : ");
    out.println(Hz);

    for (auto i = 0; i < plength; ++i) {
        out.print("Posture ");
        out.print(i);
        out.print(" : ");
        out.println(time[i]);
    }
    out.flush();
    return true;
}

void Pets::slower(u8 minHz) noexcept
{
    if (Hz > minHz) {
        Hz -= 1;
    }
}

void Pets::faster(u8 maxHz) noexcept
{
    if (Hz < maxHz) {
        Hz += 1;
    }
}

void Pets::record(Post pos, duration d) noexcept
{
    u8 idx = static_cast<u8>(pos);
    time[idx] += d;
}

Post Pets::posture(const Unit & u) const noexcept
{
    return Post::Unknown;
}

Ori Pets::orientation(const Unit & u) const noexcept
{
    return Ori::Unknown;
}

