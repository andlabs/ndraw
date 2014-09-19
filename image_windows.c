// 18 september 2014

#include "winapi_windows.h"
#include "_cgo_export.h"

HBITMAP newBitmap(int dx, int dy, void **ppvBits)
{
	BITMAPINFO bi;
	HBITMAP b;

	ZeroMemory(&bi, sizeof (BITMAPINFO));
	bi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = (LONG) dx;
	bi.bmiHeader.biHeight = -((LONG) dy);                   // negative height to force top-down drawing
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = (DWORD) (dx * dy * 4);
	b = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (VOID **) ppvBits, NULL, 0);
	if (b == NULL)
		xpanic("error creating Image", GetLastError());
	// see image.Image() in image_windows.go for details
	memset(*ppvBits, 0xFF, dx * dy * 4);
	return b;
}

HDC newDCForBitmap(HBITMAP bitmap, HBITMAP *prev)
{
	HDC screen, dc;

	screen = GetDC(NULL);
	if (screen == NULL)
		xpanic("error getting screen DC for NewImage()", GetLastError());
	dc = CreateCompatibleDC(screen);
	if (dc == NULL)
		xpanic("error creating memory DC for NewImage()", GetLastError());
	*prev = (HBITMAP) SelectObject(dc, bitmap);
	if (*prev == NULL)
		xpanic("error selecting bitmap into memory DC for NewImage()", GetLastError());
	if (ReleaseDC(NULL, screen) == 0)
		xpanic("error releasing screen DC for NewImage()", GetLastError());
	return dc;
}

void imageClose(HBITMAP bitmap, HDC dc, HBITMAP prev)
{
	if (SelectObject(dc, prev) != bitmap)
		xpanic("error restoring initial DC bitmap in Image.Close()", GetLastError());
	if (DeleteDC(dc) == 0)
		xpanic("error removing image DC in Image.Close()", GetLastError());
	if (DeleteObject(bitmap) == 0)
		xpanic("error removing bitmap in Image.Close()", GetLastError());
}

void moveTo(HDC dc, int x, int y)
{
	if (MoveToEx(dc, x, y, NULL) == 0)
		xpanic("error moving to point", GetLastError());
}

void lineTo(HDC dc, int x, int y)
{
	if (LineTo(dc, x, y) == 0)
		xpanic("error drawing line to point", GetLastError());
}

void drawText(HDC dc, char *str, int x, int y)
{
	WCHAR *wstr;

	wstr = towstr(str);
	if (SetBkMode(dc, TRANSPARENT) == 0)
		xpanic("error setting text drawing to be transparent", GetLastError());
	if (TextOutW(dc, x, y, wstr, wcslen(wstr)) == 0)
		xpanic("error drawing text", GetLastError());
	free(str);
}
