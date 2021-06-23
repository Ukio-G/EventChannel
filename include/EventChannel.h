#ifndef EVENTCHANNEL_H
#define EVENTCHANNEL_H

#include <any>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

#ifndef NewSharedSubscriber
#define NewSharedSubscriber(name) 	std::shared_ptr<Subscriber> name = std::make_shared<Subscriber>("name");
#endif

class Subscriber
{
public:
	using Ptr = std::shared_ptr<Subscriber>;
	using ActionArgument = std::any;
	using ActionsType = std::function<void(ActionArgument)>;

	Subscriber() = delete;
	Subscriber(const std::string &name) : _name(name) { }

	const std::string & newAction(const std::string & topic, ActionsType action) {
		_actions[topic] = std::move(action);
		return topic;
	}

	const std::string &removeAction(const std::string &topic) {
		auto it = _actions.find(topic);
		if (it != _actions.end())
			_actions.erase(it);
		return topic;
	}

	void notify(const std::string & topic, ActionArgument data)
	{
		if (_actions.find(topic) != _actions.end())
			_actions[topic](std::move(data));
	}

	std::string getName() const
	{
		return _name;
	}
private:
	const std::string _name;
	std::unordered_map<std::string, ActionsType> _actions;
};

class EventChannel
{
public:
	using MessageType = std::any;
	void subscribe(const std::string & topic, Subscriber::Ptr s)
	{
		std::vector<Subscriber::Ptr> & v = _subscribedToTopics[topic];
		v.push_back(s);
	}

	void unsubscribe(const std::string & topic, Subscriber::Ptr s)
	{
		if (_subscribedToTopics.find(topic) != _subscribedToTopics.end())
		{
			auto &subscribers = _subscribedToTopics.at(topic);
			auto it = std::find_if(subscribers.begin(), subscribers.end(),[&s](Subscriber::Ptr sub) {
				return sub->getName() == s->getName();
			});
			if (it != subscribers.end())
				subscribers.erase(it);
		}
	}

	void publish(const std::string & topic, MessageType data)
	{
		if (_subscribedToTopics.find(topic) != _subscribedToTopics.end())
			for (auto &subscriber : _subscribedToTopics.at(topic))
				subscriber->notify(topic, data);
	}

	static EventChannel& getInstance()
	{
		if (!_instance)
			_instance = new EventChannel();
		return *_instance;
	}

	EventChannel(EventChannel &) = delete;
	EventChannel(EventChannel &&) = delete;
	EventChannel& operator=(const EventChannel&) = delete;
private:
	static EventChannel* _instance;
	EventChannel() { }
	std::unordered_map<std::string, std::vector<Subscriber::Ptr>> _subscribedToTopics;
};
#endif
