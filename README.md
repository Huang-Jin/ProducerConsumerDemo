# ProducerConsumerDemo
A demo to show the model of producer and consumer in multi-thread programming, written in 2020/03/15.

字节跳动三面提到的手写代码，当时没能写出来。
生产者消费者模型主要是利用一个缓冲区来存储数据，从而减弱生产者和消费者之间的直接关系，提高多线程下程序效率。
今天参考了一下网上的资料， https://blog.csdn.net/lvxin15353715790/article/details/89143121，CSAPP一书。

## 工具
* 实现的语言是C++.
    - 线程用的C++11提供的thread库,
    - 互斥锁mutex,
    - 条件变量condition_variable,
    - 队列queue,
    - 原子操作atomic.
* 开发环境是Visual Studio 2013

## 实现细节
* 具体的实现逻辑是构建一个queue来存储生产的数据，queue不满时可以生产，不空时可以消费。
* 对于这个队列，采用阻塞队列的实现思路。
* 先实现构造函数，初始化一个unique_lock供condition_variable使用。
    - 如何在类里面使用unique_lock等需要初始化，并且初始化会加锁的对象。这要研究下。我的理解是构造列表初始化，然后函数体里unlock。
    - 对于条件变量，申请两个，分别控制consumer和producer。
* 然后就是入和出队列的细节。
    - 首先加锁。
    - 循环判断一下目前的队列情况，对于各自的特殊情况（队满和队空）进行处理。
    - 唤醒一个线程来处理特殊情况。
    - 等待处理完毕。
    - 处理入和出队列操作。
    - 最后释放锁。

## 问题
1. 对于多线程很多细节不是很理解。
2. 多个条件变量共享一个unique_lock，内部是怎么操作的。
3. 如何判断多线程同步是成功的。如何调试bug。
