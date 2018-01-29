// RicochetRobots.cpp : Defines the entry point for the console application.
//

#include <l3pp.h>
#define TAP_AUTOFLAG 1
#include <tap/Tap.h>

#include <iostream>
#include <string>
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

bool processArgs(int argc, char* argv[]) {
	TAP::VariableArgument<std::string> map("Load map", mapname);
	TAP::Argument help("Show this &help text");
	TAP::ArgumentParser parser(help, +map);
	try {
		parser.parse(argc, argv);
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


int main(int argc, char* argv[]) {
	initLog();
	processArgs(argc, argv);
	L3PP_LOG_INFO(l3pp::Logger::getRootLogger(), "Starting rrobots, map " << mapname);

	std::ifstream t(mapname);
	if (!t) {
		L3PP_LOG_ERROR(l3pp::Logger::getRootLogger(), "Cannot open map");
		return 1;
	}
	std::stringstream buffer;
	buffer << t.rdbuf();
	ricochet::MapBuilder builder = ricochet::MapBuilder::fromJson(buffer.str());

	ricochet::Map map(10, 10);

	return 0;
}

