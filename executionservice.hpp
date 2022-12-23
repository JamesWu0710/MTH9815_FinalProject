/**
 * executionservice.hpp
 * Defines the data types and Service for executions.
 *
 * @author Breman Thuraisingham
 * @coauthor James Wu
 */
#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include "soa.hpp"
#include <string>
#include "algoexecutionservice.hpp"

 /**
 * Pre-declearations to avoid errors.
 * A listener connection execution to algo execution (triggered!)
 * from the implemented algo execution class contained in the hpp file
 * to this one, the more generic execution service one.
 */
template<typename T>
class AlgoExecutionToExecutionListener;

/**
* Service for executing orders (general).
* Keyed on product identifier.
* Type T is the product type.
* We follow the normal schedule of constructing service class.
*/
template<typename T>
class ExecutionService : public Service<string, ExecutionOrder<T>>
{
public:

	// Ctor
	ExecutionService();

	// Get data on our service given a key
	ExecutionOrder<T>& GetData(string _id);

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(ExecutionOrder<T>& _data);

	// Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
	void AddListener(ServiceListener<ExecutionOrder<T>>* _listener);

	// Get all listeners on the Service
	const vector<ServiceListener<ExecutionOrder<T>>*>& GetListeners() const;

	// Get the listener of the service
	AlgoExecutionToExecutionListener<T>* GetListener();

	// order execution upon receiving an execution request.
	void ExecuteOrder(ExecutionOrder<T>& _executionOrder);

private:
	map<string, ExecutionOrder<T>> executionOrders;
	vector<ServiceListener<ExecutionOrder<T>>*> listeners;
	AlgoExecutionToExecutionListener<T>* listener;
};

template<typename T>
ExecutionService<T>::ExecutionService()
{
	executionOrders = map<string, ExecutionOrder<T>>();
	listeners = vector<ServiceListener<ExecutionOrder<T>>*>();
	listener = new AlgoExecutionToExecutionListener<T>(this);
}

template<typename T>
ExecutionOrder<T>& ExecutionService<T>::GetData(string _id)
{
	return executionOrders[_id];
}

template<typename T>
void ExecutionService<T>::OnMessage(ExecutionOrder<T>& _data)
{
	string _id = _data.GetProduct().GetProductId();
	executionOrders[_id] = _data;
}

template<typename T>
void ExecutionService<T>::AddListener(ServiceListener<ExecutionOrder<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<ExecutionOrder<T>>*>& ExecutionService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
AlgoExecutionToExecutionListener<T>* ExecutionService<T>::GetListener()
{
	return listener;
}

template<typename T>
void ExecutionService<T>::ExecuteOrder(ExecutionOrder<T>& _executionOrder)
{
	string _productId = _executionOrder.GetProduct().GetProductId();
	executionOrders[_productId] = _executionOrder;

	// call the listeners
	for (auto& l : listeners)
	{
		l->ProcessAdd(_executionOrder);
	}
}

/**
* Template class for the Execution Service Listener
* subscribing data from algo execution to execution service.
* Type T is the product type.
* This class is essentially from the algo execution
*/
template<typename T>
class AlgoExecutionToExecutionListener : public ServiceListener<AlgoExecution<T>>
{
public:
	// ctor
	AlgoExecutionToExecutionListener(ExecutionService<T>* _service);

	// Listener callback to process an add event to the Service
	void ProcessAdd(AlgoExecution<T>& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(AlgoExecution<T>& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(AlgoExecution<T>& _data);

private:
	ExecutionService<T>* service;
};

template<typename T>
AlgoExecutionToExecutionListener<T>::AlgoExecutionToExecutionListener(ExecutionService<T>* _service)
{
	service = _service;
}

// core function here
// we are calling the algo
template<typename T>
void AlgoExecutionToExecutionListener<T>::ProcessAdd(AlgoExecution<T>& _data)
{
	// delegate to the AlgoExecution stuff to execute the order (using algo)
	ExecutionOrder<T>* execution_order = _data.GetExecutionOrder();

	// update the info
	service->OnMessage(*execution_order);

	// request the order to execute
	service->ExecuteOrder(*execution_order);
}

// do nothing here (not required)
template<typename T>
void AlgoExecutionToExecutionListener<T>::ProcessRemove(AlgoExecution<T>& _data) {}

template<typename T>
void AlgoExecutionToExecutionListener<T>::ProcessUpdate(AlgoExecution<T>& _data) {}

#endif
