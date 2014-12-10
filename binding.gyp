{
	"targets": [
		{
			"target_name": "archive",
			"sources": [
				"src/main.cc",
				"src/read.cc",
				"src/writer.cc"
			],
			"conditions": [
				["OS == 'mac'", {
					"include_dirs": [
						"libarchive/libarchive"
					],
					"libraries": [
						"../libarchive/libarchive/libarchive.a"
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