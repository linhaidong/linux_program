Tcp连接的那些事
===
1.断开链接的那些事
主动断开端：断开(调用close程序异常退出)，表示要关闭发送通道．发送完FIN会进入FIN_WAIT状态．
被动断开断：接收到FIB包．会进入CLOSE_WAIT的状态．表示还可以发送数据．
            当对端关闭后，recv读数据返回0, 但设置errorno.
            本段数据包只能成功发送一次，再次发送会产生SIGPIPE信号
            以后的执行写操作，就会返回-1.并触发SIGPIPE信号．执行读操作仍然返回0,但会产生Broken pipe错误．

代码运行结果如下：
*****************client start***************
sdsreceive massage:sdsd
enter your words:sdsd
send successful
server stop
可以正常接收，返回０
first recv, return 0, err:0, info:Success
成功发送一次
first  send, return 5,err:0, info:Success
sdsd
second recv, return 0, err:0, info:Success
third recv, return 0, err:0, info:Success
-----------------------------------------------------------------------
enter your words:recv signal pipe
send successful
server stop
first recv, return 0, err:32, info:Broken pipe
再次发送产生pipe信号
recv signal pipe
first  send, return -1,err:32, info:Broken pipe
读写返回０，错误码为32
second recv, return 0, err:32, info:Broken pipe
third recv, return 0, err:32, info:Broken pipe
-----------------------------------------------------------------------

