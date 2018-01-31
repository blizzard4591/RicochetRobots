#include <iostream>
#include <string>

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include <fcntl.h>
#include <io.h>
#endif

int main(int argc, char* argv[]) {
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
	_setmode(_fileno(stdout), _O_U16TEXT);
#endif

	std::wcout << L"My Unicode String öäü with boxes ┌─┐ has japanese 菅 直 人" << std::endl;

	std::cin.ignore();

	return 0;
}
