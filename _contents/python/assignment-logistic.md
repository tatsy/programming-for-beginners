---
layout: post
title: ロジスティック回帰の改善
---

ここまでの内容をベースにして、より識別精度の高いロジスティック回帰を目指そう。

## データセット

前回の[ロジスティック回帰 -その2-]({% link _contents/python/logistic-regression-02.md %})で使用したMNISTの亜種として、**日本語の崩し字**を含んだKMNIST (Kuzushiji-MNIST)というデータセットがある。

<https://github.com/rois-codh/kmnist>{: target="_blank" }

このデータセットでは10種類の平仮名に対して、オリジナルのMNISTと同様に60000文字の訓練データと10000文字のテストデータが与えられている。

{% include lightbox.html src="/public/images/logistic/kmnist_examples.jpg" %}

## 評価方法

提出されたプログラムを同環境で実行して、その精度を競う。ただし、プログラムを提出する際には、IPython NotebookではなくPythonの`*.py`ファイルでプログラムを作成し、

```shell
$ python train_kmnist.py
```

というコマンドでプログラムは走り出すようにすること。この際、KMNISTの訓練データ、訓練ラベル、テストデータ、テストラベルを含む8つのファイルが`kmnist`というフォルダ内に設置されているものとする。

## ルール

この際、PyTorchやTensorFlowなどの自動微分を伴う深層学習のライブラリを使用することは禁止とし、あくまでNumPyを用いた行列計算や自分で計算した導関数等に基づいて実装をすること。

また、あくまで今回の課題はロジスティック回帰を用いるものとし、隠れ層を含むようなニューラルネットを使用することは禁止とする。

実行にあたっては、学習にかかる時間が20分を超えないようにすること (評価に使う実行環境は別途説明する)。

## 始め方

* [ロジスティック回帰 -その2-]({% link _contents/python/logistic-regression-02.md %})のコードを[GitHub](https://github.com/tatsy/programming-for-beginners/blob/master/_programs/python/logistic/logistic_regression_02.ipynb)からダウンロードし、Jupyter NotebookからPythonソースコード (`*.py`)形式でエクスポートする。
* 適宜、ファイルのパスをKMNISTのものに置き換えて学習、評価を行う

## ヒント

高い識別率を実現するためのヒントとして、以下の内容を検討してみると良い。

* パラメータの初期化方法の改善
* パラメータに対する正則化項(weight decay)の追加
* データの正規化や拡張 (ノイズをのせる、回転させる)
* 確率的最急降下法の発展手法(RMSprop, Adamax, Adamなど)を使う
