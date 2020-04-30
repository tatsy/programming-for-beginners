---
layout: post
title: 画像中の図形検出
---

今回はOpenCVを用いて画像中から図形を検出する。図形の検出は、画像からのエッジの情報を取り出す処理と、エッジ情報を元に境界線を認識する処理、境界線から図形の形状を分類する処理の三つに分けられる。

## エッジの検出

画像処理におけるエッジ抽出で広く用いられているのはSobelフィルタとCanny法だろう。

### Sobelフィルタ

Sobelフィルタは画像のある方向に対する勾配をとるときに、その方向と直交する方向に平滑化をかけることで、ノイズの影響を抑えつつエッジを検出する方法である。画像処理の教科書だけでなくWeb上にも多くの解説があるので、各自調べて見てほしい。

### Canny法

Canny法はSobelフィルタと比べると少し処理が複雑で、いくつかの処理の組み合わせからなる。

まず画像に対してガウシアンフィルタをかけてノイズの影響を抑える。その後、画像の縦方向と横方向について画像の勾配を計算する。この処理には多くの場合、Sobelフィルタが用いられる。Sobelフィルタでは、一定の幅を持つエッジが検出されるが、Canny法は勾配の方向に沿って、勾配の値が最大となる場所だけをエッジ上の画素の候補とする。

その後、候補となる画素の中で勾配が一定以上となる場所を閾値処理により取り出すことで、最終的なエッジが得られる。なお、最後の閾値処理が一般的な閾値処理ではなくヒステリシス閾値処理と呼ばれる二つの閾値を用いた適応的な閾値処理である。これについても各自、どのようなものか調べてみてほしい(この二つの閾値がOpenCVの関数の引数になっている)。

以下がOpenCVでSobelフィルタとCanny法によりエッジ画像を得るプログラムと、その結果である。なお、以下のコードでは結果を見やすくするため、Sobelフィルタの結果を強調した後、二値化の処理をいれている。

```python
# 画像をグレースケールにする
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# Sobelフィルタ
dx = cv2.Sobel(gray, cv2.CV_8U, 1, 0)
dy = cv2.Sobel(gray, cv2.CV_8U, 0, 1)
sobel = np.sqrt(dx * dx + dy * dy)
sobel = (sobel * 128.0).astype('uint8')
_, sobel = cv2.threshold(sobel, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
# Canny法
canny = cv2.Canny(gray, 100, 200)
```

<table class="images">
<tr>
  <td>{{ '**入力画像**' | markdownify }}</td>
  <td>{{ '**Sobelフィルタ**' | markdownify }}</td>
  <td>{{ '**Canny法**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/figure_detection/figures.jpg" %}</td>
  <td>{% include lightbox.html src="/public/images/figure_detection/sobel.jpg" %}</td>
  <td>{% include lightbox.html src="/public/images/figure_detection/canny.jpg" %}</td>
</tr>
</table>

### エッジの統合

結果を見てもらうと分かる通り、上記の入力画像は図形の輪郭線が幅を持っており、そのせいでエッジが二重線になっていることが分かる。もちろん、これはこれで間違いとは言えないのだが、今回のケースではエッジが内側の線と外側の線の間で曖昧に検出されており、この後の輪郭線検出に影響を及ぼす可能性がある。この問題は自然画像を入力とする場合には、より顕著となる。

そこで、輪郭線を抽出する前に、モルフォロジー演算によりエッジの統合を行っておく。モルフォロジー演算は一般的には二値画像を入力として、白色の領域を広げたり狭めたりする操作である(モルフォロジー演算の数学的な意味とは解釈が異なるので注意)。今回は白色領域を広げる処理であるdilationと白色領域を狭める処理であるerosionを連続して施すことでエッジの統合を行う。

モルフォロジー演算は構造要素と呼ばれる小さな画像のようなものを入力の二値画像にしたがって繰り返す操作に対応する。したがって、OpenCV(やMATLABなどの他のライブラリ)では、モルフォロジー演算の関数は二値画像と構造要素を引数にとる。OpenCVを用いる場合には、dilationとerosionはそれぞれ以下のようなコードで実現される。

```python
res_erode = cv2.erode(binary, np.ones((5, 5), dtype=binary.dtype))
res_dilate = cv2.dilate(binary, np.ones((5, 5), dtype=binary.dtype))
```

エッジを統合する場合、エッジが白色の領域に検出されているとすると、先にdilation、次にerosionの順序で処理を施す。dilationにより二重のエッジのそれぞれが太くなり、重なることで、太い1本のエッジとなる。その後、erosionをかけることで、そのエッジが細くなり、1本の細いエッジとなる。

<table class="images">
<tr>
  <td>{{ '**dilation後**' | markdownify }}</td>
  <td>{{ '**dilation + erosion後**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/figure_detection/dilate.jpg" %}</td>
  <td>{% include lightbox.html src="/public/images/figure_detection/unified_edges.jpg" %}</td>
</tr>
</table>

上記の結果の通り、dilationとerosionを組み合わせることで二本の細いエッジが一本のエッジに統合されていることが分かる。

## 輪郭線の検出

ここまでに検出したエッジの情報を使って輪郭線を抽出する。輪郭線の抽出にはOpenCVの`findContours`という関数を用いる。余談だが、この関数には1985年にSuzukiさんとAbeさんという日本人が提案したアルゴリズムが実装されている(*1)。

(*1) S. Suzuki and K. Abe, "Topological structural analysis of digitized binary images by border following", Computer Vision, Graphics and Image Processing", 1985.

このアルゴリズムは二値画像(1がエッジで0が背景の色とする)をスキャンラインオーダー (左上から右下の順番) に走査していき、1が見つかったら、そのエッジに階層番号を付与する。階層番号は例えば二重丸のような物があったときに、外側の輪郭が1、内側の輪郭が2になるような番号である。この階層番号は最初1に初期化しておいて、1の画素(エッジと思われる画素)が見つかったら、そこに今の階層番号を記録します。以後1の画素が連続して見つかる間は同じ番号を記録していきますが、1のあとに