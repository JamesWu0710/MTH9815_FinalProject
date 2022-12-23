#define _CRT_SECURE_NO_WARNINGS 1

// main file for the final project
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

int main() {
	std::cout << GetTimeStamp() << " Program Started. " << std::endl;
	//initialize(); // run if there are no existing data txts
	std::cout << GetTimeStamp() << " Data Prepared." << std::endl;

	// 1) Service initialization
	PricingService<Bond> pricingService;
	TradeBookingService<Bond> tradeBookingService;
	PositionService<Bond> positionService;
	RiskService<Bond> riskService;
	MarketDataService<Bond> marketDataService;
	AlgoExecutionService<Bond> algoExecutionService;
	AlgoStreamingService<Bond> algoStreamingService;
	GUIService<Bond> guiService;
	ExecutionService<Bond> executionService;
	StreamingService<Bond> streamingService;
	InquiryService<Bond> inquiryService;
	std::cout << "Services initialized.\n";

	// 2) Historical Data Services
	HistoricalDataService<Position<Bond>> historicalPositionService(POSITION);
	HistoricalDataService<PV01<Bond>> historicalRiskService(RISK);
	HistoricalDataService<ExecutionOrder<Bond>> historicalExecutionService(EXECUTION);
	HistoricalDataService<PriceStream<Bond>> historicalStreamingService(STREAMING);
	HistoricalDataService<Inquiry<Bond>> historicalInquiryService(INQUIRY);
	std::cout << "Historical services initialized.\n";

	// 3) linking using listeners
	std::cout << GetTimeStamp() << " Services Linking... " << std::endl;
	pricingService.AddListener(algoStreamingService.GetListener());
	pricingService.AddListener(guiService.GetListener());
	algoStreamingService.AddListener(streamingService.GetListener());
	streamingService.AddListener(historicalStreamingService.GetServiceListener());
	marketDataService.AddListener(algoExecutionService.GetListener());
	algoExecutionService.AddListener(executionService.GetListener());
	executionService.AddListener(tradeBookingService.GetListener());
	executionService.AddListener(historicalExecutionService.GetServiceListener());
	tradeBookingService.AddListener(positionService.GetListener());
	positionService.AddListener(riskService.GetListener());
	positionService.AddListener(historicalPositionService.GetServiceListener());
	riskService.AddListener(historicalRiskService.GetServiceListener());
	inquiryService.AddListener(historicalInquiryService.GetServiceListener());
	std::cout << GetTimeStamp() << " Services Linked." << std::endl;

	// 4) Storing data into local files.
	std::cout << GetTimeStamp() << " Price Data Processing..." << std::endl;
	ifstream priceData("prices.txt");
	pricingService.GetConnector()->Subscribe(priceData);
	std::cout << GetTimeStamp() << " Price Data Processed." << std::endl;

	std::cout << GetTimeStamp() << " Trade Data Processing..." << std::endl;
	ifstream tradeData("trades.txt");
	tradeBookingService.GetConnector()->Subscribe(tradeData);
	std::cout << GetTimeStamp() << " Trade Data Processed." << std::endl;

	std::cout << GetTimeStamp() << " Market Data Processing..." << std::endl;
	ifstream marketData("marketdata.txt");
	marketDataService.GetConnector()->Subscribe(marketData);
	std::cout << GetTimeStamp() << " Market Data Processed." << std::endl;

	std::cout << GetTimeStamp() << " Inquiry Data Processing..." << std::endl;
	ifstream inquiryData("inquiries.txt");
	inquiryService.GetConnector()->Subscribe(inquiryData);
	std::cout << GetTimeStamp() << " Inquiry Data Processed." << std::endl;

	std::cout << GetTimeStamp() << "Program Ending..." << std::endl;
	std::cout << GetTimeStamp() << "Program Ended." << std::endl;
	system("pause");
	return 0;
}