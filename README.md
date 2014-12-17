# Install

	npm install libarchive

# Build

## All platforms

	git submodule update --init
	npm install

## OS X

	cd deps/libarchive
	cmake .
	make archive_static
	cd ../..
	./node_modules/.bin/node-pre-gyp rebuild

## Linux

	cd deps/libarchive
	cmake .

Edit `deps/libarchive/libarchive/CMakeFiles/archive_static.dir/flags.make` and add `-fPIC` to `CFLAGS`.

	make archive_static
	cd ../..
	./node_modules/.bin/node-pre-gyp rebuild

## Windows (x64)

	MSBUILD=%ProgramFiles(x86)%\MSBuild\12.0\Bin\MSBuild

	pushd deps\zlib-win64
	"%MSBUILD%" /p:Platform=x64 /p:Configuration=Release zlib.msvs2013.vcxproj
	popd

	pushd deps\libarchive
	cmake -A x64 -G "Visual Studio 12 2013" -D ZLIB_LIBRARY=..\zlib-win64\x64\Release\zlib.lib -D ZLIB_INCLUDE_DIR=..\zlib-win64 .
	"%MSBUILD%" /p:Configuration=Release libarchive\archive_static.vcxproj
	popd

	.\node_modules\.bin\node-pre-gyp rebuild

## Windows (ia32)

	MSBUILD=%ProgramFiles(x86)%\MSBuild\12.0\Bin\MSBuild

	pushd deps\zlib-win64
	"%MSBUILD%" /p:Platform=win32 /p:Configuration=Release zlib.msvs2013.vcxproj
	popd

	pushd deps\libarchive
	cmake -A win32 -G "Visual Studio 12 2013" -D ZLIB_LIBRARY=..\zlib-win64\Release\zlib.lib -D ZLIB_INCLUDE_DIR=..\zlib-win64 .
	"%MSBUILD%" /p:Configuration=Release libarchive\archive_static.vcxproj
	popd

	.\node_modules\.bin\node-pre-gyp --target_arch=ia32 rebuild
