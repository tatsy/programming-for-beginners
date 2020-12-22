---
layout: post
title: バイナリファイルの読み込み
---

## MNISTのダウンロード

MNIST (Mixed National Institute of Standards and Technology) とは同名の機関が作った手書き文字のデータベース。 32x32の白黒画像が学習用に60000枚、テスト用に10000枚含まれている。以下のウェブページからデータをダウンロード。

<http://yann.lecun.com/exdb/mnist/>{: target="_blank" }

* train-images-idx3-ubyte.gz:  training set images (9912422 bytes)
* train-labels-idx1-ubyte.gz:  training set labels (28881 bytes)
* t10k-images-idx3-ubyte.gz:   test set images (1648877 bytes)
* t10k-labels-idx1-ubyte.gz:   test set labels (4542 bytes) 

*注意: これらのファイルは `*.gz` 形式で圧縮されているので展開してから使う*

これらのファイルのうち `images-idx3` とあるものは画像の情報を `labels-idx1` とあるものは、各画像が数字のいくつを表しているのかを保存している。各ファイル内のデータのバイトオーダ (何バイト目に何が書いてあるか) はページ下部の「FILE FORMATS FOR THE MNIST DATABASE」に説明がある。

## バイナリデータを読む

Pythonではバイナリデータの読み込みは `struct` というパッケージを使う。

```python
import struct

# ファイルを開く (rbはread binaryの意味)
fp = open('binary_file.bin', 'rb')

# 4バイト読む
b = fp.read(4)

# バイトを整数に変換
# MNISTの先頭4バイトはファイル識別用のマジックナンバー
magic = struct.unpack('>i', b)
print(magic)

# ファイルを閉じる
fp.close()
```

上記のコードで `struct.unpack('>i', b)` とあるが、この中の `>i` の部分はリトルエンディアン (`>`) で、4バイト符号付き整数を読むということ。他のフォーマットについては[ドキュメント](https://docs.python.org/ja/3/library/struct.html){: target="_blank" }を見よう。

## MNIST内の画像を表示する

「FILE FORMATS FOR THE MNIST DATABASE」によれば画像ファイルは最初の4バイトがマジックナンバー (2051) で、その後、4バイト区切りで画像数 (6000)、画像の高さ (28)、画像の幅 (28)が入っている。

```python
# マジックナンバー
magic = struct.unpack('>i', fp.read(4))
# 画像数、高さ、幅 (一気に複数の数字をunpackすることもできる)
n_images, height, width = struct.unpack('>iii', fp.read(4 * 3))
```

それ以後は1枚1枚の画像の情報が書かれている。1枚の画像の各画素は白黒の値が0-255 (1バイト符号なし整数)で書かれていて、それが28x28=784画素分ある。故に画像1枚分のデータは784バイトになる。

```python
pixels = struct.unpack('>' + 'B' * 28 * 28, fp.read(1 * 28 * 28))
```

このままでは画像として表示できないので、これを **NumPy** の配列に変更して、形を変更しよう。

```python
pixels = np.asarray(pixels, dtype='uint8')
pixels = pixels.reshape((height, width))
```

試しに表示してみる。

```python
plt.imshow(pixels)
plt.show()
```

すると以下のような画像が出る。

{% include lightbox.html src="/public/images/read_binary/mnist_1.jpg" %}

色が変だが、これはMatplotLibのカラーマップが自動的に適用されたため。グレースケールの画像にしたければ、カラーマップを自分で指定する。

```python
plt.imshow(pixels, cmap='gray')
plt.show()
```

{% include lightbox.html src="/public/images/read_binary/mnist_2.jpg" %}

## 画像をタイル状に表示する

これは一工夫必要。まず100枚画像を読み込もう。

```python
images = []
for i in range(100):
    pixels = struct.unpack('>' + 'B' * 28 * 28, fp.read(1 * 28 * 28))
    pixels = np.asarray(pixels, dtype='uint8')
    pixels = pixels.reshape((height, width))
    images.append(pixels)
```

これを10x10のタイル状に表示したい。MatplotLibは描画のウィンドウを表す `figure` と、実際に画像やグラフが表示される `axis` があり、`figure`に`axis`を追加していく形で複数の画像を表示できる。

```python
fig = plt.figure(figsize=(20, 20))
for i in range(10):
    for j in range(10):
        index = i * 10 + j
        image = images[index]

        # 10x10のタイルのindex番目のaxisという意味
        # 番号は1から始まるので1を足しておく
        ax = fig.add_subplot(10, 10, index + 1)
        # グレースケールで表示
        ax.imshow(image, cmap='gray')
        # 画像の周りにある目盛りを消す
        ax.axis('off')

plt.show()
```

{% include lightbox.html src="/public/images/read_binary/mnist_tile.jpg" %}