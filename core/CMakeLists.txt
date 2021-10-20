set(SOURCES ./Src/main.cpp ./Src/heap.cpp)

idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS . Inc
                        REQUIRES freertos esp_common heap esp_event nvs_flash json gsl jsoncpp)

set_source_files_properties(${SOURCES}
                                PROPERTIES COMPILE_OPTIONS
            "-Wall;-Wextra;-Wshadow;-Wnon-virtual-dtor;-Wcast-align;-Wold-style-cast;-Woverloaded-virtual;-Wconversion;-Wsign-conversion;-Wduplicated-cond;-Wduplicated-branches;-Wlogical-op;-Wnull-dereference;-Wuseless-cast;-Wdouble-promotion")