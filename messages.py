from twisted.internet.protocol import ClientFactory, Protocol, DatagramProtocol
from twisted.internet import reactor
from twisted.internet.task import LoopingCall
import struct

class ClientUDPProtocol(DatagramProtocol):
    def __init__(self, port, address, on_received):
        self.port = port
        self.address = address
        self.on_received = on_received

    def startProtocol(self):
        self.transport.connect(self.address, self.port)

    def sendDatagram(self, data):
        #print "Sending ", data
        self.transport.write(data)

    def datagramReceived(self, data, host):
        self.on_received(data)

def FloatToInt(a):
    s = struct.pack('f', a)
    return struct.unpack('i', s)[0]

class Sender(object):
    def __init__(self):
        def rec(x):
            print x
        self.udpProtocol = ClientUDPProtocol(5556, "192.168.1.1", rec)
        self.udpConnection = reactor.listenUDP(0, self.udpProtocol)

        self.send0 = self.udpProtocol.sendDatagram
        self.seq = 1
        self.repeat = None
    
    def send(self, a):
        self.send0(a.replace("$", str(self.seq)))
        self.seq += 1
    
    def takeoff(self, on):
        tosend = "AT*REF=$,%d\r"%((1<<18)+(1<<20)+(1<<22)+(1<<24)+(1<<28)+(1<<9)*on)
        self.send(tosend)
        self.repeat = None

    def emergency(self, on):
        tosend = "AT*REF=$,%d\r"%((1<<18)+(1<<20)+(1<<22)+(1<<24)+(1<<28)+(1<<8)*on)
        self.send(tosend)
        self.repeat = None
                                   
    def command(self, fb, lr, rot, alt):
        tosend = ("AT*PCMD=$,%d,%d,%d,%d,%d\r"%(1, FloatToInt(-lr), FloatToInt(-fb), FloatToInt(alt), FloatToInt(-rot)))
        self.repeat = tosend

    def watchdog(self):
        self.send("AT*COMWDG")

    def resettrim(self):
        self.send("AT*FTRIM")

    def magcalib(self):
        self.send("AT*CALIB")

    def update(self):
        if self.repeat != None:
            #print "Repeating", self.repeat
            self.send(self.repeat)
        self.watchdog()

def start(f, fps):
    tick = LoopingCall(f)
    try:
        tick.start(1.0 / fps)
        reactor.run() #not sure it is useful
    except KeyboardInterrupt:
        reactor.stop()
        raise KeyboardInterrupt
