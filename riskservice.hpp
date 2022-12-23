/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Breman Thuraisingham
 * @coauthor James Wu
 */
#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include "soa.hpp"
#include "positionservice.hpp"

 /**
  * PV01 risk.
  * Type T is the product type.
  */
template<typename T>
class PV01
{

public:

	// ctor for a PV01 value
	PV01() = default;
	PV01(const T& _product, double _pv01, long _quantity);

	// Get the product on this PV01 value
	const T& GetProduct() const;

	// Get the PV01 value
	double GetPV01() const;

	// Get the quantity that this risk value is associated with
	long GetQuantity() const;

	// Set the quantity that this risk value is associated with
	void SetQuantity(long _q);

	// To string, easier to output and store
	vector<string> ToStrings() const;

private:
	T product;
	double pv01;
	long quantity;

};

template<typename T>
PV01<T>::PV01(const T& _product, double _pv01, long _quantity) :
	product(_product)
{
	pv01 = _pv01;
	quantity = _quantity;
}

template<typename T>
const T& PV01<T>::GetProduct() const
{
	return product;
}

template<typename T>
double PV01<T>::GetPV01() const
{
	return pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const
{
	return quantity;
}

template<typename T>
void PV01<T>::SetQuantity(long _q)
{
	quantity = _q;
}

template<typename T>
vector<string> PV01<T>::ToStrings() const
{
	string _product = product.GetProductId();
	string _pv01 = to_string(pv01);
	string _quantity = to_string(quantity);

	vector<string> _strings{ _product ,_pv01,_quantity };
	return _strings;
}

/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{

public:

	// ctor for a bucket sector
	BucketedSector(const vector<T>& _products, string _name);

	// Get the products associated with this bucket
	const vector<T>& GetProducts() const;

	// Get the name of the bucket
	const string& GetName() const;

private:
	vector<T> products;
	string name;

};

template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
	products(_products)
{
	name = _name;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const
{
	return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const
{
	return name;
}

// Implementing the RiskService
/**
* Pre-declearations to avoid errors.
*/
template<typename T>
class RiskToPositionListener;		// the listener connecting risk and position service(s)


/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string, PV01 <T> >
{

public:

	// ctor
	RiskService();

	// Add a position that the service will risk
	void AddPosition(Position<T>& position);

	// Get the bucketed risk for the bucket sector
	const PV01< BucketedSector<T> >& GetBucketedRisk(const BucketedSector<T>& sector) const;

	// Get data via a key
	PV01<T>& GetData(string _key);

	// The callback function upon receiving new data
	void OnMessage(PV01<T>& _data);

	// Add a listener to the Service
	void AddListener(ServiceListener<PV01<T>>* _listener);

	// Fetch all listeners on the Service
	const vector<ServiceListener<PV01<T>>*>& GetListeners() const;

	// Fetch the (risk-position specific) listener of the service
	RiskToPositionListener<T>* GetListener();

private:
	map<string, PV01<T>> pv01s;
	vector<ServiceListener<PV01<T>>*> listeners;
	RiskToPositionListener<T>* listener;
};

template<typename T>
RiskService<T>::RiskService()
{
	pv01s = map<string, PV01<T>>();
	listeners = vector<ServiceListener<PV01<T>>*>();
	listener = new RiskToPositionListener<T>(this);
}

template<typename T>
PV01<T>& RiskService<T>::GetData(string _key)
{
	return pv01s[_key];
}

template<typename T>
void RiskService<T>::OnMessage(PV01<T>& _data)
{
	string id = _data.GetProduct().GetProductId();
	pv01s[id] = _data;
}

template<typename T>
void RiskService<T>::AddListener(ServiceListener<PV01<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<PV01<T>>*>& RiskService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
RiskToPositionListener<T>* RiskService<T>::GetListener()
{
	return listener;
}

// important, add position function
// connection with the Position class
template<typename T>
void RiskService<T>::AddPosition(Position<T>& _position)
{
	T _product = _position.GetProduct();
	string _id = _product.GetProductId();
	double _pv01Value = GetPV01(_id);		// call the utility function to fetch the PV
	long _quantity = _position.GetAggregatePosition();
	PV01<T> _pv01(_product, _pv01Value, _quantity);
	pv01s[_id] = _pv01;

	for (auto& l : listeners)
	{
		l->ProcessAdd(_pv01);
	}
}

template<typename T>
const PV01<BucketedSector<T>>& RiskService<T>::GetBucketedRisk(const BucketedSector<T>& _sector) const
{
	BucketedSector<T> _product = _sector;
	double _pv01 = 0.0;
	long _quantity = 1;

	vector<T>& _products = _sector.GetProducts();

	// summing all the risks.
	for (auto& p : _products)
	{
		string _pId = p.GetProductId();
		long _q = pv01s[_pId].GetQuantity();
		double _val = pv01s[_pId].GetPV01();
		_pv01 += _val * (double)_q;
	}

	return PV01<BucketedSector<T>>(_product, _pv01, _quantity);
}

/**
* Risk Service Listener subscribing data from Position Service to Risk Service.
* Type T is the product type.
*/
template<typename T>
class RiskToPositionListener : public ServiceListener<Position<T>>
{

private:

	RiskService<T>* service;

public:

	// Ctor
	RiskToPositionListener(RiskService<T>* _service);

	// Listener callback to process an add event to the Service
	void ProcessAdd(Position<T>& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(Position<T>& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Position<T>& _data);

};

template<typename T>
RiskToPositionListener<T>::RiskToPositionListener(RiskService<T>* _service)
{
	service = _service;
}

template<typename T>
void RiskToPositionListener<T>::ProcessAdd(Position<T>& _data)
{
	service->AddPosition(_data);
}

// do nothing for these methods (not required)
template<typename T>
void RiskToPositionListener<T>::ProcessRemove(Position<T>& _data) {}

template<typename T>
void RiskToPositionListener<T>::ProcessUpdate(Position<T>& _data) {}

#endif
