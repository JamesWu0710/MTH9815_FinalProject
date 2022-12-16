// main file for the final project
#include "marketdataservice.hpp"
#include <iostream>
#include "datageneration.hpp"
#include "pricingservice.hpp"
#include <random>

int main() {
	std::cout << GetTimeStamp() << std::endl;
	//GeneratePricesAll(); run this for the first time if there's no data.
	GenerateAllTradeData();
	return 0;
}