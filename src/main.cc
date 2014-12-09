#include <string>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <archive.h>
#include <archive_entry.h>
#include "read.h"
#include "writer.h"

void InitAll(Handle<Object> exports) {
	exports->Set(String::NewSymbol("read"), FunctionTemplate::New(Read)->GetFunction());
	Writer::Init(exports);
}

NODE_MODULE(archive, InitAll)
