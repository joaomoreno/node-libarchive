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
				"libarchive/libarchive"
			],
			"conditions": [
				["OS == 'mac'", {
					"libraries": [
						"../libarchive/libarchive/libarchive.a"
					]
				}],
				["OS == 'win'", {
					"link_settings": {
						"libraries": [
							"<(PRODUCT_DIR)\\..\\..\\libarchive\\libarchive\\Debug\\archive_static.lib;C:\\Program Files\\GnuWin32\\lib\\zlib.lib"
						]
					},
					"defines": [
						"LIBARCHIVE_STATIC"
					],
					"msvs_settings": {
						"VCLinkerTool": {
							"IgnoreDefaultLibraryNames": ["libcmt"]
						}
					}
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
				},
				{
					"files": [ "C:\\Program Files\\GnuWin32\\bin\\zlib1.dll" ],
					"destination": "<(module_path)",
				}
			]
		}
	]
}