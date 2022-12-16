/**
 * pricingservice.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Breman Thuraisingham
 * @co-author: James Wu
 */
#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include "datageneration.hpp"
#include <string>
#include "soa.hpp"

 /**
  * A price object consisting of mid and bid/offer spread.
  * Type T is the product type.
  */
template<typename T>
class Price
{

public:

	// ctor for a price
	Price(const T& _product, double _mid, double _bidOfferSpread);

	// Get the product
	const T& GetProduct() const;

	// Get the mid price
	double GetMid() const;

	// Get the bid/offer spread around the mid
	double GetBidOfferSpread() const;

	// Convert the information into strings
	vector<string> ToString() const;
private:
	const T& product;
	double mid;
	double bidOfferSpread;

};

// pre-declaration to avoid errors
template<typename T>
class PricingConnector;

/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PricingService : public Service<string, Price <T> >
{
public:
	PricingService();
	~PricingService();

	// fetch orderbook with given product id
	Price<T>& GetPriceData(const string& _id);

	// call back function for the connector
	void OnMessage(Price<T>& _data);

	// add listener to the service
	void AddListener(ServiceListener<Price<T>>* listener);

	// fetch the active listeners on the service
	const vector<ServiceListener<Price<T>>*>& GetListeners() const;

	// fetch the connector
	PricingConnector<T>* GetConnector();

private:
	map<string, Price<T>> prices;
	vector<ServiceListener<Price<T>>*> listeners;
	PricingConnector<T>* connector;
};

template<typename T>
Price<T>::Price(const T& _product, double _mid, double _bidOfferSpread) :
	product(_product)
{
	mid = _mid;
	bidOfferSpread = _bidOfferSpread;
}

template<typename T>
const T& Price<T>::GetProduct() const
{
	return product;
}

template<typename T>
double Price<T>::GetMid() const
{
	return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const
{
	return bidOfferSpread;
}

template<typename T>
vector<string> Price<T>::ToString() const
{
	string _product = product.GetProductId();
	string _mid = PriceToString(mid);
	string _bidOfferSpread = PriceToString(bidOfferSpread);

	vector<string> _strings;
	_strings.push_back(_product);
	_strings.push_back(_mid);
	_strings.push_back(_bidOfferSpread);
	return _strings;
}

template<typename T>
PricingService<T>::PricingService() :prices(), listeners(), connector()
{
	prices = map<string, Price<T>>();
	listeners = vector<ServiceListener<Price<T>>*>();
	connector = new PricingConnector<T>(this);
}

template<typename T>
PricingService<T>::~PricingService()
{
	delete connector;
}

template<typename T>
 Price<T>& PricingService<T>::GetPriceData(const string& _id)
{
	 return prices[_id];
}

 template<typename T>
 void PricingService<T>::OnMessage(Price<T>& _data)
 {
	 string product_id = _data.GetProduct().GetProductId();
	 prices[product_id] = _data;

	 for (auto& listener : listeners) {
		 listener->ProcessAdd(_data);
	 }
 }

 template<typename T>
 void PricingService<T>::AddListener(ServiceListener<Price<T>>* listener)
 {
	 listeners.push_back(listeners);
 }

 template<typename T>
 const vector<ServiceListener<Price<T>>*>& PricingService<T>::GetListeners() const
 {
	 return listeners;
 }

 template<typename T>
 PricingConnector<T>* PricingService<T>::GetConnector()
 {
	 return connector;
 }

 /* Now defining the pricing connector
 * subscribing data to the pricing service.
 * When we subscribe, we fetch the data from th required path.
 */



#endif
