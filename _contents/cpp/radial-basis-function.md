---
layout: post
title: "RBFによる関数の補間"
---

RBF (= radial basis function)とは、一般には、とある点との距離だけによって値が決まる関数のことを指し、代表的なものとしてはガウス関数などがある。

例えば、RBFの中心サンプル点を$\mathbf{c}$としたとすれば、点$\mathbf{x}$におけるRBF $\phi$の値は、$r = \| \mathbf{c} - \mathbf{x} \|$の関数として表される。

## RBFによる関数の近似

一方で、何らかの関数をRBFの集まりによって補間しようとする場合には、これだけでは問題が起こる可能性がある。例えば、$N$個の点$\mathbf{x}_i$ ($i = 0, 1, \ldots, N-1$)に対して、補間したい関数の値$y_i$が分かっているとしよう。

この場合、補間された関数$f(\mathbf{x})$は未知の重み係数$w_i$を用いて次のように表せる。

$$
f(\mathbf{x}) = \sum_{i=0}^{N-1} w_i \phi(\| \mathbf{x} - \mathbf{x}_i \|)
$$

したがって、入力点群 $\{ \mathbf{x}_i \}$と、それらに対する関数の参照値 $\{ d_i \}$が与えられているなら、各$i$について、$f(\mathbf{x}_i) = d_i$が成り立てば良いことになる。

この等式を全ての$i$について考えると、行列形式として、以下のように書き直すことができる。

$$
\mathbf{A} \mathbf{w} = \mathbf{d}
$$

ただし、

$$
A_{ij} = \phi(\| x_i - x_j\|)
$$

であり、$\mathbf{d}$は関数の参照値$d_i$を並べたベクトルである。従って、この補間が上手く計算できるためには$\mathbf{A}$が正則行列である必要があることが分かる。しかしながら、実際にはRBF関数を用いて計算された任意の行列が正則行列となるとは限らない。

ちなみに、RBFの性質を議論する上では上記の行列$\mathbf{A}$が正定値行列であるかどうかを議論することが多いが、例えばシグモイドカーネルと呼ばれる$\tanh(a \mathbf{x}_i^\top \mathbf{x}_j + b))$は正定値ではない。

このような場合に、上手く解を見つける方法として、RBFに多項式項を追加した以下の式が良く用いられる。

$$
\psi(\mathbf{x}) = p(\mathbf{x}) + \phi(\| x_i - x \|)
$$

ここで$p(\mathbf{x})$は$\mathbf{x}$の要素に対する多項式で、三次元ベクトル$\mathbf{x} = (x, y, z)$の場合で言えば、

$$
p(\mathbf{x}) = \lambda_x x + \lambda_y y + \lambda_z z + \lambda
$$

などがそれに対応する。

この場合、補間後の関数を構成するパラメータが$\lambda_x, \lambda_y, \lambda_z, \lambda$だけ増加するので、その自由度の増加分を打ち消すために、以下のような条件を課す。

$$
\sum_{i=0}^{N-1} w_i x_i = \sum_{i=0}^{N-1} w_i y_i = \sum_{i=0}^{N-1} w_i z_i = \sum_{i=0}^{N-1} w_i = 0.
$$

これは、より高次の多項式を用いる場合でも同様で、二次式まで含める場合であれば

$$
p(\mathbf{x}) = \lambda_{xx} x^2 + \lambda_{yy} y^2 + \lambda_{zz} z^2 + \lambda_{xy} xy + \lambda_{yz} yz + \lambda_{zx} zx + ...
$$

のように多項式項を定義した上で、追加制約として

$$
\sum_{i=0}^{N-1} w_i x_i^2 = \sum_{i=0}^{N-1} w_i y_i^2 = \sum_{i=0}^{N-1} w_i z_i^2 = \sum_{i=0}^{N-1} w_i x_i y_i = \sum_{i=0}^{N-1} w_i y_i z_i = \sum_{i=0}^{N-1} w_i z_i x_i = 0.
$$

