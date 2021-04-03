{
	"targets": [
		{
			"target_name": "addon",
			"cflags!" : [ "-fno-exceptions" ],
			"cflags_cc!" : [ "-fno-exceptions" ],
			"cflags" : [ "-Wall" ],
			"include_dirs" : [
				"include", "/usr/local/include/node"
			],
			"sources": [
				"src/BoardFunctions.cpp", "src/CommonElements.cpp", "src/nodeExports.cpp"
			],
			'conditions' : [
				[ 'OS=="mac"', { "xcode_settings" : { "GCC_ENABLE_CPP_EXCEPTIONS" : "YES" } } ]
			]
		}
	]
}
