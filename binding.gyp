{
	"targets": [
		{
			"target_name": "hello",
			"sources": [
				"src/main.cc",
				"src/node-archive.cc"
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
		}
	]
}