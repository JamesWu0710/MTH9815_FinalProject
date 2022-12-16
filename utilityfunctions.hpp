#ifndef utilityfunctions_hpp
#define utilityfunctions_hpp

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <map>
#include <random>
#include <cstdlib>
#include <time.h>
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

// create the maps: <maturity, <name, maturity_date>>
const map<int, pair<string, date>> bondMap({
	{2, {"91282CFX4", {2024, Nov, 30}}},
	{3, {"91282CFW6", {2025, Nov, 15}}},
	{5, {"91282CFZ9", {2027, Nov, 30}}},
	{7, {"91282CFY2", {2029, Nov, 30}}},
	{10, {"91282CFV8", {2032, Nov, 15}}},
	{20, {"912810TM0", {2042, Nov, 30}}},
	{30, {"912810TL2", {2052, Nov, 15}}} });

// fetch contract names by maturity
string FetchCusipId(int mat) {
	string id = bondMap.at(mat).first;
	return id;
}

Bond FetchBond(int mat) {
	string id = bondMap.at(mat).first;
	string ticker = "US" + to_string(mat) + "Y";
	return Bond(id, CUSIP, ticker, 0.0, bondMap.at(mat).second);
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
	return (string)time_string + '.' + m_seconds;
}

// utility function to generate random trading Ids
string GenerateTradingId(int length = 12)
{
	srand(time(0));
	string _base = "QAZWSXEDCRFVTGBYHNUJMIKOLP1472583690";
	vector<int> _randoms;
	for (int i = 0; i < length; i++) {
		_randoms.push_back(rand() % 36);
	}
	string _id = "";
	for (auto& r : _randoms)
	{
		_id.push_back(_base[r]);
	}
	return _id;
}

#endif // !utilityfunctions_hpp
