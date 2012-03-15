#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from FeedUpdate import FeedUpdate, FeedUpdateData
import datetime
import os
import glob
import codecs
import re
import stat

class indexData(FeedUpdateData):
    def getCheckUrl():
        return None

    def __init__(self):
        super().__init__()
        self.__updateExist = False
        self.__title = ''
        self.__url = ''
        self.__content = ''

    def setFeed(self, feed):
        super().setFeed(feed)

    def updateExist(self):
        return self.__updateExist

    def getTitle(self):
        return self.__title

    def getUrl(self):
        return self.__url

    def getSummary(self):
        return ""

    def getContent(self):
        return self.__content

    def setBody(self, body):
        super().setBody(body)
        assert(body == None)

        outPath = 'index.html'
        feedList = indexData.__getAfterFeedList('*.atom')
        beforeFeedList = indexData.__getBeforeFeedList(outPath)
        if (feedList == beforeFeedList):
            self.__updateExist = False
            return
        indexData.__outHtml(outPath, feedList)

        diff = set(feedList) - set(beforeFeedList)
        title = 'AddFeed(%d)' % len(diff)
        contentImpl = ''
        for feed in diff:
            contentImpl += '<li><a href="%s">%s</a></li>\n' % (feed, feed.split('.')[0])
        content = '<ul>\n%s</ul>' % contentImpl
        self.__updateExist = True
        self.__title = title
        self.__content = content
        self.__url = 'http://sweetie089.no-ip.biz/atom/'

    def __getAfterFeedList(pattern):
        list = []
        for path in glob.glob(pattern):
            if (os.path.basename(path) == 'index.atom'):
                continue
            list.append(path)
        list.sort()
        return list

    def __getBeforeFeedList(path):
        if (not os.path.exists(path)):
            return []
        f = codecs.open(path, 'r', 'utf-8')
        data = f.read()
        list = re.compile("<li[^>]*><a[^>]*>([^<]+)</a></li>", re.DOTALL).findall(data)
        list.sort()
        return list

    def __outHtml(outPath, feedList):
        template_html = '''<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta http-equiv="Content-Style-Type" content="text/css" />
	<meta http-equiv="Content-Script-Type" content="text/javascript" />

	<title>自作Atomフィード集</title>

	<link rel="SHORTCUT ICON" href="favicon.ico" />
    <link rel="alternate" type="application/atom+xml" title="一覧追加フィード" href="./index.atom" />
</head>
<body>
<ul>
<!-- replace -->
</ul>
</body>
</html>
'''
        replace_html = ''
        for path in feedList:
            replace_html += '<li><a href="' + path + '">' + path.replace('.py', '') + '</a></li>\n'
        html = template_html.replace('<!-- replace -->', replace_html)
        f = codecs.open(outPath, 'w', 'utf-8')
        f.write(html)
        f.close()
        os.chmod(outPath, stat.S_IROTH | stat.S_IWOTH | stat.S_IRGRP | stat.S_IWGRP | stat.S_IRUSR | stat.S_IWUSR | stat.S_IREAD | stat.S_IWRITE)

def main():
    FeedUpdate(__file__, 'http://sweetie089.no-ip.biz/atom/').run()


if __name__ == '__main__':
    main()
