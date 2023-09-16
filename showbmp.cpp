/*
 * @Author: Amos Cao
 * @Date: 2023-09-13 21:41:50
 * @LastEditors: Amos Cao
 * @LastEditTime: 2023-09-16 16:07:53
 * @Description: sad代码dog
 */
#include <iostream>
#include <windows.h>

using namespace std;

// 编制读入并显示bmp格式的程序

// 位图（Bitmap）其数据没有经过压缩，或最多只采用行程长度编码（RLE，run-length encoding）来进行轻度的无损数据压缩
// 正是因为它没有进行数据压缩，其内部存储的色彩信息（灰度图，RGB 或 ARGB）直接以二进制的形式暴露在外
// 位图格式的文件头长度可变，而且其中参数繁多。但是我们日常生活中遇到的 .bmp 格式图片的文件头长度绝大多数都是 54 字节，
// 其中包括 14 字节的 Bitmap 文件头以及 40 字节的 DIB (Device Independent Bitmap) 数据头，或称位图信息数据头（BItmap Information Header）。

// 1.BMP 文件头 信息头 调色板 裸数据
// 2.代码示例读取BMP显示信息
// 3.代码示例读取BMP图像保存在另一BMP文件
// 4.代码示例BGR888转存BGR555的BMP文件

// 在你的 BITMAPFILEHEADER 结构体中，虽然每个成员的大小总和是14字节，
// 但编译器可能会在结构体中的某些位置插入填充字节以确保对齐。
// 这种情况下，结构体的大小会被编译器自动调整，以满足所使用的平台的对齐要求。

// 文件信息头结构体
// #pragma pack(push, 1) // 指定按照1字节对齐
// typedef struct tagBITMAPFILEHEADER
// {
//    unsigned short bfType;      // 19778，必须是BM字符串，对应的十六进制为0x4d42,十进制为19778，否则不是bmp格式文件
//    unsigned int bfSize;        // 文件大小 以字节为单位(2-5字节)
//    unsigned short bfReserved1; // 保留，必须设置为0 (6-7字节)
//    unsigned short bfReserved2; // 保留，必须设置为0 (8-9字节)
//    unsigned int bfOffBits;     // 从文件头到像素数据的偏移  (10-13字节)
// } BITMAPFILEHEADER;
// #pragma pack(pop) // 恢复默认的对齐方式

// 图像信息头结构体
// typedef struct tagBITMAPINFOHEADER
// {
//    unsigned int biSize;         // 此结构体的大小 (14-17字节)
//    long biWidth;                // 图像的宽  (18-21字节)
//    long biHeight;               // 图像的高  (22-25字节)
//    unsigned short biPlanes;     // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1 (26-27字节)
//    unsigned short biBitCount;   // 一像素所占的位数，一般为24   (28-29字节)
//    unsigned int biCompression;  // 说明图象数据压缩的类型，0为不压缩。 (30-33字节)
//    unsigned int biSizeImage;    // 像素数据所占大小, 这个值应该等于上面文件头结构中bfSize-bfOffBits (34-37字节)
//    long biXPelsPerMeter;        // 说明水平分辨率，用象素/米表示。一般为0 (38-41字节)
//    long biYPelsPerMeter;        // 说明垂直分辨率，用象素/米表示。一般为0 (42-45字节)
//    unsigned int biClrUsed;      // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。 (46-49字节)
//    unsigned int biClrImportant; // 说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。(50-53字节)
// } BITMAPINFOHEADER;

// 24位图像素信息结构体,即调色板
// typedef struct _PixelInfo
// {
//    unsigned char rgbBlue;     // 该颜色的蓝色分量  (值范围为0-255)
//    unsigned char rgbGreen;    // 该颜色的绿色分量  (值范围为0-255)
//    unsigned char rgbRed;      // 该颜色的红色分量  (值范围为0-255)
//    unsigned char rgbReserved; // 保留，必须为0
// } PixelInfo;

BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;

void showBmpHead(BITMAPFILEHEADER pBmpHead)
{ // 定义显示信息的函数，传入文件头结构体
   cout << "BMP文件大小：" << fileHeader.bfSize / 1024 << "KB\t" << fileHeader.bfSize / 1024.0 / 1024.0 << "MB" << endl;
   cout << "保留字必须为0：" << fileHeader.bfReserved1 << endl;
   cout << "保留字必须为0：" << fileHeader.bfReserved2 << endl;
   cout << "实际位图数据的偏移字节数:" << fileHeader.bfOffBits << endl;
}

