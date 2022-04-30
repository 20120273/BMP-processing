#pragma once
#include<iostream>
#include<string>
#include<fstream>
using namespace std;
struct header
{
	char sign[2];
	int bfsize;
	int bfReserved;
	int dataoffset;
};
struct DIB
{
	int DIBsize;
	int Width;
	int Height;
	short Planes;
	short bpp;
	int compression;
	int imagesize;
	int XpixelPerM;
	int YpixelPerM;
	int colorused;
	int importantcolor;
};
struct AnhBMP
{
	header a;  
	DIB b;
	unsigned char* data;   //luu du lieu pixel cua anh
	char* phandu; //luu phan du giua phan DIB va du lieu diem anh
};
void docanh(char* file, AnhBMP& anh);
struct mau
{
	unsigned char A;
	unsigned char B;
	unsigned char G;
	unsigned char R;
};
void ghianh(char* file, AnhBMP anh);
void chuyensang8bpp(char*  fileouput,AnhBMP anh);
void chuyendatasangpixel(AnhBMP anh, mau*& pixel);
void chuyentupixelsangdata(AnhBMP anh, mau* pixel);
void resize(AnhBMP anh, int s,char* fileoutput);


