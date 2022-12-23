# MTH9815_FinalProject
MTH9815 Software Engineering Final Project: BondTradingSystem

Author: Chengxun James Wu

# Project Design Idea

# Description of Project Requirements

# How to run the codes?
- To compile it using g++, we can follow **g++ -std=c++17 main.cpp -o test -I C:/boost/include/boost-1_80 -L C:/boost/lib -lws2_32 -lwsock32**. Remember to specify your paths of the boost library! Then, run test.exe to see the operations.
- Or, you can directly run the test.exe file contained in this repository.
- If you prefer Visual Studio, please refer to the zip file contained in this repository.

# File Descriptions

# Notes:
- The timing function localtime() does not seem to work well with g++ on Windows. Using Visual Studio with pre-processor _CRT_SECURE_NO_WARNINGS can help solve this problem. Anyway, it's a minor case compared to the whole project :-)
