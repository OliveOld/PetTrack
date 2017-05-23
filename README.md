# PetTrack
 
## Communication Protocol

### 1. Packet Structure
Device와 App의 통신을 위한 정보는 3 부분으로 구성되며, 필요에 의해 1 byte만 전송되거나, 최대 6 byte를 전송한다.

```c++
struct Packet
{
    uint8_t  prefix;
    uint8_t  param;
    uint32_t value;
};
```

 - `prefix`: Operation Code가 기록된다. 
   - Monitoring
   - Reporting
   - Training
   - Disconnect
 - `param`: Operation을 위한 인자. 상위 4비트는 자세, 하위 4비트는 속성을 지정하는데 사용된다.
 - `value`: 값을 전달하거나 받는경우 사용한다. 이 값은 실제로는 signed 일 수 있으나, Bit Shift가 필요한 경우를 고려하여 4 byte unsigned 정수를 사용한다.

#### Prefix
```c++
enum Oper
{
    OP_Discon = -1,
    OP_Report = 1,
    OP_Train,   // 2
    OP_Sync,    // 3
};
```

#### 자세(Posture)
```c++
const int Postures = 8;
enum Pos
{
    P_Unknown = 0,
    P_Lie,     // 1
    P_LieSide, // 2
    P_LieBack, // 3
    P_Sit,     // 4
    P_Stand,   // 5
    P_Walk,    // 6
    P_Run      // 7
};
```

#### 속성(Attribute)
```c++
enum Attr
{
    A_Mean = 1,
    A_Stdev,    // 2
    A_Time      // 3
};
```

### 2. Monitoring
 - 기본상태

모니터링 단계에서는 통신이 없으며, Bluetooth 연결이 확인되는 즉시 다른 상태로 전이한다. Bluetooth 연결이 종료되거나, 일정시간 후에도 통신이 없는 경우 다시 이 상태로 돌아온다.


### 3. Reporting
- Device -->> App

Device내에 저장된 값을 App으로 받는다.

#### Request(App)
```
 [ Report(1) ][ Posture|Attribute ]
```
2 byte. `prefix`는 Report를 의미하는 정수값(1)이며, `param`에 자세(Posture)와 속성(Attribute)를 지정한다.

#### Response(Device)
```
 [ Report(1) ][ Posture|Attribute ][ Value(4 byte) ]
```
6 byte. Request로 받은 `prefix`와 `param`을 그대로 사용하며, 적합한 값을 `value`에 채워서 전송한다.


### 4. Training
- Device <<-- App

현재 강아지의 자세를 알려주는 용도로 사용한다.

#### Request(App)
```
 [ Train(2) ][ Posture|Attribute ]
```
2 byte. Attribute 부분은 무시된다. App 측에서 최초의 메세지를 완전히(`param`이 빠진 경우) 전송하지 않으면, 일정 시간 후 Monitoring 상태로 변경된다.

Device는 한번 메세지를 받은 이후로는 반려견이 마지막으로 받은 자세를 유지하고 있다고 가정한다.

#### Response(Device)
```
 [ Train(2) ][ Posture|Attribute ]
```
2 byte. App의 메세지를 수신/처리했다는 의미(Ack)로 사용된다. 

### 5. Synchronizing
- Device <<-- App

Device는 RAM을 사용하므로 전력이 공급되지 않으면 기록된 값을 잃게 되며, 이 경우 다시 Training을 해야하는 문제가 생길 수 있다. 이를 위해 기기에서 값을 보내 Device의 메모리를 갱신하는 기능이 필요하며, Sync는 이를 지원한다.

#### Request(App)
```
 [ Sync(3) ][ Posture|Attribute ][ Value(4 byte) ]
```
6 byte. 현재 자세, 속성, 값을 전송한다.

#### Response(Device)
```
 [ Sync(3) ][ Posture|Attribute ]
```
2 byte. 해당 자세/속성에 대해 기록을 마친 경우 응답을 위해 전송된다.


### 6. Disconnect
- Device <<-->> App 

패킷의 `prefix`, `param` 잘못된 경우, Device는 오류를 별도로 처리하지 않고 연결을 종료한다.  
혹은 명시적으로 App 측에서 연결을 종료할 수 있다. 먼저 1 byte를 전송하는 측은 1 byte를 수신함으로써 서로 연결이 종료함을 알 수 있다. (수신하지 못하는 경우 timeout 처리가 필요)

#### Request(App or Device)
```
 [ Disconn(-1) ]
```
1 byte.

#### Response(App or Device)
```
 [ Disconn(-1) ]
```
1 byte.

