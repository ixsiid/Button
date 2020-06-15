#include "button.hpp"

#include <algorithm>

#define TAG "Button"
#include "log.h"

bool Button::isr = false;

Button::Button(uint8_t *pins, size_t count) {
	gpio_config_t pin;
	pin.pin_bit_mask = 0;
	pin.mode		  = GPIO_MODE_INPUT;
	pin.pull_up_en	  = GPIO_PULLUP_ENABLE;
	pin.pull_down_en = GPIO_PULLDOWN_DISABLE;
	pin.intr_type	  = GPIO_INTR_ANYEDGE;

	int64_t now = esp_timer_get_time();
	for (int i = 0; i < count; i++) {
		pin.pin_bit_mask += (uint64_t)1 << pins[i];

		InterruptManager_t *p = new InterruptManager_t();
		p->pin			  = (gpio_num_t)pins[i];
		p->pressed		  = false;
		p->released		  = false;
		p->up_last_changed	  = now;
		p->down_last_changed  = now;
		this->pins.push_back(p);
	}
	gpio_config(&pin);

	if (!isr) {
		gpio_install_isr_service(0);
		isr = true;
	}

	std::for_each(this->pins.cbegin(), this->pins.cend(), [](InterruptManager_t *p) {
		gpio_isr_handler_add(p->pin, [](void *pin) {
			InterruptManager_t *p = (InterruptManager_t *)pin;

			int64_t now = esp_timer_get_time();
			if (gpio_get_level(p->pin)) {
				if (now - p->up_last_changed > dtime_threshould) {
					p->released = true;
				}
				p->up_last_changed = now;
			} else {
				if (now - p->down_last_changed > dtime_threshould) {
					p->pressed = true;
				}
				p->down_last_changed = now;
			}
		}, p);
	});
}

void Button::check(std::function<void(uint8_t)> pressed, std::function<void(uint8_t)> released) {
	std::for_each(this->pins.cbegin(), this->pins.cend(), [&](InterruptManager_t *p) {
		if (p->pressed) {
			p->pressed = false;
			if (pressed) pressed(p->pin);
		}
		if (p->released) {
			p->released = false;
			if (released) released(p->pin);
		}
	});
}
