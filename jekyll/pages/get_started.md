---
layout: page
title: 環境設定
permalink: /get_started/
---

## GitHubのアカウントを作る

* [GitHub](https://github.com/){: target="_blank" } にアクセスしてアカウントを作る
* 学生の場合にはアカデミック・ライセンスでアカウントが作れる
* Windowsの人は[Git for Windows](https://gitforwindows.org/){: target="_blank" } をインストール

## Minicondaのインストール

* [Miniconda](https://docs.conda.io/en/latest/miniconda.html){: target="_blank" } にアクセスして、手順に従いインストール
* 作業用のPython環境を作る。WindowsならPowerShell, Macならターミナルを使う

```shell
# 仮想環境の作成
conda create -n beginner python=3.6
# 仮想環境の有効化
conda activate beginner
# 仮想環境の無効化
```

* 必要なライブラリをインストール

```shell
# 仮想環境を有効化した後で
conda install numpy scipy matplotlib scikit-learn scikit-image opencv
conda install -c conda-forge jupyterlab
```

* Jupyter Labの起動

```shell
jupyter lab
```

以下の画面が出る。

{% include lightbox.html src="/public/images/get_started/jupyterlab.jpg" %}

