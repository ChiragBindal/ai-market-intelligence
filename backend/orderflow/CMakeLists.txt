cmake_minimum_required(VERSION 3.16)
project(orderflow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXCHANGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../exchange)

add_library(exchange
    ${EXCHANGE_DIR}/src/Order.cpp
    ${EXCHANGE_DIR}/src/Trade.cpp
    ${EXCHANGE_DIR}/src/PriceLevel.cpp
    ${EXCHANGE_DIR}/src/OrderBook.cpp
    ${EXCHANGE_DIR}/src/MatchingEngine.cpp
)

target_include_directories(exchange PUBLIC
    ${EXCHANGE_DIR}/include/models
    ${EXCHANGE_DIR}/include/enums
    ${EXCHANGE_DIR}/include/orderbook
    ${EXCHANGE_DIR}/include/matching
    ${EXCHANGE_DIR}/include/utility
)

set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../common)

add_library(common_event
    ${COMMON_DIR}/event/EventDispatcher.cpp
    ${COMMON_DIR}/event/TradeEvent.cpp
)

target_include_directories(common_event PUBLIC
    ${COMMON_DIR}/event
    ${COMMON_DIR}/engine
    ${COMMON_DIR}/types
    ${EXCHANGE_DIR}/include/models
    ${EXCHANGE_DIR}/include/enums
)

target_link_libraries(common_event PUBLIC exchange)

add_library(orderflow
    src/DeltaEngine.cpp
    src/FootPrintLevel.cpp
    src/FootPrintEngine.cpp
    src/ExhaustionEngine.cpp
    src/ImbalanceEngine.cpp
    src/AbsorptionEngine.cpp
)

target_include_directories(orderflow PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${COMMON_DIR}/event
    ${COMMON_DIR}/engine
    ${COMMON_DIR}/types
    ${EXCHANGE_DIR}/include/models
    ${EXCHANGE_DIR}/include/enums
)

target_link_libraries(orderflow PUBLIC exchange common_event)

enable_testing()

add_executable(orderflow_tests
    tests/main.cpp
    tests/DeltaEngineTest.cpp
    tests/EventDispatcherTest.cpp
    tests/FootPrintLevelTest.cpp
    tests/FootPrintEngineTest.cpp
    tests/ExhaustionEngineTest.cpp
    tests/ImbalanceEngineTest.cpp
    tests/AbsorptionEngineTest.cpp
)

target_include_directories(orderflow_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${COMMON_DIR}/event
    ${COMMON_DIR}/engine
    ${COMMON_DIR}/types
    ${EXCHANGE_DIR}/include/models
    ${EXCHANGE_DIR}/include/enums
    ${EXCHANGE_DIR}/include/utility
)

target_link_libraries(orderflow_tests PRIVATE orderflow)

add_test(NAME orderflow_tests COMMAND orderflow_tests)
