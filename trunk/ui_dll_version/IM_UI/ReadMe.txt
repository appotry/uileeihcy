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

/*
GDI+ currently has no support to raster operations. When we use R2_XOR pen operations, we use the Graphics.GetHdc()
method to get the handle to the device context. During the operation when your application uses the HDC, the GDI+ 
should not draw anything on the Graphics object until the Graphics.ReleaseHdc method is called. Every GetHdc call
must be followed by a call to ReleaseHdc on a Graphics object, as in the following snippet:

	IntPtr hdc1 = g1.GetHdc();
	//Do something with hdc1
	g.ReleaseHdc (hdc1);

	g2 = Graphics.FromImage (curBitmap);
	IntPtr hdc1 = g1.GetHdc();
	IntPtr hdc2 = g2.GetHdc();
	BitBlt (hdc2, 0, 0,
	this.ClientRectangle.Width,
	this.ClientRectangle.Height,
	hdcl, 0, 0, 13369376);
	g2.DrawRectangle (Pens.Red, 40, 40, 200, 200);
	g1.ReleaseHdc (hdcl);
	g2.ReleaseHdc (hdc2);

If we make a GDI+ call after GetHdc, the system will throw an "object busy" exception. For example, in the preceding
code snippet we make a DrawRectangle call after GetHdc and before ReleaseHdc. As a result we will get an exception
saying, "The object is currently in use elsewhere."

Using GDI on a GDI+ Graphics Object Backed by a Bitmap

After a call to GetHdc, we can simply call a Graphics object from a bitmap that returns a new HBITMAP structure. 
This bitmap does not contain the original image, but rather a sentinel pattern, which allows GDI+ to tract changes
to the bitmap. When ReleaseHdc is called, changes are copied back to the original image. This type of device context
is not suitable for raster operations because the handle to device context is considered write-only, and raster 
operations require it to be read-only. This approach may also degrade the performance because creating a new bitmap 
and saving changes to the original bitmap operations may tie up all your resources.


*/
///////////////////////////////////////////////

经过一段时间的煎熬，发现完全使用GDI/HDC来实现分层窗口，效率是不会高的。
因这这个还是得借助于Graphics::GetHDC/ReleaseHDC来完成，但这个的效率太低了...
而且为了如果一个GDI HDC绘制在两个 GDI+ HDC中间，会导致ReleaseHDC时，将GDI
绘制的全部覆盖了.