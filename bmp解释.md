<!--
 * @Author: Amos Cao
 * @Date: 2023-09-14 20:58:32
 * @LastEditors: Amos Cao
 * @LastEditTime: 2023-09-16 15:41:30
 * @Description: sad代码dog
-->
BMP文件格式，又称为Bitmap（位图）

由于它可以不作任何变换地保存图像像素域的数据，因此成为我们取得RAW数据的重要来源。

BMP文件的数据按照从文件头开始的先后顺序分为四个部分：
* bmp文件头(bmp file header)：提供文件的格式、大小等信息
* 位图信息头(bitmap information)：提供图像数据的尺寸、位平面数、压缩方式、颜色索引等信息
* 调色板(color palette)：可选，如使用索引来表示图像，调色板就是索引与其对应的颜色的映射表
* 位图数据(bitmap data)：就是图像数据啦

下面结合Windows结构体的定义，通过一个表来分析这四个部分。

![BMP图像四部分](img\bmp文件四部分.webp)

我们一般见到的图像以24位图像为主，即R、G、B三种颜色各用8个bit来表示，这样的图像我们称为真彩色，这种情况下是不需要调色板的，也就是所位图信息头后面紧跟的就是位图数据了。

因此，我们常常见到有这样一种说法：**位图文件从文件头开始偏移54个字节就是位图数据了，这其实说的是24或32位图的情况。这也就解释了我们按照这种程序写出来的程序为什么对某些位图文件没用了。**

在BMP文件中，如果一个数据需要用几个字节来表示的话，那么该数据的存放字节顺序为“低地址村存放低位数据，高地址存放高位数据”。如数据0x1756在内存中的存储顺序为：

![bmp数据存放](img\bmp数据存放.webp)


这种存储方式称为小端方式(little endian) , 与之相反的是大端方式（big endian）。对两者的使用情况有兴趣的可以深究一下，其中还是有学问的。

## 位图文件的基本结构

1. 文件头信息块
文件信息头 （14字节）存储文件类型，文件大小等信息
```
// 文件信息头结构体
typedef struct tagBITMAPFILEHEADER
{
    unsigned short bfType;
    // 19778，必须是BM字符串，对应的十六进制为0x4d42,十进制为19778，否则不是bmp格式文件
    unsigned int   bfSize;        // 文件大小 以字节为单位(2-5字节)
    unsigned short bfReserved1;   // 保留，必须设置为0 (6-7字节)
    unsigned short bfReserved2;   // 保留，必须设置为0 (8-9字节)
    unsigned int   bfOffBits;     // 从文件头到像素数据的偏移  (10-13字节)
} BITMAPFILEHEADER;

```


2. 图像描述信息块
图片信息头 （40字节）存储着图像的尺寸，颜色索引，位平面数等信息
```
//图像信息头结构体
typedef struct tagBITMAPINFOHEADER
{
    unsigned int    biSize;          // 此结构体的大小 (14-17字节)
    long            biWidth;         // 图像的宽  (18-21字节)
    long            biHeight;        // 图像的高  (22-25字节)
    unsigned short  biPlanes;        
    // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1 (26-27字节)
    unsigned short  biBitCount;      // 一像素所占的位数，一般为24   (28-29字节)
    unsigned int    biCompression;   // 说明图象数据压缩的类型，0为不压缩。 (30-33字节)
    unsigned int    biSizeImage;     
    // 像素数据所占大小, 这个值应该等于上面文件头结构中bfSize-bfOffBits (34-37字节)
    long            biXPelsPerMeter; // 说明水平分辨率，用象素/米表示。一般为0 (38-41字节)
    long            biYPelsPerMeter; // 说明垂直分辨率，用象素/米表示。一般为0 (42-45字节)
    unsigned int    biClrUsed;       
    // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。 (46-49字节)
    unsigned int    biClrImportant;  
    // 说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。(50-53字节)
} BITMAPINFOHEADER;

```

