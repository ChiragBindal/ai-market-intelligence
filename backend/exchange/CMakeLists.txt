cmake_minimum_required(VERSION 3.16)
project(exchange LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(exchange
    src/Order.cpp
    src/Trade.cpp
    src/PriceLevel.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
)

target_include_directories(exchange PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/models
    ${CMAKE_CURRENT_SOURCE_DIR}/include/enums
    ${CMAKE_CURRENT_SOURCE_DIR}/include/orderbook
    ${CMAKE_CURRENT_SOURCE_DIR}/include/matching
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utility
)

add_executable(exchange_demo src/main.cpp)
target_link_libraries(exchange_demo PRIVATE exchange)
