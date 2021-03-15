---
layout: post
title: Point to Point ICP
---

## 問題の定式化

今二つの点群$\mathcal{X} = \\{ \mathbf{x}_i \\}\_{i=1}^{N}$と$\mathcal{Y} = \\{ \mathbf{y}_j \\}\_{j=1}^{M}$が与えられており、点群$\mathcal{X}$を点群$\mathcal{Y}$に向かって位置揃えすることを考える。

点群同士の距離の定義はハウスドルフ距離やChamfer距離などいくつもの距離が知られているが、ここでは一番単純に$\mathcal{X}$の各点から$\mathcal{Y}$上で一番近い点までの距離が各々で小さくなる状況を目指す。

まず、点群$\mathcal{X}$の点$\mathbf{x}\_i$を$\mathcal{Y}$上で一番近い点に射影する演算子$P_\mathcal{Y}$を考える。これは2点間のユークリッド距離をノルム$\\| \cdot \\|$を用いて表すこととして、以下のように定義される。

$$
P_{\mathcal{Y}}(\mathbf{x}) = \arg\min_{\mathbf{y} \in \mathcal{Y}} \| \mathbf{x} - \mathbf{y} \|
$$

以後は簡単のために、射影後の点を$\mathbf{p}\_i = P_{\mathcal{Y}}(\mathbf{x}_i)$のように表すこととする。ICPの各ステップでは点群$\mathcal{X}$の各点$\mathbf{x}\_i$を、この最近傍点$\mathbf{p}\_i$に近づけるように剛体変換する。

今、$\mathcal{X}$を$\mathcal{Y}$に位置揃えするための剛体変換を回転行列$\mathbb{R}$と並進の量を表すベクトル$\mathbf{t}$によって表現することとすると、今求めるべき変換は以下の最小化問題を得ことで与えられる。

$$
\begin{equation}
\min_{\mathbf{R}, \mathbf{t}} \sum_{i = 1}^{N} \| \mathbf{R}\mathbf{x}_i + \mathbf{t} - \mathbf{p}_i \|^2
\label{eq:least-squares}
\end{equation}
$$

この式は、$\mathbf{x}\_i$を剛体変換した点と最近傍点$\mathbf{p}\_i$の間の距離がユークリッド距離を基準として小さくなることを表している。なお、この時の剛体変換は点群全体に適用されるものであり、$\mathbf{R}$ならびに$\mathbf{t}$には添え字が付かないことに注意すること。

この最小化問題を得ためには、安直には求めたい変数である$\mathbf{R}$ならびに$\mathbf{t}$で誤差関数を微分して、その値が0となるようなものを求めれば良い。この考え方は$\mathbf{t}$を求めるのには有効であるのだが、$\mathbf{R}$は回転行列であり、一般的な行列ではないことから、単純に最適解を求めることは難しい。

回転行列は一般に行列式が1 (マイナスの場合には鏡映反転が入るため)であるような直交行列なので、この性質を考慮して最適化問題を解く必要がある。

## 並進成分の求解

前述の通り式\eqref{eq:least-squares}は、$\mathbf{t}$については、誤差関数の微分を取ることで解を得ることができる。そこで、まずはその解について分析を試みる。式\eqref{eq:least-squares}の誤差関数部分を$\mathbf{t}$で偏微分し、それが0であると仮定すると、以下の方程式が得られる。

$$
\sum_{i=1}^{N} (\mathbf{t} + \mathbf{R} \mathbf{x}_i - \mathbf{p}_i) = 0
$$

これを$\mathbf{t}$ついて整理すると、

$$
\begin{equation}
\mathbf{t} = \frac{1}{N} \sum_{i=1}^N (\mathbf{p}_i - \mathbf{R} \mathbf{x}_i) = \bar{\mathbf{p}} - \mathbf{R} \bar{\mathbf{x}}
\label{eq:optimal-trans}
\end{equation}
$$

となることが分かる。なお$\bar{\mathbf{x}}$ならびに$\bar{\mathbf{p}}$は、それぞれ$\mathbf{x}_i$ならびに$\mathbf{p}_i$の平均を表すものとする。

