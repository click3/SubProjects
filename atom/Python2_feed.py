#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import datetime

class Python2Data(FeedUpdateData):
    def getCheckUrl():
        return 'http://www.python.org/download/'

    def __init__(self):
        super().__init__()
        self.__updateExist = False
        self.__title = ''
        self.__url = ''

    def setFeed(self, feed):
        super().setFeed(feed)

    def setBody(self, body):
        super().setBody(body)
        assert(isinstance(body, str))
        p = re.compile('Python\s*(2.[\d.]+)', re.DOTALL)
        result = p.search(body)
        if (result == None):
            self.__updateExist = False
            return
        version = result.group(1)
        title = "Python" + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            self.__updateExist = False
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'http://www.python.org/download/releases/' + version + '/'


    def updateExist(self):
        return self.__updateExist

    def getTitle(self):
        return self.__title

    def getUrl(self):
        return self.__url

    def getSummary(self):
        return ""

    def getUpdated(self):
        return datetime.datetime.utcnow()

def main():
    FeedUpdate(__file__, 'http://www.python.org/').run()


if __name__ == '__main__':
    main()