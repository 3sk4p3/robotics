//---------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

// Szkielet kodu dla wykorzystania biblioteki  ParallelPortInterface_B.dll
// Nalezy dodac do projektu plik ParallelPortInterface_B.lib
// Nalezy dopisac ponizszy blok "Deklaracje funkcji importowanych ....  ------
// -------------  Deklaracje funkcji importowanych z pliku DLL --------------
extern "C" __declspec(dllimport) unsigned char inportb(unsigned short port);

extern "C" __declspec(dllimport) void outportb(unsigned short port, unsigned char val);

extern "C" __declspec(dllimport) void ParallelPortDestroyFormDLL();

extern "C" __declspec(dllimport) AnsiString about( int &parametr, int &error);

#define Data_Register 0x378
#define Control_Register 0x37A

template<typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& orig)
{
	std::vector<T> ret;
	for (const auto& v : orig)
		ret.insert(ret.end(), v.begin(), v.end());
	return ret;
}

std::vector<std::vector<int>> readAndProcessBitmap(const char* file_name, size_t width, size_t height) {
	std::fstream File(file_name);
	if (!File.is_open()) {
		std::cerr << "file " << file_name << " doesn't exist\n";
		exit(EXIT_FAILURE);
	}

	std::vector<std::vector<int>> bitmap(height, std::vector<int>(width));
	for (size_t i = 0; i < height; ++i) {
		for (size_t j = 0; j < width; ++j) {
			File >> bitmap[i][j];
		}
	}

	std::vector<std::vector<int>> values(height / 8, std::vector<int>(width));
	for (size_t page = 0; page < height / 8; ++page) {
		for (size_t col = 0; col < width; ++col) {
			int sum = 0;
			for (size_t row = page * 8; row < (page + 1) * 8; ++row) {
				if (bitmap[row][col]) {
					sum |= (1 << (row % 8));
				}
			}
			values[page][col] = sum;
		}
	}

	return flatten(values);
}

