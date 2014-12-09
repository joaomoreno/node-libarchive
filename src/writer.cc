#include <node.h>
#include <v8.h>
#include <archive.h>
#include "writer.h"

using namespace v8;
using namespace node;

Persistent<Function> Writer::constructor;

Writer::Writer(const char *filename) {
	filename_ = new std::string(filename);

	archive_ = archive_write_new();
	archive_write_set_format_zip(archive_);
	archive_write_add_filter_none(archive_);
	archive_write_open_filename(archive_, filename);
}

Writer::~Writer() {
	archive_write_free(archive_);
	delete filename_;
}

void Writer::Init(Handle<Object> exports) {
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("Writer"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	tpl->PrototypeTemplate()->Set(String::NewSymbol("writeFile"), FunctionTemplate::New(WriteFile)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("writeDirectory"), FunctionTemplate::New(WriteDirectory)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("writeSymlink"), FunctionTemplate::New(WriteSymlink)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("close"), FunctionTemplate::New(Close)->GetFunction());

	constructor = Persistent<Function>::New(tpl->GetFunction());
	exports->Set(String::NewSymbol("Writer"), constructor);
}

Handle<Value> Writer::New(const Arguments& args) {
	HandleScope scope;

	if (args.IsConstructCall()) {
		if (args.Length() < 1) {
			ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
			return scope.Close(Undefined());
		}

		Writer* obj = new Writer(*String::Utf8Value(args[0]));
		obj->Wrap(args.This());
		args.This()->Set(String::New("filename"), args[0]);

		return args.This();
	} else {
		const int argc = 0;
		Local<Value> argv[argc] = { };
		return scope.Close(constructor->NewInstance(argc, argv));
	}
}

Handle<Value> Writer::WriteFile(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Null());
}

Handle<Value> Writer::WriteDirectory(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Null());
}

Handle<Value> Writer::WriteSymlink(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Null());
}

Handle<Value> Writer::Close(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Null());
}
