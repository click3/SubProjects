#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import datetime

class MinGWData(FeedUpdateData):
    def getCheckUrl():
        return 'http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/'

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
        p = re.compile('mingw-get-inst-(\d*)', re.DOTALL)
        result = p.search(body)
        if (result == None):
            self.__updateExist = False
            return
        version = result.group(1)
        title = 'MinGW' + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            self.__updateExist = False
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/mingw-get-inst-' + version


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
    FeedUpdate(__file__, 'http://sourceforge.net/projects/mingw/').run()


if __name__ == '__main__':
    main()
