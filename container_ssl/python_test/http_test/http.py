#!/usr/bin/python  
#-*- coding:utf-8 -*-  
############################  
#File Name: http.py
#Author: linengier  
#Mail: linengier@126.com  
#Created Time: 2017-01-11 16:31:53
############################ 
import SimpleHTTPServer  
import SocketServer  
import re  
  
def htc(m):  
    return chr(int(m.group(1),16))  
  
def urldecode(url):  
    rex=re.compile('%([0-9a-hA-H][0-9a-hA-H])',re.M)  
    return rex.sub(htc,url)  
def process(self,type):
        content=""
        if type==1:#post方法，接收post参数
            datas=self.rfile.read(int(self.headers['content-length']))
            datas=urllib.unquote(datas).decode("utf-8",'ignore')#指定编码方式
            datas=transDicts(datas)#将参数转换为字典
            ifdatas.has_key('data'):
            content="data:"+datas['data']+"\r\n"
        if'?'in self.path:
            query=urllib.splitquery(self.path)
            action=query[0]
            if query[1]:#接收get参数
                 queryParams={}
                for qpinquery[1].split('&'):
                    kv=qp.split('=')
                    queryParams[kv[0]]=urllib.unquote(kv[1]).decode("utf-8",'ignore')
                    content+=kv[0]+':'+queryParams[kv[0]]+"\r\n"
            enc="UTF-8"
            content=content.encode(enc)
            f=io.BytesIO()
            f.write(content)
            f.seek(0)
            self.send_response(200)
            self.send_header("Content-type","text/html;charset=%s" %enc)
            self.send_header("Content-Length",str(len(content)))
            self.end_headers()
            shutil.copyfileobj(f,self.wfile)

class SETHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):  
    def createHTML(self):  
        html = file("index.html", "r")  
        for line in html:  
            self.wfile.write(line)  
              
    def do_GET(self):  
        print "GET"  
        print self.headers;  
        self.createHTML()  
          
    def do_POST(self):  
        print "POST"  
        print self.headers;  
        length = int(self.headers.getheader('content-length'))  
        qs = self.rfile.read(length)  
        url=urldecode(qs)  
        print "url="  
        print url  
        self.createHTML()  
          
httpd = SocketServer.TCPServer(("", 8000), SETHandler)  
httpd.serve_forever()  
