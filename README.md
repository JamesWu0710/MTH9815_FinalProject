# MTH9815_FinalProject
MTH9815 Software Engineering Final Project: BondTradingSystem

Author: Chengxun James Wu

# Description of Project Requirements
This part is adopted from Breman's project requirements.

- Develop a bond trading system for US Treasuries with seven securities: 2Y, 3Y, 5Y, 7Y, 10Y, 20Y, and 30Y. Look up the CUSIPS, coupons, and maturity dates for each security. Ticker is T.
- The idea is to connection different processes: **Listeners, Connectors that Subscribe and Publish**. We have a new definition of a Service in soa.hpp, with the concept of a ServiceListener and Connector also defined. A ServiceListener is a listener to events on the service where data is added to the service, updated on the service, or removed from the service. A Connector is a class that flows data into the Service from some connectivity source (e.g. a socket, file, etc) via the Service.OnMessage() method. The Publish() method on the Connector publishes data to the connectivity source and can be invoked from a Service. Some Connectors are publish-only that do not invoke Service.OnMessage(). Some Connectors are subscribe-only where Publish() does nothing. Other Connectors can do both publish and subscribe.

# Project Design Idea
- To create different Bond services, we first create the template classes that accepts a geenral product defined in product.hpp. Then in main.cpp, we specify the product type to be **Bonds**. This allows a more **generic framework that can be transplanted to other products easily**.
- Regarding data, we generate the data according to the rules defined in the project requirements. This is mainly done in datageneration.hpp, and the input data can be generated if we call the Initialize method in the main file.
- **How to connect different services?** Following the guidelines, I adopt the method of adding listeners from one service to another. For example, I have AlgoExecutionToExecutionListener that specifies that: AlgoExecution sends information to Execution, which forms the connections between two services. Similar examples can also be found in other services. This linking design is also instrumental in the project.
- **How to read and store data?** Following the guidelines, I adopt the Publish and Subscribe with connectors outlined in the project descriptions. For example, PricingService uses a Connector to subscribe data from the stored prices information. After reading the data, the connector calls the *OnMessage* method of the corresponding service to apply actions on the data obtained to pass into the system.
- The codes follow the original codes provided by the Professor.


# How to run the codes?
- To compile it using g++, we can follow **g++ -std=c++17 main.cpp -o test -I C:/boost/include/boost-1_80 -L C:/boost/lib -lws2_32 -lwsock32**. Remember to specify your paths of the boost library! Then, run test.exe to see the operations.
- Or, you can directly run the test.exe file contained in this repository.
- If you prefer Visual Studio, please refer to the zip file contained in this repository.

# File Descriptions
- algoexecutionservice.hpp
- algostreamingservice.hpp
- datageneration.hpp
- executionservice.hpp
- GUIservice.hpp
- historicaldataservice.hpp
- marketdataservice.hpp
- positionservice.hpp
- pricingeservice.hpp
- product.hpp
- riskservice.hpp
- soa.hpp
- streamingservice.hpp
- tradingbookservice.hpp
- utilityfunctions.hpp
- main.cpp

# Notes:
- The timing function localtime() does not seem to work well with g++ on Windows. Using Visual Studio with pre-processor _CRT_SECURE_NO_WARNINGS can help solve this problem. Anyway, it's a minor case compared to the whole project :-)
