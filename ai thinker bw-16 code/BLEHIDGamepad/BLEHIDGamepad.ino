#include "BLEHIDDevice.h"
#include "BLEHIDGamepad.h"
#include "BLEDevice.h"

#define UART_BAUDRATE 500000

BLEHIDGamepad padDev;
BLEAdvertData advdata;

#define XINPUT_GAMEPAD_DPAD_UP        0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN      0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT      0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT     0x0008
#define XINPUT_GAMEPAD_START          0x0010
#define XINPUT_GAMEPAD_BACK           0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB     0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB    0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER  0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER 0x0200
#define XINPUT_GAMEPAD_GUIDE          0x0400
#define XINPUT_GAMEPAD_A              0x1000
#define XINPUT_GAMEPAD_B              0x2000
#define XINPUT_GAMEPAD_X              0x4000
#define XINPUT_GAMEPAD_Y              0x8000

uint8_t hid_gamepad_report_descriptor[] = {
    TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(1))
};

void setup() {

    Serial.begin(115200);
    Serial1.begin(UART_BAUDRATE);

    Serial.println("BLE XINPUT BRIDGE");

    advdata.addFlags();
    advdata.addCompleteName("GameSir Bridge");
    advdata.addAppearance(GAP_GATT_APPEARANCE_GAMEPAD);
    advdata.addCompleteServices(BLEUUID(HID_SERVICE_UUID));

    BLEHIDDev.init();

    BLEHIDDev.setReportMap(
        hid_gamepad_report_descriptor,
        sizeof(hid_gamepad_report_descriptor)
    );

    padDev.setReportID(1);

    BLE.init();

    BLE.configAdvert()->setAdvData(advdata);

    BLE.setDeviceName("GameSir Bridge");

    BLE.setDeviceAppearance(
        GAP_GATT_APPEARANCE_GAMEPAD
    );

    BLE.configSecurity()->setPairable(true);

    BLE.configSecurity()->setAuthFlags(
        GAP_AUTHEN_BIT_BONDING_FLAG
    );

    BLE.configServer(3);

    BLE.addService(BLEHIDDev.hidService());
    BLE.addService(BLEHIDDev.battService());
    BLE.addService(BLEHIDDev.devInfoService());

    BLE.beginPeripheral();

    Serial.println("BLE READY");
}

void loop() {

    if (!BLE.connected())
        return;

    if (Serial1.available() < 3)
        return;

    if (Serial1.read() != 0xAA)
        return;

    if (Serial1.read() != 0xBB)
        return;

    uint8_t len = Serial1.read();

    if (len != 12)
        return;

    while (Serial1.available() < 12);

    uint8_t buf[12];

    Serial1.readBytes(buf,12);

    uint16_t x_btns =
        buf[0] | (buf[1]<<8);

    uint8_t lt = buf[2];
    uint8_t rt = buf[3];

    int16_t lx =
        (int16_t)(buf[4] | (buf[5]<<8));

    int16_t ly =
        (int16_t)(buf[6] | (buf[7]<<8));

    int16_t rx =
        (int16_t)(buf[8] | (buf[9]<<8));

    int16_t ry =
        (int16_t)(buf[10] | (buf[11]<<8));

    uint32_t buttons = 0;

    if(x_btns & XINPUT_GAMEPAD_A)
        buttons |= GAMEPAD_BUTTON_1;

    if(x_btns & XINPUT_GAMEPAD_B)
        buttons |= GAMEPAD_BUTTON_2;

    if(x_btns & XINPUT_GAMEPAD_X)
        buttons |= GAMEPAD_BUTTON_3;

    if(x_btns & XINPUT_GAMEPAD_Y)
        buttons |= GAMEPAD_BUTTON_4;

    if(x_btns & XINPUT_GAMEPAD_LEFT_SHOULDER)
        buttons |= GAMEPAD_BUTTON_5;

    if(x_btns & XINPUT_GAMEPAD_RIGHT_SHOULDER)
        buttons |= GAMEPAD_BUTTON_6;

    if(x_btns & XINPUT_GAMEPAD_BACK)
        buttons |= GAMEPAD_BUTTON_7;

    if(x_btns & XINPUT_GAMEPAD_START)
        buttons |= GAMEPAD_BUTTON_8;

    if(x_btns & XINPUT_GAMEPAD_LEFT_THUMB)
        buttons |= GAMEPAD_BUTTON_9;

    if(x_btns & XINPUT_GAMEPAD_RIGHT_THUMB)
        buttons |= GAMEPAD_BUTTON_10;

    if(x_btns & XINPUT_GAMEPAD_GUIDE)
        buttons |= GAMEPAD_BUTTON_11;

    uint8_t hat = GAMEPAD_HAT_CENTERED;

    bool up =
        x_btns & XINPUT_GAMEPAD_DPAD_UP;

    bool down =
        x_btns & XINPUT_GAMEPAD_DPAD_DOWN;

    bool left =
        x_btns & XINPUT_GAMEPAD_DPAD_LEFT;

    bool right =
        x_btns & XINPUT_GAMEPAD_DPAD_RIGHT;

    if(up && right)
        hat = GAMEPAD_HAT_UP_RIGHT;

    else if(right && down)
        hat = GAMEPAD_HAT_DOWN_RIGHT;

    else if(down && left)
        hat = GAMEPAD_HAT_DOWN_LEFT;

    else if(left && up)
        hat = GAMEPAD_HAT_UP_LEFT;

    else if(up)
        hat = GAMEPAD_HAT_UP;

    else if(right)
        hat = GAMEPAD_HAT_RIGHT;

    else if(down)
        hat = GAMEPAD_HAT_DOWN;

    else if(left)
        hat = GAMEPAD_HAT_LEFT;

    int16_t outLT =
        map(lt,0,255,-32767,32767);

    int16_t outRT =
        map(rt,0,255,-32767,32767);

    padDev.gamepadReport(
        buttons,
        hat,
        lx,
        -ly,
        outLT,
        outRT,
        rx,
        -ry
    );
}