#!/usr/bin/python  
#-*- coding:utf-8 -*-  
############################  
#File Name: ssl_client.py
#Author: linengier  
#Mail: linengier@126.com  
#Created Time: 2017-01-11 19:35:45
############################ 
import os  
import httplib  
import socket  
  
KEY_FILE = os.getcwd() + '/key.pem'  
CERT_FILE = os.getcwd() + '/client/cert.pem'  
GET = "GET"  
  
  
conn = httplib.HTTPSConnection('0.0.0.0', '4242', cert_file = CERT_FILE)  
  
conn.request(GET, "/this.txt")  
response = conn.getresponse()  
print response.status, response.reason, response.read()  
  
conn.close()  