3. 颜色表
调色板 （由颜色索引数决定）(可以没有此信息，下面的例子就因为采用了24位真彩色保存所以没有这部分信息）
```
//24位图像素信息结构体,即调色板
typedef struct _PixelInfo {
    unsigned char rgbBlue;   //该颜色的蓝色分量  (值范围为0-255)
    unsigned char rgbGreen;  //该颜色的绿色分量  (值范围为0-255)
    unsigned char rgbRed;    //该颜色的红色分量  (值范围为0-255)
    unsigned char rgbReserved;// 保留，必须为0
} PixelInfo;
```

4. 图像数据区
位图数据 （由图像尺寸决定）每一个像素的信息在这里存储

颜色表接下来位为位图文件的图像数据区，在此部分记录着每点像素对应的颜色号，其记录方式也随颜色模式而定，既2色图像每点占1位（8位为1字节）；16色图像每点占4位（半字节）；256色图像每点占8位（1字节）；真彩色图像每点占24位（3字节）。所以整个数据区的大小也会随之变化。究其规律而言，可的出如下计算公式：图像数据信息大小=（图像宽度 * 图像高度 * 记录像素的位数）/8。

BMP存储像素值的方式为从下至上，从左至右，紧随着文件头存储的字节为图像最下一行的数值，从左下角开始依次存储。

22 22 22 23 … 为图像左下角像素的数值，依次向右存储，最后一行扫描完后，紧接着存储上一行，最后一个byte存储的是图像右上角像素的数值。

54个字节的图像头文件 + widthheight3的图像数据

数据排列方式：B G R(第一个像素点) B G R(第一个像素点) … … …

注意：
```
bmp文件格式是windows环境中交换图数据的标准。无压缩故占用空间大，图像深度1/4/8/24bit，位映射存储，存储数据时图像的扫描从左到右从下到上的顺序。24位bmp存储是按BGR存的
具体说是这样，假设现在有一张图，宽6个像素，高随便，24位格式保存（每个像素点占三个字节，分别代表RGB三颜色）
那么它每行有效数据部分应该是6×3=18字节，18不是4的倍数，所以要补齐，补到20字节。最终存储格式如下图：

前18位为有效像素数据，最后两位XX为填补数据，无意义
BGR BGR BGR BGR BGR BGR XX
BGR BGR BGR BGR BGR BGR XX
BGR BGR BGR BGR BGR BGR XX

当行数据不能被4整除时，会出现图片倾斜的问题，例如一张750*450的bmp图片，每一行的像素应该占750x3 = 2250字节，但2250不能被4整除，所以系统会自动在每一行后面补齐2个字节数据，达到2252字节，这样图片的长就被拉长了，就会出现倾斜现象。
所以为了解决这个问题，我们应该判断图片的行数据字节是否能被4整除，如果不行，我们将系统补齐的多余的字节剔除掉
对于24位bmp图，每个像素占3个字节，读到每行末尾换行读取时，需要跳过补齐的字节

```

## 具体例子


右键单击我们开头画的图片可以查看该图片的分辨率，宽度，高度和位深度。为959*959像素。是24位真彩色位图。959*959*24/(8*1024*1024) = 2.63MB,与显示的图片大小相符。

![属性](img\属性.png)

接下来用Vscode打开这张BMP图像，显示的是十六进制的代码

![十六进制图像](img\十六进制图像.png)

使用的是小端存储。

文件信息头结构体第一个数据是unsigned short（16位）类型的bfType变量。观察十六进制代码结果可以看到第一行开头的42 4D倒着念就是4D 42(刚好16位对应unsigned short类型），即bftype=0x4D42（转换为十进制为19778，实际上所有BMP图像的bfType对应属性都是这个值）。

按照这个方法可得出第二个数据bfSize类型为unsigned int（32位），图中对应的十六进制代码为002A24F6（转换为十进制为2,761,974），这代表文件大小为2,761,974字节=2.63MB

接下来利用类似的方法可以从十六进制代码中得到这张位图的文件头信息块和图像描述信息块所存储的信息

```
unsigned short bfType          = 0x4D42     = 19778
unsigned int   bfSize          = 0x002A24F6 = 2,761,974字节=2,761,974/(1024*1024)=2.63MB
unsigned short bfReserved1     = 00 00
unsigned short bfReserved2     = 00 00
unsigned int   bfOffBits       = 0X00000036 = 0x36 = 54字节

unsigned int   biSize          = 0x00000028 = 0x28  = 40字节(图像信息头结构体大小就是40字节)
long           biWidth         = 0x00000480 = 0x03BF = 959像素;
long           biHeight        = 0x00000288 = 0x03BF = 959像素 ;
unsigned short biPlanes        = 0x0001     = 0x1   = 1;
unsigned short biBitCount      = 0x0018     = 0x18  = 24位;
unsigned int   biCompression   = 0x00000000 = 0;
unsigned int   biSizeImage     = 0x00222C00 = 0;(等于bfSize-bfOffBits)
long           biXPelsPerMeter = 0x00000000 = 0;
long           biYPelsPerMeter = 0x00000000 = 0;  
unsigned int   biClrUsed       = 0x00000000 = 0;
unsigned int   biClrImportant  = 0x00000000 = 0;

/*因为采用了24位真彩色格式保存，所以没有颜色表信息。紧跟着上述文件头信息块和图像
描述信息块存储的信息的就是图像数据区的信息。每一个像素为24位,即3字节，例如紧跟着
的FFFFFF这三个字节就代表白色*/
```

## 使用的函数
1. fopen

FILE * fopen(const char * path,const char * mode)；

path是字符串类型的bmp图片路径；mode读取方式，等下回用到"rb"，读写打开一个二进制文件，允许读写数据，文件必须存在。

2. fseek

int fseek(FILE *stream, long offset, int fromwhere);

函数设置文件指针stream的位置，stream指向以fromwhere为基准，偏移offset个字节的位置。

3. fread
size_t fread ( void *buffer, size_t size, size_t count, FILE *stream);

从stream中读取count个单位大小为size个字节，存在buffer中。

4. SetPixel
```
函数功能：该函数将指定坐标处的像素设为指定的颜色。 　　
函数原型：COLORREF SetPixel(HDC hdc, int X,int Y, COLORREF crColor)；
参数：
hdc：设备环境句柄。
X：指定要设置的点的X轴坐标，按逻辑单位表示坐标。 　　
Y：指定要设置的点的Y轴坐标，按逻辑单位表示坐标。
crColor：指定要用来绘制该点的颜色。
返回值：如果函数执行成功，那么返回值就是函数设置像素的RGB颜色值。这个值可能与crColor指定的颜我色有不同，之所以有时发生这种情况是因为没有找到对指定颜色进行真正匹配造成的；如果函数失败，那么返回值是C1。
```
