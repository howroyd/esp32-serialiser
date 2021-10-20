set(SOURCES jsoncpp/src/lib_json/json_reader.cpp
            jsoncpp/src/lib_json/json_value.cpp
            jsoncpp/src/lib_json/json_writer.cpp)

#COMPILE_DEFINITIONS
#CXX_COMPILE_OPTIONS

idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS jsoncpp/include)

target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-DJSON_USE_EXCEPTION=0")