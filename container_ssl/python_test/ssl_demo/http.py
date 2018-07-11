#!/usr/bin/python  
#-*- coding:utf-8 -*-  
############################  
#File Name: http.py
#Author: linengier  
#Mail: linengier@126.com  
#Created Time: 2017-01-11 16:21:37
############################ 
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer  
import io,shutil    
import urllib  
import os, sys  
  
class MyRequestHandler(BaseHTTPRequestHandler):  
    def do_GET(self):  
        mpath,margs=urllib.splitquery(self.path) # ?分割  
        self.do_action(mpath, margs)  
  
    def do_POST(self):  
        mpath,margs=urllib.splitquery(self.path)  
        datas = self.rfile.read(int(self.headers['content-length']))  
        self.do_action(mpath, datas)  
  
    def do_action(self, path, args):  
            self.outputtxt(path+args)  
  
    def outputtxt(self, content):  
        #指定返回编码  
        enc = "UTF-8"  
        content = content.encode(enc)            
        f = io.BytesIO()  
        f.write(content)  
        f.seek(0)    
        self.send_response(200)    
        self.send_header("Content-type", "text/html; charset=%s" % enc)    
        self.send_header("Content-Length", str(len(content)))    
        self.end_headers()    
        shutil.copyfileobj(f,self.wfile)  

httpd = HTTPServer(('localhost', 4443), MyRequestHandler)
#httpd.socket = ssl.wrap_socket (httpd.socket, certfile='path/to/localhost.pem', server_side=True)
httpd.serve_forever()
