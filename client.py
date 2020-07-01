#!/usr/bin/python

import socket
import cv2
import numpy


def recv_size(sock, count):
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf:
            return None
        buf += newbuf
        count -= len(newbuf)
    return buf

def recv_all(sock, count):
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf:
            return None
        buf += newbuf
        count -= len(newbuf)
    return buf


sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

address_server = ('127.0.0.1', 9999)
sock.connect(address_server)

while(True):
    length = recv_size(sock, 16)
    if length: #收成功
        stringData = recv_all(sock, int(length))
        data = numpy.fromstring(stringData, dtype='uint8')
        decimg = cv2.imdecode(data, 1)
        decimg = cv2.resize(decimg,(200,200))
        cv2.imshow('SERVER', decimg)
        cv2.waitKey(10)

sock.close()
cv2.destroyAllWindows()
