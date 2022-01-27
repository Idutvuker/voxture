#pragma once

class Timer {
    double timestamp = -1;
public:
    double tick() {
        double newTime = glfwGetTime();
        double res = newTime - timestamp;
        timestamp = newTime;

        return res;
    }

};