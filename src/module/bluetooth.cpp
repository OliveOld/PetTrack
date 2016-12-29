// 
// 
// 

#include "./bluetooth.h"


bluetooth::bluetooth(u8 _send, u8 _recv) :
    SoftwareSerial(_recv, _send),
    send_pin{_send}, recv_pin{_recv}
{

}

void bluetooth::init(u32 baud)
{
    this->begin(baud);
}

