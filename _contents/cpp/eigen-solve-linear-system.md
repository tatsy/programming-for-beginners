---
layout: post
title: "Eigenを用いた線形問題の解法"
---

## Eigenのインクルードと簡単な設定

Eigenで行列を扱う場合には、ヘッダのインクルード後、少しだけ型定義などをしておくほうが便利である。

### ヘッダのインクルード

まず、ヘッダを以下のようにインクルードする。

```c++
#include <Eigen/Core>
```

この`Core`というヘッダは、Eigenの行列の定義だけを抽出したものである。これに加えて、行列の分解や線形問題のソルバーなどを含めたい場合には、

```c++
#include <Eigen/Dense>
```

とすれば良い。ただし、この`Dense`というヘッダは、その中で上記の行列分解や線形ソルバーのためのヘッダをインクルードするため、コンパイルに異様に時間がかかるようになる。

これを防ぎたければ、必要な機能だけを選んで以下のようにするのが良い。

```c++
#include <Eigen/Core>
#include <Eigen/LU>
```

これは疎行列を扱う場合も、同様 (あるいはもっと問題は深刻)で、以下のように必要なヘッダだけをインクルードすることが、開発の効率を上げるためにも重要である。

```c++
// Sparseヘッダはコンパイルが恐ろしく遅くなる！！！
#include <Eigen/Sparse>

// 代わりにこうする
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
```

### Eigenのための型定義

Eigenの行列型はテンプレートを使って、

```c++
// in namespace Eigen
template <typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>
class Matrix;
```

