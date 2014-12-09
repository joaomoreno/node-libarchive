#include <node.h>
#include <v8.h>
#include "writer.h"

using namespace v8;
using namespace node;

Persistent<Function> Writer::constructor;

Writer::Writer() {
}

Writer::~Writer() {
}

void Writer::Init(Handle<Object> exports) {
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("Writer"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	constructor = Persistent<Function>::New(tpl->GetFunction());
	exports->Set(String::NewSymbol("Writer"), constructor);
}

Handle<Value> Writer::New(const Arguments& args) {
	HandleScope scope;

	if (args.IsConstructCall()) {
		Writer* obj = new Writer();
		obj->Wrap(args.This());
		return args.This();
	} else {
		const int argc = 0;
		Local<Value> argv[argc] = { };
		return scope.Close(constructor->NewInstance(argc, argv));
	}
}
