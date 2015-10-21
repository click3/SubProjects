#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import os
from AtomFeed import AtomFeed
import codecs
import datetime

class TortoiseGitData(FeedUpdateData):
    def getCheckUrl():
        return 'https://tortoisegit.org/'

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
        p = re.compile(r'<a href="/download/">Stable:\s*((?:\d+\.?)+)</a>')
        result = p.search(body)
        if (result == None):
            self.__isError = True
            return
        version = result.group(1)
        title = 'TortoiseGit' + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'https://tortoisegit.org/docs/releasenotes/#Release_' + version

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
    return FeedUpdate(__file__, 'http://code.google.com/p/tortoisegit/').run()


if __name__ == '__main__':
    main()
