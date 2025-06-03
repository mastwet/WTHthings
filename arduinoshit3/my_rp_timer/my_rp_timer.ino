#include "driver/timer.h"
#include "hal/timer_types.h"

#define TIMER_DIVIDER         800  
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_INTERVAL0_SEC   (1.0) // sample test interval for the first timer
#define TEST_WITH_RELOAD      1     // testing will be done with auto reload

#define TIMER_RESOLUTION_HZ   1000000 // 1MHz resolution
#define TIMER_ALARM_PERIOD_S  0.5     // Alarm period 0.5s

void IRAM_ATTR timer_group0_isr(void *para);

// Initialize timer group 0, timer 0 to periodically trigger an interrupt every second
void init_timer(int timer_period_us) {
    timer_config_t config = {
        //.alarm_en = TIMER_ALARM_EN,
        
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
        //.counter_en = TIMER_PAUSE,

        //.intr_type = TIMER_INTR_LEVEL,
        //.auto_reload = TEST_WITH_RELOAD,
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, timer_period_us);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_group0_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
}

// This function will be called by the timer interrupt handler
void IRAM_ATTR timer_group0_isr(void *para) {
    timer_spinlock_take(TIMER_GROUP_0);
    // Clear the interrupt, and update the alarm time for the timer
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_spinlock_give(TIMER_GROUP_0);
}


unsigned long rp_millis() {
    unsigned long millis_value;
    uint64_t timer_counter_value = 0;

    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_counter_value);
    millis_value = (unsigned long) (timer_counter_value / 1000ULL); 

    return millis_value;
}

void setup() {
  // put your setup code here, to run once:
  init_timer(1000);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int time_now = rp_millis();
  Serial.println(time_now);
  delay(1000);
}
