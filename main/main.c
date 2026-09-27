#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUZZER_GPIO 10

#define BUTTON_C_GPIO 4
#define BUTTON_D_GPIO 5
#define BUTTON_E_GPIO 6

#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330

static void buzzer_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .freq_hz          = 1000,
        .clk_cfg          = LEDC_AUTO_CLK
    };

    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num   = BUZZER_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 512,
        .hpoint     = 0
    };

    ledc_channel_config(&channel);
}

static void buttons_init(void)
{
    gpio_config_t buttons = {
        .pin_bit_mask =
            (1ULL << BUTTON_C_GPIO) |
            (1ULL << BUTTON_D_GPIO) |
            (1ULL << BUTTON_E_GPIO),

        .mode = GPIO_MODE_INPUT,

        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,

        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&buttons);
}

static void buzzer_set_note(uint32_t frequency)
{
    if (frequency == 0) {
        ledc_set_duty(
            LEDC_LOW_SPEED_MODE,
            LEDC_CHANNEL_0,
            0
        );

        ledc_update_duty(
            LEDC_LOW_SPEED_MODE,
            LEDC_CHANNEL_0
        );

        return;
    }

    ledc_set_freq(
        LEDC_LOW_SPEED_MODE,
        LEDC_TIMER_0,
        frequency
    );

    ledc_set_duty(
        LEDC_LOW_SPEED_MODE,
        LEDC_CHANNEL_0,
        512
    );

    ledc_update_duty(
        LEDC_LOW_SPEED_MODE,
        LEDC_CHANNEL_0
    );
}

void app_main(void)
{
    buzzer_init();
    buttons_init();

    while (1) {

        if (gpio_get_level(BUTTON_C_GPIO) == 0) {
            // GPIO39 has highest priority
            buzzer_set_note(NOTE_C4);

        } else if (gpio_get_level(BUTTON_D_GPIO) == 0) {
            // GPIO40 has second priority
            buzzer_set_note(NOTE_D4);

        } else if (gpio_get_level(BUTTON_E_GPIO) == 0) {
            // GPIO41 has lowest priority
            buzzer_set_note(NOTE_E4);

        } else {
            // Nothing pressed
            buzzer_set_note(0);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}