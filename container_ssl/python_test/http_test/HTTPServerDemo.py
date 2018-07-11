#encoding=utf-8
'''
Created on 2012-11-7

@author: Steven
http://www.lifeba.org
基于BaseHTTPServer的http server实现，包括get，post方法，get参数接收，post参数接收。
'''
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import io,shutil  
import urllib,time
import getopt,string
import json
import sys
import ssl

class MyRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.process(2)

    def do_POST(self):
        self.process(1)
        
    def process(self,type):
        content =""
        if type==1:#post方法，接收post参数
            datas = self.rfile.read(int(self.headers['content-length']))
            datas = urllib.unquote(datas).decode("utf-8", 'ignore')#指定编码方式
            datas = transDicts(datas)#将参数转换为字典
            if datas.has_key('data'):
                content = "data:"+datas['data']+"\r\n"
        if type == 2:
             #path is "Contains the request path"; self.path = '/?name=123' 
            if '?' in self.path:
                print("path:%s", self.path)
                query = urllib.splitquery(self.path)
                print("query:%s", query)
                action = query[0] 
                #query=('/', 'name=123')
                if query[1]:#接收get参数
                    queryParams = {}
                    #kv =['name', '123']
                    for qp in query[1].split('&'):
                        kv  = qp.split('=')
                        queryParams[kv[0]] = urllib.unquote(kv[1]).decode("utf-8", 'ignore')
                        content+= kv[0]+':'+queryParams[kv[0]]+"\r\n"
                
                
                python_to_json = json.dumps(queryParams, ensure_ascii=False)
                #json_to_python = json.loads(python_to_json)
                #指定返回编码
                enc="UTF-8"  
               # content = content.encode(enc)        
                python_to_json = python_to_json.encode(enc)
                f = io.BytesIO()  
                f.write(python_to_json)  
                f.seek(0)  
                self.send_response(200)  
                self.send_header("Content-type", "text/html; charset=%s" % enc)  
                #self.send_header("Content-Length", str(len(content)))  
                self.send_header("Content-Length", str(len(python_to_json)))  
                self.end_headers()  
                shutil.copyfileobj(f,self.wfile)   

def transDicts(params):
    dicts={}
    if len(params)==0:
        return
    params = params.split('&')
    for param in params:
        dicts[param.split('=')[0]]=param.split('=')[1]
    return dicts
       
if __name__=='__main__':
    try:
        server = HTTPServer(('', 8000), MyRequestHandler)
        print 'started httpserver...'
        server.socket = ssl.wrap_socket (server.socket, certfile='cert.pem', server_side=True)
        server.serve_forever()
    except KeyboardInterrupt:
        server.socket.close()
    pass
