---
layout: post
title: "ロジスティック回帰 -その2-"
---

## ロジスティック回帰の理論

[ロジスティック回帰 -その1-]({% link _python/logistic-regression-01.md %})で述べた通り、ロジスティック回帰による識別モデルは、線形変換とsoftmax関数の組み合わせである。softmax関数にはパラメータ変数は含まれないので、回帰モデル自体のパラメータは行列 $\mathbf{A}$ とバイアスベクトル $\mathbf{b}$ のみである。

従って、これらのパラメータを、より損失関数が小さくなるように最適化することが、上記のロジスティック回帰の目的となる。

### 損失関数

今、学習データに含まれる画像データを $\mathbf{x}$, ラベルのデータを $\mathbf{y}$ とする。すると、とある パラメータ$\mathbf{A}, \mathbf{b}$を用いたデータからの予測値 $\tilde{\mathbf{y}}$は、

$$
\tilde{\mathbf{y}} = \sigma(\mathbf{Ax} + \mathbf{b})
$$

となるのであった(簡単のためにsoftmax関数を$\sigma$とした)。この予測値が実際の学習データに含まれる真値にどれくらい近いかを計るとすれば、もっとも単純なものはL2ノルムを使うものだろう。

$$
L_2(\mathbf{y}, \tilde{\mathbf{y}}) = \| \mathbf{y} - \tilde{\mathbf{y}} \|^2
$$

ここでは損失の測り方をL2ノルムとしたが、当然ながら、その他の測り方も可能である。一般にロジスティック回帰においては交差エントロピー (cross entropy) がよく用いられる。交差エントロピーはベクトルの次元を $D$として以下のように書ける。

$$
L_{CE}(\mathbf{y}, \tilde{\mathbf{y}}) = \sum_{d=1}^D -y_d \log \tilde{y}_d
$$

ただし、問題が二者識別問題でsoftmax関数の代わりにsigmoid関数が使われる場合には、上記の代わりに以下のバイナリ交差エントロピー (binary cross entropy) が用いられる。

$$
L_{BCE}(\mathbf{y}, \tilde{\mathbf{y}}) = \sum_{d=1}^D \left[ -y_d \log \tilde{y}_d - (1 - y_d) \log (1 - \tilde{y}_d) \right]
$$

交差エントロピーはL2ノルムなどと比べ、誤差が大きい場合により強く誤差がかかるようになっている。通常、学習データに含まれるラベルは$[0, 1, 0, 0, \ldots, 0]$のような一つだけが1になっているベクトル(この形式をone-hot形式という)だから、仮に$y_j=1$であるとすれば、交差エントロピーと、その微分は以下のようになる。

$$
\begin{aligned}
    L &= \begin{cases}
    -\log \tilde{y}_j & (i = j) \\
    0 & \text{otherwise}
    \end{cases} \\

    \frac{\partial L}{\partial \tilde{y}_i} &= \begin{cases}
    -\frac{1}{\tilde{y}_j} & (i = j) \\
    0 & \text{otherwise}
    \end{cases}
\end{aligned}
$$

このように、$\tilde{y}_j$が欲しい真値(=1)と遠い0を取ろうとすると、勾配が無限に大きな値を取ることが分かる。一般に上記のような損失関数は勾配情報を使った最小化アルゴリズム(最急降下法やニュートン法)を用いて最適化されるため、どのような勾配が得られるかはとても重要だ。

 ただし、交差エントロピーは確率的な出力 (上記の例ではベクトルの要素の合計が1になる) 場合にしか使えないので気をつけること。

### チェイン・ルール

ロジスティック回帰をはじめとして、現在の深層学習を含むニューラルネットワークの理論には合成関数の微分に使われる「チェイン・ルール」が非常に重要な概念となる。これが、ニューラルネットワークの理論を支える重要な発見の一つである「誤差逆伝搬法(backpropagation)」の基礎となる。

