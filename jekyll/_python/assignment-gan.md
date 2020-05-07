---
layout: post
title: GANによる高品質画像生成レース
---

ここまでの内容をベースにより良い画像生成を目指しましょう。

## データ・セット

[Oxford flower dataset](https://www.robots.ox.ac.uk/~vgg/data/flowers/){: target="_blank" }の102カテゴリを使って行います。このデータセットは必ずしも正方形の画像が入っているわけではないので、画像の中心でクロップして128x128のサイズに正規化したものをお渡しします。

## 評価方法

上記のような、10x10の画像タイル1枚を提出してもらいます。名前が分からない状態で、それを参加者に見せて、投票で優勝を決めます。

## ルール

* マシンによる有利・不利を避けるため、学習はColabを用いて行うこととします。
  * Colabはリロードなしで90分、一定間隔でリロードしても12時間までしか使えません。
  * 一定間隔でリロードさせるためには、ブラウザ拡張のツールなどを使うと良いでしょう。
* きれいな画像が出るまで乱数をサンプルすることは認めますが、きれいな画像が出るサンプルを100個集めて、それを結果の画像に使うのは禁止とします。

## 始め方

* Google Colaboratoryのページを開き、適当なノートを作成する。その後、Google Driveに移動し「Colab Notebooks」というフォルダが作成されていることを確認する。
* 「Colab Notebooks」の中にOxford flower datasetを展開したものを「OxfordFlower」というフォルダ名で配置する
* Colab上で学習を行う[サンプルプログラム (単純なDCGAN)](https://drive.google.com/open?id=1DOfgEWJ7MFdzDhy4K2bVy9v_fIvye40d){: target="_blank" } をダウンロードし、同じく「Colab Notebooks」に配置する。
* Colabで上記のサンプルプログラムを開き、実行してみる。上手く実行できたら、以後、これを改良して、より良い画像が生成されるように改良する。

## 参考: GANの学習を安定化させる技術

DCGANのような画像生成の技術が出始めてから数年で、非常に多くの学習安定化技術が提案されています。前回[GANの学習安定化テクニック]({% link _python/stabilize-gan-training.md %}) で紹介したfeature matchingやminibatch discrimination意外にも、多くの技術をWeb上で探すことができるので、自分なりに調べて実装してみると、思いがけず精度が向上することがあるかもしれません。

比較的多いものは[Wasserstein GAN](https://arxiv.org/abs/1704.00028){: target="_blank" }や[Least-squares GAN](https://arxiv.org/abs/1611.04076){: target="_blank" }のようにロス関数を工夫することで学習を安定化させる方法です。この他にも[Weight normalization](https://arxiv.org/abs/1602.07868)や[Spectral normalization](https://arxiv.org/abs/1802.05957){: target="_blank" }のように正規化のやり方を工夫するものや、[Self-attention GAN](https://arxiv.org/abs/1805.08318){: target="_blank" }のようにネットワーク構造自体を工夫するものなどがあります。これらのテクニックを網羅的に解説したものとしては、以下の資料が参考になると思います。

* GANを学習させる際の14のテクニック (NIPS2016チュートリアルの日本語訳) \\
  <https://qiita.com/underfitting/items/a0cbb035568dea33b2d7>{: target="_blank" } \\
  (少し内容が古いので鵜呑みにし過ぎない方が良い)
* Shall we GANs? (SSII 2019 チュートリアル) \\
  <https://www.slideshare.net/SSII_Slides/ssii2019ts11>{: target="_blank" }
