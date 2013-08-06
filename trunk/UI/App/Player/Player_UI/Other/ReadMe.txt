启动流程如下：

1. 加载配置文件 player.xml
2. 加载当前皮肤
3. 创建各个窗口，但不显示
4. 初始化音频模块
5. 加载播放列表
6. 显示所有窗口
7. 播放上一次曲目


// 歌词相关逻辑

1. 获取歌词服务器

GET /dll/lyricsvr.dll?svrlst&ci=65106c57565a2256244244161c464808425e5353011d01 HTTP/1.1
Referer: http://ttlrcct2.qianqian.com/
Accept: image/gif, image/x-xbitmap, image/jpg, image/pjpeg, text/html, text/xml, */*
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)
Host: ttlrcct2.qianqian.com
Connection: Keep-Alive

HTTP/1.1 200 OK
Connection: close
Content-type: text/html; charset=UTF-8
Content-Length: 382
Date: Fri, 12 Apr 2013 08:00:13 GMT
Server: apache

<ttp_lrcsvr query_url="http://ttlrcct.qianqian.com/dll/lyricsvr.dll?svrlst">
.<server name="...............(......)" url="http://ttlrcct.qianqian.com/dll/lyricsvr.dll" />
.<server name="...............(......)" url="http://ttlrccnc.qianqian.com/dll/lyricsvr.dll" />
.<extra title=".............................." url="http://ttplayer.qianqian.com/geci/download.php" />
</ttp_lrcsvr>

2. 获取歌词列表

GET /dll/lyricsvr.dll?sh?Artist=C39EBA83BA83&Title=ED540278&Flags=2&ci=65106c57565a2256244244161c464808425e5353011d01 HTTP/1.1
Referer: http://ttlrcct.qianqian.com/
Accept: image/gif, image/x-xbitmap, image/jpg, image/pjpeg, text/html, text/xml, */*
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)
Host: ttlrcct.qianqian.com
Connection: Keep-Alive
Cache-Control: no-cache

HTTP/1.1 200 OK
Connection: close
Content-type: text/html; charset=UTF-8
Content-Length: 122
Date: Fri, 12 Apr 2013 08:00:13 GMT
Server: apache

<?xml version="1.0" encoding="UTF-8" ?>
<result>
.<lrc id="149461" artist="........." title="......"></lrc>
</result>

