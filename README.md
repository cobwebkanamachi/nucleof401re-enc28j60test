# nucleof401re-enc28j60test
This is an experimental one.
Based on ArduinoShieldSPI(Mr.Marcelo Barros's) + UIPEthernet(Mr.Hudak's HTTPServer_Echo_ENC28J60).
I mingled two works in one.

I got response show  bellow.

Setup SPI ...
Setup ethernet ...
REV = 7
LINK = 1
Local IP = MYIP
..........Setup SPI ...
Setup ethernet ...
REV = 7
LINK = 1
Local IP = MYIP
in client

in size>0

GET request received:

GET / HTTP/1.1
Echo done.
in client

in size>0

in client

in size>0
in hand on telnet (here).
bellow are request from browser(chrome).
---

GET request received:

GET / HTTP/1.1
Host: MYIP
Connection: keep-alive
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.13 Safari/537.36
Accept-Encoding: gzip, deflate, sdch
Accept-Language: ja,en-US;q=0.8,en;q=0.6

Echo done.
in client

in size>0

GET request received:

GET /favicon.ico HTTP/1.1
Host: MYIP
Connection: keep-alive
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.13 Safari/537.36
Accept: */*
Referer: http://MYIP/
Accept-Encoding: gzip, deflate, sdch
Accept-Language: ja,en-US;q=0.8,en;q=0.6

Echo done.

