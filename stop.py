#!/usr/bin/python
import messages

sender = messages.Sender()
def test():
    sender.takeoff(0)

messages.start(test, 30)
