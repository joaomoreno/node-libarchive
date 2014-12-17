$MSBUILD="${env:ProgramFiles(x86)}\MSBuild\12.0\Bin\MSBuild"

pushd deps\zlib-win64
git clean -xfd .
.$MSBUILD /p:Platform=win32 /p:Configuration=Release zlib.msvs2013.vcxproj
popd

pushd deps\libarchive
git clean -xfd .
cmake -A win32 -G "Visual Studio 12 2013" -D ZLIB_LIBRARY=..\zlib-win64\Release\zlib.lib -D ZLIB_INCLUDE_DIR=..\zlib-win64 .
.$MSBUILD /p:Configuration=Release libarchive\archive_static.vcxproj
popd

.\node_modules\.bin\node-pre-gyp --target_arch=ia32 rebuild
.\node_modules\.bin\node-pre-gyp --target_arch=ia32 package
.\node_modules\.bin\node-pre-gyp --target_arch=ia32 publish