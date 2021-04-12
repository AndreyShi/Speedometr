#include "Core.h"
#include <iostream>
#pragma warning( disable : 4091 )
#pragma warning( disable : 4996 )
Core::Core()
	:data(1),
	n_after_dot(0)
{
	for (int c = 0; c < N_AXIS; c++) {
		new_coeff_[c].binary = 1;
		new_coeff_[c].n_after_dot = 0;
	}
}
long Core::conv_c(long tmp, int i, char type, char no_endless_circle)
{
	mult(i, &tmp);
	//if ((init.mRef_ext[i] && !type) || (ref_.pRef[i] && type)) {
	//	if (MsgKey.offset_number == 0 || type)// type для программных концевиков только в зоне G53
	//		tmp = display.offset_g53[i] + Lir.size_2r2[i] + tmp;
	//	else
	//		tmp = display.offset_g53[i] + Lir.size_2r2[i] + tmp - display.offset_g5x[MsgKey.offset_number - 1][i];
	//}
	//if (flash_prg.axis_cfg[i].r_d == CPrg::R_D::DIAMETER)
	//	tmp = tmp * 2;
	//if (flash_prg.axis_cfg[i].format == CPrg::FORMAT::LINEAR_INCH_4)
	//	coeff.div(255, &tmp, 1, 254, 1);// делим

	/*if (flash_prg.axis_cfg[i].direction == CPrg::DIRECTION::MINES)
		tmp = -tmp;*/

	/*if (!no_endless_circle) {
		flash_prg.to_format_angle_axis_if_offsets_were_added(&tmp, i);
	}*/

	//if (P2[i] == CInit::AXIS_NAME::AXIS_Z && _modals_cmd.T_instr) //ось с инструментом (к относительной координате или абс?)
	//	tmp = tmp + T_instr[_modals_cmd.T_instr - 1].l; //пока только на длину

	return tmp;
}

MULT_OR_DIV_ANSWER Core::mult(unsigned char n_axis, long* _data, unsigned char n_a_d_in, long bin_in)
{
	// 19.02.2018 worst case it takes 2423 * 31.25 = 75718.75 ns
		long long result, b, c;
	long coeff_, int_res;
	unsigned char e;
	bool nine = false;
	switch (n_axis) {
	case 0:coeff_ = new_coeff_[0].binary;
		e = new_coeff_[0].n_after_dot;
		break;
	case 1:coeff_ = new_coeff_[1].binary;
		e = new_coeff_[1].n_after_dot;
		break;
	case 2:coeff_ = new_coeff_[2].binary;
		e = new_coeff_[2].n_after_dot;
		break;
	case 3:coeff_ = new_coeff_[3].binary;
		e = new_coeff_[3].n_after_dot;
		break;
	case 4:coeff_ = correction.binary;
		e = correction.n_after_dot;
		nine = true;
		break;
	case 255:
		coeff_ = bin_in;
		e = n_a_d_in;
		break;
		//case 5:coeff_=speed_measure.binary;
		//e=speed_measure.n_after_dot;
		//break;
	default:
		return MULT_OR_DIV_ANSWER::NO_CHANNEL;
	}
	//		if(e>7) {answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;return answer;}
	//02.05.2018	
	if (e > 8 && nine == false) {
		answer = MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
		return answer;
	}
	else if (e > 9 && nine == true) {
		answer = MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
		return answer;
	}
	result = *_data;
	result = result * coeff_;
	//to make round...
	if (e != 0) {
		e = e - 1;
		if (result > LONG_MAX || result < LONG_MIN) {
			switch (e)
			{
			case 0: break;
			case 1:result = result / 10; break;
			case 2:result = result / 100; break;
			case 3:result = result / 1000; break;
			case 4:result = result / 10000; break;
			case 5:result = result / 100000; break;
			case 6:result = result / 1000000; break;
			case 7:result = result / 10000000; break;
			case 8:result = result / 100000000; break;
				//  case 9: result=result/1000000000;break;
				//mistake n_a_p operator
			default: {answer = MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
				return answer; }
			}
		}
		else {
			int_res = result;
			switch (e)
			{
			case 0: break;
			case 1:int_res = int_res / 10; break;
			case 2:int_res = int_res / 100; break;
			case 3:int_res = int_res / 1000; break;
			case 4:int_res = int_res / 10000; break;
			case 5:int_res = int_res / 100000; break;
			case 6:int_res = int_res / 1000000; break;
			case 7:int_res = int_res / 10000000; break;
			case 8:int_res = int_res / 100000000; break;
				//  case 9: result=result/1000000000;break;
				//mistake n_a_p operator
			default: {answer = MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
				return answer; }
			}
			result = int_res;
		}
		b = result;
		if (result > LONG_MAX || result < LONG_MIN)
			result = result / 10;
		else {
			int_res = result;
			int_res = int_res / 10;
			result = int_res;
		}
		c = result;
		c = c * 10;
		b = b - c;
		//work with negative...
		//ошибка : первый дискрет в два раза меньше потом нормально, т е происходит смещение 
	   /* if(b<0)
		{
			b=-b;
			if(b>4)
			result--;
		}
		else {
			if(b>4)
			result++;
		}*/
		//===
	}
	if (result > LONG_MAX || result < LONG_MIN) {
		answer = MULT_OR_DIV_ANSWER::RESULT_IS_TOO_BIG_MULT;
		return answer;
	}
	else
		*_data = result;
	return MULT_OR_DIV_ANSWER::OK;

}

