#define STRINGIFY(a) #a
#define SUPPRESS_WARN_BEGIN _Pragma( STRINGIFY(GCC diagnostic push) )\
_Pragma( STRINGIFY(GCC diagnostic push ) )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wall") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wextra") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wshadow") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wnon-virtual-dtor") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wpedantic") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wcast-align") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wold-style-cast") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Woverloaded-virtual") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wconversion") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wsign-conversion") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wduplicated-cond") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wduplicated-branches") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wlogical-op") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wnull-dereference") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wuseless-cast") )\
_Pragma( STRINGIFY(GCC diagnostic ignored "-Wdouble-promotion") )

#define SUPPRESS_WARN_END _Pragma( STRINGIFY(GCC diagnostic pop) )