#include <node.h>
#include <v8.h>
#include "archive.h"

using namespace v8;

Handle<Value> Method(const Arguments& args) {
  HandleScope scope;
  struct archive *a;
  int r;


  a = archive_read_new();
  r = archive_read_free(a);

  return scope.Close(String::New("world"));
}

void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("hello"),
    FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(hello, init)
