#!/usr/bin/python3.2
# -*- coding: utf-8 -*-

import os
import glob
import codecs

def main():
    template_html = '''<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta http-equiv="Content-Style-Type" content="text/css" />
	<meta http-equiv="Content-Script-Type" content="text/javascript" />

	<title>自作Atomフィード集</title>

	<link rel="SHORTCUT ICON" href="favicon.ico" />
</head>
<body>
<ul>
<!-- replace -->
</ul>
</body>
</html>
'''
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    feedList = []
    for path in glob.glob('*.atom'):
        feedList.append(path)
    feedList.sort()
    replace_html = ''
    for path in feedList:
        replace_html += '<li><a href="' + path + '">' + path.replace('.py', '') + '</a></li>\n'
    html = template_html.replace('<!-- replace -->', replace_html)
    f = codecs.open('index.html', 'w', 'utf-8')
    f.write(html)
    f.close()

if __name__ == '__main__':
    main()
