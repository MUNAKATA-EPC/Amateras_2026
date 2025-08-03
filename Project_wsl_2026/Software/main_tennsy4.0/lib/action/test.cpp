#include "test.hpp"

void play_test(int test_mode)
{
    IRserial_update();   // 更新
    LINEserial_update(); // 更新
    BNO055_update();     // 更新

    switch (test_mode)
    {
    case TEST_ALL_CHECK_WITH_SERIAL:
        break;

    case TEST_IR_CHECK_WITH_SERIAL:
        break;

    case TEST_LINE_CHECK_WITH_SERIAL:
        break;

    case TEST_GYRO_CHECK_WITH_SERIAL:
        break;

    case TEST_CAM_CHECK_WITH_SERIAL:
        Serial.println("I haven't made it yet.");
        break;

    default:
        Serial.println("error");
        break;
    }
}