void outputBitmapAsHex(const std::vector<std::vector<int>>& values) {
	std::cout << "static const char bitmapData[] = {" << std::endl;
	for (const auto& row : values) {
		for (const auto& val : row) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << val << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << "};" << std::endl;
}

//-- koniec deklaracji --------------------------------------------------------

const char* file_name = "bitmapa.txt";
std::vector<int>Bitmap = readAndProcessBitmap(file_name, 64, 64);

static const char  font5x8[] = {
0x00, 0x00, 0x00, 0x00, 0x00,// (spacja)
0x00, 0x00, 0x5F, 0x00, 0x00,// !
0x00, 0x07, 0x00, 0x07, 0x00,// "
0x14, 0x7F, 0x14, 0x7F, 0x14,// #
0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
0x23, 0x13, 0x08, 0x64, 0x62,// %
0x36, 0x49, 0x55, 0x22, 0x50,// &
0x00, 0x05, 0x03, 0x00, 0x00,// '
0x00, 0x1C, 0x22, 0x41, 0x00,// (
0x00, 0x41, 0x22, 0x1C, 0x00,// )
0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
0x08, 0x08, 0x3E, 0x08, 0x08,// +
0x00, 0x50, 0x30, 0x00, 0x00,// ,
0x08, 0x08, 0x08, 0x08, 0x08,// -
0x00, 0x30, 0x30, 0x00, 0x00,// .
0x20, 0x10, 0x08, 0x04, 0x02,// /
0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
0x00, 0x42, 0x7F, 0x40, 0x00,// 1
0x42, 0x61, 0x51, 0x49, 0x46,// 2
0x21, 0x41, 0x45, 0x4B, 0x31,// 3
0x18, 0x14, 0x12, 0x7F, 0x10,// 4
0x27, 0x45, 0x45, 0x45, 0x39,// 5
0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
0x01, 0x71, 0x09, 0x05, 0x03,// 7
0x36, 0x49, 0x49, 0x49, 0x36,// 8
0x06, 0x49, 0x49, 0x29, 0x1E,// 9
0x00, 0x36, 0x36, 0x00, 0x00,// :
0x00, 0x56, 0x36, 0x00, 0x00,// ;
0x00, 0x08, 0x14, 0x22, 0x41,// <
0x14, 0x14, 0x14, 0x14, 0x14,// =
0x41, 0x22, 0x14, 0x08, 0x00,// >
0x02, 0x01, 0x51, 0x09, 0x06,// ?
0x32, 0x49, 0x79, 0x41, 0x3E,// @
0x7E, 0x11, 0x11, 0x11, 0x7E,// A
0x7F, 0x49, 0x49, 0x49, 0x36,// B
0x3E, 0x41, 0x41, 0x41, 0x22,// C
0x7F, 0x41, 0x41, 0x22, 0x1C,// D
0x7F, 0x49, 0x49, 0x49, 0x41,// E
0x7F, 0x09, 0x09, 0x01, 0x01,// F
0x3E, 0x41, 0x41, 0x51, 0x32,// G
0x7F, 0x08, 0x08, 0x08, 0x7F,// H
0x00, 0x41, 0x7F, 0x41, 0x00,// I
0x20, 0x40, 0x41, 0x3F, 0x01,// J
0x7F, 0x08, 0x14, 0x22, 0x41,// K
0x7F, 0x40, 0x40, 0x40, 0x40,// L
0x7F, 0x02, 0x04, 0x02, 0x7F,// M
0x7F, 0x04, 0x08, 0x10, 0x7F,// N
0x3E, 0x41, 0x41, 0x41, 0x3E,// O
0x7F, 0x09, 0x09, 0x09, 0x06,// P
0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
0x7F, 0x09, 0x19, 0x29, 0x46,// R
0x46, 0x49, 0x49, 0x49, 0x31,// S
0x01, 0x01, 0x7F, 0x01, 0x01,// T
0x3F, 0x40, 0x40, 0x40, 0x3F,// U
0x1F, 0x20, 0x40, 0x20, 0x1F,// V
0x7F, 0x20, 0x18, 0x20, 0x7F,// W
0x63, 0x14, 0x08, 0x14, 0x63,// X
0x03, 0x04, 0x78, 0x04, 0x03,// Y
0x61, 0x51, 0x49, 0x45, 0x43,// Z
0x00, 0x00, 0x7F, 0x41, 0x41,// [
0x02, 0x04, 0x08, 0x10, 0x20,// "\"
0x41, 0x41, 0x7F, 0x00, 0x00,// ]
0x04, 0x02, 0x01, 0x02, 0x04,// ^
0x40, 0x40, 0x40, 0x40, 0x40,// _
0x00, 0x01, 0x02, 0x04, 0x00,// `
0x20, 0x54, 0x54, 0x54, 0x78,// a
0x7F, 0x48, 0x44, 0x44, 0x38,// b
0x38, 0x44, 0x44, 0x44, 0x20,// c
0x38, 0x44, 0x44, 0x48, 0x7F,// d
0x38, 0x54, 0x54, 0x54, 0x18,// e
0x08, 0x7E, 0x09, 0x01, 0x02,// f
0x08, 0x14, 0x54, 0x54, 0x3C,// g
0x7F, 0x08, 0x04, 0x04, 0x78,// h
0x00, 0x44, 0x7D, 0x40, 0x00,// i
0x20, 0x40, 0x44, 0x3D, 0x00,// j
0x00, 0x7F, 0x10, 0x28, 0x44,// k
0x00, 0x41, 0x7F, 0x40, 0x00,// l
0x7C, 0x04, 0x18, 0x04, 0x78,// m
0x7C, 0x08, 0x04, 0x04, 0x78,// n
0x38, 0x44, 0x44, 0x44, 0x38,// o
0x7C, 0x14, 0x14, 0x14, 0x08,// p
0x08, 0x14, 0x14, 0x18, 0x7C,// q
0x7C, 0x08, 0x04, 0x04, 0x08,// r
0x48, 0x54, 0x54, 0x54, 0x20,// s
0x04, 0x3F, 0x44, 0x40, 0x20,// t
0x3C, 0x40, 0x40, 0x20, 0x7C,// u
0x1C, 0x20, 0x40, 0x20, 0x1C,// v
0x3C, 0x40, 0x30, 0x40, 0x3C,// w
0x44, 0x28, 0x10, 0x28, 0x44,// x
0x0C, 0x50, 0x50, 0x50, 0x3C,// y
0x44, 0x64, 0x54, 0x4C, 0x44,// z
0x00, 0x08, 0x36, 0x41, 0x00,// {
0x00, 0x00, 0x7F, 0x00, 0x00,// |
0x00, 0x41, 0x36, 0x08, 0x00,// }
0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};
//

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)    // Constructor
	: TForm(Owner)
{
   Left = 200; Top = 200;

}
//---------------------------------------------------------------------------