3. 下载歌词
GET /dll/lyricsvr.dll?dl?Id=155050&Code=-277916630&ci=65106c57565a2256244244161c464808425e5353011d01 HTTP/1.1
Referer: http://ttlrcct.qianqian.com/
Accept: image/gif, image/x-xbitmap, image/jpg, image/pjpeg, text/html, text/xml, */*
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)
Host: ttlrcct.qianqian.com
Connection: Keep-Alive
Cache-Control: no-cache

HTTP/1.1 200 OK
Connection: close
tt-title: 4353435359972C544B623A6748722C006B70ED70D153035E21000A00A973FC62FE56386E0F620CFF628D690050006100640020006D0069006E006900BD5501FF0A00
tt-url: 68007400740070003A002F002F007700770077002E00620061006900640075002E0063006F006D002F00630062002E007000680070003F0063003D004900670046005F0070007900660071006E0057005200730072006A0054005900500030004B00590054006A0059006B00300041003700620035004800660033005000310062006400720030004B00620075004800590059006E003100630033006E003100440030005400410071003100350048006E007A006E006A00540073006E0036004B003100350048004B00680050006A00620059005000680052006B0075004800660076006D005700540031006E0031006D00300075005A00660071006E0048006E007600500048005400640072004800440064006E0066004B006400540068007300710070005A007700590054006A004300450051004C0077005900540041006B003900580079004E007A0051004D00370047006D00790034006B0070007900370038005100680050004500550069007100620055004C00490038005500410071003900750037007100380075006700540045006D0067004B006F005100680039005900550079007300300054006800660071006E0066004B004200550048005900730030005A004B007A003500480030003000490079002D006200350048006300330072006A00300059006E006A0066003000550076002D0062003500480052006B005000570044004C00300041005000470075006A0059006B005000480052007600500031005400300055004C004900380035004800520073006E005700540033003000410050007A006D00310059006B0072004800300033006E0030000A0068007400740070003A002F002F007700770077002E00620061006900640075002E0063006F006D002F00630062002E007000680070003F0063003D004900670046005F0070007900660071005000480044007A006E0048006D007300500066004B00590054006A0059006B00300041003700620035004800360034006E003100360031006E0036004B00620075004800590033006E005700300034006E006A00520030005400410071003100350048006E007A006E006A0054007300500030004B0031003500480030004C00500068006D0064007500680052003100500048003600730075006A00300059006E0079006D00300075005A00660071006E0048006E007600500048005400640072004800440064006E0066004B006400540068007300710070005A007700590054006A0043004500510076006B00760058007900710064005100680046003900700079007700640051006800500045005500690071002D00490068004E003800490061007100310051003100630073006E0048006E00730050005A004B0047006D004C0077007A006D00670075002D0055006100710047005500680077002D0058006100340043004900410064005F003500760075007A003500670037006B0070004D006600300054006800660071006E0066004B004200550048005900730030005A004B007A003500480030003000490079002D006200350048006300330072006A00300059006E006A0066003000550076002D0062003500480052006B005000570044004C00300041005000470075006A0059006B005000480052007600500031005400300055004C004900380035004800520073006E005700540033003000410050007A006D003100590031006E006A0044007600500066000A00
Content-type: text/html; charset=UTF-8
Content-Length: 2706
Date: Fri, 12 Apr 2013 09:32:31 GMT
Server: apache

[00:00.00]Baby
[00:00.50]Justin Bieber 
[00:01.00]
[00:02.44]Oh woah
[00:06.06]Oh woah
[00:09.76]Oh woah
[00:13.30]
[00:14.22]You know you love me, I know you care
[00:17.95]You shout whenever, and I'll be there
[00:21.68]You want my love, you want my heart
[00:25.36]And we will never ever ever be apart
[00:29.19]Are we an item? Girl quit playing
[00:32.60]We're just friends, what are you saying
[00:36.45]Said there's another and look right in my eyes
[00:40.01]My first love broke my heart for the first time
[00:43.17]And I was like
[00:44.28]
[00:44.32]Baby, baby, baby, oh like
[00:47.59]Baby, baby, baby, no like
[00:51.38]Baby, baby, baby, oh
[00:55.38]I thought you'd always been mine, mine
[00:59.04]Baby, baby, baby, oh like
[01:02.88]Baby, baby, baby, no like
[01:06.58]Baby, baby, baby, oh
[01:09.99]I thought you'd always been mine, mine
[01:13.67]
[01:13.77]For you, I would have done whatever
[01:16.99]And I just can't believe we ain't together
[01:20.69]And I wanna play it cool, but I'm losing you
[01:24.34]I'll buy you anything, I'll buy you any ring
[01:28.11]And I'm in pieces, baby fix me
[01:31.70]And you'll shake me till you wake me from this bad dream
[01:35.68]I'm going down, down, down, down
[01:39.21]And I just can't believe my first love won't be around
[01:42.85]And I'm now like
[01:43.40]
[01:43.48]Baby, baby, baby, oh like
[01:47.26]Baby, baby, baby, no like
[01:50.88]Baby, baby, baby, oh
[01:54.31]I thought you'd always been mine, mine
[01:58.15]Baby, baby, baby, oh like
[02:02.02]Baby, baby, baby, no like
[02:05.67]Baby, baby, baby, oh
[02:09.21]I thought you'd always been mine, mine
[02:12.61]
[02:13.46]When I was 13 I had my first love
[02:16.93]There was nobody that compared to my baby
[02:18.48]And nobody came between us or could ever come above
[02:20.68]She had me going crazy
[02:22.69]Oh I was starstruck
[02:24.38]She woke me up daily don't need no starbucks
[02:27.89]She made my heart pound
[02:29.77]Asking for a beat when I see her in the street
[02:31.48]And at school on the playground
[02:33.44]But I really wanna see her on the weekend
[02:35.27]She knows she got me dazy
[02:37.34]Cause she was so amazing
[02:39.16]And now my heart is breaking
[02:41.08]But I'll just keep on saying
[02:42.56]
[02:42.71]Baby, baby, baby, oh like
[02:46.30]Baby, baby, baby, no like
[02:49.92]Baby, baby, baby, oh
[02:53.44]I thought you'd always been mine, mine
[02:57.34]Baby, baby, baby, oh like
[03:01.10]Baby, baby, baby, no like
[03:04.73]Baby, baby, baby, oh
[03:08.23]I thought you'd always been mine, mine
[03:10.92]Now I'm all gone
[03:14.59]Now I'm all gone
[03:18.34]Now I'm all gone
[03:22.03]Now I'm all gone
[03:25.67]I am gone
[03:28.15]

那个tag-url解析出来为
http://www.baidu.com/cb.php?c=IgF_pyfqnWRsrjTYP0KYTjYk0A7b5Hf3P1bdr0KbuHYYn1c3n1D0TAq15HnznjTsn6K15HKhPjbYPhRkuHfvmWT1n1m0uZfqnHnvPHTdrHDdnfKdThsqpZwYTjCEQLwYTAk9XyNzQM7Gmy4kpy78QhPEUiqbULI8UAq9u7q8ugTEmgKoQh9YUys0ThfqnfKBUHYs0ZKz5H00Iy-b5Hc3rj0Ynjf0Uv-b5HRkPWDL0APGujYkPHRvP1T0ULI85HRsnWT30APzm1YkrH03n0
http://www.baidu.com/cb.php?c=IgF_pyfqPHDznHmsPfKYTjYk0A7b5H64n161n6KbuHY3nW04njR0TAq15HnznjTsP0K15H0LPhmduhR1PH6suj0Ynym0uZfqnHnvPHTdrHDdnfKdThsqpZwYTjCEQvkvXyqdQhF9pywdQhPEUiq-IhN8Iaq1Q1csnHnsPZKGmLwzmgu-UaqGUhw-Xa4CIAd_5vuz5g7kpMf0ThfqnfKBUHYs0ZKz5H00Iy-b5Hc3rj0Ynjf0Uv-b5HRkPWDL0APGujYkPHRvP1T0ULI85HRsnWT30APzm1Y1njDvPf
不知道是做什么的