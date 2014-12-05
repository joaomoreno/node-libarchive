#include <string>
#include <node.h>
#include <uv.h>
#include "node-archive.h"

using namespace v8;

Persistent<Function> Archive::constructor;

Archive::Archive(struct archive *_archive) {
	archive = _archive;
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
	tpl->PrototypeTemplate()->Set(String::NewSymbol("filename"), String::NewSymbol("unknown"));
		
	constructor = Persistent<Function>::New(tpl->GetFunction());
	exports->Set(String::NewSymbol("open"), FunctionTemplate::New(Open)->GetFunction());
}

Handle<Value> Archive::New(const Arguments& args) {
	HandleScope scope;

	if (args.IsConstructCall()) {
		OpenData* data = (OpenData*) External::Unwrap(args[0]);
		Archive* obj = new Archive(data->archive);
		
		args.This()->Set(String::NewSymbol("filename"), String::NewSymbol(data->filename->c_str()));
		
		obj->Wrap(args.This());
		return args.This();
	} else {
		const int argc = 1;
		Local<Value> argv[argc] = { args[0] };
		return scope.Close(constructor->NewInstance(argc, argv));
	}
}

// Open

void Archive::DoOpen(uv_work_t *req) {
	OpenData *data = (OpenData*) req->data;
	
	data->archive = archive_read_new();
	
	if (ARCHIVE_OK != (data->result = archive_read_support_filter_all(data->archive))) {
		return;
	}
	
	if (ARCHIVE_OK != (data->result = archive_read_support_format_all(data->archive))) {
		return;
	}
	
	data->result = archive_read_open_filename(
		data->archive,
		data->filename->c_str(),
		10240
	);
}

void Archive::OnOpen(uv_work_t *req) {
	HandleScope scope;
	
	OpenData *data = (OpenData*) req->data;
	
	Handle<Value> argv[2] = { Null(), Null() };
	
	if (data->result != ARCHIVE_OK) {
		argv[0] = Exception::Error(String::Concat(String::New("Could not open "), String::New(data->filename->c_str())));
	} else {
		Handle<Value> instanceArgv[] = { External::Wrap(data) };
		argv[1] = Archive::constructor->NewInstance(1, instanceArgv);
	}
	
	TryCatch tryCatch;
	data->callback->Call(Context::GetCurrent()->Global(), 2, argv);
	if (tryCatch.HasCaught()) {
		node::FatalException(tryCatch);
	}
	
	data->callback.Dispose();
	delete data->filename;
	delete data;
	delete req;
}

Handle<Value> Archive::Open(const Arguments& args) {
	HandleScope scope;
	
	if (args.Length() < 2) {
	    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
	    return scope.Close(Undefined());
	}
	
	if (!args[0]->IsString() || !args[1]->IsFunction()) {
	    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
	    return scope.Close(Undefined());
	}
	
	uv_work_t *req = new uv_work_t();
	OpenData *data = new OpenData();
	
	req->data = data;
	
	data->filename = new std::string(*String::Utf8Value(args[0]));
	data->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
	
	uv_queue_work(uv_default_loop(), req, DoOpen, (uv_after_work_cb) OnOpen);
	
	return scope.Close(Undefined());
}
