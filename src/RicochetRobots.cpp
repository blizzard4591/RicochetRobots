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
#include "Map.h"

static std::string mapname;

void initLog() {
	l3pp::initialize();
	auto sink = l3pp::basic_stream_sink<wchar_t>::create(std::wclog);
	l3pp::getRootLogger<wchar_t>()->addSink(sink);
	l3pp::getRootLogger<wchar_t>()->setLevel(l3pp::LogLevel::INFO);
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


int wmain(int argc, wchar_t* argv[]) {
	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
	std::wcout.imbue(loc);

	initLog();
	
	std::vector<std::string> args;
	for (int i = 1; i < argc; i++) {
		args.emplace_back(toUtf8(argv[i]));
	}

	processArgs(args);
	L3PP_LOG_INFO(l3pp::getRootLogger<wchar_t>(), L"Starting rrobots, map ");// << mapname);

	std::ifstream t(mapname);
	if (!t) {
		L3PP_LOG_ERROR(l3pp::getRootLogger<wchar_t>(), L"Cannot open map");
		return 1;
	}
	std::stringstream buffer;
	buffer << t.rdbuf();
	ricochet::MapBuilder builder = ricochet::MapBuilder::fromJson(buffer.str());

	ricochet::Map test = builder.toMap();

	test.insertRobot(ricochet::Robot::BLUE, ricochet::Pos{1, 0});
	test.insertBarrier(ricochet::Barrier{ricochet::BarrierType::BWD, ricochet::Color::RED}, ricochet::Pos{1, 5});
	test.insertBarrier(ricochet::Barrier{ricochet::BarrierType::FWD, ricochet::Color::RED}, ricochet::Pos{5, 5});
	std::wcout << L"Map data: " << std::endl;
	std::wcout.imbue(loc);
	std::wcout << test.toString() << std::endl;

	auto next = test.nextPos(ricochet::Pos{1, 0}, ricochet::Direction::SOUTH, ricochet::Color::BLUE);

	test.moveRobot(ricochet::Pos{1, 0}, next);

	return 0;
}

