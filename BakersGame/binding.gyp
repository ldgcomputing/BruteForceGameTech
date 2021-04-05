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
				"src/nodeExports.cpp", "src/nodeSupport.cpp", "src/SolitaireBoard.cpp",
				"src/SolitaireSolver_BakersGameEasy.cpp", "src/SolitaireSolver_BakersGameStnd.cpp",
				"src/SolitaireSolver.cpp" 
			],
			'conditions' : [
				[ 'OS=="mac"', { "xcode_settings" : { "GCC_ENABLE_CPP_EXCEPTIONS" : "YES" } } ]
			]
		}
	]
}
