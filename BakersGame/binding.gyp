{
	"targets": [
		{
			"target_name": "addon",
			"cflags!" : [ "-fno-exceptions" ],
			"cflags_cc!" : [ "-fno-exceptions" ],
			"cflags" : [ "-Wall", "-DACTIVE_LOGGING" ],
			"cflags_cc" : [ "-DACTIVE_LOGGING" ],
			"include_dirs" : [
				"include", "/usr/local/include/node"
			],
			"sources": [
				"src/dbSupport.cpp", "src/nodeExports.cpp",
				"src/nodeSupport.cpp", "src/SolitaireBoard.cpp",
				"src/SolitaireSolver_BakersGameEasy.cpp", "src/SolitaireSolver_BakersGameStnd.cpp",
				"src/SolitaireSolver.cpp", "src/solverSupport.cpp",
				"src/sqlite3.c", "src/v8Support.cpp" 
			],
			'conditions' : [
				[ 'OS=="mac"', { "xcode_settings" : { "GCC_ENABLE_CPP_EXCEPTIONS" : "YES" } } ]
			]
		}
	]
}
