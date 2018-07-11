#!/usr/bin/python  
#-*- coding:utf-8 -*-  
############################  
#File Name: test.py
#Author: linengier  
#Mail: linengier@126.com  
#Created Time: 2017-01-03 20:06:51
############################ 
import os, time
print ".....pid %d" % os.getpid()
print "..........pgid is %d" % os.getpgid(0)
'''
while 1:
    time.sleep(1)
'''
pid=os.fork()
if pid == 0:
    while 1:
        time.sleep(1)
        print "child process"
        print "..........pgid is %d" % os.getpgid(0)
        file_object = open('/tmp/test.txt', 'a')
        all_the_text = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
        file_object.write(all_the_text)
        file_object.write("\r\n")
        file_object.flush()
        file_object.close( )
 
elif pid > 0:
    time.sleep(1)
    os._exit(0)
