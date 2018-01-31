#include <codecvt>
#include <fstream>
#include <iostream>
#include <string>
#include <locale>

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include <fcntl.h>
#include <io.h>
#endif

int main(int argc, char* argv[]) {
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
	//_setmode(_fileno(stdout), _O_U16TEXT);
#endif

	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);

	std::wstring const text = L"My Unicode String öäü with boxes ┌─┐ has japanese 菅 直 人";
	std::wcout.imbue(loc);
	std::wcout << text << std::endl;

	std::wofstream fout("myfile.txt");
	fout.imbue(loc);
	if (!fout.is_open()) {
		std::wcout << "Before: Not open..." << std::endl;
	}
	fout << L"Հայաստան Россия Österreich Ελλάδα भारत" << std::endl;
	if (!fout.good()) {
		std::wcout << "After: Not good..." << std::endl;
	}
	fout.close();

	std::wofstream myfile;
	myfile.imbue(loc);
	myfile.open("debug.txt");
	myfile << text << std::endl;
	myfile.close();

	std::cin.ignore();

	return 0;
}
