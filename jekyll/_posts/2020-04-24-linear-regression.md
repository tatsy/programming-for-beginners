---
layout: post
title: 線形回帰
categories: [python]
---

## ロジスティック回帰の概要

[バイナリファイルの読み込み]({% post_url 2020-04-24-read-binary%})で読み込んだMNISTのデータを使って、単純な線形モデルで数字を予想する。

一般に線形回帰、といったときにはデータベクトル $\mathbf{x}$ を入力として、何らかの推定値 $\mathbf{y}$ を線形式、すなわち

$$
\mathbf{y} = \mathbf{Ax} + \mathbf{b}
$$

という式で関係付ける。出力の推定値が何らかの確率であれば、これにsigmoid関数やsoftmax関数をつけて、

$$
\begin{aligned}
    \mathbf{y} = \text{sigmoid}(\mathbf{Ax} + \mathbf{b}) \\
    \mathbf{y} = \text{softmax}(\mathbf{Ax} + \mathbf{b}) \\
\end{aligned}
$$

のようにする。特にsigmoid関数を使うものをロジスティック回帰という(softmaxを使うものも含む場合もある)。なお、sigmoid関数、softmax関数はそれぞれ次のように表される。

$$
\begin{aligned}
    \text{sigmoid}(x) = \frac{1}{1 + e^{-x}} \\
    \text{softmax}(\mathbf{x})_i = \frac{e^{x_i}}{\sum_{k=1}^d e^{x_k}}
\end{aligned}
$$ 

機械学習における推定のタスクにおいて、二者分類のときにはsigmoid関数が、多者分類のときにはsoftmax関数が使われるのが一般的である。今回のMNISTは出力の推定値が **画像$\mathbf{x}$が数字の$i$を表す確率$y_i$**を求めるのが目標なのでsoftmax関数を使う。

## scikit-learnによるロジスティック回帰

scikit-learnはとても簡単。前回の内容に従って画像を読み込むと、画像とラベルを格納したNumPyの配列はそれぞれ (60000, 28, 28, 1) と (60000, 1) になっているはずである。前者は各画素に0-255の画素値が含まれており、後者は、その画像がどの数字なのかを表す整数が格納されている。

上記の通り、線形モデルではベクトルを扱うので、画像の画素値を1列に並べ直してベクトル化しよう。

```python
n_images = len(images)
X = images.reshape((n_images, -1))
```

さらに、配列の型を1バイト符号なし整数から32ビット浮動小数に変換しておく。

```python
X = X.astype('float32')
```

あとは[ドキュメント](https://scikit-learn.org/stable/modules/generated/sklearn.linear_model.LogisticRegression.html){: target="_blank" }の内容に従い`LogisticRegression`クラスを使って識別器を学習する。

```python
from sklearn.linear_model import LogisticRegression
clf = LogisticRegression(random_state=0, multi_class='multinomial').fit(X, y)
```

少し時間がかかるが、しばらくすると学習が終わる。なお上記のコード内での `y` は整数型のラベルを格納したNumPyの配列で良い(分かりやすくするために名前を変更した)。

## テストデータに対する予測

学習が終わったら、次はテストデータを読み込んで、正解率を求める。上記の `clf` というオブジェクトに対して、以下のようなコードを呼び出すと、ラベルが自動で計算される。

```python
pred_labels = clf.predict(X_test)
```

正解率は、このラベルがテストのラベルとどのくらい近いかを計算すれば良いのだから、正解かどうか(0 or 1)の平均を全てのテストデータに対して計算すれば良い。

```python
acc = (pred_labels == y_test).mean()
```

ただしく学習できていれば90%程度の正解率となるはずである。

## ロジスティック回帰を再現する

上記の通り、ロジスティック回帰による識別モデルは、線形変換とsoftmax関数の組み合わせである。softmax関数にはパラメータ変数は含まれないので、回帰モデル自体のパラメータは行列 $\mathb{A}$ とバイアスベクトル $\mathbf{b}$ のみである。
