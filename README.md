smartquant是一个事件驱动型的高频交易系统，主要用于做市和打单。这个项目主要是学习c++的时候顺便写的，花了大概两周时间，系统本身没啥特点，就是简单，单进程，也就是说交易、行情和策略是在一个进程里的。具体内部延迟也没测过，只是简单试验了一下做市，速度应该是够用的，毕竟是在本地运行的，而且是ctp的接口，就没必要太追求极致了（盛立的接口写了一半，没测试，不想写了）

编译方法:在目录下运行build.sh就可以了，可选参数3个:debug,release,clean
依赖库:
1. 到3rd/libzmq-master下运行
   - mkdir build
   - cd build
   - cmake ..
   - sudo make -j4 install

2. 再到3rd/cppzmq目录下运行
   - mkdir build
   - cd build
   - cmake ..
   - sudo make -j4 install

algo目录下主要是algo算法引擎，instrument标的、smartorder下单管理，写策略只需要用这3个东西就可以了。

strategy目录下是策略模板，只有一个文件Template.h，如果想写自己的策略，只需要在这个目录下新建一个cpp文件，然后继承Template这个类就可以了(当然编译的时候需要修改一下strategy/CMakeLists.txt，可以参考indexArb的策略，只需要把indexArb.cpp换成自己的策略cpp就可以了，如果有用到第三方库也需要添加进去)。

策略的写法：继承Template后，实现他的两个纯虚方法init和on_event就就行了。init在策略启动时只会运行一次(策略启动通过algo/algo.h里AlgoEngine的start()方法，它会调用所有策略的init方法)。可以在init里进行订阅，绑定SmartOrder。订阅的时候调用subscribe方法就行了，会返回一个instrument的实例，具体有哪些方法可以看/algo/instrument.h，可能会用的到有：持仓数量，净持仓，今日发单了多少次，撤单了多少次，标的的行情等等。绑定SmartOrder，只需要把这个instrument传递给SmartOrder就可以了，然后设定方向，offset和订单类型(限价，市价，FOK,FAK)。只需要调用send(价格，数量)就可以发单了，一旦发单，SmartOrder内部有一个is_active会标识已经有挂单了，再次调用send时，内部会进行检查，如果is_active=true就不会发单，直接返回。撤单调用kill()就可以了，如果外面没有挂单，kill会直接返回，不会发撤单指令。其他的一些方法可以看/algo/smartorder.h。(Offset::AUTO会根据你有没持仓自动使用开仓或平仓，不使用AUTO的话，需要在初始化SmartOrder时显示指定Offset)

gateway目录下的是接口，目前只写了ctp的接口，股票level2的接口不算，因为需要在托管服务器上才能用。如果想添加接口的话可以继承gateway.h文件里的3个基类:market_data(行情)、trader(交易)、Gateway。实现各自的纯虚方法就可以了。具体的可以参考ctp的代码(CtpGateway会把CtpMd行情和Ctptd交易组合在一起，通过统一的接口调用行情和交易的方法)。

api目录下是券商的api的头文件(券商提供)，写gateway的时候会继承里面的类。

如何运行你的策略:
在main函数里实例化一个EventEngine(/event/event.h)和一个AlgoEngine(/algo/algo.h)，然后把策略需要用到的gateway加入到AlgoEngine里(AlgoEngine::add_engine方法，需要传入接口的配置文件，参考example)，实例化你自己的策略，调用AlgoEngine::add_strategy(需要传入策略的配置文件),最后调用AlgoEngine::start()就启动了。

