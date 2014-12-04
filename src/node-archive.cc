#include <string>
#include <node.h>
#include <uv.h>
#include "node-archive.h"

using namespace v8;

Persistent<Function> Archive::constructor;

Archive::Archive() {
	archive = archive_read_new();
}

Archive::~Archive() {
	archive_read_free(archive);
}

void Archive::Init(Handle<Object> exports) {
	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("Archive"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	
	// Prototype
	tpl->PrototypeTemplate()->Set(String::NewSymbol("readSupportFilterAll"), FunctionTemplate::New(ReadSupportFilterAll)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("readSupportFormatAll"), FunctionTemplate::New(ReadSupportFormatAll)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("readOpenFilename"), FunctionTemplate::New(ReadOpenFilename)->GetFunction());
	
	constructor = Persistent<Function>::New(tpl->GetFunction());
	exports->Set(String::NewSymbol("Archive"), constructor);
}

Handle<Value> Archive::New(const Arguments& args) {
	HandleScope scope;

	if (args.IsConstructCall()) {
		Archive* obj = new Archive();
		obj->Wrap(args.This());
		return args.This();
	} else {
		const int argc = 1;
		Local<Value> argv[argc] = { args[0] };
		return scope.Close(constructor->NewInstance(argc, argv));
	}
}

Handle<Value> Archive::ReadSupportFilterAll(const Arguments& args) {
	HandleScope scope;
	
	Archive* obj = ObjectWrap::Unwrap<Archive>(args.This());
	archive_read_support_filter_all(obj->archive);
	
	return scope.Close(Undefined());
}

Handle<Value> Archive::ReadSupportFormatAll(const Arguments& args) {
	HandleScope scope;
	
	Archive* obj = ObjectWrap::Unwrap<Archive>(args.This());
	archive_read_support_format_all(obj->archive);
	
	return scope.Close(Undefined());
}

typedef struct ReadOpenFilenameData {
	archive *archive;
	std::string *filename;
	long blockSize;
	Persistent<Function> callback;
	int result;
} ReadOpenFilenameData;

void DoReadOpenFilenameData(uv_work_t *req) {
	ReadOpenFilenameData *data = (ReadOpenFilenameData*) req->data;
	
	data->result = archive_read_open_filename(
		data->archive,
		data->filename->c_str(),
		data->blockSize
	);
}

void OnReadOpenFilenameData(uv_work_t *req) {
	HandleScope scope;
	
	ReadOpenFilenameData *data = (ReadOpenFilenameData*) req->data;
	Handle<Value> argv[] = {
		Null(),
		String::New(data->filename->c_str())
	};
	
	TryCatch tryCatch;
	data->callback->Call(Context::GetCurrent()->Global(), 2, argv);
	if (tryCatch.HasCaught()) {
		node::FatalException(tryCatch);
	}
	
	delete data->filename;
	data->callback.Dispose();
	delete data;
	delete req;
}

Handle<Value> Archive::ReadOpenFilename(const Arguments& args) {
	HandleScope scope;
	
	if (args.Length() < 3) {
	    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
	    return scope.Close(Undefined());
	}
	
	if (!args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsFunction()) {
	    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
	    return scope.Close(Undefined());
	}
	
	uv_work_t *req = new uv_work_t();
	ReadOpenFilenameData *data = new ReadOpenFilenameData();
	
	req->data = data;
	
	Archive *archive = ObjectWrap::Unwrap<Archive>(args.This());
	data->archive = archive->archive;
	
	data->filename = new std::string(*String::Utf8Value(args[0]));
	data->blockSize = Local<Number>::Cast(args[1])->Value();
	data->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
	
	uv_queue_work(uv_default_loop(), req, DoReadOpenFilenameData, (uv_after_work_cb) OnReadOpenFilenameData);
	
	return scope.Close(Undefined());
}
