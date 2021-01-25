---
layout: default
title: Home
permalink: /
---

## はじめに

このサイトは研究室の新入生が、Pythonを使った機械学習と、C++を使った形状処理の基礎について勉強するために作成いたしました。

執筆にあたっては、読者がPythonとC++(あるいはC言語)での最低限のプログラミング経験を持っていることを前提にしておりますので、少しハードルが高いと感じる場合には、入門書等の基礎について勉強した後にお読みいただければ幸いです。

もし、サイトの内容に誤りなどを見つけられました場合には、著者に直接ご連絡をいただくか、GitHubのissueないしpull requestを立ち上げていただければ幸いです。

<https://github.com/tatsy/programming-for-beginners>{: target="_blank" }

## 環境設定

* [Gitの設定]({% link _contents/setup/git.md %})
* [シェルの設定]({% link _contents/setup/shell.md %})
* [Python開発環境の設定]({% link _contents/setup/python.md %})
* [C++開発環境の設定]({% link _contents/setup/cpp.md %})

## Pythonプログラミング

### ロジスティック回帰

ここでは回帰モデルの基本ともいえるロジスティック回帰を用いた、手書き文字の認識を目指す。

* [バイナリデータの読み込み]({% link _contents/python/read-binary.md %})
* [ロジスティック回帰 -その1-]({% link _contents/python/logistic-regression-01.md %})
* [ロジスティック回帰 -その2-]({% link _contents/python/logistic-regression-02.md %})
* [課題: ロジスティック回帰の改善]({% link _contents/python/assignment-logistic.md %})

### 数独ソルバーを作る

ここでは数独の本を写真で撮影して、それを画像認識することで、自動的に数独を解くソルバーを作る。

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**入力画像**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**出力画像**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/sudoku/level_3_small.jpg" %}</td>
  <td>{% include lightbox.html src="/public/images/sudoku/level_3_ans_small.jpg" %}</td>
</tr>
</table>

* [画像中の図形検出]({% link _contents/python/figure-detection.md %})
* [数独を解く]({% link _contents/python/solve-sudoku.md %})
* [課題: 画像中の数独問題を解こう]({% link _contents/python/assignment-sudoku.md %})

### 深層学習による画像生成の基礎

ここでは敵対的生成ネットワーク (GAN) を利用して、Oxford flower datasetから、自然な花の画像を生成することを目指す。

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**教師画像**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**出力画像**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/gan/x_real_150.jpg" %}</td>
  <td>{% include lightbox.html src="/public/images/gan/x_fake_150.jpg" %}</td>
</tr>
</table>

* [畳み込みニューラルネット]({% link _contents/python/convolutional-network.md %})
* [敵対的生成ネットワーク(GAN)]({% link _contents/python/gan.md %})
* [条件付き画像生成 (CGAN)]({% link _contents/python/conditional-gan.md %})
* [GANの学習安定化テクニック]({% link _contents/python/stabilize-gan-training.md %})
* [課題: GANによる高品質画像生成コンテスト]({% link _contents/python/assignment-gan.md %})


## C++プログラミング

### テンプレートプログラム

以下の課題で使うテンプレート・プログラムをひとまとめにしたものを以下からダウンロードできる。

* [テンプレート・プログラム]({{ '/archives/cpp-beginners.zip' | relative_url }})

### マーチング・キューブ法によるメッシュ生成

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**ボリューム断面**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**復元メッシュ**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/march_cubes/shell_section_140.jpg" %}</td>
  <td>{% include lightbox.html src="/public/images/march_cubes/shell_mesh.jpg" %}</td>
</tr>
</table>

* [ボリュームデータの読み込み]({% link _contents/cpp/read-volume.md %})
* [大津の二値化による境界値の決定]({% link _contents/cpp/otsu-binarize.md %})
* [マーチング・キューブ法]({% link _contents/cpp/march-cubes.md %})
* [課題: マーチング・キューブ法とその拡張]({% link _contents/cpp/assignment-march-cubes.md %})

### Rigid-ICP法による点群データの位置揃え

* [アルゴリズムの概要]({% link _contents/cpp/overview-rigid-icp.md %})
* [k-d木による最近傍探索]({% link _contents/cpp/kdtree.md %})
* [Eigenを用いた線形問題の解法]({% link _contents/cpp/eigen-solve-linear-system.md %})
* [Point-to-Point ICP]({% link _contents/cpp/point-to-point-icp.md %})
* [Point-to-Plane ICP]({% link _contents/cpp/point-to-plane-icp.md %})
* [課題: Rigid-ICP法の実装]({% link _contents/cpp/assignment-rigid-icp.md %})

### 点群データからのメッシュ復元

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**入力点群**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**復元メッシュ**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/surf_recon/buddha_point.jpg" %}</td>
  <td>{% include lightbox.html src="/public/images/surf_recon/buddha_recon.jpg" %}</td>
</tr>
</table>

* [アルゴリズムの概要]({% link _contents/cpp/overview-surface-recon.md %})
* [RBFによる関数の補間]({% link _contents/cpp/radial-basis-function.md %})
* [課題: 点群データからのメッシュ復元]({% link _contents/cpp/assignment-surface-recon.md %})