今、損失関数$L$を簡単のために$\mathbf{A}$の関数だとみなそう。$A$は最初の$\tilde{\mathbf{y}}$を計算するのに使われている。その意味では$\tilde{\mathbf{y}}$も$\mathbf{A}$の関数だ。また$\mathbf{t} = \mathbf{Ax} + \mathbf{b}$のように書くこととすると、損失関数は合成関数として以下のように書ける。

$$
L(\mathbf{A}) = L_{CE}(\sigma(\mathbf{t}(\mathbf{A}))
$$

従って、これを$\mathbf{A}$の要素の一つである$A_{ij}$で微分したなら、その値は

$$
\frac{\partial L}{\partial A_{ij}} = \left( \frac{\partial{L_{CE}}}{\partial \tilde{\mathbf{y}}} \cdot \frac{\partial \tilde{\mathbf{y}}}{\partial \mathbf{t}} \cdot \frac{\partial \mathbf{t}}{\partial \mathbf{A}} \right)_{ij}
$$

のような表現で表せるはずである。実際、上式に現れる偏導関数はそれぞれ以下のように書ける。

$$
\begin{aligned}
    \left( \frac{\partial L_{CE}}{\partial \tilde{\mathbf{y}}} \right)_i &= -\frac{y_i}{\tilde{y}_i} \\
    \left( \frac{\partial \tilde{\mathbf{y}}}{\partial \mathbf{t}} \right)_{ij} &= \delta_{ij} \tilde{y}_i - \tilde{y}_i \tilde{y}_j \\
    \left( \frac{\partial \mathbf{t}}{\partial \mathbf{A}} \right)_{ijk} &= \frac{\partial t_k}{\partial a_{ij}} = \delta_{ik} x_j
\end{aligned}
$$

これらの式の導出については、自分自身でも確かめてみてほしい。それほど難しくはない。もちろん $\mathbf{b}$ に対しても同様に計算が可能だ。一番下の式だけを示せば十分だろう。

$$
\left( \frac{\partial \mathbf{t}}{\partial \mathbf{b}} \right)_{ij} = 1
$$

これらの式を使うことで、実際にパラメータである$\mathbf{A}$と$\mathbf{b}$に関する損失関数の勾配が計算できたら、もっとも単純には最急降下法を用いて、

$$
\begin{aligned}
    A_{ij}^{t+1} &= A_{ij}^t + \alpha \frac{\partial L}{\partial A_{ij}} \\
    b_{i}^{t+1} &= b_i^t + \alpha \frac{\partial L}{\partial b_i}
\end{aligned}
$$

のようにして値を更新していけば良い。なお$\alpha$は最急降下法のステップ幅である。

## ロジスティック回帰の実装

ここまで理解できたら、実際に実装をしてみよう。これをPythonでどう実装すべきか迷うかもしれない。例えば三階のテンソルが出てくる$\frac{\partial \mathbf{t}}{\partial \mathbf{A}}$は特に難しそうだ。もちろん愚直にfor文を回して要素を入れていけば良いのかもしれないが、それではあまりに効率が悪い。では、どうするか？実はちょっとしたコツがある。

NumPyは行列計算をするライブラリだが、その中に`einsum`という関数が用意されている。この関数は[アインシュタインの縮約記法](https://ja.wikipedia.org/wiki/%E3%82%A2%E3%82%A4%E3%83%B3%E3%82%B7%E3%83%A5%E3%82%BF%E3%82%A4%E3%83%B3%E3%81%AE%E7%B8%AE%E7%B4%84%E8%A8%98%E6%B3%95){: target="_blank" }を計算するためのものだ。例えば行列同士の積ならば次のように書ける。

```python
C = np.einsum('ik,kj->ij', A, B)
```

これを利用すると$\frac{\partial \mathbf{t}}{\partial \mathbf{A}}$なども比較的簡単に書ける。この式はアインシュタインの縮約記法そのものだからだ。

もう少し詳しくみていこう。式中にある $\delta_{ik}$ は $i = k$ ときだけ1である行列なのだから、単なる単位行列だ。一方、ここにかかっている $x_j$ は単なるベクトルだ。つまりここでは行列とベクトルの積を考えているわけだが、通常の行列・ベクトル積とは少し異なる。これは二つのベクトルから行列を作ることを考えると分かりやすいかもしれない。その場合には$\mathbf{x} \mathbf{x}^T$ のような積をとるだろう。これ内積のようにスカラーが得られる代わりに行列が得られる。それと同じだ。

実際に`einsum`を使って書いてみると、以下のようになる。

```python
dtdA = np.einsum('ik,j->ijk', delta, x)
```

どうだろうか？何となくイメージがつかめるはずだ。ここまでくれば、他の式についても、同様に計算できることがわかるだろう。以下では結果だけを示しておくので、どうしてそうなるかは各自考えてほしい。

```python
delta = np.identity(AA.shape[0])
dLdy = -y_real / y_pred
dydt = np.einsum('ij,i->ij', delta, y_pred) - np.einsum('i,j->ij', y_pred, y_pred)
dtdA = np.einsum('ij,k->ijk', delta, x)
dtdb = np.ones((bb.shape[-1], bb.shape[-1]))
```

各項が計算できたら、これらの積を取ろう。そうすることで$\frac{\partial L}{\partial \mathbf{A}}$および$\frac{\partial L}{\partial \mathbf{b}}$が計算できる。

```python
dLdt = np.dot(dLdy, dydt)
dLdA = np.dot(dLdt, dtdA)
dLdb = np.dot(dLdt, dtdb)
```

あとは、これを全ての学習データで平均をとって、パラメータ $\mathbf{A}$と$\mathbf{b}$を更新すれば良い。

### 確率的勾配法

ただ、残念なことにMNISTの場合でいえば60000もある学習データ全てに対しての平均を計算することは、計算効率があまり良くない。今回は数字が10種類しかないので、おそらく60000万個を一度に考えなくても、それなりの数のデータをサンプルして勾配を計算すれば全体の平均で計算した勾配と似たものが得られそうだ。

このような確率的なサンプリングにより選んだデータから勾配を計算する方法を確率的勾配法と呼ぶ。特に最急降下法の勾配をサンプルしたデータから選ぶ方法を**確率的最急降下法** (SGD = stochastic gradient descent) と呼ぶ。今回のMNISTのケースで言えば、だいたい30-50くらいのサンプルを取って勾配を計算すれば、十分にうまく収束する。

確率的勾配法を用いる際に、サンプルしてきたデータの集合をミニバッチと呼ぶ。データをミニバッチに分けるメソッドは通常の機械学習や深層学習のフレームワークに含まれていることが多いが、ここでは自前でミニバッチに分割しよう。Pythonのスライスを使えば簡単に実装できる。

NumPyには配列をシャッフルする関数が二つ用意されている。`numpy.random.shuffle`と`numpy.random.permutation`だ。`shuffle`は入力した配列自体をランダムに並び変えるのに対して`permutation`はランダムに入れ替えた配列を返す。今回は配列のインデックスを作成して、それを`permutation`で入れ替えたものを使い、ランダムな順序に学習データを取り出す。

```python
batch_size = 32
indices = np.random.permutation(np.arange(num_data))
for b in range(b, num_data, batch_size):
    batch_indices = indices[b:b+batch_size]
    X_batch = X[batch_indices, :]
    y_batch = y[batch_indices, :]
    # 以下、各バッチに対する処理
```

あとはバッチ内の各データに対して、コスト、精度、勾配の平均値を計算して、パラメータを更新する。NumPyを用いる場合、バッチ内のデータ全てに対して、一度にコストを求めたり、勾配を求めたりすることも可能(しかも高速)だが、最初はデータを一つ一つ処理して平均をとる方が簡単だろう。まずは、単純なものを作って、うまく動くことを確認してから、より高速に動作するコードを書く方が良い。

