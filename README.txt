# How to build

## All platforms

	git submodule init --update
	npm install

## OS X

	cd deps/libarchive ; cmake . ; make archive_static
	./node_modules/.bin/node-pre-gyp rebuild

## Windows (x64)

	MSBUILD=%ProgramFiles(x86)%\MSBuild\12.0\Bin\MSBuild

	pushd deps\zlib-win64
	"%MSBUILD%" /p:Platform=x64 zlib.msvs2013.vcxproj
	popd

	pushd deps\libarchive
	cmake -A x64 -G "Visual Studio 12 2013" -DZLIB_LIBRARY=..\zlib-win64\x64\Debug\zlib.lib -DZLIB_INCLUDE_DIR=..\zlib-win64 .
	"%MSBUILD%" libarchive\archive_static.vcxproj
	popd

	.\node_modules\.bin\node-pre-gyp rebuild

## Windows (ia32)

	MSBUILD=%ProgramFiles(x86)%\MSBuild\12.0\Bin\MSBuild

	pushd deps\zlib-win64
	"%MSBUILD%" /p:Platform=win32 zlib.msvs2013.vcxproj
	popd

	pushd deps\libarchive
	cmake -A win32 -G "Visual Studio 12 2013" -DZLIB_LIBRARY=..\zlib-win64\Debug\zlib.lib -DZLIB_INCLUDE_DIR=..\zlib-win64 .
	"%MSBUILD%" libarchive\archive_static.vcxproj
	popd

	.\node_modules\.bin\node-pre-gyp --target_arch=ia32 rebuild
