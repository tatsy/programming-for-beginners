---
layout: post
title: 画像中の数独を解こう
---

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**入力画像**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**出力画像**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/sudoku/level_3_small.jpg" style="width: 100%;" %}</td>
  <td>{% include lightbox.html src="/public/images/sudoku/level_3_ans_small.jpg" style="width: 100%;" %}</td>
</tr>
</table>

これまでに学んだ内容を使い、以下の画像から数独の問題を抽出し、実際に答えを求めてみよう。レベルは3つあるので、自分の興味と余裕に応じた難易度を選んでほしい。

以下のヒントを参考に、少なくともレベル1のクリアを目指そう。

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**レベル1**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**レベル2**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**レベル3**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/sudoku/level_1_small.jpg" style="width: 100%;" %}</td>
  <td>{% include lightbox.html src="/public/images/sudoku/level_2_small.jpg" style="width: 100%;" %}</td>
  <td>{% include lightbox.html src="/public/images/sudoku/level_3_small.jpg" style="width: 100%;" %}</td>
</tr>
</table>

上記の入力データはサイズを落としてあるので、元々の画像は[こちら]({% link public/images/sudoku/inputs.zip %})からダウンロードすること。

### ヒント

処理の大きな流れは以下の通りである。

* 数独の外枠を検出
* 各マスの数字を画像認識する
* 得られた数字から数独を解く

なお、数字の画像認識に関しては自作のロジスティック回帰では不十分だと思われるので `scikit-learn` のロジスティック回帰を用いて良い(最初はこれを試そう)。ただ、残念ながらそれでも精度は不十分と思われるので、必要に応じて `tessearct` と呼ばれるライブラリを使うこと(CNNによる画像認識を予習しても良い)。

レベル2以降の画像は数独の外枠が歪んで写っているので、ホモグラフィー変換を使って四角形を正方形に直す必要があり、また画像にも余計なものが多く写り込んでいるので、より頑健な外枠検出のプログラムを作る必要がある。


## 補足: Tesseractの使い方

TesseractはGoogleが開発しているOCR用のライブラリで、通常はコマンドライン上で動作する。Python用のライブラリである`pytesseract`も公開されているので、pipなどでインストールして使う (`conda`では入らない)。

TesseractはNumPyではなくPillowと呼ばれる別の画像処理ライブラリ形式のデータを受け取る。1文字だけが写り込んだ画像から数字を検出するコードは以下のようになる。

```python
from PIL import Image
config = '--psm 10 --oem 3 -c tessedit_char_whitelist=0123456789'
text = pytesseract.image_to_string(Image.fromarray(image), lang='eng', config=config)
print(text) # 一文字のstringが得られる
```

## 補足: ホモグラフィー変換

ホモグラフィー変換(日本語では透視投影変換)とは、平面上の図形をピンホールカメラで撮影したときに、元々の図形を写真に映り込む図形の形に移す変換を指す。この変換は同次座標表現と呼ばれる座標の表現法を使うことで、4x4の行列として変換を定義できる。

OpenCVには変換前後の座標の組からホモグラフィー変換の行列を求める `getPerspectiveTransform` と、その行列を使って実際に画像を変換する `warpPerspective` が用意されている。また得られる行列の逆行列を使って画像を逆変換することで、ページ上部に示した画像のように実際の画像上に数字を並べることもできる。

