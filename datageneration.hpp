#ifndef data_generation_hpp
#define data_generation_hpp

// datageneration_hpp
// The file to generate price data
// Author: James Wu

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <map>
#include <random>
#include "products.hpp"
#include "utilityfunctions.hpp"
#include "tradebookingservice.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;
using namespace boost::gregorian;

// try to generate data
// the idea of data generation:
// all the prices double between 99+1/256*2, 101-1/256*2 (to avoid reaching the lower limit)
// all the ask-bid spreads osciillate between 1/128 and 1/64 (2*mintick, 4*mintick)
// we toss a coin. given central price, C, the bid price can be
// 1) C-minTick 2) C-2*minTick with prob .5
// the ask price can be
// 1) C+minTick 2) C+2*minTick with prob .5
void GenerateProductPrice(string _id, int _size, ofstream& file) {

	// random engine
	// bernoulli RV, ready to generate the "randomness"
	thread_local random_device rd;
	thread_local mt19937_64 gen(rd());
	thread_local bernoulli_distribution d(0.5);

	double minTick = 1 / 256.0;
	double LOW_LIMIT = 99.0 + minTick * 2.0;
	double UPPER_LIMIT = 101.0 - minTick * 2.0;

	double central_price = LOW_LIMIT;
	bool up = true;

	// Start from 99.0
	// going up until reach UPPER_LIMIT
	// each movement we use a bernoulli rng to generate the final price
	for (int i = 0; i < _size; i++) {
		double ask = central_price + minTick;
		double bid = central_price - minTick;

		bool toss1 = d(gen);
		bool toss2 = d(gen);

		if (toss1) {
			ask += minTick;
		}
		if (toss2) {
			bid -= minTick;
		}

		// central price movement
		if (central_price == UPPER_LIMIT) {
			up = false;
		}
		if (central_price == LOW_LIMIT) {
			up = true;
		}

		central_price = up ? central_price + minTick : central_price - minTick;

		// store data
		file << _id << "," << PriceToString(bid) << "," << PriceToString(ask) << std::endl;
	}
}

void GeneratePricesAll() {
	const string save_path("prices.txt");
	ofstream file(save_path);

	const int orderSize = 1000000;
	for (const auto& [mat, bond] : bondMap) {
		std::cout << "Generating prices for security " << bond.first << " ...\n";
		GenerateProductPrice(bond.first, orderSize, file);
	}
}

// generate the market data
void GenerateProductMarketData(string _id, int _size, ofstream& file) {
	double mintick = 1.0 / 256.0;
	vector<int> volumeVec{ 10000000,20000000,30000000,40000000,50000000 };
	vector<double> spreadVec{ 2 * mintick ,4 * mintick ,6 * mintick ,8 * mintick };
	double LOW = 99.0 + mintick;
	double UPPER = 101.0 - mintick;
	bool up = true;
	double central_price = LOW;
	return;
}

// generate the trade data
// generaet price between 99 and 101, we use some randomness
void GenerateProductTradeData(string _id, int _size, ofstream& file) {
	srand(time(0));
	double mintick = 1.0 / 256.0;
	vector<int> volumeVec{ 10000000,20000000,30000000,40000000,50000000 };
	for (int i = 0; i < _size; i++) {
		Side _side = (i % 2) ? BUY : SELL;
		string _string_side = _side == BUY ? "BUY" : "SELL";
		int _volume = volumeVec[i % 5];
		int _numTicks = rand() % 512;
		string _book_name = "TRSY" + to_string(rand() % 3 + 1);
		double _price = 99.0 + mintick * (double)_numTicks;

		file << _id << "," << GenerateTradingId() << "," << PriceToString(_price) << "," << _book_name << "," << _volume << "," << _string_side << std::endl;
	}
}

void GenerateAllTradeData() {
	const string save_path("trades.txt");
	ofstream file(save_path);
	const int tradeSize = 10;
	for (const auto& [mat, bond] : bondMap) {
		std::cout << "Generating trades for security " << bond.first << " ...\n";
		GenerateProductTradeData(bond.first, tradeSize, file);
	}
}

#endif // !data_generation_hpp
