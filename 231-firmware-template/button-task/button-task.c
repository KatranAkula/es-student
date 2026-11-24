// button-task.c — задача дребезг подавляет машина состояний без пауз
#include "button-task.h"   //свой заголовок первым
#include "led-task.h"      //для led_task_next_state() при подтверждённом нажатии
#include "pico/stdlib.h"   //SDK: gpio, time_us_64
#include "hardware/gpio.h" //gpio_init/gpio_get/gpio_pull_up

#define BUTTON_PIN        15      //GP15, кнопка между выводом и землёй
#define DEBOUNCE_TIME_US  20000   //порог дребезга 20 мс в микросекундах

typedef enum { //четыре состояния машины кнопки
    BUTTON_RELEASED,    //отпущена
    BUTTON_PRESSING,    //нажатие? — ждём подтверждения 20 мс
    BUTTON_PRESSED,     //нажата — нажатие засчитано
    BUTTON_RELEASING,   //отпускание? — ждём подтверждения 20 мс
} button_state_t;       //имя типа для static-переменной

static button_state_t button_state;  //текущее состояние, static = не видно снаружи
static uint64_t       button_state_time_us;//метка входа в текущее состояние
static uint32_t       button_press_count; //счётчик подтверждённых нажатий

static bool read_pressed(void) //читаем уровень: нажатая кнопка замыкает вывод на землю → 0
{
    return gpio_get(BUTTON_PIN) == 0; //true — нажата
}

void button_task_init(void) //инициализация, один раз из main
{
    gpio_init(BUTTON_PIN); //включить GPIO
    gpio_set_dir(BUTTON_PIN, GPIO_IN);  //сделать входом
    gpio_pull_up(BUTTON_PIN);  //подтяжка к питанию — отпущенная читается как 1

    button_state         = BUTTON_RELEASED; //старт — отпущена
    button_state_time_us = 0;  //метка не нужна до первого нажатия
    button_press_count   = 0;  //нажатий пока ноль
}

void button_task_handle(void) //обработчик, зовёт суперцикл
{
    bool     pressed = read_pressed();  //читаем уровень один раз
    uint64_t now_us  = time_us_64();    //читаем часы один раз

    switch (button_state) { //переходы по таблице из ТЗ
    case BUTTON_RELEASED:   //отпущена
        if (pressed) {  //появилось нажатие
            button_state         = BUTTON_PRESSING; //в «нажатие?»
            button_state_time_us = now_us;    //запомнили время
        }
        break;

    case BUTTON_PRESSING:   //нажатие?
        if (!pressed) {     //уровень пропал — дребезг или помеха
            button_state = BUTTON_RELEASED;    //назад в «отпущена»
        } else if (now_us - button_state_time_us >= DEBOUNCE_TIME_US) { //держится 20 мс?
            button_state = BUTTON_PRESSED;   //в «нажата»
            button_press_count++;    //засчитали нажатие
            led_task_next_state();       //перевели светодиод в следующее состояние
        }
        break;

    case BUTTON_PRESSED:    //нажата
        if (!pressed) {     //уровень пропал
            button_state         = BUTTON_RELEASING; //в «отпускание?»
            button_state_time_us = now_us;               //запомнили время
        }
        break;

    case BUTTON_RELEASING:  //отпускание?
        if (pressed) {      //снова нажата — дребезг
            button_state = BUTTON_PRESSED;  //назад в «нажата»
        } else if (now_us - button_state_time_us >= DEBOUNCE_TIME_US) { //отпущена 20 мс?
            button_state = BUTTON_RELEASED;  //в «отпущена»
        }
        break;
    }
}

bool button_task_is_pressed(void) //кнопка нажата и в «нажата», и в «отпускание?»
{
    return button_state == BUTTON_PRESSED || button_state == BUTTON_RELEASING;
}

uint32_t button_task_get_press_count(void) //чтение счётчика нажатий
{
    return button_press_count; //возвращаем static-переменную
}