のように定義されている ([参考ページ](https://eigen.tuxfamily.org/dox/group__TutorialMatrixClass.html){: target="_blank" })。

ウェブ上のサンプルなどで良く使われている `MatrixXd`というのは基本の型が`double`で行数、列数ともにX, すなわち動的変化可能、という意味である。これは、上記の定義に当てはめれば以下のように定義される。

```c++
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixXd;
```

またサイズが可変のベクトルを表す`VectorXd`であれば、その定義は以下のようになる。

```c++
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorXd;
```

もちろん、これらをそのまま用いても良いのだが、これらを基本の型を変更できるように自前で用意しておくほうが何かと便利である。一例として以下のようにするのが良いだろう。

```c++
using ScalarType = double;  // ここは必要に応じてfloatなどに変更する

// テンプレート型として新しい型定義をする (typedefではこれはできないのでC++11のusingを使う)
template <class Scalar>
using EigenMatrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
template <class Scalar>
using EigenVector = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

// 実際に使う型の定義
using MatrixXd = EigenMatrix<ScalarType>;
using VectorXd = EigenMatrix<ScalarType>;
```

## 密行列の扱い

行列の基本的な使い方については、[ドキュメント](https://eigen.tuxfamily.org/dox/group__TutorialMatrixArithmetic.html){: target="_blank" }を確認しておこう。

ここでは、特に重要と思われる項目について簡単に解説する。

### 行列の初期化

行列を初期化する方法はいくつかあり、Eigenでは以下のようにサイズを指定して、行列を用意した後、初期化用のルーチンを呼び出すことが多い (いきなり代入しても問題ない)。

```c++
// 行列の確保
const int N = 100;
MatrixXd AA(N, N);

// 行列の初期化
AA.setZero();           // 全要素を0で埋める
AA.setOnes();           // 全要素を1で埋める
AA.setConstant(3.14);   // 全要素を指定した値で埋める
AA.setIdentity();       // 対角要素を1、非対角要素を0で埋める (正方行列でなくても良い)
AA.setRandom();         // [-1, 1]の乱数 (範囲に注意！！！！！)で全要素を埋める

// 確保と初期化を同時にやる
MatrixXd zeroMat = MatrixXd::Zero(N, N);
MatrixXd onesMat = MatrixXd::Ones(N, N);
MatrixXd consMat = MatrixXd::Constant(N, N, 3.14);
MatrixXd randMat = MatrixXd::Random(N, N); 
```

また、サイズがそれほど大きくない行列であれば、以下のように直接値を指定して初期化することもできる。

```c++
MatrixXd AA(2, 2);
AA << 1.0, 2.0, 3.0, 4.0;

// 以下の内容と同じ
AA(0, 0) = 1.0;
AA(0, 1) = 2.0;
AA(1, 0) = 3.0;
AA(1, 1) = 4.0;
```

これ以外に列ベクトルから行列を作ったりと言ったことも可能であるが、より詳しくは必要に応じて以下のドキュメントを参考にするとよいだろう。

* [Eigen: Advanced initialization](https://eigen.tuxfamily.org/dox/group__TutorialAdvancedInitialization.html){: target="_blank" }

### 線形ソルバ

連立一次方程式は一般に行列とベクトルを用いた形に書き直すことができ、このような問題のことを**線形問題**と呼ぶ。

線形問題の解法については、大きく分けて**直接法** (direct method)と**反復法**(iterative method)の2つがある。

よく知られている方法で言えば、ガウスの消去法やLU分解のように、直接的な行列の操作のみによって、解を得る方法を直接法と呼び、ヤコビ法やガウス・ザイデル法のように行列演算によって徐々に解となる値に近いベクトルを得る方法を反復法と呼ぶ。

これらの線形ソルバには非常に多くの種類があるが、Eigenには直接法、反復法で以下のものがすでに実装されている。

**直接法** (抜粋)
* 部分ピボット選択によるLU分解 (完全ピボット選択のものもある)
* 部分ピボット選択とHouseholder変換によるQR分解 (完全ピボット選択のものもある)
* コレスキー分解 (LDLTと呼ばれる半正定値行列に対するソルバもある)
* 特異値分解を用いた非正方、不定値行列に対する解法

**反復法** (抜粋, これらは疎行列でも同じものが使える)
* 前処理付き共役勾配法
* 前処理付き安定化双共役勾配法

これらの使い方については、以下の通り。

```c++
// --- 直接法 ---
VectorXd xx;
xx = AA.partialPivLu().solve(bb);            // 部分ピボット選択によるLU分解
xx = AA.colPivHouseholderQr().solve(bb);     // 部分ピボット選択とHouseholder変換によるQR分解
xx = AA.llt().solve(bb);                     // コレスキー分解
xx = AA.bdcSvd().solve(bb);                  // 特異値分解

// --- 反復法 ---
Eigen::ConjugateGradient<MatrixXd> cg;
cg.compute(AA);
xx = cg.solve(bb);

Eigen::BiCGSTAB<MatrixXd> bicg;
bicg.compute(AA);
xx = bicg.solve(bb);
```

なお、それぞれの手法は、行列がどの形のときに使えて、どの形のときにより効率的か、というのがある程度決まっている。

応用的には$\| A x - b \|^2$のような最小二乗法の解を求めることが多いと思われるが、これを$x$について微分して得られる線形問題の係数行列は$A^T A$という形をしており、これは少なくとも半正定値行列ではあるので、上の中で言えば、コレスキー分解や共役勾配法を用いるのが良い (あくまで性質としては)。

追加で、手法の特性などを考慮すると、半正定値行列に対しては、直接法ならLDLT法を、反復法ならBiCGSTAB法を使えば概ねうまくいく。

それぞれの方法について、より詳しく知りたい場合は、以下のドキュメントを参照すると良い。

* [Eigen: Catalogue of dense decomposition](https://eigen.tuxfamily.org/dox/group__TopicLinearAlgebraDecompositions.html){: target="_blank" }

## 疎行列の扱い

疎行列を取り扱うにあたっては、その初期化の方法を別途知る必要がある。

### 疎行列の作成

疎行列を計算機によって扱う場合には、一般にCRS (compressed row storage)やCCS (compressed column storage)といったデータ方式が良く用いられる。

Eigenでは、どちらの方法も使うことができるが、デフォルトではCCSが用いられるので、ここではCCSを仮定して説明を進める。

CCSでは各列のデータが圧縮されて格納される。各列のデータには、その列の非ゼロ要素の行番号とその時の値が格納されると行った具合だ。非常に単純化した擬似コードでは以下のようになる。

```c++
struct DataInRow {
    int colIndex;
    double value;
};

Array<DataInRow> rowData(numRows);
```

そのため、Eigenで疎行列を扱うときには、各非ゼロ要素に対して**行番号**, **列番号**, **要素の値**の三組が必要になる。この三組を表すために、Eigenには`Triplet`という型が用意されている。

```c++
// in namespace Eigen
template <typename Scalar, typename Index>
class Triplet;
```

この`Triplet`を使う場合も、要素と行、列番号を指定するための型を変更できるよう、以下のようにしておくのが良い。

```c++
using ScalarType = double;
using IndexType = int64_t;
using Triplet = Eigen::Triplet<ScalarType, IndexType>;
```

また、疎行列の型についても、以下のように型定義をしておく。

```c++
using ScalarType = double;  // 以前に指定していたら書く必要はない
using SparseMatrix = Eigen::SparseMatrix<ScalarType>;
```

疎行列を作る際には、この`Triplet`を配列等に入れておき、`setFromTriplet`関数で要素を指定する。

```c++
std::vector<Triplet> triplets;

// std::vectorに要素を入れていく
triplets.emplace_back(i, j, value);
....

// 疎行列の作成
SparseMatrix AA(N, N);
AA.setFromTriplets(triplets.begin(), triplets.end());
```

### 疎行列に対する線形ソルバー

密行列を扱う場合と同様、Eigenには複数の直接法と反復法によるソルバーが用意されている。

ただし、密行列の場合と違い、疎行列の場合には、疎行列でいう反復法のソルバと同じような使い方を直接法に対しても行う必要がある。

例えば、直接法のソルバーである`SparseLU`を用いる場合には、以下のようになる。

```c++
VectorXd xx;

Eigen::SparseLU<SparseMatrix> solver;
solver.compute(AA);
xx = solver.solve(bb);
```

なお、Eigenのドキュメントでは`compute()`と等価なものとして`analyzePattern()`と`factorize()`を連続して呼び出しているが、`analyzePattern()`は`factorize()`のコストを減らすための行列要素の順序入れ替えで、`factorize()`は実際の行列分解を行うメソッドである。