bool Core::translate_(char* src, char size_src)
{
	//04.03.2013 it will work only for axis
//if 7 n_a_p : 0.0097789-99.9999999
//if 6 n_a_p : 0.009779-99.999999
//if 5 n_a_p : 0.00978-99.99999
//if 4 n_a_p : 0.0098-99.9999
//if 3 n_a_p : 0.010-99.999
//if 2 n_a_p : 0.01-99.99
//if 1 n_a_p : 0.1-99.9
//if 0 n_a_p : 1-99
//     input check
	long res = 0, ch_;
	char ch, inter[11];
	unsigned char i = 0, p = 0, d = 0, s = 0, m = 0;
	bool dot = false;
	n_after_dot = 0;
	data = 1;
	unsigned char chx;
	for (; d < 11; d++)inter[d] = 0x00;
	d = 0x00;
	if (size_src == 0) { data = 1; return false; }
	//string's  parsing
	while (i < size_src) {
		ch = src[i];
		if (ch == 0x00)break;
		if (ch == 0x2e)dot = true;
		if (dot == false)
		{
			//only digit can be input before dot
			if (ch >= 0x30 && ch <= 0x39) {
				d++;
				//all zero left from dot before first digit is ignored
				if (m == 0x00) {
					if (ch != 0x30)inter[m++] = ch;
					else d--;
				}
				else inter[m++] = ch;
				i++;
			}
			else { data = 1; return false; }
			//only 2 digit can be before dot
			if (d > 2) { data = 1; return false; }
		}
		if (dot == true) {
			if (ch >= 0x30 && ch <= 0x39)
			{
				inter[m++] = ch;
				i++; p++;
				//04.05.2018		  
				//only 7 digit after dot is allowed if 10.-99.
				//only 8 digit up to 10.
				if (p >= 0x09) { inter[--m] = 0x00; p--; break; }
			}
			else {
				if (ch == 0x2e) { s++; i++; }
				//only one dot is allowed
				if (s >= 0x02) { data = 1; return false; }
				//if another letter is coming
				if (ch != 0x2e) { data = 1; return false; }
			}
		}
	}
	//d is number of digit before dot
	//p is number digit behind dot
	// inter[] = ddppppppp
	// 99 9999999 = 0x 3b 9a c9 ff
	// total string's size
	i = d + p;
	//look for last digit to the right
	for (m = 0, s = d; s < i + 1; s++)
	{
		if (inter[s] == 0x30)m++;
		if (inter[s] == 0x00)break;
	}
	//it means this .0000000
	if (m == p)p = 0;
	//it must be resolved  .0010100
	if (p > 0) {
		p++;
		for (; s != 0x00;)
		{
			chx = inter[s--];
			//   if(chx!=0x30&&chx!=0x00)break;
			//   else p--;
			if (chx == 0x30 || chx == 0x00)p--;
			if (chx == 0x31 || chx == 0x32 || chx == 0x33 || chx == 0x34 || chx == 0x35\
				|| chx == 0x36 || chx == 0x37 || chx == 0x38 || chx == 0x39) break;
		}
	}
	// total string's size
	i = d + p;
	if (d == 0x00 && p == 0x00)
	{
		data = 1; return false;
	}
	//to take binary from string...
	for (s = 0; i != 0x00; i--, s++) {
		ch = inter[i - 1] - 0x30;
		ch_ = ch;
		switch (s) {
		case 0x00:res = res + ch_ * 1; break;
		case 0x01:res = res + ch_ * 10; break;
		case 0x02:res = res + ch_ * 100; break;
		case 0x03:res = res + ch_ * 1000; break;
		case 0x04:res = res + ch_ * 10000; break;
		case 0x05:res = res + ch_ * 100000; break;
		case 0x06:res = res + ch_ * 1000000; break;
		case 0x07:res = res + ch_ * 10000000; break;
		case 0x08:res = res + ch_ * 100000000; break;
		default:break;
		}
	}
	//limitation check
		 /* switch(p){
		  case 0x08: if(res<5000001)res=5000001;break;
		  case 0x07: if (res < 500001)res=500001;break;
		  case 0x06: if (res < 50001) res=50001;break;
		  case 0x05: if (res < 5001)  res=5001;break;
		  case 0x04: if (res < 501)   res=501;break;
		  case 0x03: if (res <= 49)  { res=5;p=2;}break;
		  case 0x02: if (res <= 4)  { res=5;p=2;}break;
		  default: break;
		  } */

	data = res;
	n_after_dot = p;
	return true;
}

