// Pets.h

#ifndef _PETS_h
#define _PETS_h

#include <Arduino.h>
#include "./stop_watch.h"

// - Note
//      Posture
enum class Post  : u8
{
    Unknown,
    Sit,  Lie,
    Stand,
    Walk, Run,
};
static constexpr auto plength = 6;

static constexpr bool is_unknown(Post pos) noexcept
{
    return pos == Post::Unknown;
}

static constexpr bool is_idle(Post pos) noexcept
{
    return pos == Post::Sit 
        || pos == Post::Lie 
        || pos == Post::Stand;
}

static constexpr bool is_active(Post pos) noexcept 
{
    return is_unknown(pos) == false 
        && is_idle(pos) == false;
}

// - Note
//      Orientation
enum class Ori : u8
{
    Unknown,
    Side,
    Bottom, Top,
    Front,  Back
};

struct Unit
{
    float x, y, z;
};

// - Note
//      3-Axis Low Pass Filter
class LPF3A
{
public:
    float alpha;

    Unit la{};  // Linear Acceleration
    Unit g{};   // Gravity Acceleration
public:
    explicit LPF3A(float _alpha) : alpha{ _alpha } {}

    // - Note
    //      Receive Measured accceleration,
    //      then update Linear/Gravity acceleration.
    void separate(const Unit& ma) noexcept;
};


class Classifier
{
public:
    Post classify(Unit& ma) noexcept 
    {
        return Post::Unknown;
    }
};


class Pets
{
public:
    u8   Hz = 10;
    bool connected = false;
    duration   time[plength]{}; // Duration per Postures
public:
    void init() noexcept;
    void reset() noexcept;
    
    void sleep(duration ms) noexcept;

    // - Note
    //      Report current state
    // - Return
    //      true  : successfully reported
    //      false : failed to report
    bool report(Stream& out) noexcept;

    void slower(u8 minHz) noexcept;
    void faster(u8 maxHz) noexcept;

    // - Note
    //      Detect posture from Linear acceleration
    Post posture(const Unit& la) const noexcept;

    // - Note
    //      Detect posture orientation from Gravity accleration
    Ori  orientation(const Unit& g) const noexcept;

    // - Note
    //      Add specific posture's duration
    void record(Post pos, duration d) noexcept;
};



#endif