void showBmpInfoHead(BITMAPINFOHEADER pBmpinfoHead)
{ // 定义显示信息的函数，传入的是信息头结构体
   cout << "位图信息头:" << endl;
   cout << "信息头的大小:" << infoHeader.biSize << "字节" << endl;
   cout << "位图宽度:" << infoHeader.biWidth << "像素" << endl;
   cout << "位图高度:" << infoHeader.biHeight << "像素" << endl;
   cout << "图像的位面数(位面数是调色板的数量,默认为1个调色板):" << infoHeader.biPlanes << endl;
   cout << "每个像素的位数:" << infoHeader.biBitCount << endl;
   cout << "压缩方式:" << infoHeader.biCompression << endl;
   cout << "图像的大小:" << infoHeader.biSizeImage << endl;
   cout << "水平方向分辨率:" << infoHeader.biXPelsPerMeter << endl;
   cout << "垂直方向分辨率:" << infoHeader.biYPelsPerMeter << endl;
   cout << "使用的颜色数:" << infoHeader.biClrUsed << endl;
   cout << "重要颜色数:" << infoHeader.biClrImportant << endl;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 声明用来处理消息的函数

char *tmp;
long height, width, n;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   HDC hdc;        // 设备环境句柄
   PAINTSTRUCT ps; // 绘制结构
   RECT rect;      // 矩形结构

   switch (message) // 处理得到的消息
   {
   case WM_PAINT: // 处理窗口区域无效时发来的消息
      hdc = BeginPaint(hwnd, &ps);
      GetClientRect(hwnd, &rect);
      // DrawText(hdc, TEXT("Hello World"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER); // 文字
      BYTE b, g, r;
      for (int j = 0; j < height; j++)
      {
         for (int i = 0; i < width; i++)
         {
            b = *tmp++;
            g = *tmp++;
            r = *tmp++;
            // 图像数据是倒着存的，所以需要先画下面的
            SetPixelV(hdc, i, height - j, RGB(r, g, b));
         }
         tmp += n;
      }
      EndPaint(hwnd, &ps);
      return 0;
   case WM_DESTROY: // 处理窗口关闭时的消息
      MessageBox(hwnd, TEXT("close programm!"), TEXT("end"), MB_OK | MB_ICONINFORMATION);
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProc(hwnd, message, wParam, lParam); // DefWindowProc处理我们自定义的消息处理函数没有处理到的消息
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
   cout << "开始读取bmp图片信息" << endl;
   FILE *fp;
   fp = fopen("img/test.bmp", "rb"); // 读取目录下的image.bmp文件。

   if (fp == NULL)
   {
      cout << "打开'test.bmp'失败!" << endl;
      return -1;
   }

   cout << "初始指针的位置：" << ftell(fp) << endl;

   fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
   if (fileHeader.bfType != 0x4d42)
   {
      cout << "文件类型标识错误!" << endl;
      return -1;
   }
   else
   {
      cout << "文件类型标识正确!" << endl;
      cout << "文件标识符：" << fileHeader.bfType << endl;
   }
   showBmpHead(fileHeader);

   cout << "读取文件头信息后指针的位置：" << ftell(fp) << endl;

   cout << "----------------------------" << endl;

   fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
   showBmpInfoHead(infoHeader);

   cout << "读取信息头后指针的位置：" << ftell(fp) << endl;

   cout << "----------------------------" << endl;

   // 显示
   long channel;
   width = infoHeader.biWidth;
   height = infoHeader.biHeight;
   channel = infoHeader.biBitCount / 8;

   // 字节不对齐而显示倾斜，所以959*3 /4 余1，所以每行之后会补三个字节
   n = 4 - (width * 3 % 4);

   char *bmpBuf = new char[width * height * channel + height * n];
   cout << "文件头信息块大小：" << sizeof(BITMAPFILEHEADER) << ",图像描述信息块" << sizeof(BITMAPINFOHEADER) << endl;

   // fseek(fp, long(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)), 0); // 定位到起始位置
   fseek(fp, fileHeader.bfOffBits, 0);

   cout << "保存时，图像像素数据的偏移位置：" << ftell(fp) << endl;
   
   cout << "字节对齐，需补" << n << "字节";
   fread(bmpBuf, sizeof(char), width * height * 3 + height * n, fp);

   fclose(fp);

   // 写入文件信息
   FILE *outFile = fopen("show.bmp", "wb"); // 输出文件
   fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, outFile);
   fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, outFile);
   tmp = bmpBuf;
   for (int j = 0; j < height; j++)
      for (int i = 0; i < width * 3; i++)
         fwrite(tmp++, 1, 1, outFile);
   fclose(outFile);

   // // 显示图像
   // HWND hwnd = GetForegroundWindow(); // 窗口句柄
   // HDC hdc = GetDC(hwnd);             // 绘图句柄
   // BYTE b, g, r;
   tmp = bmpBuf;

   // for (int j = 0; j < height; j++)
   // {
   //    for (int i = 0; i < width; i++)
   //    {
   //       b = *tmp++;
   //       g = *tmp++;
   //       r = *tmp++;
   //       // 图像数据是倒着存的，所以需要先画下面的
   //       SetPixelV(hdc, i, 400 + height - j, RGB(r, g, b));
   //    }
   // }

   // // 释放绘图句柄
   // ReleaseDC(hwnd, hdc);

   static TCHAR szAppName[] = TEXT("MyWindow");
   HWND hwnd;
   MSG msg;
   WNDCLASS wndclass; // 声明一个窗口类对象

   // 以下为窗口类对象wndclass的属性
   wndclass.style = CS_HREDRAW | CS_VREDRAW;                     // 窗口样式
   wndclass.lpszClassName = szAppName;                           // 窗口类名
   wndclass.lpszMenuName = NULL;                                 // 窗口菜单:无
   wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 窗口背景颜色
   wndclass.lpfnWndProc = WndProc;                               // 窗口处理函数
   wndclass.cbWndExtra = 0;                                      // 窗口实例扩展:无
   wndclass.cbClsExtra = 0;                                      // 窗口类扩展:无
   wndclass.hInstance = hInstance;                               // 窗口实例句柄
   wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);             // 窗口最小化图标:使用缺省图标
   wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);               // 窗口采用箭头光标

   if (!RegisterClass(&wndclass))
   { // 注册窗口类, 如果注册失败弹出错误提示
      MessageBox(NULL, TEXT("窗口注册失败"), TEXT("错误"), MB_OK | MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindow(          // 创建窗口
       szAppName,                // 窗口类名
       TEXT("show bmp picture"), // 窗口标题
       WS_OVERLAPPEDWINDOW,      // 窗口的风格
       CW_USEDEFAULT,            // 窗口初始显示位置x:使用缺省值
       CW_USEDEFAULT,            // 窗口初始显示位置y:使用缺省值
       height + 200,             // 窗口的宽度:使用缺省值
       width + 200,              // 窗口的高度:使用缺省值
       NULL,                     // 父窗口:无
       NULL,                     // 子菜单:无
       hInstance,                // 该窗口应用程序的实例句柄
       NULL);

   ShowWindow(hwnd, iCmdShow); // 显示窗口
   UpdateWindow(hwnd);         // 更新窗口

   while (GetMessage(&msg, NULL, 0, 0)) // 从消息队列中获取消息
   {
      TranslateMessage(&msg); // 将虚拟键消息转换为字符消息
      DispatchMessage(&msg);  // 分发到回调函数(过程函数)
   }
   return msg.wParam;

   // system("pause");
   // return 0;
}

// 1. 使用分别读取图像的位图文件头和位图信息头的数据，其中位图信息头结构中包含图像的宽、高等信息，因此使用biWidth,biHeight来获取图像的宽和高；
// 2. 为bmpBuf分配空间，大小为width * height * 3，因为每一像素由RGB三个分量组成；
// 3. 由于使用fread读取了位图文件头和位图信息头，此时fp也随之偏移，偏移长度即为 sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) ，因此需要使用fseek函数将fp调整到指向初始位置，然后才能将从fp中读取相应数据到bmpBuf中；
// 4. 写文件时使用一个临时变量tmp指向bmpBuf头部，然后依次写入即可，注意每行的数据是 width * 3 个，同样是因为每一像素由RGB三个分量组成；
// 5. 显示图像时使用Windows.h中自带的HWND、HDC数据类型和GetForegroundWindow()、GetDC(HWND hwnd)函数获取窗口和绘图句柄，再调用SetPixel(HDC hdc, int x,int y, COLORREF color)函数进行绘图即可。其中，获取rgb数据时，文件中每一像素中的颜色信息按顺序依次为b ,g ,r。
