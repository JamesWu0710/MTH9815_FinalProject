# MTH9815_FinalProject
MTH9815 Software Engineering Final Project: BondTradingSystem

Author: Chengxun James Wu

# Description of Project Requirements
This part is adopted from Breman's project requirements.

- Develop a bond trading system for US Treasuries with seven securities: 2Y, 3Y, 5Y, 7Y, 10Y, 20Y, and 30Y. Look up the CUSIPS, coupons, and maturity dates for each security. Ticker is T.
- The securities each have a unique id: 91282CFX4, 91282CFW6, 91282CFZ9, 91282CFY2, 91282CFV8, 912810TM0, 912810TL2 standing for 2Y, 3Y, 5Y, 7Y, 10Y, 20Y, and 30Y.
- The idea is to connection different processes: **Listeners, Connectors that Subscribe and Publish**. We have a new definition of a Service in soa.hpp, with the concept of a ServiceListener and Connector also defined. A ServiceListener is a listener to events on the service where data is added to the service, updated on the service, or removed from the service. A Connector is a class that flows data into the Service from some connectivity source (e.g. a socket, file, etc) via the Service.OnMessage() method. The Publish() method on the Connector publishes data to the connectivity source and can be invoked from a Service. Some Connectors are publish-only that do not invoke Service.OnMessage(). Some Connectors are subscribe-only where Publish() does nothing. Other Connectors can do both publish and subscribe.

# Project Design Idea
- To create different Bond services, we first create the template classes that accepts a geenral product defined in product.hpp. Then in main.cpp, we specify the product type to be **Bonds**. This allows a more **generic framework that can be transplanted to other products easily**.
- Regarding data, we generate the data according to the rules defined in the project requirements. This is mainly done in datageneration.hpp, and the input data can be generated if we call the Initialize method in the main file.
- **How to connect different services?** Following the guidelines, I adopt the method of adding listeners from one service to another. For example, I have AlgoExecutionToExecutionListener that specifies that: AlgoExecution sends information to Execution, which forms the connections between two services. Similar examples can also be found in other services. This linking design is also instrumental in the project.
- **How to read and store data?** Following the guidelines, I adopt the Publish and Subscribe with connectors outlined in the project descriptions. For example, PricingService uses a Connector to subscribe data from the stored prices information. After reading the data, the connector calls the *OnMessage* method of the corresponding service to apply actions on the data obtained to pass into the system.
- The codes follow the original codes provided by the Professor with some discretionary additions (like the ToStrings method that converts the classes into strings).


# How to run the codes?
- To compile it using g++, we can follow **g++ -std=c++17 main.cpp -o test -I C:/boost/include/boost-1_80 -L C:/boost/lib -lws2_32 -lwsock32**. Remember to specify your paths of the boost library! Then, run test.exe to see the operations.
- Or, you can directly run the test.exe file contained in this repository.
- If you prefer Visual Studio, please refer to the zip file contained in this repository.

# File Descriptions
This part will list all the files and the classes within. All the services are keyed on the product ID.
- algoexecutionservice.hpp:
  -  ExecutionOrder: modeling orders to execute, containing basic attributes and a *ToStrings* function that converts the attributes to a string.
  -  AlgoExecution: modeling the execution of orders
  -  AlgoExecutionService: modeling the algorithmic execution service that accepts listeners from AlgoExecution and a listener connecting to **MarketDataService**. It also contains an AlgoOrderExecution method that executes an order (via notifying the listeners) when the spread is smaller than the SPREAD LIMIT.
  -  AlgoExecutionToMarketDataListener: modeling the listener connecting the two services.
- algostreamingservice.hpp
  - PriceStreamOrder: modeling the order streams. Equipped with a _ToStrings_ method that converts the attributes to a string.
  - PriceStream: modeling the (concatenated) price streams. Equipped with a _ToStrings_ method that converts the attributes to a string.
  - AlgoStream: modeling the algo streams. Stores the priceStream information formulated from input PriceStreamOrder objects.
  - AlgoStreamingService: modeling the algo order streaming service that publishes price (by specifying the visible and hidden quantities)
  - AlgoStreamingToPricingListener: modeling the listener that connects from **AlgoStreamingService** to **PricingService**.
- datageneration.hpp: function programming that generates data for all the bonds.
  - GenerateAllPrices: generate price.txt. Set the default size to be 10000 instead of 1000000 for easier debugging and testing.
  - GenerateAllMarketData: generate marketdata.txt. Set the default size to be 10000 instead of 1000000 for easier debugging and testing.
  - GenerateAllTradeData: generate trades.txt. Default size 10.
  - GenerateAllInquiryData: generate inquiries.txt. Default size 10.
- executionservice.hpp
  -  ExecutionService: modeling the order execution service.
  -  AlgoExecutionToExecutionListener: modeling the listner connecting from **AlgoExecutionService** to **ExecutionService**.
