export ARCH=x86_64
export PATH=$(pwd)/x86_64-linux-android-4.9/bin/:$PATH


make -j6 all && sudo cp arch/x86/boot/bzImage /data/androidhack/projects/linux-tablet-tools/prebuilt/kernel

