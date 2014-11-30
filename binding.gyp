{
  "targets": [
    {
      "target_name": "hello",
      "sources": [ "src/hello.cc" ],
      "conditions": [
        ["OS == 'mac'", {
          "libraries": [
            "-larchive",
            "-Llibarchive/libarchive"
          ],
          "include_dirs": [
            "libarchive/libarchive"
          ]
        }]
      ]
    }
  ]
}