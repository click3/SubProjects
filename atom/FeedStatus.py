#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import re
import datetime

class FeedStatusData(FeedUpdateData):
    def getCheckUrl():
        return None

    def __init__(self, errorList):
        super().__init__()
        self.__errorList = errorList
        self.__updateExist = False
        self.__isError = False

    def setFeed(self, feed):
        super().setFeed(feed)

    def setBody(self, body):
        super().setBody(body)
        summary = self.getSummary()
        entrys = super().getFeed().getEntry()
        if (entrys[len(entrys)-1]['summary'] == summary):
            return
        self.__updateExist = True

    def updateExist(self):
        return self.__updateExist

    def getTitle(self):
        return "feed status " + datetime.datetime.now().strftime('%Y%m%d%H%M')

    def getUrl(self):
        return "http://feeds.click3.org/application/"

    def getSummary(self):
        return str(self.__errorList)

    def getUpdated(self):
        return datetime.datetime.utcnow()

    def isError(self):
        return self.__isError


def main(errorList):
    return FeedUpdate(__file__, 'http://feeds.click3.org/application/', errorList).run()


if __name__ == '__main__':
    main([])
