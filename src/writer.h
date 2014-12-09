#ifndef WRITER_H
#define WRITER_H

#include <string>
#include <node.h>
#include <v8.h>
#include <archive.h>

using namespace v8;
using namespace node;

class Writer : ObjectWrap {
	public:
		static void Init(Handle<Object> exports);

	private:
		explicit Writer(const char *filename);
		~Writer();

		static Handle<Value> New(const Arguments& args);
		static Persistent<Function> constructor;

		static Handle<Value> WriteFile(const Arguments& args);
		static Handle<Value> WriteDirectory(const Arguments& args);
		static Handle<Value> WriteSymlink(const Arguments& args);
		static Handle<Value> Close(const Arguments& args);

		std::string *filename_;
		archive *archive_;
};

#endif
