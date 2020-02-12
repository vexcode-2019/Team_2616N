#include "motorTemps.hpp"
#include "general.hpp"

pros::Mutex mutexControllerScreen;
constexpr int tempLimit = 55;
const std::vector<motorCode> motorCodes = 
        {
            {&b_left_mtr, "BL"},      {&f_left_mtr, "FL"},
            {&b_right_mtr, "BR"},     {&f_right_mtr, "FR"},
            {&lift_mtr, "LT"},        {&tray_mtr, "TR"},
            {&left_intake_mtr, "LI"}, {&right_intake_mtr, "RI"}
        };

void overTempWarning()
{
    std::vector<motorCode> overTempMotors;
    bool overTemp = false;
    static bool overTempStatic;

    for (auto& i: motorCodes) {
        if (i.motor->get_temperature() >= tempLimit) {
            if (!overTempStatic)
                controller.rumble("...");
            overTempMotors.insert(overTempMotors.end(), i);
            overTempStatic = true;
            overTemp = true;
        }
    }

    overTempStatic = overTemp;

    mutexControllerScreen.take(TIMEOUT_MAX);
    if (overTemp){
        controller.clear();
        std::vector<std::string> buffer;
        for (auto& i : overTempMotors)
            buffer.insert(buffer.end(), i.code + " - " + std::to_string(i.motor->get_temperature()));

        if (buffer.size() <= 6) {
            if (buffer.size() % 2 == 0) {
                for (int i = 0; i < buffer.size(); i += 2) {
                    std::string str = buffer[i] + ' ' + buffer[i+1];
                    controller.set_text(i / 2, 0, str.c_str());
                }
            } else {
                for (int i = 0; i < buffer.size() - 1; i += 2) {
                    std::string str = buffer[i] + ' ' + buffer[i+1];
                    controller.set_text(i / 2, 0, str.c_str());
                }
                controller.set_text(((buffer.size() - 1) / 2), 0, buffer[4].c_str());
                pros::delay(50);
            }
        } else {
            for (int i = 0; i < 6; i += 2) {
                std::string str = buffer[i] + ' ' + buffer[i+1];
                controller.set_text(i / 2, 0, str.c_str());
            }
        }
    }
    mutexControllerScreen.give();
}

void OTWarning_task(void * a) {
    while (true) {
        overTempWarning();
        pros::delay(500);
    }
}

void showTemps(void * a) {
    int count;
    bool released;
    while (true) {
        count = 0;
        released = false;
        mutexControllerScreen.take(TIMEOUT_MAX);
        controller.clear();
        while (controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) {
            count++;
            if (count % 4 <= 1) {
                controller.print(0, 0, "BL - %d FL - %d", 
                    (int)b_left_mtr.get_temperature(), (int)f_left_mtr.get_temperature());
            } else {
                controller.print(0, 0, "BR - %d FR - %d",
                    (int)b_right_mtr.get_temperature(), (int)f_right_mtr.get_temperature());
            }
            controller.print(1, 0, "TR - %d LT - %d",
                    (int)tray_mtr.get_temperature(), (int)lift_mtr.get_temperature());
            controller.print(2, 0, "LI - %d RI - %d",
                    (int)left_intake_mtr.get_temperature(), (int)right_intake_mtr.get_temperature());
            pros::delay(500);
            released = true;
        }
        if (released){
            controller.clear();
        }
        controller.print(0, 0, "Battery - %d%%", (int)pros::battery::get_capacity());
        controller.print(1, 0, "Cont Bat - %d%%", controller.get_battery_capacity());
        controller.set_text(2, 0, "-_-_-_-_-_-_-_-");
        mutexControllerScreen.give();
        pros::delay(350);
    }
}