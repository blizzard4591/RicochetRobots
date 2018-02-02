// RicochetRobots.cpp : Defines the entry point for the console application.
//

#include <l3pp.h>
#define TAP_AUTOFLAG 1
#include <tap/Tap.h>

#include <locale>
#include <codecvt>

#include <iostream>
#include <string>
#include "MapBuilder.h"
#include "Game.h"

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include <fcntl.h>
#include <io.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

static std::string mapname;

class wide_stream_sink: public l3pp::basic_sink<char> {
	/// Output stream.
	mutable std::unique_ptr<std::wostream> os;

	explicit wide_stream_sink(std::wostream& _os) :
			os(new std::wostream(_os.rdbuf())) {}

public:
	void log(l3pp::basic_log_entry<char> const& context) const override {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wstr = converter.from_bytes(this->formatMessage(context));
		*os << wstr << std::flush;
	}

	/**
	 * Create a StreamSink from some output stream.
	* @param os Output stream.
	*/
	static auto create(std::wostream& os) {
		return l3pp::ptr<wide_stream_sink>(new wide_stream_sink(os));
	}
};

void initLog() {
	l3pp::initialize();
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
	std::wclog.imbue(loc);
	auto sink = wide_stream_sink::create(std::wclog);
#else
	auto sink = l3pp::stream_sink::create(std::clog);
#endif
	l3pp::getRootLogger()->addSink(sink);
	l3pp::getRootLogger()->setLevel(l3pp::LogLevel::INFO);
}

std::string toUtf8(std::wstring const& str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(str);
}

bool processArgs(std::vector<std::string>& args) {
	TAP::variable_argument<std::string> map("Load map", mapname);
	TAP::argument help("Show this &help text");
	TAP::argument_parser parser(help, +map);
	try {
		parser.parse(args);
	} catch (TAP::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	if (help) {
		std::cout << parser.help() << std::endl;
		return false;
	}

	return true;
}

int program(std::vector<std::string>& args) {
	processArgs(args);
	L3PP_LOG_INFO(l3pp::getRootLogger(), "Starting rrobots, map " << mapname);

	std::ifstream t(mapname);
	if (!t) {
		L3PP_LOG_ERROR(l3pp::getRootLogger(), "Cannot open map");
		return 1;
	}
	std::stringstream buffer;
	buffer << t.rdbuf();
	ricochet::MapBuilder builder = ricochet::MapBuilder::fromJson(buffer.str());

	ricochet::Map test = builder.toMap();
	L3PP_LOG_INFO(l3pp::getRootLogger(), "Map data: \n" << test.toString());

	ricochet::Game game(std::move(test), false);
	L3PP_LOG_INFO(l3pp::getRootLogger(), "Map data 2: \n" << game.getMap().toString());

	L3PP_LOG_INFO(l3pp::getRootLogger(), "Move seq: " << game.doMove({ricochet::Move{ricochet::Color::BLUE, ricochet::Direction::SOUTH}}, true));

	auto dir = ricochet::Direction::SOUTH;
	L3PP_LOG_INFO(l3pp::getRootLogger(), "Move: " << const_cast<ricochet::Map&>(game.getMap()).moveRobot(ricochet::Color::BLUE, dir));

	L3PP_LOG_INFO(l3pp::getRootLogger(), "Map data 3: \n" << game.getMap().toString());

	return 0;
}
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)

int wmain(int argc, wchar_t* argv[]) {
	UINT const oldInputCodepage = GetConsoleCP();
	UINT const oldOutputCodepage = GetConsoleOutputCP();

	// UTF-8
	if (!SetConsoleCP(65001)) {
		std::cerr << "Setting Codepage failed: " << GetLastError() << std::endl;
		return -1;
	}; 
	if (!SetConsoleOutputCP(65001)) {
		std::cerr << "Setting Output Codepage failed: " << GetLastError() << std::endl;
		return -1;
	};

	_setmode(_fileno(stdout), _O_U16TEXT);

	initLog();

	std::vector<std::string> args;
	for (int i = 1; i < argc; i++) {
		args.emplace_back(toUtf8(argv[i]));
	}

	int const result =  program(args);

	// Reset Console CP
	SetConsoleCP(oldInputCodepage);
	SetConsoleOutputCP(oldOutputCodepage);

	return result;
}

#else

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "");

	initLog();

	std::vector<std::string> args;
	for (int i = 1; i < argc; i++) {
		args.emplace_back(argv[i]);
	}

	return program(args);
}

#endif