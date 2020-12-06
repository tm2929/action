#pragma once

class Encoding
{
public:
	// UTF-8 to UTF-16 �ϊ�
	static void UTF8ToUTF16(const char* utf8, wchar_t* utf16, int utf16_size);

	// UTF-16 to UTF-8 �ϊ�
	static void UTF16ToUTF8(const wchar_t* utf16, char* utf8, int utf8_size);

	// ���e�������� to UTF-16 �ϊ�
	static void StringToUTF16(const char* string, wchar_t* utf16, int utf16_size);

	// UTF-16 to ���e�������� �ϊ�
	static void UTF16ToString(const wchar_t* utf16, char* string, int string_size);

	// ���e�������� to UTF-8 �ϊ�
	static void StringToUTF8(const char* string, char* utf8, int utf8_size);
};
