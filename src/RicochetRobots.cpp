// RicochetRobots.cpp : Defines the entry point for the console application.
//

#include <l3pp.h>
#define TAP_AUTOFLAG 1
#include <tap/Tap.h>

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include <fcntl.h>
#include <io.h>
#endif
#if defined(_STUPID) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER))
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <locale>
#include <codecvt>

#include <iostream>
#include <string>
#include "MapBuilder.h"
#include "Map.h"

static std::string mapname;

void initLog() {
	l3pp::Logger::initialize();
	l3pp::SinkPtr sink = l3pp::StreamSink::create(std::clog);
	l3pp::Logger::getRootLogger()->addSink(sink);
	l3pp::Logger::getRootLogger()->setLevel(l3pp::LogLevel::INFO);
}

std::string toUtf8(std::wstring const& str) {
#if defined(_STUPID)
	std::string ret;
	int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
	if (len > 0) {
		ret.resize(len);
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), &ret[0], len, NULL, NULL);
	}
	return ret;
#else
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(str);
#endif
}

bool processArgs(std::vector<std::string>& args) {
	TAP::VariableArgument<std::string> map("Load map", mapname);
	TAP::Argument help("Show this &help text");
	TAP::ArgumentParser parser(help, +map);
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
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	initLog();
	
	std::vector<std::string> args;
	for (int i = 1; i < argc; i++) {
		args.emplace_back(toUtf8(argv[i]));
	}

	processArgs(args);
	L3PP_LOG_INFO(l3pp::Logger::getRootLogger(), "Starting rrobots, map " << mapname);

	std::ifstream t(mapname);
	if (!t) {
		L3PP_LOG_ERROR(l3pp::Logger::getRootLogger(), "Cannot open map");
		return 1;
	}
	std::stringstream buffer;
	buffer << t.rdbuf();
	ricochet::MapBuilder builder = ricochet::MapBuilder::fromJson(buffer.str());

	ricochet::Map test = builder.toMap();
	std::cout << "Map data: " << std::endl;
	std::cout << test.toString() << std::endl;

	ricochet::Map map(10, 10);

	return 0;
}

