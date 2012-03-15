#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import os
from AtomFeed import AtomFeed
import codecs
import datetime

class msysGitData(FeedUpdateData):
    def getCheckUrl():
        return 'http://code.google.com/p/msysgit/downloads/list'

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
        p = re.compile('(msysGit-fullinstall-(.*?)-preview.*?.exe)')
        result = p.search(body)
        if (result == None):
            self.__updateExist = False
            return
        version = result.group(2)
        title = 'msysGit' + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            self.__updateExist = False
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'http://code.google.com/p/msysgit/downloads/detail?name=' + result.group(1)


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
    FeedUpdate(__file__, 'http://code.google.com/p/msysgit/').run()


if __name__ == '__main__':
    main()
