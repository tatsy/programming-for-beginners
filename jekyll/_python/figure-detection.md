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

なおdilationのあとにerosionをする操作をclosingとも呼び、以下のコードでも同じことができる。

```python
res_close = cv2.morphologyEx(binary, cv2.MORPH_CLOSE, np.ones((5, 5), dtype=binary.dtype))
```

## 輪郭線の検出

ここまでに検出したエッジの情報を使って輪郭線を抽出する。輪郭線の抽出にはOpenCVの`findContours`という関数を用いる。この関数には1985年にSuzukiさんとAbeさんという日本人が提案したアルゴリズムが実装されている(*1)。

(*1) S. Suzuki and K. Abe, "Topological structural analysis of digitized binary images by border following", Computer Vision, Graphics and Image Processing", 1985.

このアルゴリズムは二値画像(1がエッジで0が背景の色とする)をラスタスキャン (最初の列を左から右に見ていき、右端に到達したら、左から右に見ていく、以後繰り返し) していき、1が見つかったら、その画素から始まる輪郭線を追跡していく。

輪郭線の追跡はある画素の周りを時計回りに見ていき、初めて1となる画素にスキャン位置を移動する、という操作を繰り返すことで行われる。端点に到達するか、元の位置の画素に戻ってきたら、再びラスタスキャン順に値が1の画素で、まだどの輪郭にも含まれていない画素を探し、以後、同様の処理を繰り返していく。

OpenCVの`findContours`は3つの引数を取り、第1引数が入力の二値画像、第2引数が検出される輪郭線の階層関係、第3引数が抽出する輪郭線をどのように近似表現するかを表す。代表的な使い方は、

```python
_, contours, _ = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
```

になる。`cv2.RETR_EXTERNAL` は二重丸のような階層的な輪郭線が現れたときに最も外側の閉曲線のみを抽出する。また`cv2.CHAIN_APPROX_SIMPLE`は抽出される輪郭線が水平方向、垂直方向、45度傾き, 135度の傾きに直線的につながるときに、それらを端点だけで近似する。

この他の使い方については公式の[ドキュメント](https://docs.opencv.org/2.4/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html#findcontours){: target="_blank" }を参考にしてほしい。

## 輪郭線の描画

上記の方法で検出された輪郭線は`findContours`と対になる`drawContours`という関数で描画できる。

```python
cv2.drawContours(image, contours, -1, (255, 0, 0), 3, cv2.LINE_AA)
plt.imshow(image)
plt.show()
```

こちらも詳しい使い方は公式の[ドキュメント](https://docs.opencv.org/2.4/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html#drawcontours){: target="_blank" }を参考にしてほしい。

実際に上記の方法で検出した輪郭線を描画してみたものが以下である。見ての通り、各図形の外側と画像自体の輪郭に輪郭線が検出されていることがわかる。

{% include lightbox.html src="/public/images/figure_detection/draw_contours.jpg" %}

## 輪郭線の分類

各輪郭線がどのような図形か(円か？多角形か？そうなら何角形か？)を判別するには、輪郭線を多角形として近似して、その時の角の数を調べればいい。

上記の通り、`findContours`で検出された輪郭線(`cv2.CHAIN_APPROX_SIMPLE`を使った場合)は特定の向きの直線のみを端点で近似している。そのため、現在検出されている多角形は、必ずしも角の点だけで表されているわけではない (`contours[i]`の長さを見てみると分かる)。

そこで、検出されている輪郭線を更に粗く角の点だけで近似することを考える。これにはOpenCVの`cv2.approxPolyDP`という関数が使える。この関数には[Ramer-Douglas-Peucker法](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm){: target="_blank" }というアルゴリズムが使われている。

このアルゴリズムは、ある図形を適当な離散点(開曲線なら端点)を選んでその間を直線で結んだときに、最も離れている点を順次追加してポリライン(直線を結んで曲線を近似したもの)を更新していく。最も離れた点までの距離が一定以下になったら処理を終える。閉曲線の場合には、すべての候補となる点の中で最も離れた2点を見つけ、その二点を直線で結んで、二点の上部と下部を別々のポリラインで近似する。

それでは`cv2.findContours`で検出した各輪郭線について`cv2.approxPolyDP`を用いて近似してみる。

```python
for cnt in contours:
    # 輪郭線の長さを計算
    arclen = cv2.arcLength(cnt, True)
    # 輪郭線の近似
    approx = cv2.approxPolyDP(cnt, 0.01 * arclen, True)
    # 何角形かを見てみる
    print(len(approx))
    # 輪郭線の描画
    cv2.drawContours(image, [approx], -1, (255, 0, 0), 3, cv2.LINE_AA)
```

上記のコードでは`cv2.arcLength`を使って曲線の長さを計算し、その長さの100分の1の精度を持つように輪郭線を近似している。この精度はあまり細かすぎると角が増え、粗過ぎても角が減ってしまうので、多少の調整が必要ではある。`print`で出力された内容を見てみると6や4といった角の数と思われる数字が得られていることが分かる。

角の数が正しく得られていることが分かったら、その角の数に基づいて、画像中にラベルを入れてみよう。OpenCVでテキストを画像中に入れるには`cv2.putText`を使う。ラベルを入れるコードは例えば次のようになるだろう。

```python
n_gon = len(approx)
text = 'unknown'
if n_gon > 10:
    text = 'circle'
elif n_gon == 6:
    text = 'hexagon'
elif n_gon == 5:
    text = 'pentagon'
elif n_gon == 4:
    text = 'rectangle'
elif n_gon == 3:
    text = 'triangle'
    
font = cv2.FONT_HERSHEY_SIMPLEX
position = np.asarray(approx).reshape((-1, 2)).max(axis=0).astype('int32')
px, py = position
cv2.putText(result, text, (px + 10, py + 10), font, 1.0, (255, 255, 255), 2, cv2.LINE_AA)
```

正しくプログラムがかけていれば、以下のような画像が得られるはずである。

{% include lightbox.html src="/public/images/figure_detection/detection.jpg" %}

