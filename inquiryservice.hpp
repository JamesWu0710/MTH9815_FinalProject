/**
 * inquiryservice.hpp
 * Defines the data types and Service for customer inquiries.
 *
 * @author Breman Thuraisingham
 * @co-author James Wu
 */
#ifndef INQUIRY_SERVICE_HPP
#define INQUIRY_SERVICE_HPP

#include "soa.hpp"
#include "tradebookingservice.hpp"
#include "utilityfunctions.hpp"

 // Various inqyury states
enum InquiryState { RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };

/**
 * Inquiry object modeling a customer inquiry from a client.
 * Type T is the product type.
 */
template<typename T>
class Inquiry
{

public:

	// ctor for an inquiry
	Inquiry() = default;
	Inquiry(string _inquiryId, const T& _product, Side _side, long _quantity, double _price, InquiryState _state);

	// Get the inquiry ID
	const string& GetInquiryId() const;

	// Get the product
	const T& GetProduct() const;

	// Get the side on the inquiry
	Side GetSide() const;

	// Get the quantity that the client is inquiring for
	long GetQuantity() const;

	// Get the price that we have responded back with
	double GetPrice() const;

	// Get the current state on the inquiry
	InquiryState GetState() const;

	// Set the price that we have responded back with
	void SetPrice(double _price);

	// Set the current state on the inquiry
	void SetState(InquiryState _state);

	// Store attributes as strings
	vector<string> ToStrings() const;

private:
	string inquiryId;
	T product;
	Side side;
	long quantity;
	double price;
	InquiryState state;

};

template<typename T>
Inquiry<T>::Inquiry(string _inquiryId, const T& _product, Side _side, long _quantity, double _price, InquiryState _state) :
	product(_product)
{
	inquiryId = _inquiryId;
	side = _side;
	quantity = _quantity;
	price = _price;
	state = _state;
}

template<typename T>
const string& Inquiry<T>::GetInquiryId() const
{
	return inquiryId;
}

template<typename T>
const T& Inquiry<T>::GetProduct() const
{
	return product;
}

template<typename T>
Side Inquiry<T>::GetSide() const
{
	return side;
}

template<typename T>
long Inquiry<T>::GetQuantity() const
{
	return quantity;
}

template<typename T>
double Inquiry<T>::GetPrice() const
{
	return price;
}

template<typename T>
InquiryState Inquiry<T>::GetState() const
{
	return state;
}

template<typename T>
void Inquiry<T>::SetPrice(double _price)
{
	price = _price;
}

template<typename T>
void Inquiry<T>::SetState(InquiryState _state)
{
	state = _state;
}

template<typename T>
vector<string> Inquiry<T>::ToStrings() const
{
	string _inquiryId = inquiryId;
	string _product = product.GetProductId();
	string _side;
	switch (side)
	{
	case BUY:
		_side = "BUY";
		break;
	case SELL:
		_side = "SELL";
		break;
	}
	string _quantity = to_string(quantity);
	string _price = PriceToString(price);
	string _state;
	if (state == RECEIVED) {
		_state = "RECEIVED";
	}
	if (state == QUOTED) {
		_state = "QUOTED";
	}
	if (state == DONE) {
		_state = "DONE";
	}
	if (state == REJECTED) {
		_state = "REJECTED";
	}
	if (state == CUSTOMER_REJECTED) {
		_state = "CUSTOMER_REJECTED";
	}

	vector<string> _strings{ _inquiryId ,_product,_side,_quantity,_price,_state };
	return _strings;
}

/**
* Pre-declearations to avoid errors.
*/
template<typename T>
class InquiryConnector;

/**
 * Service for customer inquirry objects.
 * Keyed on inquiry identifier (NOTE: this is NOT a product identifier since each inquiry must be unique).
 * Type T is the product type.
 */
template<typename T>
class InquiryService : public Service<string, Inquiry <T> >
{

public:

	// Ctor
	InquiryService();

	// Get data on our service given a key
	Inquiry<T>& GetData(string _key);

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Inquiry<T>& _data);

	// Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
	void AddListener(ServiceListener<Inquiry<T>>* _listener);

	// Get all listeners on the Service
	const vector<ServiceListener<Inquiry<T>>*>& GetListeners() const;

	// Get the connector of the service
	InquiryConnector<T>* GetConnector();

	// Send a quote back to the client
	void SendQuote(const string& _inquiryId, double _price);

	// Reject an inquiry from the client
	void RejectInquiry(const string& _inquiryId);


