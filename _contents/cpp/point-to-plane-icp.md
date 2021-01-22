---
layout: post
title: Point to Plane ICP
---

## 問題の定式化

Point to **Plane** ICPはPoint to Point ICPとは異なり、目的位置を表す点群に法線方向が与えられていることを前提にします。ここで言う**Plane**というのは目的位置の点を通り、法線に直交するような平面のことを指す。

従って、Point to Plane ICPでは最適化問題が各頂点$\mathbf{x}_i$の最近傍点$\mathbf{p}_i$とその最近傍点の法線$\mathbf{n}_i$を用いて以下のように定義される。

$$
\begin{equation}
\min_{\mathbf{R}, \mathbf{t}} \sum_{i=1}^{N} \left( (\mathbf{R} \mathbf{x}_i + \mathbf{t} - \mathbf{p}_i) \cdot \mathbf{n}_i \right)^2
\label{eq:least-squares}
\end{equation}
$$

この定式化は一見Point to Point ICPのそれに近く、同じように解けそうなのであるが、それは$\mathbf{t}$に対してだけで、$\mathbf{R}$に対しての最適化問題は $\mathbf{R}$が行列式が1の直交行列であるという制約から解析的な解をもたない。そのため、この最適化問題は数値的に解く必要がある。

例えば回転行列を四元数を用いて表すと、四元数の要素4次元と並進を表すベクトル3次元の合計7次元のベクトルを未知数とするような非線形最適化問題が定義できるので、それを最急降下法などの一般的な解法によって解く方法もある。

今回は、実装をより簡単にするため、回転行列の回転角度が十分に小さいとみなせると仮定し、その際に得られる一般的な行列表示に対して最適化問題を解く。上記の最適化は$\mathbf{R}$が直交行列であるが故に解析解が得られなかったので、このように制約を緩和すれば解析的に解を得ることができる。

## 回転行列の線形化

回転行列$\mathbf{R}$は回転軸を$\mathbf{w}$、回転角度を$\theta$とする時、ロドリゲスの回転公式によって以下のように与えられる。

$$
\mathbf{R}(\mathbf{w}, \theta) = \mathbf{I} + \sin\theta \, \mathbf{K}(\mathbf{w}) + (1 - \cos\theta) \, \mathbf{K}(\mathbf{w})^2
$$

ここで行列$\mathbf{K}(\mathbf{w})$は$\mathbf{w}$と外積を取る操作に対応する作用素で以下のように表せる。

$$
\mathbf{K}(\mathbf{w}) = \begin{pmatrix}
0 & -w_z & w_y \\
w_z & 0  &-w_x \\
-w_y & w_x & 0
\end{pmatrix}
$$

この回転行列の定義において$\theta$が十分に小さいと仮定すると、$\cos\theta \approx 1$, $\sin\theta \approx \theta$となることから、

$$
\mathbf{R}(\mathbf{w}, \theta)
\approx \mathbf{I} + \theta \mathbf{K}(\mathbf{w})
= \mathbf{I} + \mathbf{K}(\theta \mathbf{w})
= \mathbf{I} + \begin{pmatrix}
0 & -\theta w_z & \theta w_y \\
\theta w_z & 0  &-\theta w_x \\
-\theta w_y & \theta w_x & 0
\end{pmatrix}
$$

という関係式が得られる。ここで改めて$\mathbf{a} = \theta\mathbf{w}$と置き直すと、回転行列を表すのに必要な未知数は3次元ベクトル$\mathbf{a}$の各要素のみとなる。

## 最適化問題の変形

線形化された回転行列$\mathbf{I} + \mathbf{K}(\mathbf{a})$を用いると、$\mathbf{K}$が外積を表す作用素だったことに注意して、剛体変換の式は以下のように書き直せる。

$$
\begin{aligned}
\mathbf{R}\mathbf{x} + \mathbf{t}
&= (\mathbf{I} + \mathbf{K}(\mathbf{a})) \mathbf{x} + \mathbf{t} \\
&= \mathbf{x} + \mathbf{a} \times \mathbf{x} + \mathbf{t}
\end{aligned}
$$

