#ifndef NODE_ARCHIVE_H
#define NODE_ARCHIVE_H

#include <string>
#include <node.h>
#include <uv.h>
#include "archive.h"

using namespace v8;

typedef struct OpenData {
	archive *archive;
	std::string *filename;
	Persistent<Function> callback;
	int result;
} OpenData;

class Archive : public node::ObjectWrap {
	public:
		static void Init(Handle<Object> exports);
		static Handle<Value> Open(const Arguments& args);
	
	private:
		explicit Archive(archive *_archive);
		~Archive();
		
		struct archive *archive;
		
		static Handle<Value> New(const Arguments& args);
		static Persistent<Function> constructor;
		
		static void DoOpen(uv_work_t *req);
		static void OnOpen(uv_work_t *req);
};

#endif