void Core::set_axis_coeff(unsigned char n_channel, long bin, unsigned char dot)
{
	switch (n_channel) {
	case 0:new_coeff_[0].binary = bin; new_coeff_[0].n_after_dot = dot; break;
	case 1:new_coeff_[1].binary = bin; new_coeff_[1].n_after_dot = dot; break;
	case 2:new_coeff_[2].binary = bin; new_coeff_[2].n_after_dot = dot; break;
	case 3:new_coeff_[3].binary = bin; new_coeff_[3].n_after_dot = dot; break;
	case 4:correction.binary = bin; correction.n_after_dot = dot; break;
		//case 5:speed_measure.binary=bin;speed_measure.n_after_dot=dot;break;
	}
}

void Core::convert_channel_counter_to_text_string(unsigned char n_ch, long counter, unsigned char* txt_out,int f)
{
	if ((f >= CPrg::FORMAT::LINEAR_3 && f <= CPrg::FORMAT::CIRCLE_4) ||
		f == CPrg::FORMAT::LINEAR_INCH_4 ||
		f == CPrg::FORMAT::LINEAR_6)
	{
		std::sprintf((char*)txt_out, "%d", counter);//itoa_quick_lir(&counter, txt_out);
		formatting2((char*)txt_out, f);
	}
	/*if (f == CPrg::FORMAT::CIRCLE_SECUNDO || f == CPrg::FORMAT::CIRCLE_MINUTO || f == CPrg::FORMAT::CIRCLE_SECUNDO_1)
	{
		convert_long_to_secundo2(counter, txt_out, f);
	}*/
}

void Core::formatting2(char* txt_out, int f)
{
	int16_t cnt_zero;
	bool minus = false;

	if (f == CPrg::FORMAT::LINEAR_1 || f == CPrg::FORMAT::CIRCLE_1)
		cnt_zero = 1;
	else if (f == CPrg::FORMAT::LINEAR_2 || f == CPrg::FORMAT::CIRCLE_2)
		cnt_zero = 2;
	else if (f == CPrg::FORMAT::LINEAR_3 || f == CPrg::FORMAT::CIRCLE_3)
		cnt_zero = 3;
	else if (f == CPrg::FORMAT::LINEAR_4 || f == CPrg::FORMAT::CIRCLE_4 || f == CPrg::FORMAT::LINEAR_INCH_4)
		cnt_zero = 4;
	else if (f == CPrg::FORMAT::LINEAR_6)
		cnt_zero = 6;
	else
		cnt_zero = 0;

	if (delete_sym((uint8_t*)txt_out, 15, '-'))
		minus = true;
	add_filler((uint8_t*)txt_out, 15, cnt_zero, '0');
	insert_sym((uint8_t*)txt_out, 15, cnt_zero, '.');

	if (minus)
		insert_sym((uint8_t*)txt_out, 15, strnlen_lir((char*)txt_out), '-');

	if (f == CPrg::FORMAT::CIRCLE_1 || f == CPrg::FORMAT::CIRCLE_2 || f == CPrg::FORMAT::CIRCLE_3 || f == CPrg::FORMAT::CIRCLE_4)
		strncatsym_lir((char*)txt_out, 15, '°');
	else if (f == CPrg::FORMAT::LINEAR_INCH_4)
		strncatsym_lir((char*)txt_out, 15, '\"');//0x02

	return;
}

