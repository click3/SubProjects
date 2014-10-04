#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import os
from AtomFeed import AtomFeed
import codecs
import datetime

class OperaData(FeedUpdateData):
    def getCheckUrl():
        return 'http://www.opera.com/docs/changelogs/windows/'

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
        p = re.compile(r'<a [^>]*href="([^"]*)"[^>]*>Opera\s*([\d\.]+)</a>', re.DOTALL)
        result = p.search(body)
        if (result == None):
            self.__isError = True
            return
        url = result.group(1)
        version = result.group(2)
        title = 'Opera' + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            self.__updateExist = False
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'http://www.opera.com' + url

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
    return FeedUpdate(__file__, 'http://jp.opera.com/').run()


if __name__ == '__main__':
    main()
