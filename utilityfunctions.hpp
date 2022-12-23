#define _CRT_SECURE_NO_WARNINGS 1

#ifndef UTILITY_FUNCTIONS_HPP
#define UTILITY_FUNCTIONS_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <map>
#include <random>
#include <cstdlib>
#include <time.h>
#include <fstream>
#include "products.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;
using namespace boost::gregorian;

double StringToPrice(const string& str_price) {
	auto separate_pos = str_price.find('-');

	// integer part and float part(s)
	double res = stoi(str_price.substr(0, separate_pos));
	string xy = str_price.substr(separate_pos + 1, 2);
	res += stoi(xy) / 32.0;
	char z = str_price[str_price.size() - 1];
	if (z == '+') {
		res += 1.0 / 64.0;
	}
	else {
		res += int(z - '0') / 256.0;
	}
	return res;
}

string PriceToString(double price) {
	int int_part = int(price);
	double flt_part = price - int_part;

	string res;
	res += to_string(int_part) + "-";
	flt_part *= 32;
	int_part = int(flt_part);
	if (int_part < 10) {
		res += '0';
	}
	res += to_string(int_part);

	flt_part -= double(int_part);
	// determine the last digit

	int_part = int(flt_part * 8.);
	if (int_part == 4) {
		res += "+";
	}
	else {
		res += to_string(int_part);
	}
	return res;
}

// the function to fetch the PV01 value.
// data calculated at December 19th, 2022
// although not very accurate, but the scales make sense
// data source (yield and bond price, etc): https://www.cnbc.com/quotes
double GetPV01(string _id) {
	map<string, double> PV01Map({
		{"91282CFX4", 0.01967211},
		{"91282CFW6", 0.028849852},
		{"91282CFZ9", 0.048555605 },
		{"91282CFY2", 0.068303332 },
		{"91282CFV8", 0.08071955 },
		{"912810TM0", 0.118325668 },
		{"912810TL2", 0.185319634 } });
	return PV01Map[_id];
}

// create the maps: <maturity, <name, maturity_date>>
const map<int, pair<string, date>> bondMap({
	{2, {"91282CFX4", {2024, Nov, 30}}},
	{3, {"91282CFW6", {2025, Nov, 15}}},
	{5, {"91282CFZ9", {2027, Nov, 30}}},
	{7, {"91282CFY2", {2029, Nov, 30}}},
	{10, {"91282CFV8", {2032, Nov, 15}}},
	{20, {"912810TM0", {2042, Nov, 30}}},
	{30, {"912810TL2", {2052, Nov, 15}}} });

const map<string, int> bondIdMatMap({
	{"91282CFX4", 2},
	{"91282CFW6", 3},
	{"91282CFZ9", 5 },
	{"91282CFY2", 7 },
	{"91282CFV8", 10 },
	{"912810TM0", 20 },
	{"912810TL2", 30 }});

// the map between bond id and their coupons, in order to fetch the corresponding coupon rate.
// data from internet
const map<string, double> bondIdCouponMap({
	{"91282CFX4", 0.04500},
	{"91282CFW6", 0.04000},
	{"91282CFZ9", 0.03875},
	{"91282CFY2", 0.03875},
	{"91282CFV8", 0.04125},
	{"912810TM0", 0.04000},
	{"912810TL2", 0.04000}});

// fetch contract names by maturity
string FetchCusipId(int mat) {
	string id = bondMap.at(mat).first;
	return id;
}

Bond FetchBond(int mat) {
	string id = bondMap.at(mat).first;
	string ticker = "US" + to_string(mat) + "Y";
	return Bond(id, CUSIP, ticker, bondIdCouponMap.at(id), bondMap.at(mat).second);
}

Bond FetchBond(string _id) {
	int _mat = bondIdMatMap.at(_id);
	return FetchBond(_mat);
}

// utility function that fetches time
string GetTimeStamp() {
	auto curr_time = chrono::system_clock::now();
	auto curr_time_t = chrono::system_clock::to_time_t(curr_time);

	// milliseconed precision?
	auto seconds = chrono::time_point_cast<chrono::seconds>(curr_time);
	auto milliseconds = (chrono::duration_cast<chrono::milliseconds>(curr_time - seconds)).count();

	string m_seconds;
	if (milliseconds < 10) {
		m_seconds = "00" + to_string(milliseconds);
	}
	else if ((milliseconds >= 10) && (milliseconds < 100)) {
		m_seconds = "0" + to_string(milliseconds);
	}
	else {
		m_seconds = to_string(milliseconds);
	}

	char time_string[24];
	strftime(time_string, 24, "%F %T", localtime(&curr_time_t));
	return static_cast<string>(time_string) + "." + m_seconds;
}

// utility function that fetches current millisecond
long GetMillisecond()
{
	auto timePoint = chrono::system_clock::now();
	auto sec = chrono::time_point_cast<chrono::seconds>(timePoint);
	auto millisec = chrono::duration_cast<chrono::milliseconds>(timePoint - sec);
	long _millisecCount = static_cast<long>(millisec.count());
	return _millisecCount;
}

// utility function to generate random trading Ids
string GenerateTradingId(int length = 12)
{
	thread_local random_device rd;
	thread_local mt19937_64 gen(rd());
	thread_local uniform_real_distribution<double> d(0.0, 1.0);

	string _base = "ZAQWSXCDERFVBGTYHNMJUIKLOP1472583690";
	string ID = "";
	for (int i = 0; i < length; i++) {
		double _r = d(gen);
		ID.push_back(_base[(int)(_r * 36)]);
	}
	return ID;
}

// utility function to separate the line
// deliminator is ","
std::vector<string> lineToCells(string line) {
	stringstream lineStream(line);
	string cell;
	vector<string> cells;
	while (getline(lineStream, cell, ','))
	{
		cells.push_back(cell);
	}
	return cells;
}

#endif // !UTILITY_FUNCTIONS_HPP
