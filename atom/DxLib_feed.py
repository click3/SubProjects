#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import datetime

class DxLibData(FeedUpdateData):
    def getCheckUrl():
        return 'http://homepage2.nifty.com/natupaji/DxLib/dxdload.html'

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
        p = re.compile('Ver\s*([\d\.]+[a-z]?)', re.DOTALL)
        result = p.search(body)
        if (result == None):
            self.__isError = True
            return
        version = result.group(1)
        title = self.__class__.__name__.split("Data")[0] + version
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['title'] == title):
            return
        self.__updateExist = True
        self.__title = title
        self.__url = 'http://homepage2.nifty.com/natupaji/DxLib/dxlog.html'

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

    def getCharset(self):
        return 'shift_jis'

    def isError(self):
        return self.__isError


def main():
    return FeedUpdate(__file__, 'http://homepage2.nifty.com/natupaji/DxLib/').run()


if __name__ == '__main__':
    main()
