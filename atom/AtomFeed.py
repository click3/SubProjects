#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

from xml.dom import minidom, Node
from xml.dom.minidom import parseString
import io
import codecs
from datetime import datetime
import copy
import types
import uuid

class AtomFeed:
    def __init__(self, title, url = None, feed_url = None, author = None):
        if (url == None):
            assert(feed_url == None)
            assert(author == None)
            self.__initImplOpen(title)
            return
        assert(feed_url != None)
        assert(author != None)
        self.__initImplNew(title, url, feed_url, author)

    def __initImplNew(self, title, url, feed_url, author):
        assert(isinstance(title, str))
        assert(isinstance(url, str))
        assert(isinstance(feed_url, str))
        assert(isinstance(author, (str, list)))
        self.__title = title
        self.__url = url
        self.__feed_url = feed_url
        self.__author = author if (not isinstance(author, str)) else [author]
        self.__entry = []

    def __getXml(self, file):
        xmlStr = ""
        if (isinstance(file, str)):
            assert(len(file) > 0)
            if (file[0] == '<'):
                xmlStr = file
            else:
                f = codecs.open(file, 'r', 'utf-8')
                xmlStr = f.read()
                f.close()
        else:
            xmlStr = file.read()
        return parseString(xmlStr)

    def __parseHeader(self, root_element):
        entryElements = []
        for element in root_element.childNodes:
            if (element.nodeName == 'title'):
                assert(element.childNodes.length == 1)
                text = element.firstChild
                assert(text.nodeType == Node.TEXT_NODE)
                self.__title = text.data
            elif (element.nodeName == 'link'):
                rel = element.getAttribute('rel')
                href = element.getAttribute('href')
                if (rel == 'self'):
                    self.__feed_url = href
                else:
                    self.__url = href
            elif (element.nodeName == 'updated'):
                pass
                # nonuse
            elif (element.nodeName == 'author'):
                self.__author = []
                for name in element.childNodes:
                    if (name.nodeName != 'name'):
                        continue
                    assert(name.childNodes.length == 1)
                    text = name.firstChild
                    assert(text.nodeType == Node.TEXT_NODE)
                    self.__author.append(text.data)
            elif (element.nodeName == 'id'):
                pass
                # nonuse
            elif (element.nodeName == 'entry'):
                entryElements.append(element)
        assert(isinstance(self.__title, str))
        assert(isinstance(self.__feed_url, str))
        assert(isinstance(self.__url, str))
        assert(isinstance(self.__author, list))
        entryElements.reverse()
        return entryElements

    def __parseEntry(self, entryElements):
        self.__entry = []
        for entryElement in entryElements:
            entry = {}
            for element in entryElement.childNodes:
                if (element.nodeName == 'title'):
                    assert(element.childNodes.length == 1)
                    text = element.firstChild
                    assert(text.nodeType == Node.TEXT_NODE)
                    entry['title'] = text.data
                elif (element.nodeName == 'link'):
                    entry['url'] = element.getAttribute('href')
                elif (element.nodeName == 'id'):
                    pass
                    # nonuse
                elif (element.nodeName == 'updated'):
                    assert(element.childNodes.length == 1)
                    text = element.firstChild
                    assert(text.nodeType == Node.TEXT_NODE)
                    entry['updated'] = datetime.strptime(text.data, '%Y-%m-%dT%H:%M:%SZ')
                elif (element.nodeName == 'summary'):
                    data = ""
                    if (element.childNodes.length == 1):
                        text = element.firstChild
                        assert(text.nodeType == Node.TEXT_NODE)
                        data = text.data
                    entry['summary'] = data
                elif (element.nodeName == 'content'):
                    if (element.childNodes.length == 1):
                        text = element.firstChild
                        assert(text.nodeType == Node.TEXT_NODE)
                        data = text.data
                        entry['content'] = data
            assert(len(entry) == 4)
            assert(isinstance(entry['title'], str))
            assert(isinstance(entry['url'], str))
            assert(isinstance(entry['updated'], datetime))
            assert('summary' not in entry or isinstance(entry['summary'], str))
            assert('content' not in entry or isinstance(entry['content'], str))
            self.__entry.append(entry)

    def __initImplOpen(self, file):
        xml = self.__getXml(file)
        assert(xml.childNodes.length == 1)
        root = xml.firstChild
        assert(root.tagName == 'feed')
        assert(root.getAttribute('xmlns') == 'http://www.w3.org/2005/Atom')
        entryElements = self.__parseHeader(root)
        self.__parseEntry(entryElements)

    def __setHeader(self, doc):
        feed = doc.createElement('feed')
        feed.setAttribute('xmlns', 'http://www.w3.org/2005/Atom')
        doc.appendChild(feed)

        title = doc.createElement('title')
        url = doc.createElement('link')
        feed_url = doc.createElement('link')
        updated = doc.createElement('updated')
        author = doc.createElement('author')
        id = doc.createElement('id')

        title.appendChild(doc.createTextNode(self.__title))
        url.setAttribute('href', self.__url)
        feed_url.setAttribute('rel', 'self')
        feed_url.setAttribute('href', self.__feed_url)
        t = self.__entry[len(self.__entry)-1]['updated']
        updated.appendChild(doc.createTextNode(t.strftime('%Y-%m-%dT%H:%M:%SZ')))
        for name in self.__author:
            element = doc.createElement('name')
            element.appendChild(doc.createTextNode(name))
            author.appendChild(element)
        id.appendChild(doc.createTextNode(self.__feed_url))

        feed.appendChild(title)
        feed.appendChild(url)
        feed.appendChild(feed_url)
        feed.appendChild(updated)
        feed.appendChild(author)
        feed.appendChild(id)

        return feed

    def __setEntry(self, doc, rootElement):
        while(len(self.__entry) > 0):
            entry = self.__entry.pop()
            element = doc.createElement('entry')
            title = doc.createElement('title')
            link = doc.createElement('link')
            id = doc.createElement('id')
            updated = doc.createElement('updated')

            title.appendChild(doc.createTextNode(entry['title']))
            link.setAttribute('href', entry['url'])
            idImpl = uuid.uuid3(uuid.NAMESPACE_URL, 'data:text/plain,' + entry['title'] + entry['url'])
            id.appendChild(doc.createTextNode('urn:uuid:' + str(idImpl)))
            updated.appendChild(doc.createTextNode(entry['updated'].strftime('%Y-%m-%dT%H:%M:%SZ')))

            element.appendChild(title)
            element.appendChild(link)
            element.appendChild(id)
            element.appendChild(updated)
            if ('content' in entry):
                content = doc.createElement('content')
                content.setAttribute('type', 'html')
                content.appendChild(doc.createTextNode(entry['content']))
                element.appendChild(content)
            else:
                summary = doc.createElement('summary')
                if ('summary' in entry and entry['summary']):
                    summary.appendChild(doc.createTextNode(entry['summary']))
                else:
                    summary.appendChild(doc.createTextNode(entry['title']))
                element.appendChild(summary)
            rootElement.appendChild(element)

    def toXml(self):
        doc = minidom.Document()
        rootElement = self.__setHeader(doc)
        self.__setEntry(doc, rootElement)
        return doc.toxml(encoding = 'utf-8')

    def appendEntry(self, entry):
        assert(isinstance(entry, dict))
        assert(len(entry) in (4, 5))
        assert(isinstance(entry['title'], str))
        assert(isinstance(entry['url'], str))
        assert(isinstance(entry['updated'], datetime))
        assert('summary' not in entry or isinstance(entry['summary'], str))
        assert('content' not in entry or isinstance(entry['content'], (str, None.__class__)))
        self.__entry.append(entry)

    def getEntry(self):
        return copy.deepcopy(self.__entry)
