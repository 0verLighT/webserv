#!/usr/bin/python3

import os
import sys

print("Content-Type: text/plain")
print()
print("method=" + os.environ.get("REQUEST_METHOD", ""))
print("query=" + os.environ.get("QUERY_STRING", ""))
print("content_type=" + os.environ.get("CONTENT_TYPE", ""))
print("body=" + sys.stdin.read())
