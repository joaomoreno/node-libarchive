{
	"targets": [
		{
			"target_name": "archive",
			"sources": [
				"src/main.cc",
				"src/read.cc"
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