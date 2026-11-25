#include <Ps3Controller.h>

#define LED_PIN      2
#define UART_TX      17
#define UART_RX      16
#define UART_BAUD    9600

#define COLLISION_MS 300

uint8_t       player_led      = 1;
uint8_t       speed_val       = 0;
unsigned long collision_until = 0;
unsigned long last_check_ms   = 0;
unsigned long last_sent_ms    = 0;
uint8_t       last_big        = 0;
uint8_t       last_small      = 0;
bool          force_send      = false;

void notify() {
    uint8_t lx = (uint8_t)(Ps3.data.analog.stick.lx + 128);
    Serial2.write(0x01);
    Serial2.write(lx);

    uint8_t buttons = 0;
    if (Ps3.data.button.cross)    buttons |= (1 << 0);
    if (Ps3.data.button.circle)   buttons |= (1 << 1);
    if (Ps3.data.button.square)   buttons |= (1 << 2);
    if (Ps3.data.button.triangle) buttons |= (1 << 3);
    if (Ps3.data.button.l1)       buttons |= (1 << 4);
    if (Ps3.data.button.r1)       buttons |= (1 << 5);
    if (Ps3.data.button.start)    buttons |= (1 << 6);
    if (Ps3.data.button.select)   buttons |= (1 << 7);

    Serial2.write(0x02);
    Serial2.write(buttons);

    Serial2.write(0x03);
    Serial2.write((uint8_t)Ps3.data.analog.button.l2);

    Serial2.write(0x04);
    Serial2.write((uint8_t)Ps3.data.analog.button.r2);
}

void onConnect() {
    digitalWrite(LED_PIN, HIGH);
    player_led = 1;
    Ps3.setPlayer(1);
}

void sendRumble(uint8_t left, uint8_t right) {
    ps3_cmd_t cmd = {};
    cmd.rumble_left_intensity  = left;
    cmd.rumble_left_duration   = left  ? 0xff : 0;
    cmd.rumble_right_intensity = right;
    cmd.rumble_right_duration  = right ? 0xff : 0;
    cmd.led1 = (player_led == 1);
    cmd.led2 = (player_led == 2);
    cmd.led3 = (player_led == 3);
    cmd.led4 = (player_led == 4);
    ps3Cmd(cmd);
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial2.begin(UART_BAUD, SERIAL_8N1, UART_RX, UART_TX);

    Ps3.attach(notify);
    Ps3.attachOnConnect(onConnect);
    Ps3.begin();
}

void loop() {
    if (!Ps3.isConnected()) {
        digitalWrite(LED_PIN, LOW);
        while (Serial2.available()) Serial2.read();
        return;
    }

    while (Serial2.available() >= 2) {
        uint8_t id = Serial2.peek();

        if (id < 0x05 || id > 0x07) {
            Serial2.read();
            continue;
        }

        Serial2.read();
        uint8_t val = Serial2.read();

        switch (id) {
            case 0x05:
                if (val >= 1 && val <= 4) {
                    player_led = val;
                    force_send = true;
                }
                break;

            case 0x06:
                speed_val = val;
                break;

            case 0x07:
                collision_until = millis() + COLLISION_MS;
                break;
        }
    }

    unsigned long now = millis();

    if (now - last_check_ms >= 50) {
        last_check_ms = now;

        bool hit = (now < collision_until);

        uint8_t left = hit ? 255 : 0;

        uint8_t pulse_on = (speed_val && (now % 200) < (uint32_t)speed_val * 200 / 255) ? 255 : 0;
        uint8_t right = hit ? 255 : pulse_on;

        if (left != last_big || right != last_small || force_send ||
            ((left || right) && now - last_sent_ms >= 1000)) {

            sendRumble(left, right);
            last_big     = left;
            last_small   = right;
            last_sent_ms = now;
            force_send   = false;
        }
    }
}