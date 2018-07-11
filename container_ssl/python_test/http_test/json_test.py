#!/usr/bin/python  
#-*- coding:utf-8 -*-  
############################  
#File Name: json.py
#Author: linengier  
#Mail: linengier@126.com  
#Created Time: 2017-01-11 18:51:55
############################ 
import json
import sys

if __name__ == '__main__':
    # 将python对象test转换json对象
    test = {}
    k1 = 'name'
    v1 = 'lin'
    k2 = 'sex'
    v2 = 'man'
    test[k1]=v1
    test[k2]=v2
    print type(test)
    #python 字典转换为json
    python_to_json = json.dumps(test,ensure_ascii=False)
    print python_to_json
    print type(python_to_json)

    # 将json对象转换成python对象
    json_to_python = json.loads(python_to_json)
    print type(json_to_python)
    print json_to_python[k1]
    print json_to_python
