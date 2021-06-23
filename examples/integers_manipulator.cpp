#include <iostream>
#include "EventChannel.h"
#include <vector>
#include <algorithm>
#include <numeric>


EventChannel* EventChannel::_instance = nullptr;
std::vector<Subscriber::Ptr> subscibers;
void f()
{
	EventChannel::getInstance().publish("makeSum", std::vector<int>{1,2,3});
}


void g()
{
	EventChannel::getInstance().publish("printerElements", std::vector<int>{1,2,3});
}


void createIntegerHandlers()
{
	NewSharedSubscriber(integer_manipulator)
	subscibers.push_back(integer_manipulator);

	// Create handlers for multiple topics
	integer_manipulator->newAction("makeSum", [](Subscriber::ActionArgument argument) {
		std::vector<int> data = std::any_cast<std::vector<int>>(argument);
		std::cout << "makeSum result = " << std::accumulate(data.begin(), data.end(), 0)<< std::endl;
	});

	integer_manipulator->newAction("printerElements", [](Subscriber::ActionArgument argument) {
		std::vector<int> data = std::any_cast<std::vector<int>>(argument);
		for (auto &item : data)
			std::cout << "item " <<item << std::endl;
	});


	// Subscribe one subscriber object to multiple topics
	EventChannel::getInstance().subscribe("makeSum", integer_manipulator);
	EventChannel::getInstance().subscribe("printerElements", integer_manipulator);
}

int main()
{
	createIntegerHandlers();

	f();
	g();

	EventChannel::getInstance().unsubscribe("printerElements", subscibers.back());

	f();
	g();	// No effect after unsubscribe

	return 0;
}
