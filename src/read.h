#include <string>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <archive.h>
#include <archive_entry.h>

using namespace v8;
using namespace node;

typedef struct ReadData {
	archive *archive;
	archive_entry *entry;
	size_t bufferSize;
	char *bufferData;
	Persistent<Function> onEntry;
	Persistent<Function> onDone;
	std::string *filename;
	int result;
} ReadData;

Handle<Value> Read(const Arguments& args);
