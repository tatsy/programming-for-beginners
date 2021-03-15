---
layout: post
title: C++開発環境の設定
---

## C++開発環境の選び方

C言語、あるいはC++はPythonと異なり、コンパイルが必要な言語なので、最も原始的にはエディタ(Vim, Emacsなど)でソースコードを編集したら、ターミナルに移ってコンパイルをかける必要がある。

自分で開発環境を設定していない限りはVimやEmacsにはC++のコンパイルエラーやソースコードのフォーマットを行う機能はないので、コンパイルしては間違いを直して、再度コンパイルするという作業の流れになりがちである。

一方で、Windowsで言うところのVisual Studio (Visual Studio Codeではなく)を使えば、ソースコードを分析して、コンパイルが通るのか通らないのかを予め分析してくれたり、インクルードしようとするヘッダを補完してくれたりと、効率よくプログラム開発を行うための機能が多く備わっている。

このようなコーディングをサポートする機能を多く備えた開発環境のことを統合開発環境 (Integrated Development Environment, IDE)などと呼びますが、一般的には立ち上がりが遅く、また簡単なプログラムを書くだけでも、管理されるファイルの数が多くなるなどのデメリットもある。

最近はこのようなIDEと原始的なターミナル上の開発の中間的な存在としてVisual Studio CodeやAtomなど、最低限の機能は最初からついていて、拡張しようと思えば、比較的簡単にいろいろできる、というものも多くある。

どのような開発環境を選ぶかは好みの問題もあるが、個人的には最初のうち、とくにヘッダファイルや関数、クラスの名前を覚えていないうちは統合開発環境に頼ってしまうのが良いと思っている。プログラムを書くのはあくまで何かを作りたいからであり、開発環境の設定にコストをかける(そして多くの場合かけがちになる、自戒を込めて...)のは本末転倒だと思うからだ。

その意味で、WindowsならVisual Studioを、MacやLinuxであれば、JetBrain社が開発しているCLionを使うのが良いと思う。Visual Studioは誰でも、CLionも学生ならば無償で使えるので、ぜひ試してみてほしい。

**Visual Studio Community**  
<https://visualstudio.microsoft.com/ja/vs/community/>{: target="_blank" }

**CLion**  
<https://www.jetbrains.com/ja-jp/clion/>{: target="_blank" }

### 注意

Visual Studioでは初期状態でC++のコンパイラがインストールされないので、インストール時に「C++によるデスクトップ開発」という項目にチェックを入れてください。


## CMakeの利用

### CMakeとは？

本資料では、開発環境による設定の差異を減らすためにCMakeというプロジェクト設定ツールを使って、ソースコードを管理している。CMakeはWindows, MacといったOSだけでなく、Visual StudioやCLionといった開発環境の違いも吸収する形で、統一的なソースコード管理が可能になる。

プロジェクトの設定は `CMakeLists.txt` というテキストファイルを各フォルダに階層的に配置する形で行われる。CMakeの書き方は、それはそれで一つのプログラミング言語のようなものであり、一朝一夕に説明することは困難なので、詳細は割愛するが、一例として `main.cpp`, `module.h`, `module.cpp` という3つのファイルから構成される実行可能プログラム(名前を`myprogram`とする)を作る場合には、CMakeLists.txt内に以下の内容を記述する。

```cmake
add_executable(myprogram main.cpp module.h module.cpp)
```

CMakeではVisual Studioと同様に1つの「プロジェクト」(VSの言葉ではソリューション)の中で、複数の実行可能ファイルやライブラリ (VSの言葉ではプロジェクト)を管理するため、上記に加え、プロジェクト名とCMakeのバージョンに関する記述を冒頭で記述する必要がある。

```cmake
cmake_minimum_required(VERSION 2.8.0)
project(MyProject)
```

なお、この設定は冒頭に1どだけ行えばよく、複数の実行可能ファイルやライブラリを追加する場合には、`add_executable` や `add_library` の呼び出しだけを追加していけば良い。

本資料では、すでにプロジェクトの設定と、ソースコードの編集に足るCMakeの記述をしてあるので、以下では、Windows, Macのそれぞれでの利用法について述べる。

### CMakeのインストール

CMakeはCUI環境とGUI環境の両方が用意されていて、なれないうちはGUI環境を使うのが良いかもしれない (特に自分でCMakeLists.txtを書く場合)。ただし、単に配布されているプログラムをビルドする目的で使うだけならターミナルを利用するほうが簡単なようにも思う。

GUI環境をインストールすれば、CUI環境も同時にインストールが可能なので、今回はCMakeを開発するKitWareのウェブページからGUI版をインストールする。**なお、MacやLinuxでCLionを開発環境に使う場合は、CLionと一緒にインストールが可能なので、以下のステップは省略して良い。**

<https://cmake.org/download/>{:target="_blank"}

上記のページには*Source distributions*と*Binary distributions*があるが、よほど特殊な事情がない限りは*Binary distributions*で良いので、その中から自分の環境にあったものを選ぶ。特にWindowsならば`*.msi`、Macならば`*.dmg`が拡張子になっているものを選ぶと良い。

{% include lightbox.html src="/public/images/setup/cmake_downloads.jpg" %}

Windowsの場合には、インストールの途中でCMakeの実行可能プログラムに環境変数のパスを通すかどうかを聞かれるので、パスを通すように設定しておく。

## 開発環境の設定 (Windows)

### Eigenのインストール

