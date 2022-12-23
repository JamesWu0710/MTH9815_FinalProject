#ifndef GUIservice_hpp
#define GUIservice_hpp

/* GUIservice.hpp
* the GUI-related information
* containing data types and different services
* Transplanted other codes provided by professor
* Author: James Wu
*/

#include "soa.hpp"
#include "utilityfunctions.hpp"
#include "pricingservice.hpp"

// Pre-declearations to avoid errors.

template<typename T>
class GUIConnector;
template<typename T>
class GUIToPricingListener;

// the GUI service with given product type T
// used to stream the prices.
// *emulate the other implemented classes
template <typename T>
class GUIService : Service<string, Price<T>> {

public:
	GUIService();
	~GUIService();

	// fetch data with given product id
	Price<T>& GetData(string _key);

	// call back function for the connector
	void OnMessage(Price<T>& _data);

	// add listener to the service
	void AddListener(ServiceListener<Price<T>>* listener);

	// fetch the active listeners on the service
	const vector<ServiceListener<Price<T>>*>& GetListeners() const;

	// fetch the connector
	GUIConnector<T>* GetConnector();

	// fetch the listener
	ServiceListener<Price<T>>* GetListener();

	// fetch the throttle
	int GetThrottle() const;

	// set the throttle
	void SetThrottle(int _throttle);

	// fetch the current millisecond
	int GetMillisec() const;

	// set the millisecond
	void SetMillisec(int _millisec);

private:
	map<string, Price<T>> GUIs;
	vector<ServiceListener<Price<T>>*>listeners;
	GUIConnector<T>* connector;
	ServiceListener<Price<T>>* listener;

	int throttle;
	int millisec;
};

template<typename T>
GUIService<T>::GUIService() {
	GUIs = map<string, Price<T>>();
	listeners = vector<ServiceListener<Price<T>>*>();
	connector = new GUIConnector<T>(this);
	listener = new GUIToPricingListener<T>(this);
	throttle = 300;
	millisec = 0;
}

template<typename T>
GUIService<T>::~GUIService() {}

template<typename T>
Price<T>& GUIService<T>::GetData(string _key) {
	return GUIs[_key];
}

template<typename T>
void GUIService<T>::OnMessage(Price<T>& _data)
{
	// pulish the data
	string product_id = _data.GetProduct().GetProductId();
	GUIs[product_id] = _data;
	connector->Publish(_data);
}

template<typename T>
void GUIService<T>::AddListener(ServiceListener<Price<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<Price<T>>*>& GUIService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
GUIConnector<T>* GUIService<T>::GetConnector()
{
	return connector;
}

template<typename T>
ServiceListener<Price<T>>* GUIService<T>::GetListener()
{
	return listener;
}

template<typename T>
int GUIService<T>::GetThrottle() const
{
	return throttle;
}

template<typename T>
int GUIService<T>::GetMillisec() const
{
	return millisec;
}

template<typename T>
void GUIService<T>::SetThrottle(int _throttle)
{
	throttle = _throttle;
}
template<typename T>
void GUIService<T>::SetMillisec(int _millisec)
{
	millisec = _millisec;
}

template<typename T>
class GUIConnector :public Connector<Price<T>> {
public:
	GUIConnector(GUIService<T>* _service);

	// interactions with connector
	// publish the data, save the records
	void Publish(Price<T>& _data);

	// subscribe data from connector
	// not needed, declared here for uniformity in design
	void Subscribe(ifstream& _data);

private:
	GUIService<T>* service;
};

template<typename T>
GUIConnector<T>::GUIConnector(GUIService<T>* _service)
{
	service = _service;
}

template<typename T>
void GUIConnector<T>::Publish(Price<T>& _data)
{
	int _throttle = service->GetThrottle();
	int _millisec = service->GetMillisec();
	int currentMillisec = GetMillisecond();		// fetch the system time in milliseconds

	// update time
	while (currentMillisec < _millisec) {
		currentMillisec += 1000;
	}
	if (currentMillisec - _millisec >= _throttle)
	{
		service->SetMillisec(currentMillisec);
		ofstream _file;
		_file.open("gui.txt", ios::app);

		// update the information into GUI to keep records.
		_file << GetTimeStamp() << ",";
		for (auto& s : _data.ToStrings())
		{
			_file << s << ",";
		}
		_file << "\n";
	}
}

template<typename T>
void GUIConnector<T>::Subscribe(ifstream& _data) {}
/*GUI Service listener on the BondPricingService class
* Update the information upon receiving message from the pricers
*/

template<typename T>
class GUIToPricingListener : public ServiceListener<Price<T>> {
public:
	GUIToPricingListener(GUIService<T>* _service);

	// Listener callback to process an add event to the Service
	void ProcessAdd(Price<T>& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(Price<T>& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Price<T>& _data);
private:
	GUIService<T>* service;
};

template<typename T>
GUIToPricingListener<T>::GUIToPricingListener(GUIService<T>* _service)
{
	service = _service;
}

// invoke the on message update for the GUI service upon receiving data.
template<typename T>
void GUIToPricingListener<T>::ProcessAdd(Price<T>& _data)
{
	service->OnMessage(_data);
}

template<typename T>
void GUIToPricingListener<T>::ProcessRemove(Price<T>& _data) {}

template<typename T>
void GUIToPricingListener<T>::ProcessUpdate(Price<T>& _data) {}

#endif // !GUIservice_hpp
