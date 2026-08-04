#include "EventDispatcher.h"
#include "DeltaEngine.h"
#include "FootPrintEngine.h"
#include "BinanceAdapter.h"
#include "Side.h"
#include "TestRunner.h"

void runBinanceAdapterTests(TestRunner& runner) {
    runner.run("Binance WebSocket JSON - buyer taker (aggressive BUY)", [&]() {
        const std::string json =
            R"({"e":"trade","E":1672515782136,"s":"BTCUSDT","t":12345,"p":"65000.50","q":"0.25","b":88,"a":50,"T":1672515782135,"m":false,"M":true})";

        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        dispatcher.subscribe(&deltaEngine);

        BinanceAdapter adapter(dispatcher, "BTCUSDT", 10);
        adapter.ingestWebSocketMessage(json);

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 0.25, "buy volume from parsed trade");
        EXPECT_EQUAL(runner, deltaEngine.getSellVolume(), 0.0, "no sell volume");
        EXPECT_EQUAL(runner, deltaEngine.getDelta(), 0.25, "positive delta");
    });

    runner.run("Binance WebSocket JSON - buyer maker (aggressive SELL)", [&]() {
        const std::string json =
            R"({"e":"trade","E":1672515782136,"s":"BTCUSDT","t":99,"p":"100.0","q":"2.0","b":1,"a":2,"T":1672515782135,"m":true,"M":true})";

        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        dispatcher.subscribe(&deltaEngine);

        BinanceAdapter adapter(dispatcher, "BTCUSDT", 10);
        adapter.ingestWebSocketMessage(json);

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 0.0, "no buy volume");
        EXPECT_EQUAL(runner, deltaEngine.getSellVolume(), 2.0, "sell volume");
        EXPECT_EQUAL(runner, deltaEngine.getDelta(), -2.0, "negative delta");
    });

    runner.run("Binance REST JSON array -> DeltaEngine", [&]() {
        const std::string jsonArray =
            R"([{"id":1,"price":"10.0","qty":"1.0","time":1000,"isBuyerMaker":false,"isBestMatch":true},{"id":2,"price":"11.0","qty":"3.0","time":1001,"isBuyerMaker":true,"isBestMatch":true}])";

        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        dispatcher.subscribe(&deltaEngine);

        BinanceAdapter adapter(dispatcher, "ETHUSDT", 10);
        adapter.ingestRestTradesJson(jsonArray);

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 1.0, "buy from first trade");
        EXPECT_EQUAL(runner, deltaEngine.getSellVolume(), 3.0, "sell from second trade");
        EXPECT_EQUAL(runner, deltaEngine.getDelta(), -2.0, "net delta");
    });

    runner.run("Binance WebSocket -> DeltaEngine + FootPrintEngine", [&]() {
        const std::string buy =
            R"({"e":"trade","E":1,"s":"BTCUSDT","t":1,"p":"100.00","q":"1.5","b":1,"a":2,"T":1000,"m":false,"M":true})";
        const std::string sell =
            R"({"e":"trade","E":2,"s":"BTCUSDT","t":2,"p":"100.01","q":"0.5","b":3,"a":4,"T":1001,"m":true,"M":true})";

        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        FootPrintEngine footPrintEngine(0.01);
        dispatcher.subscribe(&deltaEngine);
        dispatcher.subscribe(&footPrintEngine);

        BinanceAdapter adapter(dispatcher, "BTCUSDT", 10);
        adapter.ingestWebSocketMessage(buy);
        adapter.ingestWebSocketMessage(sell);

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 1.5, "delta buy");
        EXPECT_EQUAL(runner, deltaEngine.getSellVolume(), 0.5, "delta sell");
        EXPECT_EQUAL(runner, deltaEngine.getDelta(), 1.0, "delta net");

        const FootPrintSnapshot snap = footPrintEngine.snapshot();
        EXPECT_TRUE(runner, snap.levelCount == 2, "two price levels");
        EXPECT_EQUAL(runner, snap.totalBuyVolume, 1.5, "footprint buy");
        EXPECT_EQUAL(runner, snap.totalSellVolume, 0.5, "footprint sell");
        EXPECT_EQUAL(runner, snap.totalDelta, 1.0, "footprint delta matches session delta");

        const FootPrintLevel* level100 = footPrintEngine.getLevel(100.00);
        const FootPrintLevel* level100_01 = footPrintEngine.getLevel(100.01);
        EXPECT_TRUE(runner, level100 != nullptr, "level 100 exists");
        EXPECT_TRUE(runner, level100_01 != nullptr, "level 100.01 exists");
        if (level100 != nullptr) {
            EXPECT_EQUAL(runner, level100->getBuyVolume(), 1.5, "100 buy volume");
        }
        if (level100_01 != nullptr) {
            EXPECT_EQUAL(runner, level100_01->getSellVolume(), 0.5, "100.01 sell volume");
        }
    });
}
