---
layout: page
title: Pythonプログラミング
permalink: /python/
---

## 環境設定など

* [環境設定]({% link _python/devenv-setup.md %})

## 数独ソルバーを作る

ここでは数独の本を写真で撮影して、それを画像認識することで、自動的に数独を解くソルバーを作ります。

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**入力画像**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**出力画像**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/sudoku/level_3.jpg" style="width: 100%;" %}</td>
  <td>{% include lightbox.html src="/public/images/sudoku/level_3_ans.jpg" style="width: 100%;" %}</td>
</tr>
</table>

* [バイナリデータの読み込み]({% link _python/read-binary.md %})
* [ロジスティック回帰 -その1-]({% link _python/logistic-regression-01.md %})
* [ロジスティック回帰 -その2-]({% link _python/logistic-regression-02.md %})
* [画像中の図形検出]({% link _python/figure-detection.md %})
* [数独を解く]({% link _python/solve-sudoku.md %})
* [(課題)画像中の数独問題を解こう]({% link _python/assignment-sudoku.md %})

## 深層学習による画像生成の基礎

* [畳み込みニューラルネット]({% link _python/convolutional-network.md %})
* [敵対的生成ネットワーク(GAN)]({% link _python/gan.md %})
* [条件付き画像生成 (CGAN)]({% link _python/conditional-gan.md %})
* [GANの学習安定化テクニック]({% link _python/stabilize-gan-training.md %})
* [(課題)GANによる高品質画像生成レース]({% link _python/assignment-gan.md %})
