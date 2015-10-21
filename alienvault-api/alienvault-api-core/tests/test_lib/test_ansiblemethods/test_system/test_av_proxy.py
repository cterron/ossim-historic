from nose import with_setup
from nose.tools import raises
import sys
import os
import urllib2
import httplib

from apimethods.system.proxy import AVProxy

TEST_PROXY_FILE_BASE = "/tmp/curlrc."

class Test():

    content = [(TEST_PROXY_FILE_BASE + "0", "proxy = http://testproxy.com"),
               (TEST_PROXY_FILE_BASE + "1", "proxy = http://testproxy.com:1234"),
               (TEST_PROXY_FILE_BASE + "2", "proxy = http://testproxy.com:1234\nproxy-user = user:password"),
               (TEST_PROXY_FILE_BASE + "3", "bad proxy content\nproxo = aaa")]

    def setup(self):
        for i in range (len(Test.content)):
            f = open(Test.content[i][0], "w")
            f.write(Test.content[i][1])
            f.close()

    def teardown(self):
        for (filename, content) in Test.content:
            os.remove(filename)

    @classmethod
    def setup_class(cls):
        pass

    @classmethod
    def teardown_class(cls):
        pass

    def get_proxy_url(self, proxy_file_content):
        return proxy_file_content.split(" = ")[1].replace("http://", "").split("\n")[0]

    def test_constructor(self):
        proxy = AVProxy(proxy_file=Test.content[0][0])
        assert proxy.get_proxy_url() == self.get_proxy_url(Test.content[0][1])
        assert proxy.need_authentication() == False
        del proxy

        proxy = AVProxy(proxy_file=Test.content[1][0])
        assert proxy.get_proxy_url() == self.get_proxy_url(Test.content[1][1])
        assert proxy.need_authentication() == False
        del proxy

        proxy = AVProxy(proxy_file=Test.content[2][0])
        assert proxy.get_proxy_url() == self.get_proxy_url(Test.content[2][1])
        assert proxy.need_authentication() == True
        del proxy

        # Bad proxy file
        proxy = AVProxy(proxy_file=Test.content[3][0])
        assert proxy.get_proxy_url() == None
        assert proxy.need_authentication() == False
        del proxy

        # Non-existent proxy file
        proxy = AVProxy(proxy_file="/tmp/as/sdadsa")
        assert proxy.get_proxy_url() == None
        assert proxy.need_authentication() == False
        del proxy

    def test_no_proxy_connect(self):
        # Open connection without proxy
        proxy = AVProxy()
        #     using an url
        response = proxy.open("http://python.org")
        assert response is not None
        del response
        #     using a request
        request = urllib2.Request("http://python.org")
        response = proxy.open(request)
        assert response is not None

    @raises(urllib2.URLError, IOError, httplib.HTTPException)
    def test_bad_proxy_connect_url(self):
        # Open connection through proxy without authentication
        proxy = AVProxy(proxy_file=Test.content[0][0])
        response = proxy.open("http://python.org", timeout=1)

    @raises(urllib2.URLError, IOError, httplib.HTTPException)
    def test_bad_proxy_connect_request(self):
        # Open connection through proxy without authentication
        proxy = AVProxy(proxy_file=Test.content[0][0])
        request = urllib2.Request("http://python.org")
        response = proxy.open(request, timeout=1)

    @raises(urllib2.URLError, IOError, httplib.HTTPException)
    def test_bad_proxy_connect_url_auth(self):
        # Open connection through proxy with authentication
        proxy = AVProxy(proxy_file=Test.content[2][0])
        response = proxy.open("http://python.org", timeout=0.5)

    @raises(urllib2.URLError, IOError, httplib.HTTPException)
    def test_bad_proxy_connect_request_auth(self):
        # Open connection through proxy with authentication
        proxy = AVProxy(proxy_file=Test.content[2][0])
        request = urllib2.Request("http://python.org")
        response = proxy.open(request, timeout=0.5)

    @raises(urllib2.URLError, IOError, httplib.HTTPException)
    def test_no_proxy_connect_url_aut(self):
        # Open connection through proxy with authentication
        proxy = AVProxy(proxy_file=Test.content[2][0])
        response = proxy.open("http://python.org", timeout=0.5)

    def test_no_proxy_connect_retry(self):
        # No proxy with retries
        proxy = AVProxy()
        response = proxy.open("http://python.org", retries=0.5)
        assert response is not None

    @raises(urllib2.URLError, IOError, httplib.HTTPException)
    def test_no_proxy_connect_url_aut(self):
        # Bad Proxy with retries
        proxy = AVProxy(proxy_file=Test.content[2][0])
        response = proxy.open("http://python.org", timeout=0.5, retries=1)
