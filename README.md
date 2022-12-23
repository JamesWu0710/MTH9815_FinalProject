# MTH9815_FinalProject
MTH9815 Software Engineering Final Project: BondTradingSystem

Author: Chengxun James Wu

# Description of Project Requirements
This part is adopted from Breman's project requirements.

- Develop a bond trading system for US Treasuries with seven securities: 2Y, 3Y, 5Y, 7Y, 10Y, 20Y, and 30Y. Look up the CUSIPS, coupons, and maturity dates for each security. Ticker is T.
- The idea is to connection different processes: **Listeners, Connectors that Subscribe and Publish**. We have a new definition of a Service in soa.hpp, with the concept of a ServiceListener and Connector also defined. A ServiceListener is a listener to events on the service where data is added to the service, updated on the service, or removed from the service. A Connector is a class that flows data into the Service from some connectivity source (e.g. a socket, file, etc) via the Service.OnMessage() method. The Publish() method on the Connector publishes data to the connectivity source and can be invoked from a Service. Some Connectors are publish-only that do not invoke Service.OnMessage(). Some Connectors are subscribe-only where Publish() does nothing. Other Connectors can do both publish and subscribe.

# Project Design Idea

# How to run the codes?
- To compile it using g++, we can follow **g++ -std=c++17 main.cpp -o test -I C:/boost/include/boost-1_80 -L C:/boost/lib -lws2_32 -lwsock32**. Remember to specify your paths of the boost library! Then, run test.exe to see the operations.
- Or, you can directly run the test.exe file contained in this repository.
- If you prefer Visual Studio, please refer to the zip file contained in this repository.

# File Descriptions

# Notes:
- The timing function localtime() does not seem to work well with g++ on Windows. Using Visual Studio with pre-processor _CRT_SECURE_NO_WARNINGS can help solve this problem. Anyway, it's a minor case compared to the whole project :-)
