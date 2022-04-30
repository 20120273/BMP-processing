#pragma once
#include"bmp.h"

void docanh(char* file, AnhBMP& anh)
{
	fstream doc;
	doc.open(file, ios::in | ios::binary);
	if (!doc.is_open())   //kiem tra xem co mo duoc tap tin khong
	{
		cout << "Khong doc duoc ";
		return;
	}
	doc.read(anh.a.sign, 2);  //doc signature cua anh
	doc.read((char*)&(anh.a.bfsize), 12); //doc phan con lai cua header
	doc.read((char*)&(anh.b), 40);//doc DIB cua anh
	if (anh.b.DIBsize != 40)  //vi DIB cua anh co nhieu phien ban khac nhau nen ta se luu them phan du neu co
	{
		anh.phandu = new char[anh.a.dataoffset - 54];
		doc.read(anh.phandu, anh.a.dataoffset - 54);
	}
	doc.seekg(anh.a.dataoffset, ios::beg);
	anh.data = new unsigned char[anh.b.imagesize];
	doc.read((char*)anh.data, anh.b.imagesize);
	doc.close();
	cout << "Doc thanh cong! " << endl;
}
void ghianh(char* file, AnhBMP anh)
{
	fstream luu;
	luu.open(file, ios::out | ios::binary);
	if (!luu.is_open())
	{
		cout << "Khong doc duoc ";
		return;
	}
	luu.write(anh.a.sign, 2);
	luu.write((char*)&(anh.a.bfsize), 12);
	luu.write((char*)&anh.b, 40);
	if (anh.b.bpp == 8)  //de hien thi anh 8bpp thi em se luu bang mau co kich thuoc la 256*4
	{
		char* colorboard = new char[1024];
		int k = 0;
		for (int i = 0; i < 256; i++) //bang mau se co dang (0,0,0,0,1,1,1,0,2,2,2,0,...,255,255,255,0)
		{
			colorboard[k++] = i;
			colorboard[k++] = i;
			colorboard[k++] = i;
			colorboard[k++] = 0;
		}
		luu.write(colorboard, 1024);
		delete[] colorboard;
	}
	if (anh.b.DIBsize != 40)  //vi DIB cua anh co nhieu phien ban khac nhau nen ta se ghi them phan du neu co
	{
		luu.write(anh.phandu, anh.a.dataoffset - 54);
	}
	luu.seekp(anh.a.dataoffset, ios::beg);
	luu.write((char*)anh.data, anh.b.imagesize);
	luu.close();
	cout << "Ghi thanh cong" << endl;
}
void chuyensang8bpp(char* fileoutput,AnhBMP anh)
{
	AnhBMP anh8bpp;   //phan thong tin va du lieu cua anh 8bpp
	anh8bpp.a = anh.a; //về cơ bản thì phần này của ảnh 8bpp giống ảnh 24/32 bpp, chỉ khác ở các thành phần như bpp,imagesize nên ta gán luôn cho nó bằng ảnh 24/32 bpp và sửa lại các thông số đó
	anh8bpp.b = anh.b;
	anh8bpp.b.bpp = 8;
	anh8bpp	.b.colorused = 256;
	anh8bpp.b.DIBsize = 40; //Tuy nhiên máy của em lại nếu chỉ sửa 2 phần này thì không hiển thị được ảnh, phải lưu thêm bảng màu. Vì vậy em phải chỉnh DIBsize về 40 và chỉnh dataoffset lên 54+1024=1078, trong đó 1024 là để lưu bảng màu
	anh8bpp.a.dataoffset = 54+1024;
	int padding = 4 - ((anh.b.Width) % 4); //em tính phần padding của ảnh 8bpp bằng cách lấy chiều rộng chia lấy dư cho 4( vì số byte của mỗi pixel bằng 1 nên em không cần nhân thêm vào
	if (padding == 4)              //sau đó em sẽ lấy 4 trừ đi phần dư để biết số byte cần bù vào
	{
		padding = 0;  //nếu chiều rộng ảnh chia hết cho 4 thì sẽ dư 0, đến lúc này sẽ không cần padding thêm vào nhưng padding lúc này sẽ bằng 4-0=4 nên phải chỉnh nó về không
	}
	anh8bpp.b.imagesize = anh8bpp.b.Height * (anh8bpp.b.Width + padding); //kích thước phần dữ liệu điểm ảnh bằng chiều dài*(rộng +padding)
	anh8bpp.data = new unsigned char[anh8bpp.b.imagesize];
	int k = 0; //ta se tao bien k de dem va truy xuat vao phan data cua anh 8bpp
	if (anh.b.bpp == 24)
	{
		for (int i = 0; i < anh.b.Height; i++)
		{
			for (int j = 0; j < (anh.b.Width * (anh.b.bpp/8)); j++) //ta sẽ chạy chiều dài  byte cua 1 hàng (ko gồm padding)=chiều dài * số byte 1 pixel
			{
				if (j % 3 == 2) //vd như byte số 2, thì ta sẽ được 1 bộ số 0,1,2. Đây chính là 3 byte lưu 3 màu B,G,R của ảnh
				{
					anh8bpp.data[k++] = anh.data[i * (anh.b.Width * (anh.b.bpp / 8) + padding) + j - 2] / 3 + anh.data[i * (anh.b.Width * (anh.b.bpp / 8) + padding) + j - 1] / 3 + anh.data[i * (anh.b.Width * (anh.b.bpp / 8) + padding) + j] / 3;
				}//để chuyển qua cần lấy trung bình cộng 3 byte này gắn qua điểm ảnh mới
			}
			k += padding; //cái này là bỏ qua phần padding, tức là các byte padding ta sẽ không lưu gì hết
		}
	}
	if (anh.b.bpp == 32)
	{
		for (int i = 0; i < anh.b.Height; i++)
		{
			for (int j = 0; j < (anh.b.Width * (anh.b.bpp/8)); j++) //tương tự 24 bpp
			{
				if (j % 4 == 3) //vì 1 pixel của nó lưu 4 byte theo thứ tự A,B,G,R mà t chỉ cần lấy trung binh cộng của B,G,R nên phải bỏ qua byte đầu của mỗi bộ số
				{				//vd bộ số 0,1,2,3 thì ta chỉ lấy 1,2,3 nên số cuối cùng của bộ phải chia 3 dư 4
					anh8bpp.data[k++] = anh.data[i * (anh.b.Width * (anh.b.bpp / 8) + padding) + j - 2] / 3 + anh.data[i * (anh.b.Width * (anh.b.bpp / 8) + padding) + j - 1] / 3 + anh.data[i * (anh.b.Width * (anh.b.bpp / 8) + padding) + j] / 3;
				}
			}
			k += padding; //tương tự 24 bpp
		}
	}
	ghianh(fileoutput, anh8bpp);
	delete[] anh8bpp.data;
}
void chuyendatasangpixel(AnhBMP anh, mau*& pixel) //ta chuyển dạng các byte trong data thành các màu trong 1 pixel
{
	pixel = new mau[anh.b.Height * anh.b.Width]; //số pixel sẽ bằng số pixel trên hàng( chiều rộng) * số pixel trên cột( chiều dài)
	int padding = 4 - ((anh.b.Width * (anh.b.bpp / 8)) % 4);
	if (padding == 4)
	{
		padding = 0;
	}
	if (anh.b.bpp == 24)
	{
		int k = 0; //biến k để đếm và truy xuất đến các phần tử của data
		for (int i = 0; i < anh.b.Height; i++)
		{
			for (int j = 0; j < anh.b.Width; j++)
			{
				pixel[i * anh.b.Width + j].B = anh.data[k++];
				pixel[i * anh.b.Width + j].G = anh.data[k++];
				pixel[i * anh.b.Width + j].R = anh.data[k++];
			}
			k += padding; //bỏ qua phần padding
		}
	}
	if (anh.b.bpp == 32)
	{
		int k = 0;//biến k để đếm và truy xuất đến các phần tử của data
		for (int i = 0; i < anh.b.Height; i++)
		{
			for (int j = 0; j < anh.b.Width; j++)
			{
				pixel[i * anh.b.Width + j].A = anh.data[k++];
				pixel[i * anh.b.Width + j].B = anh.data[k++];
				pixel[i * anh.b.Width + j].G = anh.data[k++];
				pixel[i * anh.b.Width + j].R = anh.data[k++];
			}
			k += padding;// bỏ qua phần padding
		}
	}
}
void chuyentupixelsangdata(AnhBMP anh, mau* pixel)// chuyển lại các pixel sang dạng các byte data
{
	int padding = 4 - ((anh.b.Width * (anh.b.bpp / 8)) % 4);
	if (padding == 4)
	{
		padding = 0;
	}
	int k = 0; //biến k để đếm và truy xuất đến các phần tử của data
	if (anh.b.bpp == 24)
	{
		for (int i = 0; i < anh.b.Height; i++)
		{
			for (int j = 0; j < anh.b.Width; j++)
			{
				anh.data[k++] = pixel[i * anh.b.Width + j].B;
				anh.data[k++] = pixel[i * anh.b.Width + j].G;
				anh.data[k++] = pixel[i * anh.b.Width + j].R;
			}
			k += padding;// bỏ qua và không truy xuất đến phần padding
		}
	}
	if (anh.b.bpp == 32)
	{
		for (int i = 0; i < anh.b.Height; i++)
		{
			for (int j = 0; j < anh.b.Width; j++)
			{
				anh.data[k++] = pixel[i * anh.b.Width + j].A;
				anh.data[k++] = pixel[i * anh.b.Width + j].B;
				anh.data[k++] = pixel[i * anh.b.Width + j].G;
				anh.data[k++] = pixel[i * anh.b.Width + j].R;
			}
			k += padding;// bỏ qua và không truy xuất đến phần padding
		}
	}
}
void resize(AnhBMP anh, int s, char* fileoutput)
{
	AnhBMP anhresize;
	anhresize.a = anh.a; //khi resize ảnh thì chỉnh có phần Width, Height và imagesize thay đổi nên chỉ cần đổi thông số 3 cái này
	anhresize.b = anh.b;
	anhresize.phandu = anh.phandu; 
	anhresize.b.Height = anh.b.Height / s;
	anhresize.b.Width = anh.b.Width / s;
	int padding = 4 - ((anhresize.b.Width * (anhresize.b.bpp / 8)) % 4);
	if (padding == 4)
	{
		padding = 0;
	}
	anhresize.b.imagesize = anhresize.b.Height * (anhresize.b.Width * (anhresize.b.bpp / 8) + padding);
	anhresize.data = new unsigned char[anhresize.b.imagesize];
	mau* pixel;
	mau* resizemau = new mau[anhresize.b.Height * anhresize.b.Width]; //ta tạo một pixel mới để lưu pixel của ảnh resize
	chuyendatasangpixel(anh, pixel); //chuyển sang dạng pixel để dễ làm 
	int sodu1 = anh.b.Height % s; //vì ta sẽ chia các ảnh thành các ô SxS nên sẽ có một số pixel dư, nên ta sẽ tính phần dư để lưu xuống sau này 
	int sodu2 = anh.b.Width % s;
	int k = 0;
	if (anh.b.bpp == 8)
	{
		for (int i = 0; i < (anh.b.Height - sodu1); i = i + s)
		{
			for (int j = 0; j < (anh.b.Width - sodu2); j = j + s)
			{
				int avaX = 0;
				for (int l = 0; l < s; l++)  // vì các ô SxS sẽ là mảng 2 chiều có cột và hàng =S nên để truy xuất đến từng phần tử thì	mình sẽ cho chạy 2 vòng lặp phụ
				{
					for (int h = 0; h < s; h++)
					{
						avaX += anh.data[(i + l) * anh.b.Width + j + h];
					}
				}
				anhresize.data[k++] = avaX / (s * s);
			}
		}
	}
	if (anh.b.bpp == 24)
	{
		for (int i = 0; i < (anh.b.Height-sodu1); i = i + s)
		{
			for (int j = 0; j < (anh.b.Width-sodu2); j = j + s)
			{
				int avaB = 0;
				int avaG = 0;
				int avaR = 0;
				for (int l = 0; l < s; l++)  // vì các ô SxS sẽ là mảng 2 chiều có cột và hàng =S nên để truy xuất đến từng phần tử thì	mình sẽ cho chạy 2 vòng lặp phụ
				{
					for (int h = 0; h < s; h++)
					{
						avaB += pixel[(i + l) * anh.b.Width + (j + h)].B; //tính tổng các giá trị màu trong ô SxS
						avaG += pixel[(i + l) * anh.b.Width + (j + h)].G;
						avaR += pixel[(i + l) * anh.b.Width + (j + h)].R;
					}
				}
				resizemau[k].B = avaB / (s * s);  //chia cho S*S phần tử để lấy trung bình cộng
				resizemau[k].G = avaG / (s * s);
				resizemau[k++].R = avaR / (s * s);
			}
		}
	}
	if (anh.b.bpp == 32)
	{
		for (int i = 0; i < anh.b.Height-sodu1; i = i + s)
		{
			for (int j = 0; j < anh.b.Width-sodu2; j = j + s)
			{
				int avaA = 0;
				int avaB = 0;
				int avaG = 0;
				int avaR = 0;
				for (int l = 0; l < s; l++)
				{
					for (int h = 0; h < s; h++)
					{
						avaA += pixel[(i + l) * anh.b.Width + (j + h)].A;
						avaB += pixel[(i + l) * anh.b.Width + (j + h)].B;
						avaG += pixel[(i + l) * anh.b.Width + (j + h)].G;
						avaR += pixel[(i + l) * anh.b.Width + (j + h)].R;
					}
				}
				resizemau[k].A = avaA / (s * s);
				resizemau[k].B = avaB / (s * s);
				resizemau[k].G = avaG / (s * s);
				resizemau[k++].R = avaR / (s * s);
			}
		}
	}
	if (anh.b.bpp != 8)  //ảnh 8bpp thì chúng ta đã gán thẳng data ở trên nên không cần chuyển
	{
		chuyentupixelsangdata(anhresize, resizemau);
	}
	ghianh(fileoutput, anhresize);
	delete[] pixel;
	delete[] resizemau;
	delete[] anhresize.data;
}


