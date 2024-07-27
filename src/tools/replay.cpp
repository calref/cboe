#include "replay.hpp"

// Input recording system

#include "ticpp.h"
#include <ctime>
#include <iomanip>
#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include <cppcodec/base64_rfc4648.hpp>
using base64 = cppcodec::base64_rfc4648;

bool recording = false;
bool replaying = false;

using namespace ticpp;
Document log_document;
std::string log_file;
Element* next_action;

bool init_action_log(std::string command, std::string file) {
	if (command == "record") {
		// If a filename is given, use it as a base, but insert a timestamp for uniqueness.
		if (file.empty())
			file = "BoE";

		if (boost::ends_with(file, ".xml"))
			file = file.substr(0, file.length() - 4);

		// Get a time stamp
		std::time_t t = time(nullptr);
		auto tm = *std::localtime(&t);
		std::ostringstream stream;
		stream << file << std::put_time(&tm, "_%d-%m-%Y_%H-%M-%S") << ".xml";
		log_file = stream.str();
		
		try {
			Element root_element("actions");
			log_document.InsertEndChild(root_element);
			log_document.SaveFile(log_file);
			recording = true;
			std::cout << "Recording this session: " << log_file << std::endl;
		} catch(...) {
			std::cout << "Failed to write to file " << log_file << std::endl;
		}
		return true;
	}
	else if (command == "replay") {
		try {
			log_document.LoadFile(file);
			Element* root = log_document.FirstChildElement();
			next_action = root->FirstChildElement();
			replaying = true;
		} catch(...) {
			std::cout << "Failed to load file " << file << std::endl;
		}
		return true;
	}
	return false;
}

void record_action(std::string action_type, std::string inner_text) {
	Element* root = log_document.FirstChildElement();
	Element next_action(action_type);
	Text action_text(inner_text);
	next_action.InsertEndChild(action_text);
	root->InsertEndChild(next_action);
	log_document.SaveFile(log_file);
}

void record_action(std::string action_type, std::map<std::string,std::string> info) {
	Element* root = log_document.FirstChildElement();
	Element next_action(action_type);
	for(auto& p : info){
		Element next_child(p.first);
		Text child_text(p.second);
		next_child.InsertEndChild(child_text);
		next_action.InsertEndChild(next_child);
	}
	root->InsertEndChild(next_action);
	log_document.SaveFile(log_file);
}

bool has_next_action() {
	return next_action != nullptr;
}

std::string next_action_type() {
	return next_action->Value();
}

Element* pop_next_action(std::string expected_action_type) {
	if (expected_action_type != "" && next_action->Value() != expected_action_type) {
		std::ostringstream stream;
		stream << "Replay error! Expected '" << expected_action_type << "' action next";
		throw stream.str();
	}

	Element* to_return = next_action;
	
	next_action = next_action->NextSiblingElement(false);
	
	return to_return;
}

std::map<std::string,std::string> info_from_action(Element* action) {
	std::map<std::string,std::string> info = {};
	Element* next_child = action->FirstChildElement(false);
	while(next_child){
		info[next_child->Value()] = next_child->GetText();
		next_child = next_child->NextSiblingElement(false);
	}
	return info;
}

std::string encode_file(fs::path file) {
	std::ifstream ifs(file.string(), std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(result.data(), pos);

    return base64::encode(result.data(), result.size() * sizeof(char));
}

void decode_file(std::string data, fs::path file) {
	std::ofstream ofs(file.string(), std::ios::binary|std::ios::ate);
	std::vector<uint8_t> bytes = base64::decode(data.c_str(), strlen(data.c_str()) * sizeof(char));
	char* bytes_as_c_str = reinterpret_cast<char*>(bytes.data());
	ofs.write(bytes_as_c_str, bytes.size() / sizeof(char));
}