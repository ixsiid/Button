# Button driver

Button check library on ESP-IDF


```cpp:main.cpp
void app_main() {
	_i("Start");

	xTaskCreatePinnedToCore([](void * _) {
		uint8_t buttonPins[] = {37, 38, 39};
		Button *button = new Button(buttonPins, 3);

		while (true) {
			// Main loop
			vTaskDelay(1);

			button->check(nullptr, [](uint8_t pin) {
				switch (pin) {
					case 37:
						_i("released gpio 37");
						break;
					case 38:
						_i("released gpio 38");
						break;
					case 39:
						_i("released gpio 39");
						break;
				}
			});
		}
	}, "ButtonCheck", 4096, NULL, 1, NULL, 1);
}

```
