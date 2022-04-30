#include"bmp.h"
int main(int argc,char* argv[])
{
	if (argc != 4 && argc != 5)
	{
		cout << "Nhap so luong tham so dong lenh sai"<<endl;
	}
	else
	{
		if (strcmp(argv[1], "-conv") == 0 && argc==4)
		{
			AnhBMP anh;
			docanh(argv[2], anh);
			chuyensang8bpp(argv[3], anh);
			delete[] anh.data;
		}
		else if (strcmp(argv[1], "-zoom") == 0 && argc == 5)
		{
			AnhBMP anh;
			docanh(argv[2], anh);
			int s = atoi(argv[4]);
			resize(anh, s, argv[3]);
			delete[] anh.data;
		}
		else
		{
			cout << "Nhap sai tham so dong lenh";
		}
	}
	return 0;
}