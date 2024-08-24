#include "replay.hpp"

// Input recording system

#include "ticpp.h"
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>
#include <cppcodec/base64_rfc4648.hpp>
#include "tools/framerate_limiter.hpp"
#include <locale>
#include <codecvt>
#include <string>

using base64 = cppcodec::base64_rfc4648;

bool recording = false;
bool replaying = false;

using namespace ticpp;
Document log_document;
std::string log_file;
Element* next_action;
boost::optional<cFramerateLimiter> replay_fps_limit;

bool init_action_log(std::string command, std::string file) {
	if(command == "record-unique") {
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
		file = stream.str();
		command = "record";
	}
	if(command == "record") {
		log_file = file;
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

void record_action(std::string action_type, std::string inner_text, bool cdata) {
	Element* root = log_document.FirstChildElement();
	Element next_action(action_type);
	Text action_text(inner_text);
	action_text.SetCDATA(cdata);
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

void record_field_input(cKey key) {
	std::map<std::string,std::string> info;
	std::ostringstream sstr;

	sstr << key.spec;
	info["spec"] = sstr.str();

	sstr.str("");
	if(key.spec){
		sstr << key.k;
		info["k"] = sstr.str();
	}else{
		std::wostringstream wstr;
		wstr << key.c;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::string c = converter.to_bytes(wstr.str());
		info["c"] = c;
	}

	sstr.str("");
	sstr << key.mod;
	info["mod"] = sstr.str();

	record_action("field_input", info);
}

bool has_next_action() {
	return next_action != nullptr;
}

std::string next_action_type() {
	if(next_action == nullptr){
		throw std::string { "Replay error! No action left to check type" };
	}
	return next_action->Value();
}

Element& pop_next_action(std::string expected_action_type) {
	if(next_action == nullptr){
		throw std::string { "Replay error! No action left to pop" };
	}
	if(expected_action_type != "" && next_action->Value() != expected_action_type){
		std::ostringstream stream;
		stream << "Replay error! Expected '" << expected_action_type << "' action next";
		throw stream.str();
	}

	Element* to_return = next_action;
	next_action = next_action->NextSiblingElement(false);
	
	if(replay_fps_limit.has_value()) {
		replay_fps_limit->frame_finished();
	}
	
	return *to_return;
}

std::map<std::string,std::string> info_from_action(Element& action) {
	std::map<std::string,std::string> info = {};
	Element* next_child = action.FirstChildElement(false);
	while(next_child){
		info[next_child->Value()] = next_child->GetTextOrDefault("");
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

location location_from_action(Element& action) {
	location l;
	std::istringstream sstr(action.GetText());
	sstr >> l;
	return l;
}

short short_from_action(Element& action) {
	short s;
	std::istringstream sstr(action.GetText());
	sstr >> s;
	return s;
}

cKey key_from_action(Element& action) {
	auto info = info_from_action(action);
	cKey key;
	int enum_v;

	std::istringstream sstr(info["spec"]);
	sstr >> key.spec;

	if(key.spec){
		sstr.str(info["k"]);
		sstr >> enum_v;
		key.k = static_cast<eSpecKey>(enum_v);
	}else{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wstr = converter.from_bytes(info["c"]);
		std::wistringstream wsstr(wstr);
		wsstr >> key.c;
	}

	sstr.str(info["mod"]);
	sstr >> enum_v;
	key.mod = static_cast<eKeyMod>(enum_v);

	return key;
}