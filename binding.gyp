{
	"targets": [
		{
			"target_name": "archive",
			"sources": [
				"src/main.cc",
				"src/read.cc",
				"src/writer.cc"
			],
			"include_dirs": [
				"deps/libarchive/libarchive"
			],
			"conditions": [
				["OS == 'win'", {
					"variables": {
						"archive_static.lib": "<(PRODUCT_DIR)\\..\\..\\deps\\libarchive\\libarchive\\Release\\archive_static.lib"
					},
					"conditions": [
						["target_arch == 'ia32'", {
							"variables": {
								"zlib.lib": "<(PRODUCT_DIR)\\..\\..\\deps\\zlib-win64\\Release\\zlib.lib"
							}
						}],
						["target_arch == 'x64'", {
							"variables": {
								"zlib.lib": "<(PRODUCT_DIR)\\..\\..\\deps\\zlib-win64\\x64\\Release\\zlib.lib"
							}
						}]
					],
					"link_settings": {
						"libraries": [
							"<(archive_static.lib);<(zlib.lib)"
						]
					},
					"defines": [
						"LIBARCHIVE_STATIC"
					],
					"msvs_settings": {
						"VCCLCompilerTool": {
							"RuntimeLibrary": 0
						},
						"VCLinkerTool": {
							"IgnoreDefaultLibraryNames": ["libcmt"]
						}
					}
				}, {
					"libraries": [
						"../deps/libarchive/libarchive/libarchive.a"
					]
				}]
			]
		},
		{
			"target_name": "action_after_build",
			"type": "none",
			"dependencies": [ "<(module_name)" ],
			"copies": [
				{
					"files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
					"destination": "<(module_path)",
				}
			]
		}
	]
}