#include <node.h>
#include <v8.h>
#include "node-archive.h"
//#include "archive.h"
//#include "archive_entry.h"

using namespace v8;

//Handle<Value> Method(const Arguments& args) {
//	HandleScope scope;
//	struct archive *a;
//	struct archive_entry *entry;
//	int r;
//
//	a = archive_read_new();
//	archive_read_support_filter_all(a);
//	archive_read_support_format_all(a);
//	r = archive_read_open_filename(a, "archive.zip", 10240); // Note 1
//	if (r != ARCHIVE_OK)
//		return scope.Close(String::New("could not open archive"));
//	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
//		printf("%s\n",archive_entry_pathname(entry));
//		printf("%s\n",archive_entry_symlink(entry));
//		
//		archive_read_data_skip(a);  // Note 2
//	}
//	r = archive_read_free(a);  // Note 3
//	if (r != ARCHIVE_OK)
//		return scope.Close(String::New("could not free archivve"));
//
//	return scope.Close(String::New("world"));
//}

Handle<Value> OpenArchive(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Archive::Open(args));
}

void InitAll(Handle<Object> exports) {
	Archive::Init(exports);
	
	exports->Set(String::NewSymbol("open"), FunctionTemplate::New(OpenArchive)->GetFunction());
}

NODE_MODULE(hello, InitAll)
