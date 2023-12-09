// Created by itsshux
// Do whatever you want with it, I hope you find it useful
// You can also download the executable from releases

// NOTE: This code takes screenshot of the main (default) monitor only, to choose a different monitor you can change "DISPLAY" to "\\\\.\\DISPLAY2" for example.

#include <windows.h>
#include <iostream>
#include <wingdi.h>

int main(int argc, char* argv[]) {
    const char* fileName = "screenshot.jpg";
    if (argc >= 2) fileName = argv[1];

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));
    BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = screenWidth;
    bi.biHeight = -screenHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24; // Color depth
    bi.biCompression = BI_RGB; // No compression
    HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD dwBytesWritten;
        BITMAPFILEHEADER bmfHeader;
        bmfHeader.bfType = 0x4D42; // Bitmap or BM
        bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + screenWidth * screenHeight * 3;
        bmfHeader.bfReserved1 = 0;
        bmfHeader.bfReserved2 = 0;
        bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
        WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
        GetDIBits(hMemoryDC, hBitmap, 0, screenHeight, NULL, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);
        BYTE* lpBits = new BYTE[bi.biSizeImage];
        GetDIBits(hMemoryDC, hBitmap, 0, screenHeight, lpBits, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);
        WriteFile(hFile, lpBits, bi.biSizeImage, &dwBytesWritten, NULL);
        CloseHandle(hFile);
        delete[] lpBits;
    }

    // Cleanup
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    DeleteDC(hScreenDC);

    std::cout << "Screenshot taken!" << std::endl;

    return 0;
}
