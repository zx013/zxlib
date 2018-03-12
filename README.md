标准C编写，兼容linux和windows系统，均用gcc编译，默认为debug版本，带有单元测试

可作为通用库的功能有
head：实现了宏的不定参数递归的基本结构，定义了一些基本的类型和简化操作
lib：包含了lambda表达式，类型判断和可管理的内存分配（利用多级位图记录分配状态）等
error：错误处理
test：自动化单元测试，包括单元测试中内存、时间等运行状态的监控，并带有一系列便于调试的参数

lock：原子锁，用gcc的原子操作实现，性能高于系统的锁（unused中mutex和signal）
hash：哈希表，用klib实现
list：链表，用klib实现
queue：阻塞队列
thread：多线程模块，用来实现pool线程池
pool：线程池
dict：类似python的字典，包含独立实现的hash, list等（计划替换原hash, list模块中klib实现），支持json解析，支持所有类型的数据，实现了类似c++中重载的功能，并实现了dict结构中内存的自动释放
config：读取配置
logging：日志模块

code：编解码模块，实现gbk, utf8和unicode之间的相互转换
sock：通用的网络接口，计划用libevent和libcurl来替换标准socket实现
dir：目录相关的一些操作
file：通用的文件接口，实现了基本的文件操作和预分配，文件映射等功能
tools：一些工具函数
package：特定的数据包
unit：一种在udp下的前向纠错编码
redundancy：纠错能力更高的一种编码，尚未实现完成

其余的包括了特定业务逻辑下的代码和一些算法练习代码

config为配置目录
include暂时只包含了libcurl和libevent的头文件
klib为轻量级的C库，后期将逐步替换
lib为libcurl和libevent的静态库文件
test为单元测试用到的测试目录