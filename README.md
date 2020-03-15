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
2. 如何判断多线程同步是成功的。如何调试bug。
3. **BUG**：在多个consumer线程的情况下，会出现有些线程无法退出的情况。
    - 在析构函数里
        - 加入stop，并且唤醒因条件变量阻塞的线程。
    - 在pop函数中，
        - 加入对stop的判断，当队列为空并且stop时，退出pop函数。
        - 对consumer的条件变量wait调用加入pred，队列为空或者没有停止时阻塞。
4. 之前对条件变量的wait函数理解不够深入，这次学习了一下。
    - 单参数版本，此时传入一个unique_lock类型的变量，并且已经加锁。调用wait之后释放锁，并阻塞等待notify唤醒。唤醒后加锁。要注意的是被唤醒后有可能加锁失败，此时继续阻塞。
    - 双参数版本，此时需要再加入一个Predicate类型的变量，应该是一个返回bool的函数。我一般用lamda表达式代替。返回false阻塞，true解除。要注意这里的意思是即使notify了，如果后面的条件不满足，也不会解除阻塞。
    - 对于pred不知道性能会有多大影响。
