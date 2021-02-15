---
layout: post
title: Python開発環境の設定
---

## Minicondaのインストール

[Miniconda](https://docs.conda.io/en/latest/miniconda.html){: target="_blank" } にアクセスして、手順に従いインストールする。Windowsならばインストーラがあるので、それほど難しくはないだろう。MacやLinuxの場合にはシェルスクリプトを使うことになるが、これでも難しくはない。最初はシェルを使うことに慣れないだろうが、これからはずっと使うことになる。慣れよう。

**注意**  
WindowsでMinicondaをインストールする場合は、非推奨ではあるが、環境変数の`PATH`にMinicondaを設定するように以下の画面でチェックをいれること。

{% include lightbox.html src="/public/images/setup/miniconda_add_path.jpg" %}

インストールが完了したら作業用のPython仮想環境を作る。WindowsならPowerShell, Macならターミナルを使う。仮想環境を作るコマンドは`conda create`でそのあとに`-n name_of_env`というように名前を指定する。合わせて、そのあとに`python=3.6`などのように書くことでライブラリ(Pythonの場合にはパッケージということが多い)をバージョンを指定した上で一緒にインストールすることができる。

```shell
# Anaconda環境の初期化 (最初に1回だけやればいい, 自分の使っているシェルに合わせる)
$ conda init [powershell,zsh,bash]
# 仮想環境の作成
$ conda create -n beginner python=3.6
# 仮想環境の有効化
$ conda activate beginner
# 仮想環境の無効化
$ conda deactivate
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

## Visual Studio Codeの設定

Pythonのコード開発をするためのツールは多くあり、GUI環境を使うものだと、一番気が利いているのはJetBrain社が開発している[PyCharm](https://www.jetbrains.com/ja-jp/pycharm/)のように思う。特にこだわりがなく、立ち上がりが少し遅くても気にしないなら、PyCharmを使うのが良いと思う。

一方で、最近はVisual Studioコードの進歩が凄まじく、いろいろなオープンソースのライブラリとの親和性も高いので、著者は最近は好んでVisual Studio Code (以下, vscode)を使っている。

vscodeを使う場合には、Pythonの仮想環境にリンター(linter)と呼ばれるコードのチェックや整形を自動化するツールを入れておくと便利である。著者のお勧めの組み合わせは以下の通り。

* コード規則のチェック
  * ソースコードが読みやすく、Pythonの標準に従った書き方になっているかをチェックする
  * `pylint`, `flake8などがある` (おすすめは`flake8`)
* コード静的解析
  * ソースコードを走らせずに、バグを生みそうな書き方をチェックする (これを静的解析という)
  * `mypy`の一択
* コードの自動整形
  * ソースコードを規則に従うように自動的に整形してくれる
  * `autopep8`, `yapf`, `black`などがある (おすすめは`yapf`)

これらをインストールした上で、設定ファイルを開いて以下のように設定する。設定ファイルは「Ctrl + Shift + P」(MacはCtrlの代わりにCommand)を押して「Preferences: Open Setting (JSON)」という項目を検索する。

```json
{
    ...
    // Python
    "python.condaPath": "/Users/tatsuya/miniconda3/bin/conda",
    "python.linting.enabled": true,
    "python.linting.pylintEnabled": false,
    "python.linting.flake8Enabled": true,
    "python.linting.mypyEnabled": true,
    "python.formatting.provider": "yapf",
    "[python]": {
        "editor.formatOnSave": true,
        "editor.codeActionsOnSave": {
            "source.organizeImports": true
        }
    },
    ...
}
```

インタプリタのパスについては適宜、自分のものに置き換えること。このように設定すると、ファイルを保存するたびに、文法チェックと静的解析が走り、ソースコードが自動整形される。

## Jupyter Labの利用

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