以上から、一度$\mathbf{R}$が求まれば、それを用いて並進成分$\mathbf{t}$が求まり、また上記の式が$\mathbf{R}$だけの関数になっていることから、この式を元の誤差関数に代入することだ$\mathbf{R}$だけに関する最適化問題が得られることが分かる。

## 特異値分解を用いた回転行列の求解

上記の議論から式\eqref{eq:optimal-trans}を式\eqref{eq:least-squares}に代入して、回転行列$\mathbf{R}$だけに関する最小化問題を定義する。

$$
\min_{\mathbf{R}} \sum_{i=1}^{N} \| \mathbf{R}(\mathbf{x}_i - \bar{\mathbf{x}}) - (\mathbf{p}_i - \bar{\mathbf{p}}) \|^2
$$

これを簡単のため$\hat{\mathbf{x}}_i = \mathbf{x}_i - \bar{\mathbf{x}}$, $\hat{\mathbf{p}}_i = \mathbf{p}_i - \bar{\mathbf{p}}$と書き直す。

$$
\min_{\mathbf{R}} \sum_{i=1}^{N} \| \mathbf{R} \hat{\mathbf{x}}_i - \hat{\mathbf{p}}_i \|^2
$$

今、$\mathbf{R}\hat{\mathbf{x}} - \hat{\mathbf{p}}$の部分は3次元のベクトルであるので、それを縦ベクトルとして、それを横方向に$N$個分並べたような行列を考えてみる。

上記の誤差関数はベクトルの各要素の二乗を足し上げたものであるので、各ベクトルを並べた行列を考えた場合も、行列の各要素の二乗を足し上げれば同じ値が得られる。このような行列の各要素の二乗を足し上げて、ベクトルのユークリッドノルムと同様に平方根をとったものをフロベニウス・ノルム(Frobenius norm)と呼び、以下のように定義される。

$$
\| \mathbf{M} \|_{F} = \left( \sum_{i, j} M_{ij}^2 \right)^{\frac{1}{2}}
$$

このフロベニウス・ノルムを用いると、上記の誤差関数は以下のように書き直すことができる。

$$
\begin{equation}
\min_{\mathbf{R}} \| \mathbf{R} \mathbf{X} - \mathbf{P} \|_{F}^2
\label{eq:least-squares-frobenius}
\end{equation}
$$

ここで$\mathbf{X}$ならびに$\mathbf{P}$は3行$N$列の行列で、$\mathbf{x}_i$ならびに$\mathbf{p}_i$を縦ベクトルとして、それらを横方向に$N$個結合したものを表す。

$$
\begin{aligned}
\mathbf{X} &= \begin{pmatrix}
\hat{\mathbf{x}}_1 & \hat{\mathbf{x}}_2 & \cdots & \hat{\mathbf{x}}_N
\end{pmatrix} \in \mathbb{R}^{3 \times N} \\
\mathbf{P} &= \begin{pmatrix}
\hat{\mathbf{p}}_1 & \hat{\mathbf{p}}_2 & \cdots & \hat{\mathbf{p}}_N
\end{pmatrix} \in \mathbb{R}^{3 \times N}
\end{aligned}
$$

フロベニウス・ノルムには、以下のような関係式が成り立つことが知られている。

$$
\| \mathbf{M} \|_{F}^2 = \mathrm{tr} (\mathbf{M}^{T} \mathbf{M})
$$

これを用いると式\eqref{eq:least-squares-frobenius}は以下のように式変形できる。

$$
\begin{aligned}
\| \mathbf{R} \mathbf{X} - \mathbf{P} \|_{F}^2
&= \mathrm{tr}(\mathbf{X}^{T} \mathbf{X}) - 2 \mathrm{tr}(\mathbf{X}^T \mathbf{R}^T \mathbf{P}) + \mathrm{tr} (\mathbf{P}^{T} \mathbf{P})
\end{aligned}
$$

上記の式では、$\mathbf{R}$が直交行列であり$\mathbf{R}^{T} \mathbf{R}$が単位行列になることに注意すること。この式では3つある項のうち中央の項だけが最適化変数である$\mathbf{R}$を含んでいるので、この項の符号が負であることに注意すると、以下の最大化として書き直せる。

$$
\max_{\mathbf{R}} \mathrm{tr} (\mathbf{X}^{T} \mathbf{R}^{T} \mathbf{P})
$$

