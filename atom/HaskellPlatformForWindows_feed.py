#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import datetime

class HaskellPlatformForWindowsData(FeedUpdateData):
    def getCheckUrl():
        return 'https://www.haskell.org/platform/windows.html'

    def __init__(self):
        super().__init__()
        self.__updateExist = False
        self.__title = ''
        self.__url = ''
        self.__isError = False

    def setFeed(self, feed):
        super().setFeed(feed)

    def setBody(self, body):
        super().setBody(body)
        assert(isinstance(body, str))
        p = re.compile(r'<a [^>]*href="(//haskell.org/platform/download/[^/]+/HaskellPlatform-((?:[\d]+\.?)+(?:-\w+))-i386-setup\.exe)"[^>]*>', re.DOTALL)
        result = p.search(body)
        if (result == None):
            self.__isError = True
            return
        url = result.group(1)
        version = result.group(2)
        title = 'Haskell' + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'https:' + url

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

    def isError(self):
        return self.__isError


def main():
    return FeedUpdate(__file__, 'http://www.haskell.org/platform/windows.html').run()


if __name__ == '__main__':
    main()
