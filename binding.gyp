{
  "targets": [
    {
      "target_name": "hello",
      "sources": [ "src/hello.cc" ],
      "conditions": [
        ["OS == 'mac'", {
          "include_dirs": [
            "libarchive/libarchive"
          ]
        }]
      ]
    }
  ]
}