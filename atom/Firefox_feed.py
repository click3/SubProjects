#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import os
from AtomFeed import AtomFeed
import codecs
import datetime
from distutils.version import LooseVersion

class FirefoxData(FeedUpdateData):
    def getCheckUrl():
        return 'https://www.mozilla.org/en-US/firefox/releases/'

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
        p = re.compile(r'<a href="\.\./([\d\.]+)/releasenotes/">\1</a>', re.DOTALL)
        versionList = p.findall(body)
        if (not versionList):
            self.__isError = True
            return
        versionList = sorted(versionList, key=lambda x: LooseVersion(x))
        version = versionList[-1]
        title = 'Firefox' + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'https://www.mozilla.org/en-US/firefox/' + version + '/releasenotes/'

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
    return FeedUpdate(__file__, 'http://mozilla.jp/firefox/').run()


if __name__ == '__main__':
    main()
