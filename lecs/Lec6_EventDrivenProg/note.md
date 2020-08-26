这节的主要任务是create and schedule events

## Creating a simple event callback

在gem5的event-driven模型中，每个event的都有回调函数，并由其来处理。通常，这个class继承于`Event`, 但是gem5提供了wrapper function来创建简单events

在hello_object.hh中，我们增加了三样东西

* `processEvent()`: 声明一个函数，每次event fire时都执行这个函数
* `EventFunctionWrapper`: 增加一个Event实例
* `startup()`: 

```cpp
class HelloObject: public SimObject {
private:
    void processEvent();
    EventFunctionWrapper event;
public:
    HelloObject(HelloObjectParams* p);
    void startup();
}
```



hello_object.cc

```cpp
HelloObject::HelloObject(HelloObjectParams* params):
	SimObject(params),
	event([this]{processEvent();}, name()) 
{
	DPRINTF(Hello, "Created the hello object\n");
}
```

实现process function

```cpp
void HelloObject::processEvent() {
    DPRINTF(Hello, "Hello world! Processing the event!\n");
}
```



## Scheduling events

使用`schedule()`函数

`schedule (Event *event, Tick when)`

&emsp;Schedules the event (**event**) to execute at time **when**. This function places the event into the event queue and will execute the event at tick **when**.

```cpp
void HelloObject::startup() {
    schedule(event, 100);
}
```

运行后输出应为如下所示

```shell
gem5 Simulator System.  http://gem5.org
gem5 is copyrighted software; use the --copyright option for details.

gem5 compiled Jan  4 2017 11:01:46
gem5 started Jan  4 2017 13:41:38
gem5 executing on chinook, pid 1834
command line: build/X86/gem5.opt --debug-flags=Hello configs/learning_gem5/part2/run_hello.py

Global frequency set at 1000000000000 ticks per second
      0: hello: Created the hello object
Beginning simulation!
info: Entering event queue @ 0.  Starting simulation...
    100: hello: Hello world! Processing the event!
Exiting @ tick 18446744073709551615 because simulate() limit reached
```



## More Event Scheduling

```cpp
class HelloObject : public SimObject
{
  private:
    void processEvent();

    EventFunctionWrapper event;

    Tick latency;

    int timesLeft;

  public:
    HelloObject(HelloObjectParams *p);

    void startup();
};
```



```cpp
HelloObject::HelloObject(HelloObjectParams *params) :
    SimObject(params), event([this]{processEvent();}, name()),
    latency(100), timesLeft(10)
{
    DPRINTF(Hello, "Created the hello object\n");
}
```



```cpp
void HelloObject::startup()
{
    schedule(event, latency);
}

void HelloObject::processEvent()
{
    timesLeft--;
    DPRINTF(Hello, "Hello world! Processing the event! %d left\n", timesLeft);

    if (timesLeft <= 0) {
        DPRINTF(Hello, "Done firing!\n");
    } else {
        schedule(event, curTick() + latency);
    }
}
```

输出如下所示

```shell
gem5 Simulator System.  http://gem5.org
gem5 is copyrighted software; use the --copyright option for details.

gem5 compiled Jan  4 2017 13:53:35
gem5 started Jan  4 2017 13:54:11
gem5 executing on chinook, pid 2326
command line: build/X86/gem5.opt --debug-flags=Hello configs/learning_gem5/part2/run_hello.py

Global frequency set at 1000000000000 ticks per second
      0: hello: Created the hello object
Beginning simulation!
info: Entering event queue @ 0.  Starting simulation...
    100: hello: Hello world! Processing the event! 9 left
    200: hello: Hello world! Processing the event! 8 left
    300: hello: Hello world! Processing the event! 7 left
    400: hello: Hello world! Processing the event! 6 left
    500: hello: Hello world! Processing the event! 5 left
    600: hello: Hello world! Processing the event! 4 left
    700: hello: Hello world! Processing the event! 3 left
    800: hello: Hello world! Processing the event! 2 left
    900: hello: Hello world! Processing the event! 1 left
   1000: hello: Hello world! Processing the event! 0 left
   1000: hello: Done firing!
Exiting @ tick 18446744073709551615 because simulate() limit reached
```