__fastcall TForm1::~TForm1() // Destructor
{
  ParallelPortDestroyFormDLL();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  unsigned char data;
  try
  {
	data = StrToInt( Edit1->Text );
  }
  catch(...)
  {
	ShowMessage( "Dana niepoprawna" );
	return;
  }

  outportb( Data_Register, data);

}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button2Click(TObject *Sender)
{
  Edit2->Text = IntToStr( inportb(Data_Register) );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	outportb( Data_Register, 63);
	outportb( Control_Register, 13);
	outportb( Control_Register, 12);
	outportb( Control_Register, 13);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
	outportb( Data_Register, 62);
	outportb( Control_Register, 13);
	outportb( Control_Register, 12);
	outportb( Control_Register, 13);
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button5Click(TObject *Sender)
{
	unsigned char data;
	try
	{
        data = StrToInt( Edit3->Text );
    }
	catch(...)
	{
		ShowMessage( "Dana niepoprawna" );
		return;
	}
	data = data + 64;
	outportb( Data_Register, data);
	outportb( Control_Register, 13);
	outportb( Control_Register, 12);
	outportb( Control_Register, 13);


	try
	{
		data = StrToInt( Edit4->Text );
    }
	catch(...)
	{
		ShowMessage( "Dana niepoprawna" );
		return;
	}
	data = data + 184;
	outportb( Data_Register, data);
	outportb( Control_Register, 13);
	outportb( Control_Register, 12);
	outportb( Control_Register, 13);


	try
	{
        data = StrToInt( Edit5->Text );
    }
	catch(...)
	{
		ShowMessage( "Dana niepoprawna" );
		return;
	}
	data = data + 192;
	outportb( Data_Register, data);
	outportb( Control_Register, 13);
	outportb( Control_Register, 12);
	outportb( Control_Register, 13);


	try
	{
        data = StrToInt( Edit6->Text );
	}
	catch(...)
	{
		ShowMessage( "Dana niepoprawna" );
		return;
	}
	/*
	outportb( Data_Register, data);
	outportb( Control_Register, 13);
	outportb( Control_Register, 12);
	outportb( Control_Register, 13);
	*/
	outportb( Data_Register, data);
	outportb( Control_Register, 5);
	outportb( Control_Register, 4);
	outportb( Control_Register, 5);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button6Click(TObject *Sender)
{
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<64;j++)
		{
			//Y
			outportb( Data_Register, j+64);
			outportb( Control_Register, 13);
			outportb( Control_Register, 12);
			outportb( Control_Register, 13);

			//X
			outportb( Data_Register, i+184);
			outportb( Control_Register, 13);
			outportb( Control_Register, 12);
			outportb( Control_Register, 13);

			//LINE
			outportb( Data_Register, j+192);
			outportb( Control_Register, 13);
			outportb( Control_Register, 12);
			outportb( Control_Register, 13);

			//DATA
			outportb( Data_Register, 0);
			outportb( Control_Register, 5);
			outportb( Control_Register, 4);
			outportb( Control_Register, 5);
		}
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button7Click(TObject *Sender)
{
int k = 0;
int Y = 0;
	for(int i=0;i<300;i++)
	{
			unsigned char data;
			//Y
			data = Y + 64;
			outportb( Data_Register, data);
			outportb( Control_Register, 13);
			outportb( Control_Register, 12);
			outportb( Control_Register, 13);
			Y++;
			if(Y==63) Y = 0;
			//X
			data = k + 184;
			outportb( Data_Register, data);
			outportb( Control_Register, 13);
			outportb( Control_Register, 12);
			outportb( Control_Register, 13);
			if(i==63 || i==126 || i==189 || i==252) k = k+1;
			//LINE
			data = 0 + 192;
			outportb( Data_Register, data);
			outportb( Control_Register, 13);
			outportb( Control_Register, 12);
			outportb( Control_Register, 13);

			//DATA
			outportb( Data_Register, font5x8[i]);
			outportb( Control_Register, 5);
			outportb( Control_Register, 4);
			outportb( Control_Register, 5);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject* Sender)
{



	for (int page = 0; page < 8; ++page) // 8 pages high
	{
		
	

		for (int y = 0; y < 64; ++y) // 64 Y-addresses wide
		{


			outportb(Data_Register, 64+y);
			outportb(Control_Register, 13);
			outportb(Control_Register, 12);
			outportb(Control_Register, 13);

			outportb(Data_Register, 184 + page);
			outportb(Control_Register, 13);
			outportb(Control_Register, 12);
			outportb(Control_Register, 13);
			
			//????
		/*	outportb(Data_Register, 192);
			outportb(Control_Register, 13);
			outportb(Control_Register, 12);
			outportb(Control_Register, 13);*/

			outportb(Data_Register, Bitmap[page*y+y]);
			outportb(Control_Register, 5);
			outportb(Control_Register, 4);
			outportb(Control_Register, 5);
		}
	}
}