private:

	map<string, Inquiry<T>> inquiries;
	vector<ServiceListener<Inquiry<T>>*> listeners;
	InquiryConnector<T>* connector;
};

template<typename T>
InquiryService<T>::InquiryService()
{
	inquiries = map<string, Inquiry<T>>();
	listeners = vector<ServiceListener<Inquiry<T>>*>();
	connector = new InquiryConnector<T>(this);
}

template<typename T>
Inquiry<T>& InquiryService<T>::GetData(string _key)
{
	return inquiries[_key];
}


// core function in this hpp
// deals with the cases of RECEIVED AND QUOTED
// then feedback to the listeners
template<typename T>
void InquiryService<T>::OnMessage(Inquiry<T>& _data)
{
	InquiryState _state = _data.GetState();
	if (_state == RECEIVED) {
		inquiries[_data.GetInquiryId()] = _data;
		connector->Publish(_data);
	}

	// QUOTED ---> DONE
	if (_state == QUOTED) {
		_data.SetState(DONE);
		inquiries[_data.GetInquiryId()] = _data;

		for (auto& l : listeners)
		{
			l->ProcessAdd(_data);
		}
	}
}

template<typename T>
void InquiryService<T>::AddListener(ServiceListener<Inquiry<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<Inquiry<T>>*>& InquiryService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
InquiryConnector<T>* InquiryService<T>::GetConnector()
{
	return connector;
}

// send a quote
template<typename T>
void InquiryService<T>::SendQuote(const string& _inquiryId, double _price)
{
	Inquiry<T>& _inquiry = inquiries[_inquiryId];
	InquiryState _state = _inquiry.GetState();

	// update the inquiry price, then register the listeners
	_inquiry.SetPrice(_price);
	for (auto& l : listeners)
	{
		l->ProcessAdd(_inquiry);
	}
}

// rejection of Inquiry
// simply set the state as rejected.
template<typename T>
void InquiryService<T>::RejectInquiry(const string& _inquiryId)
{
	Inquiry<T>& _inquiry = inquiries[_inquiryId];
	_inquiry.SetState(REJECTED);
}


/**
* Inquiry Connector subscribing/publishing data from/to inquiry_service.
* Type T is the product type.
*/

template<typename T>
class InquiryConnector : public Connector<Inquiry<T>>
{

private:

	InquiryService<T>* service;

public:

	// Ctor
	InquiryConnector(InquiryService<T>* _service);
	
	// Publish data to the Connector
	void Publish(Inquiry<T>& _data);

	// Subscribe data from the Connector
	void Subscribe(ifstream& _data);

	// Re-subscribe data from the Connector
	// Needed for subscribing the data from inquiries after updating status
	void Subscribe(Inquiry<T>& _data);

};

template<typename T>
InquiryConnector<T>::InquiryConnector(InquiryService<T>* _service)
{
	service = _service;
}

template<typename T>
void InquiryConnector<T>::Publish(Inquiry<T>& _data)
{
	if (_data.GetState() == RECEIVED)
	{
		_data.SetState(QUOTED);
		this->Subscribe(_data);
	}
}

// core function here
// read the inquiries.
template<typename T>
void InquiryConnector<T>::Subscribe(ifstream& _data)
{
	string _line;
	while (getline(_data, _line))
	{
		auto _cells = lineToCells(_line);

		string _inquiryId = _cells[0];
		string _productId = _cells[1];
		Side _side = _cells[2] == "BUY" ? BUY : SELL;
		long _quantity = stol(_cells[3]);
		double _price = StringToPrice(_cells[4]);
		InquiryState _state;
		if (_cells[5] == "RECEIVED"){
			_state = RECEIVED;
		}
		else if (_cells[5] == "QUOTED") {
			_state = QUOTED;
		}
		else if (_cells[5] == "DONE") {
			_state = DONE;
		}
		else if (_cells[5] == "REJECTED") {
			_state = REJECTED;
		}
		else if (_cells[5] == "CUSTOMER_REJECTED") {
			_state = CUSTOMER_REJECTED;
		}

		T _product = FetchBond(_productId);
		Inquiry<T> _inquiry(_inquiryId, _product, _side, _quantity, _price, _state);
		service->OnMessage(_inquiry);
	}
}

template<typename T>
void InquiryConnector<T>::Subscribe(Inquiry<T>& _data)
{
	service->OnMessage(_data);
}

#endif
