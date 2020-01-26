/*
 *  resmgr.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16-09-21.
 *
 */

#ifndef BOE_RESMGR_H
#define BOE_RESMGR_H

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <stack>
#include <unordered_map>
#include <memory>
#include <sstream>

namespace ResMgr {
	namespace fs = boost::filesystem;
	
	enum eErrorCode {
		ERR_RESOLVE,
		ERR_LOAD,
	};
	
	class xError : public std::exception {
		eErrorCode code;
		const std::string explanation;
	public:
		xError(eErrorCode code, const std::string& explanation) : code(code), explanation(explanation) {}
		const char* what() const throw() override {
			return explanation.c_str();
		}
	};
	
	template<typename T>
	class cLoader {
	protected:
		using ResourceList = std::vector<std::string>;
	public:
		/// Loads a resource from the requested path
		virtual T* operator()(const fs::path& from) const = 0;
		/// Returns a string identifying the type of the resource handled by this loader
		virtual std::string typeName() const = 0;
		/// Expands a resource name into a list of possible resources to attempt resolution on, eg adding possible file extensions
		virtual ResourceList expand(const std::string& resourceName) const = 0;
		virtual ~cLoader() {}
	};

	template<typename T> class cPool;
	
	template<typename T>
	class cPointer {
		friend class cPool<T>;
		bool purgeable, orphaned = false;
		std::unique_ptr<std::shared_ptr<T>> res_ptr = nullptr;
		std::string path;
		cPointer(bool p = false) : purgeable(p) {}
	public:
		// By explicitly declaring it deleted the error messages may improve
		cPointer(const cPointer& other) = delete;
		cPointer(cPointer&& other)
			: purgeable(other.purgeable)
			, orphaned(other.orphaned)
			, res_ptr(std::move(other.res_ptr))
			, path(std::move(other.path))
		{}
		const std::shared_ptr<T>& operator->() const {
			return *res_ptr;
		}
		T& operator*() const {
			return **res_ptr;
		}
		// Overloading the address operator...
		std::shared_ptr<const T> operator&() const {
			return *res_ptr.get();
		}
	};
	
