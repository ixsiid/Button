#pragma once

#include <driver/gpio.h>
#include <esp_timer.h>

#include <functional>
#include <list>

typedef struct {
	gpio_num_t pin;

	bool released;
	bool pressed;

	int64_t up_last_changed;
	int64_t down_last_changed;
} InterruptManager_t;

class Button {
    private:
	static const int64_t dtime_threshould = 10 * 1000; // 10msec
	static bool isr;

	std::list<InterruptManager_t *> pins;

    public:
	Button(uint8_t * pins, size_t count);

	void check(std::function<void(uint8_t)> pressed, std::function<void(uint8_t)> released);
};