- GUIservice.hpp
  - GUIService: modeling the service to stream prices at given throttle (in millisecond units, defualt 300),
  - GUIConnector: modeling the connector to Price objects. _Publish()_ method records the current millisecond information and store the records into gui.txt.
  - GUIToPricingListener: modeling the listener connecting from **GUIService** to **PricingService**. The latter feedbacks with price information with updates in gui.txt by GUIService.
- historicaldataservice.hpp: connecting different services with data read from the input .txt files.
  - HistoricalDataService: modeling the historical data service. It persists objects it receives from **PositionService**, **RiskService**, **ExecutionService**, **StreamingService**, and **InquiryService**.
  - HistoricalDataConnector: modeling the connector from above-mentioned services that and store the info in position.txt, risk.txt, execution.txt, streaming.txt, inquiry.txt. 
  - HistoricalDataListener: modeling the HistoricalDataListener.
- inquiryservice.hpp
  - Inquiry: modeling the inquiries. Equipped with a _ToStrings_ method that converts the attributes to a string.
  - InquiryService: modeling the service processing incoming inquiries.
  - InquiryConnector: modeling the connector. To receive data from inquiry.txt, call _Subscribe()_ to convert into inquiries and update into the system.
- marketdataservice.hpp
  - Order: modeling orders.
  - BidOffer: modeling bid and offer, used to fetch top of orderbooks.
  - OrderBook: modeling order books.
  - MarketDataService: modeling the service that manages market data and order books.
  - MarketDataConnector: modeling the connetors. To receive data from marketdata.txt, call _Subscribe()_ to convert into market data and order books, then update into the system.
- positionservice.hpp
  - Position: modeling position objects. Equipped with a _ToStrings_ method that converts the attributes to a string.
  - PositionService: modeling the position services. It manages positions across multiple books (TSRY1, TSRY2, TSRY3) and securities (7 in total)
  - PositionToTradeBookingListener: modeling the listener connecting from **PositionService** to **TradeBookingService**. It processes the input trades and make corresponding changes in positions.
- pricingeservice.hpp
  - Price: modeling product price objects. Equipped with a _ToStrings_ method that converts the attributes to a string.
  - PricingService: modeling the pricing service, managing mid prices, ask-bid spreads and bid/offer status.
  - PricingConnector: modeling the pricing connector. To receive data from prices.txt, call _Subscribe()_ to convert into Price objects and update into the system.
- product.hpp: the base class that models different products. _We are mostly interested in the Bond class_.
- riskservice.hpp
  - PV01: the class modeling PV01. Values of PV01 are in **utilityfunctions.hpp**.
  - RiskService: the class modeling the risk service.
  - RiskToPositionListener: modeling the listener from **RiskService** to **PositionService**.
- soa.hpp: the base class of all the services, containing **ServiceListener**, **Service**, **Connector**.
- streamingservice.hpp
  - StreamingService: modeling the Streaming services.
  - StreamingToAlgoStreamingListener: modeling the listener connecting **StreamingToAlgoStreamingListener** to **AlgoStreamingService**. It calls the algo streaming service upon receving new data.
- tradingbookservice.hpp
  - Trade: modeling the trades.
  - TradeBookingService: modeling the trade booking service.
  - TradeBookingConnector: modeling the trade connector. To receive data from trades.txt, call _Subscribe()_ to convert into trades and update into the system.
  - TradeBookingToExecutionListener: modeling the listener connecting from **TradeBookingService** to **ExecutionService**.
- utilityfunctions.hpp: a set of utility functions, containing
  - StringToPrice: converting string bond quotes in the 1/256 conventions to double format.
  - PriceToString: converting the double format prices into bond quotes in the 1/256 conventions.
  - GetPV01: fetch the PV01 (approximate) values for the bonds. Calculation is done using Excel.
  - bondMap, bondIdMatMap, bondIdCouponMap: maps that map from bond Id to important attributes
  - FetchCusipId: fetch the bond id from maturity
  - FetchBond: given maturity / bond id, return the corresponding CUSIP Bond object.
  - GetTimeStamp: get the current time stamp with millisecond precision.
  - GetMillisecond: get the current millisecond (used in **GUIService**).
  - GenerateTradingId: generate the random trading IDs (in inquiries and trade generation part) based on C++ random engines.
  - LineToCells: used when reading input data which splits the line into a vector of strings.
- main.cpp: the test file of the project, including
  - An initialization method that generates all the required input data；
  - The main part that generate all the **BondServices** by specifying the template input data type as bonds;
  - The streamline: Generate all the services -> Link the services as required -> Use connectors to read and process the data -> Generate outputs.

# Notes:
- All the services above are in the form of a key-value pair: **{product id: corresponding class object}**.
- The information of bond yields (in calculating PV01) and coupons come from https://www.cnbc.com/quotes. For example: https://www.cnbc.com/quotes/US3Y gives the information of the 3 year bond.
- The timing function localtime() does not seem to work well with g++ on Windows (maybe --deprecated?). Using Visual Studio with pre-processor _CRT_SECURE_NO_WARNINGS can help solve this problem. Anyway, it's a minor case compared to the whole project :-)
