# Event channel implementation

## Synopsis

### Problem:

### Solution:

## Usage:
No need to compile library/source files. Just include EventChannel.h:
```c++
#include "EventChannel.h"
```
Now you are ready to use all features of Event channel!

Create named subscriber object: 

```c++
 std::shared_ptr<Subscriber> subscriber = std::make_shared<Subscriber>("name");
```
Or with macro:
```c++
NewSharedSubscriber(subscriber)
```


Create action for some topic:

```c++
subscriber->newAction("event_topic", [](Subscriber::ActionArgument argument) {
    std::vector<int> data = std::any_cast<std::string>(argument);
    std::cout << "argument = " << data << std::endl;
});
```

Subscribe `subscriber` object to event channel:

```c++
EventChannel::getInstance().subscribe("event_topic", subscriber);
```

Generate event for some topic:

```c++
EventChannel::getInstance().publish("event_topic", std::string("event data"));
```

One subscriber can handle multiple events:

```c++
subscriber->newAction("makeSum", [](Subscriber::ActionArgument argument) { 
    std::vector<int> data = std::any_cast<std::vector<int>>(argument); 
    std::cout << "makeSum result = " << std::accumulate(data.begin(), data.end(), 0)<< std::endl; 
});
```

```c++
subscriber->newAction("printerElements", [](Subscriber::ActionArgument argument) {
    std::vector<int> data = std::any_cast<std::vector<int>>(argument);
    for (auto &item : data)
        std::cout << "item " << item << std::endl;
});
```

## Limitations:

You should initialize static EventChannel::_instance pointer:

```c++
EventChannel* EventChannel::_instance = nullptr;
```

## Code examples:

See `examples` directory for sources with EventChannel usage.

## ToDo:

- Anonymous subscribers
- Thread-safe notifications/subscribe