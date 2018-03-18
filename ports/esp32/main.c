#include "src/runtime/tinygo.h"

#include "esp_system.h"
#include "esp_event.h"
#include "driver/gpio.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	return ESP_OK;
}

void machine_init() __asm__("machine..import");

void app_main(void)
{
	//go_init(); // TODO
	machine_init();
	go_main();

	// sleep forever
	while (true) {
		vTaskDelay(10000);
	}
}


void vPortCleanUpTCB(void *tcb) {
	printf("ERROR: TODO: vPortCleanUpTCB\n");
	for (;;);
}

void tinygo_sleep(int64_t nanoseconds) {
	vTaskDelay(nanoseconds / 1000 / 1000 / portTICK_PERIOD_MS);
}

volatile static struct {
	func fn;
	void *arg;
} go_params;

void tinygo_go_internal(void *fn_in) {
	if (go_params.fn != NULL) {
		func fn = go_params.fn;
		void *arg = go_params.arg;
		fn(arg);
	} else {
		func fn = fn_in;
		fn(NULL);
	}
	printf("go: exiting goroutine\n");
	vTaskDelete(NULL);
}

void tinygo_go(void *fn, void *arg, void *created_by) {
	if (arg == NULL) {
		xTaskCreate(tinygo_go_internal, "goroutine", CONFIG_MAIN_TASK_STACK_SIZE, fn, ESP_TASK_MAIN_PRIO, NULL);
	} else {
		// TODO: synchronisation.
		go_params.fn = fn;
		go_params.arg = arg;
		xTaskCreate(tinygo_go_internal, "goroutine", CONFIG_MAIN_TASK_STACK_SIZE, fn, ESP_TASK_MAIN_PRIO, NULL);
	}
}

__attribute__((noreturn))
void tinygo_goroutine_exit() {
	vTaskDelete(NULL);
	__builtin_unreachable();
}

void tinygo_block() {
	// TODO: replace with actual locks / notifications
	taskYIELD();
}

void tinygo_semacquire(volatile uint32_t *addr, bool profile) {
	runtime_throw("todo: semacquire");
}

void tinygo_semrelease(volatile uint32_t *addr) {
	runtime_throw("todo: semrelease");
}

void tinygo_fatal() {
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// TODO: this only resets the CPUs, not the peripherals
	esp_restart();
}
