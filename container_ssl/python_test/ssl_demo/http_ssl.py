#!/usr/bin/python  
#-*- coding:utf-8 -*-  
############################  
#File Name: http_ssl.py
#Author: linengier  
#Mail: linengier@126.com  
#Created Time: 2017-01-11 16:20:05
############################ 
import BaseHTTPServer, SimpleHTTPServer
import ssl

httpd = BaseHTTPServer.HTTPServer(('localhost', 4443), SimpleHTTPServer.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket (httpd.socket, certfile='path/to/localhost.pem', server_side=True)
httpd.serve_forever()