本資料のプログラムでは外部ライブラリとして、線形代数の代表的なライブラリである**Eigen**を用いる。

**Eigen**  
<https://eigen.tuxfamily.org/index.php>

Eigenはヘッダファイルのみから成るライブラリで、特にコンパイルなどをする必要がないのが特徴である。上記のウェブページから適当なバージョンのものをダウンロードしたら、フォルダを展開して、これまた適当な場所に配置する。

以後、C++の開発をメインに行おうと考えている場合には、Windowsではライブラリの場所がややわかりづらく成るので、例えば `C:/Libraries` のように自分で決めた場所にまとめてライブラリを置いておくなどすると、管理が円滑になる。

### CMakeによるプロジェクト生成 (Visual Studio)

Windows上でも、C++のコンパイラはいくつかの種類が選択可能なのだが、ここではVisual Studioを使う場合を前提として説明を進めていく。

まずは、CMake GUIを開き、以下の画面右上にある「Browse Source ...」を押下し、ダウンロードしてきたソースコードのルートディレクトリを選択する。その後、上記のボタンの下にある「Browse Binary ...」を押下し、ルートディレクトリ内に `build` というディレクトリを作った上で、それを選択する。

{% include lightbox.html src="/public/images/setup/cmake_gui_default.jpg" %}

ディレクトリの指定が済んだら、今度は画面下になる「Configure」ボタンを押下する。すると、画面下のウィンドウに少し文字列が表示されたあとで、エラーとなって停止するはずだ。このエラーは**Eigen**というライブラリが見つからない、というエラーなので、Eigenをインストールした場所を `EIGEN3_DIR`に指定する (以下の画面では `C:/Libraries/Eigen33` にインストールされているものとした)。

{% include lightbox.html src="/public/images/setup/cmake_gui_eigen_error.jpg" %}

指定後、再度「Configure」ボタンを押すと、今度はエラーが出ずに、画面中央の赤くなっていた部分が白くなるはずである。これが確認できたら次は同じく画面下にある「Generate」ボタンを押す。するとVisual Studioのソリューションファイル (`*.sln`が拡張子)が生成されるので、ボタンの右にある「Open Project」ボタンを押すと、以下のように設定済みのVisual Studioソリューションが開く。

{% include lightbox.html src="/public/images/setup/visual_studio_launch.jpg" %}

画面が開いたら「ビルド」メニューの「ソリューションのビルド」を選び、プログラムが正しくコンパイルされることを確認する。以後は、画面左の「ソリューション・エクスプローラ」から自分が編集したいファイルを選んで、逐次編集とコンパイル、プログラムの実行をしていけば良い。

## 開発環境の設定 (Mac)

### Eigenのインストール

Macの場合にはHomebrewを使うことで簡単にEigenのインストールができる。

```shell
brew install eigen3
```

すると `/usr/local/include/eigen3` にEigenのヘッダファイルがインストールされる。このパスは以下で使用するので覚えておくと良い。


### CMakeによるプロジェクトの生成 (CLion)

CLionを開くと、以下のような画面が開くので「New CMake Project from Sources」を選び、ダウンロードしたテンプレートファイルのルートディレクトリを指定する。

{% include lightbox.html src="/public/images/setup/clion_startup.jpg" %}

すると、以下のような画面が開き、自動的にCMakeによるプロジェクトの生成が始まる。

{% include lightbox.html src="/public/images/setup/clion_default_window.jpg" %}

が、おそらく途中でいくつかのエラーが出ると思われるので、その設定を行う。CLionのメニューから「Preferences」を選び、「Build, Execution, Development」→「CMake」を選び、以下の用に設定を行う。具体的には「Build Type」を「Release」に変更しており、「CMake Options」に`-D WITH_OPENMP=OFF` を指定している。

{% include lightbox.html src="/public/images/setup/clion_preference.jpg" %}

Macでも適切なコンパイラを選べばOpenMP(並列計算のためのライブラリ)を使用することは可能だが、最初からインストールされているApple Clangでは現状OpenMPがサポートされていないので、OpenMPの機能を切ってコンパイルする。

上記の設定後「OK」ボタンを押下すると「Preferences」の画面が閉じて、再びCMakeが走り始める。無事、CMakeが走り終えたら、「Build」メニューにある「Build Project」を押して、正しくコンパイルが通ることを確認する。

### CMakeによるプロジェクトの生成 (ターミナル)

ターミナルを使う方法は、その扱いに慣れていれば、上記のGUIやIDEを使う方法よりシンプルなので、特にライブラリのビルドだけができればよく、ソースコードの書き換えなどが必要ない場合に重宝する。

最も基本的には、ソースコードのダウンロード後、ルート・ディレクトリでターミナル環境を開き、以下のコマンドを打ち込めば良い。

```
# Windowsの場合
mkdir build && cd build
cmake .. -D EIGEN3_DIR="C:/Libraries/Eigen33/" -G "Visual Studio 16 2019"
cmake --build . --config Release --parallel 4

# Mac, Linuxの場合
mkdir build && cd build
cmake .. -D EIGEN3_DIR=/usr/local/include/eigen3/
make -j4
```

これらのコマンドはいずれの場合も、上から、プロジェクトを生成するディレクトリの作成・移動、コンパイラを指定してプロジェクトを生成 (Mac, Linuxの場合は省略可能)、並列計算でソースコードをコンパイル (4つのスレッドを使う)、という処理を表している。