bool Core::delete_sym(unsigned char* str, const short sz_str, const char& sym_)
{
	for (int i = 0; str[i] != '\0'; i++) {
		if (sz_str - 1 <= i)
			break;
		if (str[i] == sym_) {
			for (int j = i; str[j] != '\0'; j++) {
				if (sz_str - 1 <= j)
					break;
				str[j] = str[j + 1];
			}
			return true;
		}
	}
	return false;
}

void Core::add_filler(unsigned char* str, const short sz_str, const short cnt, const char filler, const char dir)
{
	signed char n = strnlen_lir((char*)str);
	if (!n) {
		for (int i = 0; i < cnt + 1; i++) {
			str[i] = filler;
			if (i == (sz_str - 1)) {
				str[i] = '\0';
				break;
			}
		}
		return;
	}

	if (dir) {
		if (cnt >= n) {
			for (int i = n - 1; i < cnt; ) {
				if (n + 1 >= sz_str)
					break;
				str[i + 1] = filler;
				i = n++;
			}
			str[n] = '\0';
		}
	}
	else {
		if (cnt >= n) {
			for (int i = n - 1; i < cnt; ) {
				if (n + 1 >= sz_str)
					break;
				str[i + 1] = str[i];
				str[i] = filler;
				if (i)
					i--;
				else
					i = n++;
			}
			str[n] = '\0';
		}
	}
}

void Core::insert_sym(unsigned char* str, const short sz_str, const short place, const char& sym_)
{
	char n = strnlen_lir((char*)str, sz_str);
	//if (n) {
	if (n + 1 < sz_str) {
		int i = n;
		for (int j = 0; i >= 0; i--) {
			str[i + 1] = str[i];
			if (j == place) {
				str[i] = sym_;
				break;
			}
			j++;
		}
	}
	//}
}


/*
функция считает размер строки без учета завершающего символа '\0'
т.к. счет начинается с нуля
*/
int Core::strnlen_lir(const char* str, int maxcount)
{
	int x = 0;
	for (; x < maxcount; x++)
	{
		if (str[x] == 0x00)
			break;
	}
	return x;
}

/*
функция конкатенирует один символ в конец строки 1
out char * str1 - указатель на первую строку
in  const int & sz_str1 - размер 1ой строки в байтах
in  char sym_ - ASCII код символа
*/
int Core::strncatsym_lir(char* str1, const int& sz_str1, char sym_)
{
	for (int i = 0; i < sz_str1 - 1; i++)
	{
		if (str1[i] == '\0')
		{
			str1[i] = sym_;
			str1[i + 1] = '\0';
			return 1;
		}
	}
	return 0;
}

void Core::print_debug_one_local_with_dot(HWND cntr, long debug_a, char n_after_dot, unsigned char page, unsigned int x_, unsigned char y_, char align, char clear)
{
	int  i;
	unsigned char str[15];

	std::sprintf((char*)str, "%d", debug_a);
	i = strnlen_lir((const char*)str);
	putdotinstring2((char*)str, n_after_dot, i);
	SendMessageA(cntr, WM_SETTEXT, NULL, (LPARAM)str);
}

int Core::putdotinstring2(char* str, int n_after_dot, int n)
{
	if (n_after_dot == 0) return 0;

	int x = 0;
	int f = n - n_after_dot;

	int a = 0;
	int b = 0;
	if (f == -4) a = 6;
	if (f == -3) a = 5;
	if (f == -2) a = 4;
	if (f == -1) a = 3;
	if (f == 0)  a = 2;
	if (f == 1)  a = 1;
	if (f == 2)  a = 0;
	if (f == 3) { a = 0; b = 1; }
	if (f == 4) { a = 0; b = 2; }
	if (f == 5) { a = 0; b = 3; }
	if (f == 6) { a = 0; b = 4; }

	char buf[15] = { 0 };
	if (f > 0)
	{
		x = n;
		while (x)
		{
			str[f + x] = str[f + x - 1];
			x--;
		}
		str[f] = '.';
	}
	if (f <= 0)
	{
		buf[0] = '0';
		buf[1] = '.';
		for (int d = 2; d < a; d++)
			buf[d] = '0';

		for (int x = 0; x < n; x++)
		{
			buf[x + a] = str[x + b];
		}
		strncpy_lir(str, buf, 13);
	}
	return 1;
}

int Core::strncpy_lir(char* Dest, const char* Source, int maxcount)
{
	for (int x = 0; x < maxcount; x++)
	{
		Dest[x] = Source[x];
		if (Source[x] == 0x00)
			break;
	}
	return 1;
}

