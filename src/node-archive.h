#ifndef NODE_ARCHIVE_H
#define NODE_ARCHIVE_H

#include <node.h>
#include "archive.h"

using namespace v8;

class Archive : public node::ObjectWrap {
	public:
		static void Init(Handle<Object> exports);
	
	private:
		explicit Archive();
		~Archive();
		
		struct archive *archive;
		
		static Handle<Value> New(const Arguments& args);
		static Persistent<Function> constructor;
		
		static Handle<Value> ReadSupportFilterAll(const Arguments& args);
		static Handle<Value> ReadSupportFormatAll(const Arguments& args);
		static Handle<Value> ReadOpenFilename(const Arguments& args);
};

#endif