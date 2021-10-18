#pragma once
#include "my_warnings.h"

SUPPRESS_WARN_BEGIN
#include <chrono>
#include <cstdlib>
#include <string_view>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#undef pdSECOND
#define pdSECOND configTICK_RATE_HZ
#undef pdMAX
#define pdMAX 0xffffffffUL

#include "esp_err.h"
#include "esp_event.h"
#include "nvs_flash.h"
SUPPRESS_WARN_END

// --------------------------------------------------------------------------------------------------------------------
class Main final // To ultimately become the co-ordinator if deemed necessary
{
    constexpr static
    std::size_t stack_size{2048};
    constexpr static
    const char* const name{"MAIN"}, *const tag{name};
    constexpr static
    TickType_t loop_delay{pdSECOND};

public:
    constexpr Main(void) noexcept {}

    esp_err_t init(void)
    {
        if (not h_task)
        {
            h_task = xTaskCreateStatic(
                      task,
                      name,
                      stack_size,
                      this,
                      5,
                      stack,
                      &task_tcb);
            if (h_task)
                return ESP_OK;
            return ESP_ERR_NO_MEM;
        }
        return
            ESP_ERR_INVALID_STATE;
    }

private:
    [[nodiscard]] static
    bool setup(Main& main);

    static
    void loop(Main& main);

    [[noreturn,gnu::nonnull]] static
    void task(void* p_main)
    {
        Main& main{*reinterpret_cast<Main*>(p_main)};

        while (not setup(main))
            vTaskDelay(pdSECOND);

        while (true)
        {
            loop(main);
            vTaskDelay(loop_delay);
        }
    }

    [[nodiscard]] static
    esp_err_t start_all_tasks(void);

    static StaticTask_t task_tcb;
    static StackType_t  stack[stack_size];
    static TaskHandle_t h_task;
}; // class Main

inline StaticTask_t Main::task_tcb{};
inline StackType_t  Main::stack[stack_size]{};
inline TaskHandle_t Main::h_task{nullptr};

// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------