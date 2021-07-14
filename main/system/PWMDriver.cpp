#include "PWMDriver.h"

namespace cima::system {
    PWMDriver::PWMDriver(gpio_num_t pwmGpioPin) : pwmGpioPin(pwmGpioPin) {

        ledc_channel_config_t ledc_channel = {0};
        ledc_channel.gpio_num = this->pwmGpioPin;
        ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
        ledc_channel.channel = LEDC_CHANNEL_0; //TODO do proměnný
        ledc_channel.intr_type = LEDC_INTR_DISABLE;
        ledc_channel.timer_sel = LEDC_TIMER_0;
        ledc_channel.duty = 0;

        ledc_timer_config_t ledc_timer;
        ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE; //TODO do proměnný
        ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;
        ledc_timer.timer_num = LEDC_TIMER_0; //TODO do proměnný
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;
        ledc_timer.freq_hz = 5000;

        ledc_channel_config(&ledc_channel);
        ledc_timer_config(&ledc_timer);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 4096);
	    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    }

    void PWMDriver::update(uint32_t dutyCycle) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, dutyCycle);
	    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    }
}