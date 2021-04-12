#pragma once
#include <Windows.h>
#include <string>
namespace CPrg {
	enum FORMAT {
		LINEAR_3 = 0,            //0.000
		LINEAR_2 = 1,            //0.00
		LINEAR_1 = 2,            //0.0
		CIRCLE_3 = 3,            //0.000°
		CIRCLE_2 = 4,            //0.00°
		CIRCLE_1 = 5,            //0.0°
		LINEAR_4 = 6,            //0.0000
		CIRCLE_4 = 7,            //0.0000°
		CIRCLE_SECUNDO = 8,      //0°00'00"
		LINEAR_INCH_4 = 9,       //0.0000"
		CIRCLE_MINUTO = 10,      //0°00'
		LINEAR_5 = 11,         //0.00000	
		LINEAR_6 = 12,         //0.000000
		LINEAR_7 = 13,         //0.0000000
		LINEAR_8 = 14,         //0.00000000
		CIRCLE_SECUNDO_1 = 15, //0°00'00.0"
		FORMAT_END = 16,
	};
}


enum MULT_OR_DIV_ANSWER {
	OK = (unsigned char)0,
	N_AFTER_DOT_TOO_BIG_MULT = (unsigned char)1,
	RESULT_IS_TOO_BIG_MULT = (unsigned char)2,
	N_AFTER_DOT_TOO_BIG_DIV = (unsigned char)3,
	RESULT_IS_TOO_BIG_DIV = (unsigned char)4,
	COEFF_IS_ZERO_DIV = (unsigned char)5,
	NO_CHANNEL = (unsigned char)6,
};

#define N_AXIS 4
class Core
{
public:
	Core();
	MULT_OR_DIV_ANSWER answer;
	struct COEFF_NEW {
		long binary;
		char n_after_dot;
	} new_coeff_[N_AXIS], correction;
	long data;
	char n_after_dot;
	long conv_c(long tmp, int i, char type, char no_endless_circle);
	MULT_OR_DIV_ANSWER mult(unsigned char n_axis, long* _data, unsigned char n_a_d_in = 0, long bin_in = 0);
	bool translate_(char* src, char size_src);
	void set_axis_coeff(unsigned char n_channel, long bin, unsigned char dot);
	void convert_channel_counter_to_text_string(unsigned char n_ch, long counter, unsigned char* txt_out, int f);
	void formatting2(char* txt_out, int f);
	bool delete_sym(unsigned char* str, const short sz_str, const char& sym_);
	void add_filler(unsigned char* str, const short sz_str, const short cnt, const char filler, const char dir = 0);
	void insert_sym(unsigned char* str, const short sz_str, const short place, const char& sym_);
	int strnlen_lir(const char* str, int maxcount = 50);
	int strncatsym_lir(char* str1, const int& sz_str1, char sym_);
	void print_debug_one_local_with_dot(HWND cntr, long debug_a, char n_after_dot, unsigned char page, unsigned int x_, unsigned char y_, char align = 0, char clear = 0);
	int putdotinstring2(char* _Source, int n_after_dot, int numberofsymbol);
	int strncpy_lir(char* Dest, const char* Source, int maxcount = 50);
	bool is_linear(int F);
	std::string conv_spd(long speed_0, int F);
	void to_convert_long_to_string_for_speed(long* data_to_conv, unsigned char* txt_out, int F);
};
extern Core Lir;

