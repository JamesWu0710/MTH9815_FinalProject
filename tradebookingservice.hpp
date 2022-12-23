/**
 * tradebookingservice.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Breman Thuraisingham
 */
#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP

#include <string>
#include <vector>
#include "executionservice.hpp"
#include "soa.hpp"

 // Trade sides
enum Side { BUY, SELL };

/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade
{

public:

	// ctor for a trade
	Trade() = default;
	Trade(const T& _product, string _tradeId, double _price, string _book, long _quantity, Side _side);

	// Get the product
	const T& GetProduct() const;

	// Get the trade ID
	const string& GetTradeId() const;

	// Get the mid price
	double GetPrice() const;

	// Get the book
	const string& GetBook() const;

	// Get the quantity
	long GetQuantity() const;

	// Get the side
	Side GetSide() const;

private:
	T product;
	string tradeId;
	double price;
	string book;
	long quantity;
	Side side;

};

template<typename T>
Trade<T>::Trade(const T& _product, string _tradeId, double _price, string _book, long _quantity, Side _side) :
	product(_product)
{
	tradeId = _tradeId;
	price = _price;
	book = _book;
	quantity = _quantity;
	side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
	return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
	return tradeId;
}

template<typename T>
double Trade<T>::GetPrice() const
{
	return price;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
	return book;
}

template<typename T>
long Trade<T>::GetQuantity() const
{
	return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
	return side;
}

/**
* Pre-declearations to avoid errors.
*/
template<typename T>
class TradeBookingConnector;
template<typename T>
class TradeBookingToExecutionListener;

/**
 * Trade Booking Service to book trades to a particular book.
 * Keyed on trade id.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingService : public Service<string, Trade <T> >
{

public:

	// Ctor
	TradeBookingService();

	// Get data on our service given a key
	Trade<T>& GetData(string _key);

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Trade<T>& _data);

	// Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
	void AddListener(ServiceListener<Trade<T>>* _listener);

	// Get all listeners on the Service
	const vector<ServiceListener<Trade<T>>*>& GetListeners() const;

	// Get the connector of the service
	TradeBookingConnector<T>* GetConnector();

	// Get the listener of the service
	TradeBookingToExecutionListener<T>* GetListener();

	// Book the trade
	void BookTrade(Trade<T>& _trade);


private:
	map<string, Trade<T>> trades;
	vector<ServiceListener<Trade<T>>*> listeners;
	TradeBookingConnector<T>* connector;
	TradeBookingToExecutionListener<T>* listener;
};

template<typename T>
TradeBookingService<T>::TradeBookingService()
{
	trades = map<string, Trade<T>>();
	listeners = vector<ServiceListener<Trade<T>>*>();
	connector = new TradeBookingConnector<T>(this);
	listener = new TradeBookingToExecutionListener<T>(this);
}

template<typename T>
Trade<T>& TradeBookingService<T>::GetData(string _key)
{
	return trades[_key];
}

template<typename T>
void TradeBookingService<T>::OnMessage(Trade<T>& _data)
{
	string _id = _data.GetTradeId();
	trades[_id] = _data;

	for (auto& l : listeners)
	{
		l->ProcessAdd(_data);
	}
}

template<typename T>
void TradeBookingService<T>::AddListener(ServiceListener<Trade<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<Trade<T>>*>& TradeBookingService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
TradeBookingConnector<T>* TradeBookingService<T>::GetConnector()
{
	return connector;
}

template<typename T>
TradeBookingToExecutionListener<T>* TradeBookingService<T>::GetListener()
{
	return listener;
}

template<typename T>
void TradeBookingService<T>::BookTrade(Trade<T>& _trade)
{
	for (auto& l : listeners)
	{
		l->ProcessAdd(_trade);
	}
}

/**
* Trade Booking Connector subscribing data to Trading Booking Service.
* Type T is the product type.
*/
template<typename T>
class TradeBookingConnector : public Connector<Trade<T>>
{

public:
	// Ctor
	TradeBookingConnector(TradeBookingService<T>* _service);

	// Publish data to the Connector
	void Publish(Trade<T>& _data);

	// Subscribe data from the Connector
	void Subscribe(ifstream& _data);

private:
	TradeBookingService<T>* service;

};

template<typename T>
TradeBookingConnector<T>::TradeBookingConnector(TradeBookingService<T>* _service)
{
	service = _service;
}

template<typename T>
void TradeBookingConnector<T>::Publish(Trade<T>& _data) {}

// core function of this hpp file: reading data from the txt file
// formulate the trades and update into the system.
template<typename T>
void TradeBookingConnector<T>::Subscribe(ifstream& _data)
{
	string _line;
	while (getline(_data, _line))
	{
		auto _cells = lineToCells(_line);

		string _productId = _cells[0];
		string _tradeId = _cells[1];
		double _price = StringToPrice(_cells[2]);
		string _book = _cells[3];
		long _quantity = stol(_cells[4]);
		Side _side;
		if (_cells[5] == "BUY") _side = BUY;
		else if (_cells[5] == "SELL") _side = SELL;
		T _product = FetchBond(_productId);
		Trade<T> _trade(_product, _tradeId, _price, _book, _quantity, _side);
		service->OnMessage(_trade);
	}
}

/**
* Trade Booking Service Listener subscribing data from Execution Service to Trading Booking Service.
* Type T is the product type.
*/
template<typename T>
class TradeBookingToExecutionListener : public ServiceListener<ExecutionOrder<T>>
{

public:

	// Ctor
	TradeBookingToExecutionListener(TradeBookingService<T>* _service);

	// Listener callback to process an add event to the Service
	void ProcessAdd(ExecutionOrder<T>& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(ExecutionOrder<T>& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(ExecutionOrder<T>& _data);

private:

	TradeBookingService<T>* service;
	long tradeBookCount;

};

template<typename T>
TradeBookingToExecutionListener<T>::TradeBookingToExecutionListener(TradeBookingService<T>* _service)
{
	service = _service;
	tradeBookCount = 0;
}

template<typename T>
void TradeBookingToExecutionListener<T>::ProcessAdd(ExecutionOrder<T>& _data)
{
	std::vector<string> marketVec{ "TRSY1","TRSY2","TRSY3" };
	tradeBookCount++;

	T _product = _data.GetProduct();
	PricingSide _pricingSide = _data.GetPricingSide();
	string _orderId = _data.GetOrderId();
	double _price = _data.GetPrice();
	long _visibleQuantity = _data.GetVisibleQuantity();
	long _hiddenQuantity = _data.GetHiddenQuantity();

	Side _side;
	if (_pricingSide == BID) {
		_side = SELL;
	}
	if (_pricingSide == OFFER) {
		_side = BUY;
	}

	string _book = marketVec[tradeBookCount % 3];
	long _quantity = _visibleQuantity + _hiddenQuantity;

	Trade<T> _trade(_product, _orderId, _price, _book, _quantity, _side);
	service->OnMessage(_trade);
	service->BookTrade(_trade);
}

template<typename T>
void TradeBookingToExecutionListener<T>::ProcessRemove(ExecutionOrder<T>& _data) {}

template<typename T>
void TradeBookingToExecutionListener<T>::ProcessUpdate(ExecutionOrder<T>& _data) {}


#endif
