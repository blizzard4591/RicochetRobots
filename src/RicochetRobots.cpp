// RicochetRobots.cpp : Defines the entry point for the console application.
//

#include <l3pp.h>
#define TAP_AUTOFLAG 1
#include <tap/Tap.h>

#include <iostream>

void initLog() {
	l3pp::Logger::initialize();
	l3pp::SinkPtr sink = l3pp::StreamSink::create(std::clog);
	l3pp::Logger::getRootLogger()->addSink(sink);
	l3pp::Logger::getRootLogger()->setLevel(l3pp::LogLevel::INFO);
}

bool processArgs(int argc, char* argv[]) {
	TAP::Argument help("Show this &help text");
	TAP::ArgumentParser parser(help);
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
}


int main(int argc, char* argv[]) {
	initLog();
	processArgs(argc, argv);
	L3PP_LOG_INFO(l3pp::Logger::getRootLogger(), "Starting rrobots");
	return 0;
}

