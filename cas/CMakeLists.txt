add_library(zcalc-cas
        cas.cpp
        cas.h
)

target_include_directories(zcalc-cas PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(zcalc-cas PUBLIC
        cln
)