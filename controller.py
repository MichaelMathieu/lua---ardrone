import os
import os.path
import messages
import threading
import Queue
import time

class PipeReader(threading.Thread):
    def __init__(self, pipename, queue):
        super(PipeReader, self).__init__()
        if not os.path.exists(pipename):
            print("Warning : fifo %s does not exist. Creating it..."%(pipename))
            os.mkfifo(pipename)
        self.pipe = open(pipename, "r")
        self.queue = queue
        self.buffer = ""
        self.go = True

    def run(self):
        while self.go:
            r = self.pipe.read(1)
            self.buffer = self.buffer + r
            if len(self.buffer) == 0:
                time.sleep(0.01)
                continue
            if self.buffer[0] != "$":
                f = self.buffer.find("$")
                if f != -1:
                    self.buffer = self.buffer[f:]
                else:
                    self.buffer = ""
            f = self.buffer.find("#")
            if f != -1:
                rec = self.buffer[1:f]
                self.buffer = self.buffer[f+1:]
                self.queue.put(rec)

        def stop(self):
            self.pipe.close()
            self.go = False

if __name__=="__main__":
    sender = messages.Sender()
    q = Queue.Queue()
    pipereader = PipeReader("fifo_ardrone_cmd", q)
    pipereader.start()
    def readpipe():
        try:
            r = q.get(False)
        except Queue.Empty:
            print "no message"
            sender.update()
        else:
            print "received", r
            if r == "T0":
                sender.takeoff(0)
            elif r == "T1":
                sender.takeoff(1)
            elif r == "E0":
                sender.emergency(0)
            elif r == "E1":
                sender.emergency(1)
            elif r[0] == "C":
                c = [float(a) for a in r[1:].split(',')]
                sender.command(c[0], c[1], c[2], c[3])
                sender.update()
    
    try:
        messages.start(readpipe, 30)
    except KeyboardInterrupt:
        pipereader.stop()
