#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

import http.client
from AtomFeed import AtomFeed
import os
import codecs
import glob
import threading
import sys
import datetime

class FeedUpdateData:
    def getCheckUrl():
        return 'http://example.com/'

    def __init__(self):
        self.__feed = None
        self.__body = None

    def setFeed(self, feed):
        assert(isinstance(feed, AtomFeed))
        self.__feed = feed

    def setBody(self, body):
        assert(isinstance(body, (str, None.__class__)))
        self.__body = body

    def getFeed(self):
        return self.__feed

    def getBody(self):
        return self.__body

    def updateExist(self):
        return False

    def getTitle(self):
        return 'dummyTitle'

    def getUrl(self):
        return 'http://example.com/'

    def getSummary(self):
        return ""

    def getUpdated(self):
        return datetime.datetime.utcnow()

    def getCharset(self):
        return 'utf-8'

    def getContent(self):
        return None

class FeedUpdate:
    def __getData(self, filename):
        filename = os.path.basename(filename).split('.')[0]
        data_name = filename.split('_')[0] + 'Data'
        return getattr(__import__(filename), data_name)()

    def __init__(self, filename, url):
        data = self.__getData(filename)
        os.chdir(os.path.dirname(os.path.abspath(__file__)))
        name = data.__class__.__name__.split('Data')[0]
        path = './' + name + '.atom'
        if (not os.path.exists(path) or os.path.getsize(path) == 0):
            feed = AtomFeed(name, url, 'http://sweetie089.no-ip.biz/atom/' + name + '.atom', ['sweetie'])
            feed.appendEntry({'title':'feed_create', 'url':'http://sweetie089.no-ip.biz/atom/' + name + '.atom', 'updated':datetime.datetime.utcnow(), 'summary':''})
            xml = feed.toXml().decode('utf-8')
            out = codecs.open(path, 'w', 'utf-8')
            out.write(xml)
            out.close()
        assert(isinstance(path, str))
        assert(isinstance(data, FeedUpdateData))
        self.__path = path
        self.__feed = AtomFeed(path)
        data.setFeed(self.__feed)
        self.__data = data

    def __httpGet(urlStr, charset):
        url = http.client.urlsplit(urlStr)
        conn = None
        if (url.scheme == 'http'):
            conn = http.client.HTTPConnection(url.netloc)
        elif (url.scheme == 'https'):
            conn = http.client.HTTPSConnection(url.netloc)
        assert(conn != None)
        conn.request('GET', url.path + '?' + url.query)
        response = conn.getresponse()
        if (response.status != 200):
            return None
        return response.readall().decode(charset)


    def run(self):
        url = self.__data.__class__.getCheckUrl()
        body = None
        if (url != None):
            body = FeedUpdate.__httpGet(url, self.__data.getCharset())
            if (body == None):
                return
        self.__data.setBody(body)
        if (not self.__data.updateExist()):
            return
        entry = {}
        entry['title'] = self.__data.getTitle()
        entry['url'] = self.__data.getUrl()
        entry['updated'] = self.__data.getUpdated()
        entry['summary'] = self.__data.getSummary()
        if (self.__data.getContent() != None):
            entry['content'] = self.__data.getContent()
        self.__feed.appendEntry(entry)
        tmpPath = self.__path + '.tmp'
        if (os.path.exists(tmpPath)):
            os.remove(tmpPath)
        xml = self.__feed.toXml().decode('utf-8')
        out = codecs.open(tmpPath, 'w', 'utf-8')
        out.write(xml)
        out.close()
        os.remove(self.__path)
        os.rename(tmpPath, self.__path)


class FeedUpdateThread(threading.Thread):
    def __init__(self, path):
        super().__init__()
        self.__path = path

    def run(self):
        moduleName = self.__path.split('.')[0]
        getattr(__import__(moduleName), 'main')()


def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    threadList = []
    for path in glob.glob('*_feed.py'):
        thread = FeedUpdateThread(path)
        thread.start()
        threadList.append(thread)
    for thread in threadList:
        thread.join()
    getattr(__import__('index'), 'main')()

if __name__ == '__main__':
    main()
