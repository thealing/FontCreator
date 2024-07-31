#include <stdio.h>
#include <windows.h>
#include <olectl.h>

int main() {
	char font[128];
	int size;
	int width;
	int height;
	FILE* cfg = fopen("font.cfg", "r");
	fgets(font, 128, cfg);
	font[strlen(font) - 1] = '\0';
	fscanf(cfg, "%i %i %i", &size, &width, &height);
	printf("FONT:    %s\n", font);
	printf("SIZE:    %i\n", size);
	printf("WIDTH:   %i\n", width);
	printf("HEIGHT:  %i\n", height);
	HWND hwnd_screen = GetDesktopWindow();
	HDC hdc_screen = GetDC(hwnd_screen);
	HDC hdc = CreateCompatibleDC(hdc_screen);
	HBITMAP hbm = CreateCompatibleBitmap(hdc_screen, 128 * width, height);
	HFONT hf = CreateFont(size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, font);
	SelectObject(hdc, hbm);
	SelectObject(hdc, hf);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	char string[128];
	for (int i = 0; i < 128; i++) {
		string[i] = isprint(i) ? i : ' ';
	}
	for (int i = 0; i < 128; i++) {
		RECT rc = { i * width, 0, (i + 1) * width, height };
		DrawText(hdc, &string[i], 1, &rc, DT_CENTER | DT_VCENTER);
	}
	PICTDESC pd = {};
	pd.cbSizeofstruct = sizeof(pd);
	pd.picType = PICTYPE_BITMAP;
	pd.bmp.hbitmap = hbm;
	IPicture* picture;
	OleCreatePictureIndirect(&pd, IID_IPicture, FALSE, (void**)&picture);
	IPictureDisp* disp;
	picture->QueryInterface(IID_IPictureDisp, (void**)&disp);
	int result = OleSavePictureFile(disp, SysAllocString(L"out.bmp"));
	if (result == S_OK) {
		printf("done\n");
	}
	else {
		printf("error %i\n", result);
	}
	return 0;
}
