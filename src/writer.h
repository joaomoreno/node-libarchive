#ifndef WRITER_H
#define WRITER_H

#include <node.h>
#include <v8.h>

using namespace v8;
using namespace node;

class Writer : ObjectWrap {
	public:
		static void Init(Handle<Object> exports);

	private:
		explicit Writer();
		~Writer();

		static Handle<Value> New(const Arguments& args);
		static Persistent<Function> constructor;
};

#endif