	template<typename T>
	class cPool {
		std::string directory;
		size_t overflow;
		std::unordered_map<std::string, cPointer<T>> resources;
		std::unordered_map<std::string, fs::path> resolvedPaths;
		std::stack<fs::path> paths;
		cLoader<T>& load;
		/// Converts a resource name to an absolute file path referencing the resource
		fs::path find(const std::string& resourceName) {
			for(const std::string name : load.expand(resourceName)) {
				fs::path path = name;
				if(!directory.empty()) path = directory/path;
				std::stack<fs::path> tmpPaths = paths;
				while(!tmpPaths.empty()) {
					fs::path thisPath = tmpPaths.top()/path;
					if(fs::exists(thisPath)) {
						resolvedPaths[name] = thisPath;
						return thisPath;
					}
					tmpPaths.pop();
				}
			}
			std::ostringstream err;
			err << "Error! Could not load resource '" << resourceName << "' of type '" << load.typeName() << "'\n";
			if(paths.empty()) err << "  (No resource paths are present on the stack.)\n";
			throw xError(ERR_RESOLVE, err.str());
		}
		/// Runs garbage collection to free up some resources.
		/// First it attempts to free orphaned resources - those whose path was popped but haven't yet been reloaded.
		/// Then, if necessary, it may free some resources marked as purgeable.
		void gc() {
			auto iter = resources.begin();
			while(iter != resources.end()) {
				if(iter->second.orphaned) {
					iter = resources.erase(iter);
				} else {
					++iter;
				}
			}
			iter = resources.begin();
			while(iter != resources.end() && resources.size() >= overflow) {
				if(iter->second.purgeable) {
					iter = resources.erase(iter);
				} else {
					++iter;
				}
			}
		}
	public:
		cPool(cLoader<T>& loader, size_t max, std::string dir = "")
			: directory(dir), overflow(max), load(loader)
		{}
		/// Gets a resource from the pool, loading it if it is not already cached.
		/// @param resourceName The name of the resource to fetch
		/// @param purgeable If true, this resource may be removed from the cache in order to
		/// reduce cache space. If the resource is not held elsewhere, this will result in its
		/// destruction. Best used for resources that are expected to be used once and then discarded.
		/// This parameter is ignored if the resource is already cached.
		/// @return A handle to the resource. This handle is dynamically updated - in the event that the
		/// resource needs to be reloaded, it will be changed to point to the up-to-date version.
		const cPointer<T>& get(const std::string& resourceName, bool purgeable = false) {
			if(false) {
			ERROR_THROW:
				std::ostringstream err;
				err << "Error! Could not load resource '" << resourceName << "' of type '" << load.typeName() << "'\n";
				err << "  (The resource loader returned a null pointer.)\n";
				throw xError(ERR_LOAD, err.str());
			}
			auto resIter = resources.find(resourceName);
			if(resIter != resources.end()) {
				auto pathIter = resolvedPaths.find(resourceName);
				if(pathIter != resolvedPaths.end()) {
					// Resource is good, so return it
					return resIter->second;
				}
				// If we get here, the resource's path has been removed from the stack, so it must be re-resolved.
				fs::path newPath = find(resourceName);
				if(newPath == resIter->second.path) {
					// Path didn't change, so remember the path and return it
					resolvedPaths[resourceName] = newPath;
					return resIter->second;
				}
				T* ptr = load(newPath);
				if(!ptr) {
					resources.erase(resIter);
					goto ERROR_THROW;
				}
				resIter->second.res_ptr->reset(ptr);
				resIter->second.path = newPath.string();
				resIter->second.orphaned = false;
				return resIter->second;
			}
			// If we get here, it hasn't even been loaded yet.
			fs::path path = find(resourceName);
			T* ptr = load(path);
			if(!ptr) goto ERROR_THROW;
			resIter = resources.emplace(resourceName, cPointer<T>(purgeable)).first;
			resIter->second.res_ptr.reset(new std::shared_ptr<T>(ptr));
			resIter->second.path = path.string();
			if(resources.size() > overflow) gc();
			return resIter->second;
		}
		/// Forcibly remove a resource from the pool. This does not destroy the resource unless other
		/// pointers to it are also released. If the resource is not cached, this does nothing.
		/// @param resourceName The name of the resource to free
		void free(const std::string& name) {
			auto iter = resources.find(name);
			if(iter != resources.end()) {
				resources.erase(iter);
				resolvedPaths.erase(name);
			}
		}
		/// Removes all cached resources, freeing them if not held by other code.
		void drain() {
			resources.clear();
		}
		/// Check if a resource with the given name exists.
		/// Calling this caches the resolved path but does not load the resource.
		/// @param resourceName The name of the resource to fetch
		bool have(const std::string& resourceName) {
			auto iter = resources.find(resourceName);
			if(iter != resources.end() && !iter->second.orphaned)
				return true;
			try {
				return find(resourceName).is_absolute();
			} catch(xError&) {
				return false;
			}
		}
		/// Pushes a new path onto the resolution stack.
		/// Calling this causes any cached resolved paths to be forgotten.
		void pushPath(const fs::path& path) {
			paths.push(path);
			resolvedPaths.clear();
		}
		/// Pops the top path from the resolution stack and returns it.
		/// Any resources previously resolved to this path are marked as orphaned.
		fs::path popPath() {
			fs::path p = paths.top();
			paths.pop();
			std::string pstr = p.string();
			auto iter = resolvedPaths.begin();
			while(iter != resolvedPaths.end()) {
				std::string cmp = iter->second.string();
				if(cmp.compare(0, pstr.size(), pstr) == 0) {
					auto resIter = resources.find(iter->first);
					if(resIter != resources.end())
						resIter->second.orphaned = true;
					iter = resolvedPaths.erase(iter);
				} else {
					++iter;
				}
			}
			return p;
		}
		/// Returns the path at the top of the resolution stack
		fs::path topPath() {
			return paths.top();
		}
	};
}


#endif
