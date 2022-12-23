#define _CRT_SECURE_NO_WARNINGS 1

/* Main file for the final project
* Author: Chengxun James Wu
*/
#include <iostream>
#include "soa.hpp"
#include "products.hpp"
#include "algoexecutionservice.hpp"
#include "algostreamingservice.hpp"
#include "executionservice.hpp"
#include "GUIservice.hpp"
#include "historicaldataservice.hpp"
#include "inquiryservice.hpp"
#include "marketdataservice.hpp"
#include "positionservice.hpp"
#include "pricingservice.hpp"
#include "riskservice.hpp"
#include "streamingservice.hpp"
#include "tradebookingservice.hpp"
#include "datageneration.hpp"
#include "utilityfunctions.hpp"
#include <random>

void initialize() {
	GenerateAllPrices();
	GenerateAllTradeData();
	GenerateAllMarketData();
	GenerateAllInquiryData();
}

/* We implement the Bond services by specifiying T as Bond in the templates
specified by the header files.
*/

int main() {
	std::cout << GetTimeStamp() << " Program Started. " << std::endl;
	//initialize(); // run if there are no existing data txts
	std::cout << GetTimeStamp() << " Data Prepared." << std::endl;

	// 1) Service initialization. Take T as bonds.
	MarketDataService<Bond> BondMarketDataService;
	PricingService<Bond> BondPricingService;
	TradeBookingService<Bond> BondTradeBookingService;
	PositionService<Bond> BondPositionService;
	RiskService<Bond> BondRiskService;
	AlgoExecutionService<Bond> BondAlgoExecutionService;
	AlgoStreamingService<Bond> BondAlgoStreamingService;
	ExecutionService<Bond> BondExecutionService;
	StreamingService<Bond> BondStreamingService;
	InquiryService<Bond> BondInquiryService;
	GUIService<Bond> BondGUIService;
	std::cout << "Services initialized.\n";

	// 2) Historical Data Services
	HistoricalDataService<Position<Bond>> histPositionService(POSITION);
	HistoricalDataService<PV01<Bond>> histRiskService(RISK);
	HistoricalDataService<ExecutionOrder<Bond>> histExecutionService(EXECUTION);
	HistoricalDataService<PriceStream<Bond>> histStreamingService(STREAMING);
	HistoricalDataService<Inquiry<Bond>> histInquiryService(INQUIRY);
	std::cout << "Historical services initialized.\n";

	// 3) linking using listeners (so many links though)
	std::cout << GetTimeStamp() << " Services Linking... " << std::endl;

	// 3.1 GUI listens to PricingService
	BondPricingService.AddListener(BondGUIService.GetListener());
	
	// 3.2 histStreaming -> streaming -> AlgoStreaming -> Pricing
	BondPricingService.AddListener(BondAlgoStreamingService.GetListener());
	BondAlgoStreamingService.AddListener(BondStreamingService.GetListener());
	BondStreamingService.AddListener(histStreamingService.GetServiceListener());

	// 3.3 histExe -> Exe -> AlgoExe -> MarketData
	BondMarketDataService.AddListener(BondAlgoExecutionService.GetListener());
	BondAlgoExecutionService.AddListener(BondExecutionService.GetListener());
	BondExecutionService.AddListener(histExecutionService.GetServiceListener());

	// 3.4 TradeBooking -> Execution.
	BondExecutionService.AddListener(BondTradeBookingService.GetListener());

	// 3.5 histPos & histRisk -> Pos & Risk; Risk -> Pos and Pos -> Trade Booking
	BondTradeBookingService.AddListener(BondPositionService.GetListener());
	BondPositionService.AddListener(BondRiskService.GetListener());
	BondPositionService.AddListener(histPositionService.GetServiceListener());
	BondRiskService.AddListener(histRiskService.GetServiceListener());

	// 3.6 histInquiry -> inquiry
	BondInquiryService.AddListener(histInquiryService.GetServiceListener());
	std::cout << GetTimeStamp() << " Services linked successfully." << std::endl;

	// 4) Recording into local files.
	std::cout << GetTimeStamp() << " Processing Price data..." << std::endl;
	ifstream priceData("prices.txt");
	BondPricingService.GetConnector()->Subscribe(priceData);
	std::cout << GetTimeStamp() << " Price data processed successfully!" << std::endl;

	std::cout << GetTimeStamp() << " Processing Trade data... " << std::endl;
	ifstream tradeData("trades.txt");
	BondTradeBookingService.GetConnector()->Subscribe(tradeData);
	std::cout << GetTimeStamp() << " Trade data processed successfully!" << std::endl;

	std::cout << GetTimeStamp() << " Processing Market data..." << std::endl;
	ifstream marketData("marketdata.txt");
	BondMarketDataService.GetConnector()->Subscribe(marketData);
	std::cout << GetTimeStamp() << " Market data processed successfully!" << std::endl;

	std::cout << GetTimeStamp() << " Processing inquiry data..." << std::endl;
	ifstream inquiryData("inquiries.txt");
	BondInquiryService.GetConnector()->Subscribe(inquiryData);
	std::cout << GetTimeStamp() << " Inquiry data processed successfully!" << std::endl;

	std::cout << GetTimeStamp() << " Finished the tasks, now exiting the program..." << std::endl;
	system("pause");
	return 0;
}