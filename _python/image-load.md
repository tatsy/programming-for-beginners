---
layout: post
title: 画像の読み込み・表示・保存
---

## OpenCVを使った画像のロード

* ライブラリのインポート

```python
import os
import numpy as np
import cv2
import matplotlib.pyplot as plt
```

* 画像を読み込む

```python
image = cv2.imread('lena.png', cv2.IMREAD_COLOR)
```

* 画像の表示

```python
plt.imshow(image)
plt.show()
```

Jupyter Labで新しいIPython Notebook (*.ipnbファイル) を作って、上記のコードを打ち込んでみよう。


## 画像の色変換

OpenCVは色のチャンネルがBGR (青, 緑, 赤) の順になっている。これを入れ替える。

```python
image = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
```

もう一度表示してみる。

画像の色表現には色々なものがある。それぞれは何が違うのだろうか？調べてみよう。

* HSV色空間
* YCbCr色空間
* Lab色空間
* XYZ色空間


## 画像のサイズを変える

```python
# 大きさを指定してサイズを変える
resized_image = cv2.resize(image, (256, 320))
# 比率を指定してサイズを変える
resized_image = cv2.resize(image, None, fx=0.5, fy=0.5)
```

## 画像の保存

```python
cv2.imwrite('lena_resized.png', resized_image)
```