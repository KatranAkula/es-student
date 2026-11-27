#pragma once
#include <stdint.h>
#include <stdbool.h>

uint64_t systime_us(void); //микросекунды с момента сброса

uint32_t systime_ms(void); //миллисекунды с момента сброса

bool systime_period_elapsed(uint64_t *last_us, uint64_t period_us);//истёк ли период: true — истёк и метка обновлена, false — ещё нет

