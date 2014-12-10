#include <string>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <archive.h>
#include <archive_entry.h>
#include "read.h"

using namespace v8;
using namespace node;

void DoNextHeader(uv_work_t *req);
void OnNextHeader(uv_work_t *req);
void DoReadEntry(uv_work_t *req);
void OnReadEntry(uv_work_t *req);
void DoRead(uv_work_t *req);

int readAll(archive *archive, char* buffer, size_t size) {
	int r, offset = 0;

	for (;;) {
		r = archive_read_data(archive, buffer, size - offset);

		if (r < 0) {
			return r;
		} else if (r == 0) {
			return ARCHIVE_OK;
		} else {
			offset += r;
		}
	}
}

Handle<Value> BufferHandle(char *data, size_t size) {
	Buffer *buffer = Buffer::New(size);
	char *bufferData = Buffer::Data(buffer);

	for (size_t i = 0; i < size; i++) {
		bufferData[i] = data[i];
	}

	Local<Object> global = Context::GetCurrent()->Global();
	Local<Function> ctor = Local<Function>::Cast(global->Get(String::New("Buffer")));
	Handle<Value> args[3] = { buffer->handle_, v8::Integer::New(size), v8::Integer::New(0) };
	return ctor->NewInstance(3, args);
}

const char *typeName(mode_t filetype) {
	switch (filetype) {
		case AE_IFREG: return "file";
		case AE_IFLNK: return "symlink";
		case AE_IFDIR: return "directory";
		default: return "unknown";
	}
}

void DoReadEntry(uv_work_t *req) {
	ReadData *data = (ReadData*) req->data;
	data->result = readAll(data->archive, data->bufferData, data->bufferSize);
}

void OnReadEntry(uv_work_t *req) {
	HandleScope scope;
	ReadData *data = (ReadData*) req->data;

	if (data->result != ARCHIVE_OK) {
		OnNextHeader(req);
		return;
	}

	mode_t filetype = archive_entry_filetype(data->entry);

	Local<Object> result = Object::New();
	result->Set(String::New("path"), String::New(archive_entry_pathname(data->entry)));
	result->Set(String::New("type"), String::New(typeName(filetype)));

	Local<Object> stat = Object::New();
	result->Set(String::New("stat"), stat);
	stat->Set(String::New("permissions"), Number::New(archive_entry_perm(data->entry)));

	if (archive_entry_atime_is_set(data->entry)) {
		stat->Set(String::New("atime"), Number::New(archive_entry_atime(data->entry)));
	}

	if (archive_entry_birthtime_is_set(data->entry)) {
		stat->Set(String::New("birthtime"), Number::New(archive_entry_birthtime(data->entry)));
	}

	if (archive_entry_ctime_is_set(data->entry)) {
		stat->Set(String::New("ctime"), Number::New(archive_entry_ctime(data->entry)));
	}

	if (archive_entry_mtime_is_set(data->entry)) {
		stat->Set(String::New("mtime"), Number::New(archive_entry_mtime(data->entry)));
	}

	if (filetype == AE_IFREG) {
		result->Set(String::New("data"), BufferHandle(data->bufferData, data->bufferSize));
	} else if (filetype == AE_IFLNK) {
		result->Set(String::New("symlink"), String::New(archive_entry_symlink(data->entry)));
	}
	
	int argc = 1;
	Handle<Value> argv[] = { result };

	TryCatch tryCatch;
	data->onEntry->Call(Context::GetCurrent()->Global(), argc, argv);
	if (tryCatch.HasCaught()) {
		node::FatalException(tryCatch);
	}

	data->bufferSize = 0;
	delete data->bufferData;
	data->bufferData = NULL;

	uv_queue_work(uv_default_loop(), req, DoNextHeader, (uv_after_work_cb) OnNextHeader);
}

void DoNextHeader(uv_work_t *req) {
	ReadData *data = (ReadData*) req->data;
	data->result = archive_read_next_header(data->archive, &data->entry);
}

void OnNextHeader(uv_work_t *req) {
	HandleScope scope;
	ReadData *data = (ReadData*) req->data;

	if (data->result == ARCHIVE_OK) {
		int size = archive_entry_size(data->entry);

		data->bufferSize = size;
		data->bufferData = new char[size];

		uv_queue_work(uv_default_loop(), req, DoReadEntry, (uv_after_work_cb) OnReadEntry);
		return;
	}

	int argc = 1;
	Handle<Value> argv[] = { Null() };

	if (data->result != ARCHIVE_EOF) {
		argv[0] = Exception::Error(String::Concat(String::New("Could not read archive "), String::New(data->filename->c_str())));
	}

	TryCatch tryCatch;
	data->onDone->Call(Context::GetCurrent()->Global(), argc, argv);
	if (tryCatch.HasCaught()) {
		node::FatalException(tryCatch);
	}

	archive_read_free(data->archive);
	data->archive = NULL;
	data->entry = NULL;

	data->onEntry.Dispose();
	data->onDone.Dispose();
	delete data->filename;
	delete data;
	delete req;
}

Handle<Value> Read(const Arguments& args) {
	HandleScope scope;
	
	if (args.Length() < 3) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	
	if (!args[0]->IsString() || !args[1]->IsFunction() || !args[2]->IsFunction()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}
	
	uv_work_t *req = new uv_work_t();
	ReadData *data = new ReadData();
	
	req->data = data;
	data->filename = new std::string(*String::Utf8Value(args[0]));
	data->onEntry = Persistent<Function>::New(Local<Function>::Cast(args[1]));
	data->onDone = Persistent<Function>::New(Local<Function>::Cast(args[2]));
	
	uv_queue_work(uv_default_loop(), req, DoRead, (uv_after_work_cb) OnNextHeader);
	
	return scope.Close(Undefined());
}

void DoRead(uv_work_t *req) {
	ReadData *data = (ReadData*) req->data;
	
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

	DoNextHeader(req);
}
