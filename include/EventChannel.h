#ifndef EVENTCHANNEL_H
#define EVENTCHANNEL_H

#include <any>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <memory>
#include <optional>
#include "EventLoop.h"

class EventChannel;

class Subscriber
{
public:
	using Ptr = std::shared_ptr<Subscriber>;
	using ActionArgument = std::any;
	using ActionsType = std::function<void(ActionArgument)>;

	Subscriber() = delete;
	Subscriber(const std::string &name) : _name(name) { }


	class Topic {
	public:
		Topic() { }

		using ActionsList = std::unordered_map<std::string, ActionsType>;
		void newAction(const std::string & action_name, ActionsType action_function) {
			if (_actions.find(action_name) == _actions.end())
				_actions[action_name] = std::move(action_function);
		}

		void removeAction(const std::string &action) {
			auto it = _actions.find(action);
			if (it != _actions.end())
				_actions.erase(it);
		}

		void notify(const ActionArgument& argument) {
			for(auto &action : _actions)
				action.second(argument);
		}

	private:
		ActionsList _actions;
	};

	static Ptr create(const std::string & name) {
		return std::make_shared<Subscriber>(name);
	}

	void notify(const std::string & topic_name, const ActionArgument &data)
	{
		if (_topics.find(topic_name) != _topics.end()) {
			_topics[topic_name].notify(data);
		}
	}

	void addActionToTopic(const std::string & topic_name, const std::string & action_name, ActionsType action_function) {
		// If for this topic no actions - create topic
		auto & topic = _topics[topic_name];
		topic.newAction(action_name, std::move(action_function));
	}

	void removeActionFromTopic(const std::string & topic_name, const std::string & action_name) {
		if (_topics.find(topic_name) != _topics.end()) {
			auto & topic = _topics[topic_name];
			topic.removeAction(action_name);
		}
	}

	std::string getName() const {
		return _name;
	}

private:
	friend EventChannel;

	const std::string _name;
	using TopicsList = std::unordered_map<std::string, Topic>;

	// This Subscriber subscribed to these topics
	// Each topic contains multiple actions to do
	TopicsList _topics;
};

class EventChannel
{
public:
	using MessageType = std::any;
	void subscribe(const std::string & topic, Subscriber::Ptr s) {
		std::vector<Subscriber::Ptr> & v = _subscribedToTopics[topic];

		// Check subscriber not in the list. If in the list - return from function
		if (std::find(v.begin(), v.end(), s) != v.end())
			return;

		v.push_back(s);
	}

	void unsubscribe(const std::string & topic, Subscriber::Ptr s) {
		if (_subscribedToTopics.find(topic) != _subscribedToTopics.end()) {
			auto &subscribers = _subscribedToTopics.at(topic);
			auto it = std::find_if(subscribers.begin(), subscribers.end(),[s](Subscriber::Ptr sub) {
				return sub->getName() == s->getName();
			});
			if (it != subscribers.end())
				subscribers.erase(it);
		}
	}

	template<int CallPolicy = ECP_DONE>
	void publish(const std::string & topic, const MessageType& data) {
		if (_subscribedToTopics.find(topic) != _subscribedToTopics.end())
			for (auto &subscriber : _subscribedToTopics.at(topic))
				notifyLoop.addFunction<CallPolicy>( [subscriber, data, topic]() {
					subscriber->notify(topic, data);
				});
	}

	static EventChannel& getInstance() {
		static EventChannel* _instance;
		if (!_instance)
			_instance = new EventChannel();
		return *_instance;
	}

	EventChannel(EventChannel &) = delete;
	EventChannel(EventChannel &&) = delete;
	EventChannel& operator=(const EventChannel&) = delete;
private:
	EventLoop notifyLoop;
	EventChannel() { }
	std::unordered_map<std::string, std::vector<Subscriber::Ptr>> _subscribedToTopics;
};
#endif