bool Core::is_linear(int F)
{
	if (F == CPrg::FORMAT::LINEAR_1 || F == CPrg::FORMAT::LINEAR_2 || F == CPrg::FORMAT::LINEAR_3
		|| F == CPrg::FORMAT::LINEAR_4 || F == CPrg::FORMAT::LINEAR_INCH_4 == F)
		return true;
	else
		return false;
}

std::string Core::conv_spd(long speed_0,int F)
{
	bool get_off_dot = false;
	bool inches = false;
	std::string mm_string(17, 0);
		switch (F) {
		case CPrg::FORMAT::LINEAR_3:
			break;
		case CPrg::FORMAT::LINEAR_2:
			speed_0 = speed_0 * 10;
			get_off_dot = true;
			break;
		case CPrg::FORMAT::LINEAR_1:
			speed_0 = speed_0 * 100;
			get_off_dot = true;
			break;
		case CPrg::FORMAT::LINEAR_4:
			break;
		case CPrg::FORMAT::LINEAR_INCH_4:
			inches = true;
			break;
		case CPrg::FORMAT::CIRCLE_3:
			speed_0 = (speed_0 * 10) / 36;
			break;
		case CPrg::FORMAT::CIRCLE_2:
			speed_0 = (speed_0 * 10) / 36;
			break;
		case CPrg::FORMAT::CIRCLE_1:
			speed_0 = (speed_0 * 10) / 36;
			break;
		case CPrg::FORMAT::CIRCLE_4:
			speed_0 = (speed_0 * 10) / 36;
			break;
		case CPrg::FORMAT::CIRCLE_SECUNDO:
			speed_0 = (speed_0 * 100) / 1296;
			break;
		case CPrg::FORMAT::CIRCLE_MINUTO:
			speed_0 = (speed_0 * 100) / 216;
			break;
		}

		mult(0, &speed_0);

		to_convert_long_to_string_for_speed(&speed_0, (unsigned char*)mm_string.c_str(), F);

		for (int i = 0; i < 17; i++) {
			if (mm_string[i] == '\0') {
				if (get_off_dot) {
					if (speed_0 > 9999999 || speed_0 < -9999999)
						i = i - 3;
					else
						i = i - 2;
				}
				if (inches) {
					mm_string[i] = 0x02;
					i++;
				}
				for (; i < 17; i++) {
					mm_string[i] = 0x20;
				}
				mm_string[16] = '\0';
				break;
			}

		}

		return mm_string;
}

void Core::to_convert_long_to_string_for_speed(long* data_to_conv, unsigned char* txt_out, int F)
{
	int f;
	bool linear = false;
	switch (F) {
	case CPrg::FORMAT::LINEAR_1:
	case CPrg::FORMAT::LINEAR_2:
	case CPrg::FORMAT::LINEAR_3:
		f = (int)CPrg::FORMAT::LINEAR_1;
		linear = true;
		break;
	case CPrg::FORMAT::LINEAR_4:
		f = (int)CPrg::FORMAT::LINEAR_2;
		linear = true;
		break;
	case CPrg::FORMAT::LINEAR_INCH_4:
		//coeff.div(255, data_to_conv, 1, 254, 1);
		f = (int)CPrg::FORMAT::LINEAR_2;
		linear = true;
		break;
	case CPrg::FORMAT::CIRCLE_3:
		f = (int)CPrg::FORMAT::LINEAR_3;
		break;
	case CPrg::FORMAT::CIRCLE_4:
		f = (int)CPrg::FORMAT::LINEAR_4;
		break;
	case CPrg::FORMAT::CIRCLE_2:
		f = (int)CPrg::FORMAT::LINEAR_2;
		break;
	case CPrg::FORMAT::CIRCLE_1:
		f = (int)CPrg::FORMAT::LINEAR_1;
		break;
	case CPrg::FORMAT::CIRCLE_SECUNDO:
		f = (int)CPrg::FORMAT::LINEAR_3;
		break;
	case CPrg::FORMAT::CIRCLE_MINUTO:
		f = (int)CPrg::FORMAT::LINEAR_2;
		break;

	default: f = (int)CPrg::FORMAT::LINEAR_1;
	}
	/*if (linear)
		txt_out[0] = 'F';
	else
		txt_out[0] = 'S';
	txt_out[1] = '=';*/

	std::sprintf((char*)txt_out, "%d", *data_to_conv);//itoa_quick_lir(data_to_conv, txt_out);
	formatting2((char*)txt_out, f);
}
