#pragma once

namespace Action {
    enum Type {
        ATTACKER,
        DEFENDER,
        TEST,
        RADICON,
        
        COUNT
    };
}

namespace Attacker {
    enum Mode {
        GYRO,
        YELLOWGOAL,
        BLUEGOAL,

        MODE_COUNT
    };
    enum Config {
        RUN,
        SPEED_LOW,
        SPEED_MIDDLE,
        SPEED_HIGH,

        CONFIG_COUNT
    };
}

namespace Defender {
    enum Mode {
        GYRO,
        YELLOWGOAL,
        BLUEGOAL,

        MODE_COUNT
    };
    enum Config {
        RUN,
        SPEED_LOW,
        SPEED_MIDDLE,
        SPEED_HIGH,

        CONFIG_COUNT
    };
}

namespace Test {
    enum Mode {
        KICKER,
        GYRO,
        CAM,

        MODE_COUNT
    };
    enum Config {
        RUN,

        CONFIG_COUNT
    };
}

namespace Radicon {
    enum Mode {
        SPEED_50CC,
        SPEED_100CC,
        SPEED_200CC,

        MODE_COUNT
    };
    enum Config {
        RUN,
        LINE_AUTO,
        ATTACKER_AUTO,
        KICKER_AUTO,

        CONFIG_COUNT
    };
}

