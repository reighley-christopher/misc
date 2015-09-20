import io
import code
import socket
import sys
import os
import select

#a little script creates a unix domain socket with a python interpretter on the other end
class Interpocket():
  def __init__(self, path, namespace):
    self.output_buffer = io.StringIO()
    self.interpreter = code.InteractiveConsole(namespace)
    self.socket = socket.socket(socket.AF_UNIX)
    os.unlink(path)
    self.socket.bind(path)    
    self.input_buffer = io.BytesIO()
    self.namespace = namespace

  def start(self):
    self.socket.listen(1)

  def end(self):
    self.conn.close()
    self.socket.shutdown()

  def poll(self):
    leave_now = False
    prompt = False
    if(not hasattr(self,"conn")):
      triple = select.select([self.socket],[],[],0.001)
      if(triple == ([],[],[])):
        return
      else:
        self.conn, addr = self.socket.accept()
        self.conn.send(b'server>')
    triple = select.select([self.conn],[],[],0.001)
    if(triple == ([],[],[])):
      return
    inpu = self.conn.recv(4096)
    for i in inpu:
      ch = bytes([i])
      if(ch==b'\n'):
        run_this = self.input_buffer.getvalue()
        old_stdout = sys.stdout
        old_stderr = sys.stderr
        sys.stdout = self.output_buffer
        sys.stderr = self.output_buffer
        try:
          prompt = self.interpreter.push(bytes.decode(run_this))
        except:
          print("Goodbye")
          leave_now=True
        sys.stdout=old_stdout
        sys.stderr = old_stderr
        self.conn.send(bytes(self.output_buffer.getvalue(), "utf-8"))
        self.output_buffer=io.StringIO()
        self.input_buffer=io.BytesIO()
        if(leave_now):
          self.conn.close()
          del self.conn
          self.interpreter.resetbuffer()
          self.socket.listen(1)
          return
        if(prompt):
          self.conn.send(b'      >')
        else:
          self.conn.send(b'server>')
      else:
        self.input_buffer.write(ch)

  def loop(self):
    self.start()
    while(True):
      self.poll()
    self.end()

if __name__ == "__main__":
  ip = Interpocket("/home/reighley/test_socket", {"hello" : "hello world"})
  ip.loop()


   
    
