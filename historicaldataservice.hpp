/**
 * historicaldataservice.hpp
 * historicaldataservice.hpp
 *
 * @author Breman Thuraisingham
 * Defines the data types and Service for historical data.
 * Should register the following listeners: Position,  Risk, Execution, Steaming, Inquiry services.
 *
 * @author Breman Thuraisingham
 * @coauthor James Wu
 */
#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

#include "soa.hpp"
#include "utilityfunctions.hpp"
#include "positionservice.hpp"
#include "riskservice.hpp"
#include "executionservice.hpp"
#include "streamingservice.hpp"
#include "inquiryservice.hpp"

enum ServiceType { POSITION, RISK, EXECUTION, STREAMING, INQUIRY };

/**
* Pre-declearations to avoid errors.
*/
template<typename V>
class HistoricalDataConnector;
template<typename V>
class HistoricalDataListener;


/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type V is the data type to persist (for some services)
 */
template<typename V>
class HistoricalDataService : Service<string, V>
{

public:

	// Constructor and destructor
	HistoricalDataService();
	HistoricalDataService(ServiceType _type);

	// Get data on our service given a key
	V& GetData(string _key);

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(V& _data);

	// Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
	void AddListener(ServiceListener<V>* _listener);

	// Get all listeners on the Service
	const vector<ServiceListener<V>*>& GetListeners() const;

	// Get the connector of the service
	HistoricalDataConnector<V>* GetConnector();

	// Get the listener of the service
	ServiceListener<V>* GetServiceListener();

	// Get the service type that historical data comes from
	ServiceType GetServiceType() const;

	// Persist data to a store
	void PersistData(string _persistKey, V& _data);
private:

	map<string, V> historicalDatas;
	vector<ServiceListener<V>*> listeners;
	HistoricalDataConnector<V>* connector;
	ServiceListener<V>* listener;
	ServiceType type;
};

// default set as INQUIRY
template<typename V>
HistoricalDataService<V>::HistoricalDataService()
{
	historicalDatas = map<string, V>();
	listeners = vector<ServiceListener<V>*>();
	connector = new HistoricalDataConnector<V>(this);
	listener = new HistoricalDataListener<V>(this);
	type = INQUIRY;
}

template<typename V>
HistoricalDataService<V>::HistoricalDataService(ServiceType _type)
{
	historicalDatas = map<string, V>();
	listeners = vector<ServiceListener<V>*>();
	connector = new HistoricalDataConnector<V>(this);
	listener = new HistoricalDataListener<V>(this);
	type = _type;
}

template<typename V>
V& HistoricalDataService<V>::GetData(string _key)
{
	return historicalDatas[_key];
}

template<typename V>
void HistoricalDataService<V>::OnMessage(V& _data)
{
	historicalDatas[_data.GetProduct().GetProductId()] = _data;
}

template<typename V>
void HistoricalDataService<V>::AddListener(ServiceListener<V>* _listener)
{
	listeners.push_back(_listener);
}

template<typename V>
const vector<ServiceListener<V>*>& HistoricalDataService<V>::GetListeners() const
{
	return listeners;
}

template<typename V>
HistoricalDataConnector<V>* HistoricalDataService<V>::GetConnector()
{
	return connector;
}

template<typename V>
ServiceListener<V>* HistoricalDataService<V>::GetServiceListener()
{
	return listener;
}

template<typename V>
ServiceType HistoricalDataService<V>::GetServiceType() const
{
	return type;
}

template<typename V>
void HistoricalDataService<V>::PersistData(string _persistKey, V& _data)
{
	connector->Publish(_data);
}

/**
* Historical Data Connector publishing data from Historical Data Service.
* Type V is the data type to persist.
* Core function: publish
*/
template<typename V>
class HistoricalDataConnector : public Connector<V>
{

private:

	HistoricalDataService<V>* service;

public:

	// Ctor
	HistoricalDataConnector(HistoricalDataService<V>* _service);

	// Publish data to the Connector
	void Publish(V& _data);

	// Subscribe data from the Connector
	void Subscribe(ifstream& _data);

};

template<typename V>
HistoricalDataConnector<V>::HistoricalDataConnector(HistoricalDataService<V>* _service)
{
	service = _service;
}

// core function!
template<typename V>
void HistoricalDataConnector<V>::Publish(V& _data)
{
	ServiceType _type = service->GetServiceType();
	ofstream _file;
	if (_type == POSITION) {
		_file.open("positions.txt", ios::app);
	}
	if (_type == RISK) {
		_file.open("risk.txt", ios::app);
	}
	if (_type == EXECUTION) {
		_file.open("executions.txt", ios::app);
	}
	if (_type == STREAMING) {
		_file.open("streaming.txt", ios::app);
	}
	if (_type == INQUIRY) {
		_file.open("allinquiries.txt", ios::app);
	}

	_file << GetTimeStamp() << ",";
	
	// ! we call the ToStrings() method to write the records into files.
	vector<string> _dataStrings = _data.ToStrings();
	for (auto& s : _dataStrings)
	{
		_file << s << ",";
	}
	_file << endl;
}

// subscribe: not implemented
template<typename V>
void HistoricalDataConnector<V>::Subscribe(ifstream& _data) {}

/**
* Historical Data Service Listener subscribing data to Historical Data.
* Type V is the data type to persist.
* Based on the persist key, we determine what to look for / update.
*/
template<typename V>
class HistoricalDataListener : public ServiceListener<V>
{

private:

	HistoricalDataService<V>* service;

public:

	// Connector and Destructor
	HistoricalDataListener(HistoricalDataService<V>* _service);
	~HistoricalDataListener();

	// Listener callback to process an add event to the Service
	void ProcessAdd(V& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(V& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(V& _data);

};

template<typename V>
HistoricalDataListener<V>::HistoricalDataListener(HistoricalDataService<V>* _service)
{
	service = _service;
}

template<typename V>
HistoricalDataListener<V>::~HistoricalDataListener() {}

template<typename V>
void HistoricalDataListener<V>::ProcessAdd(V& _data)
{
	string _persistKey = _data.GetProduct().GetProductId();
	service->PersistData(_persistKey, _data);
}

// do nothing for these methods (not required)
template<typename V>
void HistoricalDataListener<V>::ProcessRemove(V& _data) {}

template<typename V>
void HistoricalDataListener<V>::ProcessUpdate(V& _data) {}

#endif