という条件を課せば良い。

> 参考: Schaback 2007, "A Practical Guide to Radial Basis Functions" \\
> <https://num.math.uni-goettingen.de/schaback/teaching/sc.pdf>{: target="_blank" }

## RBF関数による陰関数曲面の近似

三次元空間上で定義された関数$f(\mathbf{x})$が0を取る場所で曲面が定義される時、この関数$f$を陰関数と呼び、このようにして得られる曲面を**陰関数曲面**と呼ぶ。ちなみに$z = f(x, y)$のような形で変数を整理して書く形を**陽関数**呼ぶ。

点群から曲面を復元する場合には、入力頂点とその頂点に対する法線の情報から、曲面を表す陰関数$f$を求める。初期の研究では、この$f$をRBF補間により定めるものが多く発表され、上記の議論に基づいたものとしてはCarrらの以下の論文が挙げられる。

* Carr et al. 2001, “Reconstruction and Representation of 3D Objects with Radial Basis Functions”

この手法はRBFとして関数の台(support, 関数が0ではない値をとる定義域の部分集合のこと)が無限に広がるRBF (global support RBF)としてPolyharmonic RBFという関数が用いられている。

この関数は次数が奇数か偶数かによって定義が異なるが、それぞれは以下のように定義される。

$$
\phi(r) = \begin{cases}
|r|^k & k \text{ is even} \\
|r|^{k-1} \log |r| & k \text{ is odd}
\end{cases}
$$

このようなglobal supportの関数は点の補間能力に優れているとされ、点の密度がまばらで、穴などが見られる場合に有効に働くことが知られている。

一方で、有限の大きさの台を取るRBFをcompact support RBFと呼ぶ。メッシュの復元においては、以下のWendland関数がよく用いられる。

$$
\phi(r) = (1 - r)_{+}^4 (4r + 1)
$$

ただし $(1 - r)_+$は$\text{max}(0, 1 - r)$と等価である。

上記のWendland関数は半径が1の球を台とする$C^2$級連続の関数であるが、より高次の連続性を持つWendland関数もあり、例えば$C^4$級連続のWendland関数は次のように表される (連続性は$r$そのものについてではなく$r = \sqrt{x^2 + y^2 + z^2}$の$x, y, z$についてであることに注意する) 。

$$
\phi(r) = (1 - r)_{+}^6 (35 r^2 + 18r + 3)
$$

より詳しくは、以下の論文を参考にすると良い。

* Morse et al. 2001, “Interpolating Implicit Surfaces From Scattered Surface Data Using Compactly Supported Radial Basis Functions”

Wendland関数のようなcompact supportの関数は上記の線形問題に現れる係数行列$\mathbf{A}$が、より疎な行列となるため、計算の効率に優れる。今回は計算時間をできるだけ下げる目的でcompact supportのRBFを用いる。

ここまでの議論を踏まえ、上記の多項式項を含めた線形問題を定義すると、以下のようになる。

$$
\begin{pmatrix}
\mathbf{A} & \mathbf{P} \\
\mathbf{P}^\top & \mathbf{0}
\end{pmatrix}

\begin{pmatrix}
\mathbf{w} \\
\boldsymbol\lambda
\end{pmatrix}

=

\begin{pmatrix}
\mathbf{d} \\
\mathbf{0}
\end{pmatrix}
$$

ここで$\mathbf{P} \in \mathbf{R}^{N \times 4}$は各列が入力点群のx, y, z座標ならびに1となるような行列である。

$$
\mathbf{P} = \begin{pmatrix}
x_0 & y_0 & z_0 & 1 \\
\vdots & \vdots & \vdots & \vdots \\
x_{N-1} & y_{N-1} & z_{N-1} & 1
\end{pmatrix}
$$

上記の線形問題では係数行列が対称行列となっているので、共役勾配法やLDLT法(いずれも、半正定値行列に対して利用可能)を使うことで、効率的に解を求めることができるだろう。
