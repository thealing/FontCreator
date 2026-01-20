#include <stdio.h>
#include <windows.h>
#include <wincodec.h>

int main() {
	char font[100] = {};
	int size = 0;
	int width = 0;
	int height = 0;
	char programName[MAX_PATH] = {};
	GetModuleFileName(NULL, programName, MAX_PATH);
	char* extension = strrchr(programName, '.');
	if (extension != NULL) {
		*extension = 0;
	}
	char configPath[MAX_PATH] = {};
	sprintf(configPath, "%s.cfg", programName);
	FILE* cfg = fopen(configPath, "r");
	if (cfg == NULL) {
		return 1;
	}
	fgets(font, 99, cfg);
	font[strlen(font) - 1] = 0;
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
	BITMAP bm;
	GetObject(hbm, sizeof(bm), &bm);
	int pixel_count = bm.bmWidth * bm.bmHeight;
	void* pixels = malloc(pixel_count * 4);
	GetBitmapBits(hbm, pixel_count * 4, pixels);
	for (int i = 0; i < pixel_count; i++) {
		BYTE* pixel = (BYTE*)pixels + i * 4;
		for (int j = 0; j <= 2; j++) {
			if (pixel[j] > pixel[3]) {
				pixel[3] = pixel[j];
			}
		}
	}
	SetBitmapBits(hbm, pixel_count * 4, pixels);
	CoInitialize(nullptr);
	IWICImagingFactory* factory;
	CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&factory);
	IWICBitmap* bitmap;
	factory->CreateBitmapFromHBITMAP(hbm, nullptr, WICBitmapUseAlpha, &bitmap);
	IWICStream* stream;
	factory->CreateStream(&stream);
	stream->InitializeFromFilename(L"out.png", GENERIC_WRITE);
	IWICBitmapEncoder* encoder;
	factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
	encoder->Initialize(stream, WICBitmapEncoderNoCache);
	IWICBitmapFrameEncode* frame;
	encoder->CreateNewFrame(&frame, nullptr);
	frame->Initialize(nullptr);
	frame->SetSize(bm.bmWidth, bm.bmHeight);
	WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
	frame->SetPixelFormat(&format);
	frame->WriteSource(bitmap, nullptr);
	frame->Commit();
	encoder->Commit();
	return 0;
}