これをさらに法線方向の距離を表す式に代入すると、途中のスカラー三重積に注意して

$$
\begin{aligned}
\left( \mathbf{x} + \mathbf{a} \times \mathbf{x} + \mathbf{t} - \mathbf{p} \right) \cdot \mathbf{n}
&= (\mathbf{x} \cdot \mathbf{n}) + (\mathbf{a} \times \mathbf{x}) \cdot \mathbf{n} + (\mathbf{t} \cdot \mathbf{n}) - (\mathbf{p} \cdot \mathbf{n}) \\
&= \mathbf{a} \cdot (\mathbf{x} \times \mathbf{n}) + (\mathbf{t} \cdot \mathbf{n}) + (\mathbf{x} - \mathbf{p}) \cdot \mathbf{n}
\end{aligned}
$$

と書ける。ここで、$\mathbf{a}$と$\mathbf{t}$を縦方向に連結した6次元ベクトルを考えると、この式はさらに以下のように変形できる。

$$
\begin{pmatrix}
\mathbf{x} \times \mathbf{n} \\
\mathbf{n}
\end{pmatrix}
\cdot
\begin{pmatrix}
\mathbf{a} \\
\mathbf{t}
\end{pmatrix}
+ (\mathbf{x} - \mathbf{p}) \cdot \mathbf{n}
$$

以上から、$\mathbf{a}$と$\mathbf{t}$を連結したベクトルを$\mathbf{u} \in \mathbb{R}^6$とすることで、式\eqref{eq:least-squares}の最適化問題は、$\mathbf{s}$, $\mathbf{r}$を適当なベクトルとして、以下のように書き直せる。

$$
\min_{\mathbf{u}} \| \mathbf{s} \cdot \mathbf{u} - \mathbf{r} \|^2
$$

これを$\mathbf{u}$について微分したものが0であると仮定して項を整理すれば、一般的な線形の連立一次方程式の形

$$
\mathbf{A} \mathbf{u} = \mathbf{b}
$$

が得られ、この時、$\mathbf{A}$, $\mathbf{b}$はそれぞれ

$$
\begin{aligned}
\mathbf{A} &= \sum_{i=1}^{N} \left[
\begin{pmatrix} \mathbf{x} \times \mathbf{n} \\ \mathbf{n} \end{pmatrix}
\begin{pmatrix} \mathbf{x} \times \mathbf{n} \\ \mathbf{n} \end{pmatrix}^{T}
\right] \in \mathbb{R}^{6 \times 6} \\
\mathbf{b} &= \sum_{i=1}^{N} \left[
\begin{pmatrix} \mathbf{x} \times \mathbf{n} \\ \mathbf{n} \end{pmatrix}
(\mathbf{x}_i - \mathbf{p}_i) \cdot \mathbf{n}_i
\right] \in \mathbb{R}^{6}
\end{aligned}
$$

と表せる。

## 剛体変換の復元

上記の連立一次方程式を解くことで線形化された回転行列の要素を表すベクトル$\mathbf{a}$が得られる。このベクトルが$\theta\mathbf{w}$であったことを思い出すと、

$$
\theta = \| \mathbf{a} \|, \quad \mathbf{w} = \frac{\mathbf{a}}{\theta}
$$

である。あとは、これらの値をロドリゲスの回転公式に代入することで回転行列$\mathbf{R}$が得られる。

並進の要素についても$\mathbf{u}$と同時に与えられていることから、これを以ってPoint to Plane ICPにおいても剛体変形を求めることができた。

なお、上記の計算では**回転行列の回転角度が十分に小さい**と仮定したが、ICPのステップの繰り返しにより回転成分も積み上がっていくので、1ステップ分の回転角度が小さくとも、十分に一般的な剛体変形を考慮することが可能である。
