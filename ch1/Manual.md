

##  文字列ライブラリのインストール

文字列描写には外部ライブラリである[FreeType](https://freetype.org/)を用います．

以下がインストール等の環境設定のコマンドです.筆者はバージョンfreetype-2.10.4を入れました.バージョンの数字は適宜変更してください．

```
$wget https://sourceforge.net/projects/freetype/files/freetype2/2.10.4/freetype-2.10.4.tar.gz
$tar xvf freetype-2.10.4.tar.gz
$cd freetype-2.10.4
$./configure
$make
$sudo make install
```

## 実行
ソースコードを試す際には自己責任で行いましょう．


to compile main.cpp
```
$g++ main.cpp -I /usr/local/include/freetype2 -I /usr/include/libpng16 -lfreetype -lm -std=c++17
```

to run
```
$ sudo ./a.out
```

画面によっては文字の位置がずれることがあります．
筆者が試した環境では幅800px,高さ480pxでした．



## ラズベリーパイで実行する場合
main.cppのフォントの設定をいじりましょう.(30行目付近)
```
//ラズベリーパイOS用はこちらを採用
const std::string FONT_PATH="/usr/share/fonts/truetype/piboto/Piboto-Light.ttf";

//こちらはUbuntu用なのでコメントアウト
//const std::string FONT_PATH="/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf";
```
Ctrl+Alt+F3でGUIからCUI

Ctrl+Alt+F7でCUIからGUIに設定できます．

ディスプレイとピクセル数などの環境次第で色や文字の位置が異なる可能性があります．

