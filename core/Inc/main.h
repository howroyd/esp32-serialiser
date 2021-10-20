#pragma once
#include "my_warnings.h"

SUPPRESS_WARN_BEGIN
#include <chrono>
#include <cstdlib>
#include <string_view>

#include "gsl/gsl"
#include "json/json.h"

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
    constexpr static std::size_t stack_size{2048};
    constexpr static const char *const name{"MAIN"}, *const tag{name};
    constexpr static TickType_t loop_delay{pdSECOND};

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
        return ESP_ERR_INVALID_STATE;
    }

private:
    [[nodiscard]] static bool setup(Main &main);

    static void loop(Main &main);

    [[noreturn, gnu::nonnull]] static void task(void *p_main)
    {
        Main &main{*reinterpret_cast<Main *>(p_main)};

        while (not setup(main))
            vTaskDelay(pdSECOND);

        while (true)
        {
            loop(main);
            vTaskDelay(loop_delay);
        }
    }

    [[nodiscard]] static esp_err_t start_all_tasks(void);

    static StaticTask_t task_tcb;
    static StackType_t stack[stack_size];
    static TaskHandle_t h_task;
}; // class Main

inline StaticTask_t Main::task_tcb{};
inline StackType_t Main::stack[stack_size]{};
inline TaskHandle_t Main::h_task{nullptr};

// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>
#include <string>
#include <chrono>
#include <utility>
#include <memory>

#include <cJSON.h>

class Serialiser
{
public:
    std::string_view name{"UNKNOWN"};

    struct Tags
    {
        constexpr static const std::string_view heartrate{"heartrate"};
        constexpr static const std::string_view pace{"pace"};
        constexpr static const std::string_view speed{"speed"};
        constexpr static const std::string_view climb{"climb"};
        constexpr static const std::string_view steps{"steps"};
        constexpr static const std::string_view battery{"battery"};

        [[nodiscard]] constexpr static std::size_t size(void)
        {
            return 6;
        }
    };

    using ts_t = std::chrono::time_point<std::chrono::system_clock>;
    using ts_num_t = std::chrono::seconds;

    using key_t = std::string_view;
    using data_t = std::pair<ts_t, std::string>;

    void insert(const key_t &key,
                data_t::second_type &&value)
    {
        insert(key, data_t{std::chrono::system_clock::now(), value});
    }

    using json_t = std::unique_ptr<cJSON, void (*)(cJSON *)>;

    json_t as_json(void) const
    {
        json_t root{cJSON_CreateObject(), json_deleter};

        auto this_obj = cJSON_AddObjectToObject(root.get(), std::string{name}.c_str());

        for (const auto &[key, data] : map)
        {
            auto this_key_obj = cJSON_AddObjectToObject(this_obj, std::string{key}.c_str());

            const auto ts = std::chrono::duration_cast<ts_num_t>(data.first.time_since_epoch()).count();

            cJSON_AddNumberToObject(this_key_obj, "ts", static_cast<double>(ts));
            cJSON_AddStringToObject(this_key_obj, "val", data.second.c_str());
        }

        return root;
    }

    static json_t as_json(std::string &str)
    {
        return json_t{cJSON_Parse(str.c_str()), json_deleter};
    }

    static Serialiser from_json(const std::string &name, json_t root)
    {
        Serialiser ret;

        auto this_obj = cJSON_GetObjectItemCaseSensitive(root.get(), name.c_str());

        if (cJSON_IsObject(this_obj))
        {
            auto get_map_entry = [&this_obj](const std::string &sensor_name) -> data_t
            {
                data_t map_pair;

                auto this_sensor = cJSON_GetObjectItemCaseSensitive(this_obj, sensor_name.c_str());

                if (cJSON_IsObject(this_sensor) && cJSON_HasObjectItem(this_sensor, "ts") && cJSON_HasObjectItem(this_sensor, "val"))
                {
                    const auto ts_num = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(this_sensor, "ts"));
                    const auto val = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(this_sensor, "val"));

                    const ts_t ts = ts_t{} + ts_num_t{static_cast<int>(ts_num)};

                    map_pair.first = ts;
                    map_pair.second = val;
                }

                return map_pair;
            };

            ret.insert(Tags::heartrate, get_map_entry(std::string{Tags::heartrate}));
            ret.insert(Tags::pace, get_map_entry(std::string{Tags::pace}));
            ret.insert(Tags::speed, get_map_entry(std::string{Tags::speed}));
            ret.insert(Tags::climb, get_map_entry(std::string{Tags::climb}));
            ret.insert(Tags::steps, get_map_entry(std::string{Tags::steps}));
            ret.insert(Tags::battery, get_map_entry(std::string{Tags::battery}));
        }

        return ret;
    }

private:
    std::unordered_map<key_t, data_t> map{Tags::size()};

    static void json_deleter(cJSON *root)
    {
        cJSON_Delete(root);
    }

    void insert(const key_t &key,
                data_t &&data_pair)
    {
        map.insert_or_assign(key, data_pair);
    }
};
