---
layout: post
title: 環境設定
---

## GitHubのアカウントを作る

勉強か研究かに関わらず、Gitなどのバージョン管理システムを使うことを強く勧める。今の時代、プログラムを日付ごとにコピーするのは完全に時代遅れだ。

しかし、Gitは大変便利である反面、初心者には敷居が高いということは理解できる。ともかく、最初はcommitとpushだけを覚えよう。コミットコメントなどは丁寧に書くに越したことはないが、兎にも角にも「記録を取っておく」ということが一番重要である。

Gitで変更履歴を残しておけば、プログラムが動かなくなった時に、履歴を遡って、どこからエラーが発生したかを調べられるだけでなく、エラーの起こった日がわかったら、その時から今までの変更履歴を一覧で見ることもできる。

さらに使いこなせば、自動でテストをしたりすることもできる。実際、このページはGitにコミットがプッシュされると[Travis CI](https://travis-ci.org/)というサービスを使って自動的にサーバー上のWebページを更新する仕組みになっている。

話がそれたがGitやGitHubは絶対使おう。情報系でありながらバージョン管理をしないのは今の時代恥ずかしい。*もう一度言う、絶対に使おう。*

アカウントを作るのはとても簡単だ。以下のステップに従えば10分程度でアカウントが作れる。

* [GitHub](https://github.com/){: target="_blank" } にアクセスしてアカウントを作る
* 学生の場合にはアカデミック・ライセンスでアカウントが作れる
* Windowsの人は[Git for Windows](https://gitforwindows.org/){: target="_blank" } をインストール

使い方についてはWeb上の記事を参考にするのがいい。まずはコミット(commit)とプッシュ(push)のやり方を覚えよう。その次にプル(pull)、マージ(merge)を覚えてブランチ(branch)の使い方をマスターする。最後にスタッシュ(stash)やリベース(rebase)を覚えれば、概ね使いこなしていると言っていい。

## Minicondaのインストール

[Miniconda](https://docs.conda.io/en/latest/miniconda.html){: target="_blank" } にアクセスして、手順に従いインストールする。Windowsならばインストーラがあるので、それほど難しくはないだろう。MacやLinuxの場合にはシェルスクリプトを使うことになるが、これでも難しくはない。最初はシェルを使うことに慣れないだろうが、これからはずっと使うことになる。慣れよう。

インストールが完了したら作業用のPython仮想環境を作る。WindowsならPowerShell, Macならターミナルを使う。仮想環境を作るコマンドは`conda create`でそのあとに`-n name_of_env`というように名前を指定する。合わせて、そのあとに`python=3.6`などのように書くことでライブラリ(Pythonの場合にはパッケージということが多い)をバージョンを指定した上で一緒にインストールすることができる。

```shell
# 仮想環境の作成
conda create -n beginner python=3.6
# 仮想環境の有効化
conda activate beginner
# 仮想環境の無効化
conda deactivate
```

仮想環境を作り終わったら`conda activate name_of_env`で仮想環境をオンに、`conda deactivate`でオフにできる。

ちなみにMinicondaと似たものにAnacondaがある。正直どちらを使ってもいいが、Anacondaは仮想環境を作る時に、勝手に色々なライブラリがインストールされる。それはそれで便利なのだが、ディスクをたくさん使用する(Anacondaは数GBを使う)し、必ずしもそれら全部が必要というわけでもないので、筆者はMinicondaを好んで使っている。

### 必要なライブラリをインストール

現在はNode.js (JavaScript)のNPMやRubyのGemなどインタプリタ言語を中心に多くのパッケージ管理ツールが存在している (コンパイラ言語にもJavaのMavenなどがある)。Pythonのパッケージ管理といえば、昔はvirtualenv + pipだったが、現在はAnacondaが一般的のように思う。

Anaconda環境 (Minicondaを含む) の便利な点は以下のようにコマンド一発で必要なパッケージをインストールできる点にある。

```shell
# 仮想環境を有効化した後で
conda install numpy scipy matplotlib scikit-learn scikit-image opencv
conda install -c conda-forge jupyterlab
```

二つ目のコマンドにある`-c conda-forge`というのは`conda-forge`というチャンネルから必要なパッケージをインストールせよ、という意味だ。デフォルトのconda環境には存在しないものも`conda-forge`チャネルには存在していることが多い。もちろん他にも様々なチャネルが存在していて、以下で使うPyTorchなどは`pytorch`というチャネルからインストールできる。

```shell
conda install -c pytorch pytorch torchvision
```

補足だが、ウェブ上にはAnacondaとpyenvを共存させるようなやり方を説明している記事がなぜか多い。が、これはあまり意味がないことだ。なぜならAnacondaは仮想環境ごとにPythonのバージョンを変えられるし、パッケージのインストールにpipを使ってもいい(pyenvを使っている人はcondaに欲しいパッケージがないことを気にしているようだ...)。

### Jupyter Labの起動

Jupyter LabはPythonをウェブブラウザ上でインタラクティブに実行できる環境であるIPython Notebook (Jupyter)を進化させて、より使いやすくしたものだ。あまり大規模なプログラムを書くのには向いていないが、ちょっとしたプログラムを試すのにはとても便利である。

Jupyter Labは以下のコマンドをシェル (ターミナルやPowerShell) から打つことで実行できる。

```shell
jupyter lab
```

すると、ブラウザが開き、以下の画面が出る。

{% include lightbox.html src="/public/images/setup/jupyterlab.jpg" %}

Jupyter Labの使い方は以前のJupyterとほとんど同じである。細かな使い方についてはWeb上に多くの記事があることから、そちらに譲ることとする。なお、Jupyterはマウスを使わなくてもほぼ全ての動作ができるようになっているので、ショートカットを覚えると非常に効率的にプロトタイピングができる。

## Google Colaboratoryの設定

### Colabの起動

自身のGoogleアカウントでログインし、ColabのWebページにアクセスする。

<https://colab.research.google.com/>{: target="_blank" }

すると、以下のような画面が表示されるので、右下にある「ノートブックを新規作成」を選ぶ。するとGoogleドライブに「Colab Notebooks」というフォルダが自動で生成されて、その中に新しいIPython Notebookのファイルが作られる。

{% include lightbox.html src="/public/images/setup/colab_intro.jpg" %}

### ライブラリの設定

Colabはコマンドの先頭に`!`をつけることで**pip**を使用することができる。そこで、各ノートブックの先頭に必要なライブラリをインストールするコマンドを書いておく。なおColabは最初からNumPyやscikit-learnなどの機械学習系のパッケージがインストールされているので、PyTorchだけをインストールすればおおよそ事足りる。

```shell
!pip install torch torchvision
```

### GPUの使用

なおColabは最初の状態ではGPUが使えない。試しにPyTorchでGPUが使えるかどうかを調べるコマンドを叩いてみる。

```python
print(torch.cuda.is_available())
```

すると`False`と表示されるはずだ。設定を変更するには、ページ上部のメニューから「ランタイム」→「ランタイムのタイプを変更」を選び、そこから「ハードウェアアクセラレータ」のタイプを「GPU」に変更する。再度、上記のコマンドを叩くと、今度は`True`が表示されるはずだ。

{% include lightbox.html src="/public/images/setup/colab_use_gpu.jpg" %}

ちなみにColabは使えるリソース (メモリやディスク容量) に制限があるため、あまり多くのことをするには向いていない。ローカルにGPUを使える環境があるなら、その上で上記のJupyter Labを使う方が良いだろう。
