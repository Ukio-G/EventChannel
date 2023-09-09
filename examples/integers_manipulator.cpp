#include <iostream>
#include "EventChannel.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

Subscriber::Ptr subscriber;

void f() {
	EventChannel::getInstance().publish("PublishNumbers", std::vector<int>{1,2,3,4});
}

void createIntegerHandlers() {
	subscriber = Subscriber::create("IntegerSubscriber");
	EventChannel::getInstance().subscribe("PublishNumbers", subscriber);
	// Create handlers for multiple topics
 	subscriber->addActionToTopic("PublishNumbers", "makeSum", [](Subscriber::ActionArgument argument) {
		std::vector<int> data = std::any_cast<std::vector<int>>(argument);
		std::cout << "makeSum result = " << std::accumulate(data.begin(), data.end(), 0)<< std::endl;
	});

	subscriber->addActionToTopic("PublishNumbers", "printerElements" ,[](Subscriber::ActionArgument argument) {
		std::vector<int> data = std::any_cast<std::vector<int>>(argument);
		for (auto &item : data)
			std::cout << "item " << item << std::endl;
	});
}

int main() {
	createIntegerHandlers();

	// All subscribed actions will trigger
	f();
	sleep(2);

	// Only makeSum actions will trigger
	subscriber->removeActionFromTopic("PublishNumbers", "printerElements");
	f();

	// No subscriber available for PublishNumbers topic -> no actions will trigger
	EventChannel::getInstance().unsubscribe("PublishNumbers", subscriber);
	f();

	sleep(2);
	return 0;
}
