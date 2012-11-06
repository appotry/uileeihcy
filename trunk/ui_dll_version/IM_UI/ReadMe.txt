flash的脏矩形渲染流程 + 
quake3的多线程渲染管道流水线 + 
redui的2\3d混合机制 + 
pureMVC的松耦合架构 + 
skia底层矢量绘图 + 
lua脚本控制 + 
迅雷7模板机制解决UI元素同质化 + 
ie2文字排版引擎 + 
swing容器原理


矩形区域不包括右、下边界，这一点通常会引起一定的混淆。
因此HRGN hRgn1 = ::CreateRecctRgn(0,0,0,0);是一个空区域
HRGN hRgn2 = ::CreateRectRgn(0,0,1,1,);则是一个单一的点（0，0）

一个DC有没有被裁剪，最后绘制的效率可能相差很大。
例如画一个全屏的图片，裁剪前一共需要9800ms，剪裁后，则需要62ms

// 		CRgn rgn;
// 		rgn.CreateRectRgn(0,0,1,1);
// 		dc.SelectClipRgn(&rgn);

		for (int i = 0; i < 10000; i++)
		{
			dc.BitBlt(0,0, rcClient.Width(), rcClient.Height(), &memDC, 0,0, SRCCOPY);
		}

经验证，用于传递给UpdateLayeredWindow的memdc的内容在提交之后是可以自己去修改的
因此完全没有必要为分层窗口维护 m_hRenderTarget+m_hMemDC，
直接将所有的内容提交到m_hMemDC上即可。

其实普通窗口根本就不需要一个MEMDC，例如刷新一个小按钮，只需要创建一个按钮大小的MEMDC，
将按钮画在该区域上面，最后将MEMDC提交到窗口上即可。没必要创建一个窗口大小的DC。
