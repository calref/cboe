/*
 *  resmgr.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-24.
 *
 */

#ifndef BOE_RESMGR_H
#define BOE_RESMGR_H

#include <string>
#include <map>
#include <stack>
#include <exception>
#include <memory>
#include <boost/filesystem.hpp>
#include <functional>

// Will handle loading, retaining, and releasing of resources.
// Resources include sounds, images, fonts, and cursors.
namespace ResMgr {
	namespace fs = boost::filesystem;
	using idMapFn = std::function<std::string(int)>;
	
	template<typename type> struct resPool {
		static std::map<std::string,std::shared_ptr<type> >& resources() {
			static std::map<std::string,std::shared_ptr<type> > data;
			return data;
		}
		static std::stack<fs::path>& resPaths() {
			static std::stack<fs::path> data;
			return data;
		}
		static idMapFn& mapFn() {
			static idMapFn data;
			return data;
		}
		static std::map<fs::path,fs::path>& pathFound() {
			static std::map<fs::path,fs::path> data;
			return data;
		}
		static fs::path rel2abs(fs::path path) {
			std::stack<fs::path> tmpPaths = resPaths();
			while(!tmpPaths.empty()) {
				fs::path thisPath = tmpPaths.top()/path;
				printf("Testing %s...\n",thisPath.c_str());
				if(fs::exists(thisPath)) {
					pathFound()[path] = tmpPaths.top();
					return thisPath;
				}
				tmpPaths.pop();
			}
			// If we got this far, it wasn't found.
			// Just return the original filename unchanged;
			// maybe it can be resolved anyway.
			return path;
		}
	};
	
	template<typename type> struct resLoader {
		type* operator() (std::string fname);
	};
	
	class xResMgrErr : public std::exception {
		std::string msg;
	public:
		xResMgrErr() throw() {}
		xResMgrErr(const std::string& str) throw() : msg(str) {}
		~xResMgrErr() throw() {}
		virtual const char* what() const throw() {
			return msg.c_str();
		}
	};
	
	template<typename type> void free(std::string name) {
		if(resPool<type>::resources().find(name) != resPool<type>::resources().end())
			resPool<type>::resources().erase(name);
	}
	
	template<typename type> void free(int id) {
		std::string name = resPool<type>::mapFn()(id);
		if(name != "") free<type>(name);
	}
	
	template<typename type> void freeAll() {
		resPool<type>::resources().clear();
	}
	
	template<typename type> std::shared_ptr<type> get(std::string name) {
		if(resPool<type>::resources().find(name) != resPool<type>::resources().end()) {
			if(resPool<type>::pathFound().find(name) != resPool<type>::pathFound().end()) {
				std::string curPath = resPool<type>::pathFound()[name].string();
				std::string checkPath = resPool<type>::rel2abs(name).string();
				checkPath = checkPath.substr(0,curPath.length());
				if(checkPath != curPath) {
					free<type>(name);
					return get<type>(name);
				}
			}
			return resPool<type>::resources()[name];
		} else {
			type* tmp;
			resLoader<type> load;
			tmp = load(name);
			return resPool<type>::resources()[name] = std::shared_ptr<type>(tmp);
		}
	}
	
	template<typename type> std::shared_ptr<type> get(int id) {
		std::string name = resPool<type>::mapFn()(id);
		if(name == "") throw xResMgrErr("Invalid resource ID.");
		return get<type>(name);
	}
	
	template<typename type> void pushPath(fs::path path) {
		printf("Pushing path %s in %s...\n",path.c_str(),__FUNCTION__);
		resPool<type>::resPaths().push(path);
		if(resPool<type>::resPaths().empty()) printf("A problem occurred.\n");
	}
	
	template<typename type> fs::path popPath() {
		fs::path path = resPool<type>::resPaths.top();
//		std::map<std::string,std::string>::iterator mapiter;
//		std::deque<std::string> toDestroy;
//		std::deque<std::string>::iterator iter;
//		for(mapiter = resPool<type>::pathFound().begin();
//			mapiter != resPool<type>::pathFound().end();
//			mapiter++)
//			if(mapiter->second == path)
//				toDestroy.push_back(mapiter->first);
//		for(iter = toDestroy.begin(); iter != toDestroy.end(); iter++)
//			resPool<type>::resources().erase(*iter);
		resPool<type>::resPaths.pop();
		return path;
	}
	
	template<typename type> void setIdMapFn(idMapFn f) {
		resPool<type>::mapFn() = f;
	}
	
	template<typename type> idMapFn getIdMapFn() {
		return resPool<type>::mapFn();
	}
}

#endif
