# LinuxGUI


## 環境
ubuntu22.04LTS(VMWare Workstation 16 Player上で動かしています．メモリは16GB，プロセッサのコア数は2，ハードディスクは20GB，OSは64bit)

ホストOSはwindows10(メモリ32GB、64bit)を用いています

##  文字列ライブラリのインストール

文字列描写には[FreeTypeライブラリ](https://freetype.org/)を用います．

以下がインストール等の環境設定のコマンドです.筆者はバージョンfreetype-2.10.4を入れました.バージョンの数字は適宜変更してください．

```
$wget https://sourceforge.net/projects/freetype/files/freetype2/2.10.4/freetype-2.10.4.tar.gz
$tar xvf freetype-2.10.4.tar.gz
$cd freetype-2.10.4
$./configure
$make
$sudo make install
```