次に、トレース内の行列の順序変換の公式を用いて、行列の順序を入れ替える(任意の入れ替えが可能と言うわけではないので注意)。

$$
\mathrm{tr}(\mathbf{X}^{T} \mathbf{R}^{T} \mathbf{P})
= \mathrm{tr}(\mathbf{P} \mathbf{X}^{T} \mathbf{R}^{T})
= \mathrm{tr}(\mathbf{R} \mathbf{X} \mathbf{P}^{T})
$$

ここで、やや天下り式ではあるが$\mathbf{X P}^{T}$を特異値分解して$\mathbf{U} \boldsymbol\Sigma \mathbf{V}^{T}$と書き直し、さらにトレースの行列順序変更の公式を適用する。

$$
\mathrm{tr}(\mathbf{R}\mathbf{U}\boldsymbol\Sigma\mathbf{V}^{T})
= \mathrm{tr}(\mathbf{V}^{T}\mathbf{R}\mathbf{U} \boldsymbol\Sigma)
$$

$\mathbf{V}$, $\mathbf{R}$, $\mathbf{U}$がそれぞれ直交行列であることから、これらの積である$\mathbf{Z} = \mathbf{V}^{T} \mathbf{R} \mathbf{U}$も直交行列であることに注目する。

また$\boldsymbol\Sigma$は一般に非負である特異値を対角成分に持つ対角行列であるので、各対角要素の平方根を取って得られる行列を$\boldsymbol\Sigma^{\frac{1}{2}}$と書くことで以下のように式変形できる。

$$
\mathrm{tr}(\mathbf{Z}\boldsymbol\Sigma)
= \mathrm{tr}(\boldsymbol\Sigma^{\frac{1}{2}T} \mathbf{Z} \boldsymbol\Sigma^{\frac{1}{2}})
$$

Arunらの論文[^1]によれば、直交行列$\mathbf{B}$と一般の行列$\mathbf{A}$に対して、$\mathrm{tr} (\mathbf{A}^{T} \mathbf{B} \mathbf{A}) \leq \mathrm{tr} (\mathbf{A}^{T} \mathbf{A})$であるので、これを用いれば

$$
\mathrm{tr}(\mathbf{Z} \boldsymbol\Sigma) \leq \mathrm{tr} (\boldsymbol\Sigma)
$$

であり、等式は$\mathbf{Z}$が単位行列である時に成り立つことが分かる。以上から、回転行列$\mathbf{R}$を求めると、

$$
\mathbf{V}^{T} \mathbf{R} \mathbf{U} = \mathbf{I} \quad \Leftrightarrow \quad \mathbf{R} = \mathbf{V} \mathbf{U}^{T}
$$

が、式\eqref{eq:least-squares}の最適化問題の解となる。

ただし、一般には二つの直交行列$\mathbf{U}$と$\mathbf{V}$の行列式の符号は異なる可能性がある(すなわち1, -1のどちらも取りうる)ので、鏡映反転が現れることを防ぐために、以下のように$\mathbf{R}$を定義し直す。

$$
\mathbf{R} = \mathbf{V} \mathbf{H} \mathbf{U}^{T}, \quad \mathbf{H} = \mathrm{diag} ([1, 1, \det \mathbf{VU}^{T}])
$$

このようにして回転行列が得られたら、式\eqref{eq:optimal-trans}にこれを代入して、並進成分の最適解も求める。

## 終了条件

ICPでは上記の剛体変換を求める操作を、その変化量が十分に小さくなるまで繰り返す。変化量が小さくなると回転行列$\mathbf{R}$は単位行列に、並進成分$\mathbf{t}$はゼロベクトルに近くので、移動の大きさ$d(\mathbf{R}, \mathbf{t})$を以下のように定義する。

$$
d(\mathbf{R}, \mathbf{t}) = \| \mathbf{R} - \mathbf{I} \|_{F} + \| \mathbf{t} \|
$$

予め誤差許容値を決めておき、上記の移動量が誤差許容値未満になったら処理を打ち切るようにする。

## 参考文献

  [^1]: Arun et al., "Least-Squares Fitting of Two 3-D Point Sets," PAMI, 1987.
