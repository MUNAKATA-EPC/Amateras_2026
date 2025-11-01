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
}

namespace Defender {
    enum Mode {
        GYRO,
        YELLOWGOAL,
        BLUEGOAL,

        MODE_COUNT
    };
}

namespace Test {
    enum Mode {
        KICKER,
        GYRO,
        CAM,

        MODE_COUNT
    };
}

namespace Radicon {
    enum Mode {
        SPEED_50CC,
        SPEED_100CC,
        SPEED_200CC,

        MODE_COUNT
    };
}

