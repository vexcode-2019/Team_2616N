#pragma once
#include <string>
#include "general.hpp"
#include "auton.hpp"

void autonomous(void);
std::string currentAutonSelectionCode(void);
void autonSelectorInit(void);
lv_res_t buttonPressed(lv_obj_t * obj);
void autonSelectorReset(void);
void guiManagerTask(void* a);