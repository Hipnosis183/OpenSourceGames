all: build.xml
	if [ ! -f libs/armeabi/libSDL2.so ]; then echo "-> Install SDL* in libs/armeabi/"; exit 1; fi
	(cd ../cross-android/ && make)
	cp -a ../cross-android/src/freedink libs/armeabi/libmain.so
	arm-linux-androideabi-strip libs/armeabi/*.so
	ant debug

install:
#	ant installd
	adb install -r bin/freedink-debug.apk
	adb shell am start -a android.intenon.MAIN -n org.freedink/org.freedink.FreeDink

update:
	adb shell am force-stop org.freedink
	adb push ../cross-android/src/freedink /data/data/org.freedink/lib/libmain.so
	adb shell am start -a android.intenon.MAIN -n org.freedink/org.freedink.FreeDink

update-testiogfxdisplay:
	(cd ../cross-android/src/ && make TestIOGfxDisplay)
	adb shell am force-stop org.freedink
	adb push ../cross-android/src/TestIOGfxDisplay /data/data/org.freedink/lib/libmain.so
	adb shell am start -a android.intenon.MAIN -n org.freedink/org.freedink.FreeDink
	adb logcat

SDK_TARGET=12
build.xml:
	android update project --name freedink --path . --target android-$(SDK_TARGET)

clean:
	-ant clean
	rm -f build.xml
	rm -f local.properties proguard.cfg proguard-project.txt


distclean: clean
	rm -rf libs/
