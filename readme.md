## QUICK-VGL
#### QuickVGL is a fast development for lvgl









### Build

#### Help
```shell
cd QuickVGL
./build.sh
```

#### X86 compile
```shell
cd QuickVGL
./build.sh -p x86 -d Debug

ls -lh output/lib
ls -lh output/bin
```

#### Cross Compile
```shell
PATH_AND_PREFIX_OF_TOOLCHAIN=arm-linux-gnueabihf-

cd QuickVGL
./build.sh -p ssd202 -d Release -t ${PATH_AND_PREFIX_OF_TOOLCHAIN}

ls -lh output/lib
ls -lh output/bin
```
