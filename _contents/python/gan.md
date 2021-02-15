---
layout: post
title: 敵対的生成ネットワーク
---

## GANが生まれるまで

前回までの内容は、画像をニューラルネットワークを始めとするプログラムに理解させる内容であった。このように画像に写っているものを認識・識別する機械学習モデルを識別モデルという。

これと対になる考え方に生成モデルがある。生成モデルとは、何もないところ (多くの場合はランダムノイズ) から人間にとって意味のある画像を作り出す技術である。例えば、教師データを単純な多次元の確率分布としてモデル化できれば、その多次元空間からのサンプリングにより、意味のあるデータを作り出すことができそうだ。

深層学習が現れる前は、この多次元分布のモデル化が単純な確率密度分布 (例えばガウス混合モデルなど) では難しく、画像のような情報量の多い対象を扱うことは難しかった。その状況を一変させたのが、2014年に当時モントリオール大学の博士課程の学生であったIan Goodfellow氏が提案した[GAN (Generative Adversarial Network)](https://arxiv.org/abs/1406.2661){: target="_blank" }である。

GANの考え方は、その後、様々に改良されて、現在は本物の写真とほとんど見分けがつかないような写真の生成さえ可能となっている。また、Dueling neural networkとして、2018年にMIT Technology Reviewの [10 Breakthrough Technologies](https://www.technologyreview.com/10-breakthrough-technologies/2018/){: target="_blank" }に選ばれている。

## GANの基本

ニューラルネットワークを使うかどうかはともかくとして、生成モデルの目的は、教師画像にあるような、それらしい画像を作る、というところにある。従って、プログラムにより作られた画像が十分教師画像に近ければ、そのデータ生成は成功していると言って良い。

そこで、GANでは、画像を生成する役割をニューラルネットに担当させるのと同時に、画像が教師画像らしいのかどうかを判別することもニューラルネットに担当させる。これらのネットワークをそれぞれGeneratorとDiscriminatorと呼ぶ。

学習の過程において、GeneratorはDiscrmininatorに偽物と見破られないような画像を作るように学習し、一方のDiscriminatorはGeneratorが生成した偽物を正しく見分けるように学習しようとする。そのため、この2つのネットワークがadversarial (敵対的)であるとして、このネットワークのペアをGANと呼んでいる。

一般にGeneratorは多次元のランダムベクトルから、画像を生成し、Discriminatorは画像に対して0(偽物)か1(本物)のラベルを付ける問題を解く。

## GANの構成

今日GANと呼ばれているものはRadfordらが2015年に提案した[DCGAN (Deep convolutional GAN)](https://arxiv.org/abs/1511.06434){: target="_blank" } で、この論文にはネットワークの実装にあたり、以下のような構造が学習の安定化に必要だと述べられている。

1. Poolingのレイヤーを2x2の畳み込みなどの画像サイズを小さくするConvolutionレイヤーに置き換えよ。
1. バッチ正規化をGeneratorとDiscriminatorの両方に使え。
1. より深いネットワークにするなら全結合層をなくせ。
1. Generatorの活性化にはReLUを使え。最後の出力はtanhで行え。
1. DiscriminatorにはLeaky ReLUを使え。

この中には2020年現在の最新研究では、あまり使われていない考え方もあるが、何はともあれ、これに従って、実際にネットワークを実装してみよう。

### Generatorの実装

上記のルールに従うと、Generatorは畳み込み、バッチ正規化、ReLUの3つを何度も行うネットワークになる。この3つの組をブロックと呼ぶことにすると、1つのブロックは以下のように定義できる。

```python
block = nn.Sequential(
    nn.ConvTranspose2d(in_channels, out_channels, kernel_size=4, stride=2, padding=1),
    nn.BatchNorm2d(out_channels),
    nn.ReLU(inplace=True)
)
```

Generatorは画像を徐々に大きくしていくネットワークなので、畳み込み層の代わりに逆畳み込みが使われている点に注意してほしい。上記のブロックに画像を通すと、画像のチャネル数が `in_channels` から `out_channels` に変化して、さらに画像のサイズが各方向2倍に拡大される。

ところで、Generatorの入力は多次元ベクトルだという話をした。多次元ベクトルにいきなり逆畳み込みを行うことはできないので、ベクトルを1x1の画像にデータ変換するのを忘れないようにしよう。

```python
n_batch, n_dim = x.size()  # 入力のベクトルはn_dim次元
x = x.view(n_batch, n_dim, 1, 1)  # これでチャネル数がn_dimの1x1の画像とみなされる
```

最後に画像を出力するときには、ReLUで活性化する代わりにtanh (Hyperbolic tangent)を使う。シグモイド関数は値域が`[0, 1]` であるのに対して、tanhは値域が `[-1, 1]` と分布中心が0になっているため、各畳み込み層がバイアスベクトルを学習する必要がなくなり、学習の進行が安定する傾向がある。

```python
x = torch.tanh(x)
return x
```

### Discriminatorの実装

Discriminatorの基本ブロックは、GeneratorのそれのReLUがLeaky ReLUに置き換わったものになる。Leaky ReLUは入力の値が負のときに一定の傾きを持った線形関数を当てはめる活性化関数である。Radfordらの論文には負の値に対して与える傾きは0.2が良いと書かれている。

これらに従うと、Discriminatorの基本ブロックは以下のように定義できる。

```python
block = nn.Sequential(
    nn.Conv2d(in_channels, out_channels, kernel_size=4, stride=2, padding=1),
    nn.BatchNorm2d(out_channels),
    nn.LeakyReLU(0.2)
)
```

このブロックに画像を通すと、チャネル数が`in_channels`から`out_channels`に変化し、画像のサイズが各方向で2分の1になる。

この操作を徐々に繰り返していくと、1x1の画像になるので、あとはベクトルとして出力するために、出力形状を変換する。

```python
x = x.squeeze()  # 大きさが1の次元を削る
```

なお、Discriminatorは本物か偽物かの識別問題を解くので、安直にはsigmoid関数で活性化して出力する。損失関数の計算では、これを交差エントロピーで評価するのだが、sigmoidと交差エントロピーを個別に評価するよりも、それらを合わせた関数を一度に評価するほうが数値的に安定することが知られている。

PyTorchでは、交差エントロピーの関数が `torch.nn.BCELoss` で与えられているのに対して、sigmoidを組み合わせたものは `torch.nn.BCEWithLogitsLoss` で与えられる。今回は後者を使うので、Discriminatorの出力時点ではsigmoid関数で活性化する必要はない。

### Optimizerの定義

DCGANの論文ではoptimizerにAdamと呼ばれる方法が用いられている。これは確率的最急降下法(SGD)を2つの点で拡張した方法である。

SGDは、ミニバッチで与えられる小さなデータ集合に対して、ネットワークのパラメータを更新するための勾配を求める方法であった。この方法だと、たまたま良くないデータ集合がミニバッチで与えられたときに、急激にパラメータの更新方向が変化してしまい、学習が上手くいかなくなる事がある。

これを防ぐために、各ミニバッチに対して求まった勾配をそのまま使うのではなく、これまでの履歴と今求まった勾配の情報を線形補間して、パラメータを更新する。こうすることで、仮に良くない勾配が出てきたときにも、その影響を少なく抑えることができる。この考え方はモーメンタムと呼ばれていて、これを入れたSGDをMomentum SGDなどと呼ぶ。

また、SGDは、誤差を逆伝搬して与えられる勾配を(モーメンタムを使うかはともかくとして)、そのままパラメータ更新に利用する。すると、誤差に対して寄与の大きなパラメータばかりが更新されてしまい、本来重要な可能性がある他のパラメータがあまり更新されないという事態が起こる。

これを改善するための方法として、これまで、各パラメータがどのくらい更新されたのかを記録しておき、あまり更新されていないパラメータは大きく、たくさん更新されているパラメータは小さく更新するというアプローチが考えられる。この方法を用いているものにAdaGradやRMSpropという方法がある。

Adamはこの2つの考え方をあわせたもので、モーメンタムを使って急激な勾配の変化を防ぎつつも、まだ更新が浅いパラメータを良く更新しようとする。Adamが取るパラメータは学習率 `lr` の他に`beta1`と`beta2`の2つがあり、`beta1` が勾配の急激な変化を防ぐパラメータで`beta1`が大きければ大きいほど、過去の履歴を重視し、あらたま勾配が反映されづらくなる。一方、`beta2`はこれまでの更新量に応じて、現在の更新量を調整するパラメータで、こちらは大きければ大きいほど、より以前の更新量の大きさに応じて、現在の更新量をへらす傾向にある。逆に`beta2`が小さければ、以前に比較的直近の大きな更新だけを考慮する。

Radfordらの論文では、Adamのパラメータとして`lr=2.0e-4`, `beta1=0.5`, `beta2=0.9`を使うことが推奨されているので、これに従ってGeneratorとDiscriminatorのそれぞれに対してOptimizerを定義する。

```python
optimG = torch.optim.Adam(netG.parameters(), lr=2.0e-4, betas=(0.5, 0.9))
optimD = torch.optim.Adam(netD.parameters(), lr=2.0e-4, betas=(0.5, 0.9))
```

### 学習ループ

学習の流れは、

* Generatorが画像を生成
* Discriminatorが画像を判別
* 損失関数により誤差を評価

というのが基本となる。

Generatorを学習するときにはDiscriminatorに偽物の画像だけを判別させ、その出力が1(本物)に近くなるように学習を行う。

```python
optimG.zero_grad()

z = torch.randn([n_batches, sample_dims], dtype=torch.float32, device=device)
x_fake = netG(z)
y_fake = netD(x_fake)

lossG = criterion(y_fake, torch.ones_like(y_fake))
lossG.backward()

optimG.step()
```

一方で、Discriminatorを学習する場合には、本物の画像を偽物の画像の療法をDiscriminatorに通し、その出力が、本来あるべき値、すなわち本物なら1、偽物なら0になるように学習を行う。

```python
# Discriminatorの学習
optimD.zero_grad()

z = torch.randn([n_batches, sample_dims], dtype=torch.float32, device=device)
x_fake = netG(z)
x_fake = x_fake.detach()
y_fake = netD(x_fake)
y_real = netD(x_real)

lossD = criterion(y_fake, torch.zeros_like(y_fake)) +\
        criterion(y_real, torch.ones_like(y_real))
lossD.backward()

optimD.step()
```

なお上記のコードの中に`detach()`という関数が使われているが、これは、この値に対して、それ以前に誤差を逆伝搬しないようにする関数である。Discriminatorを学習する際はGeneratorまで誤差を逆伝搬する必要がないため、このようにしている。

## 学習結果

上記のような単純な実装であったもMNISTであれば、十分に速く、かなりそれらしい画像を出すことができる。

以下は、MNISTで10エポック(Colab上で8分程度)学習したときに得られる出力画像だが、短時間の学習でもそれらしい画像が出ていることが分かる。

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**教師画像**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**出力画像**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/gan/mnist_x_real_010.jpg" style="width: 100%;" %}</td>
  <td>{% include lightbox.html src="/public/images/gan/mnist_x_fake_010.jpg" style="width: 100%;" %}</td>
</tr>
</table>

## 補足: 変分オートエンコーダ

現在主流の深層生成モデルにはGANの他にVAE (Variational Autoencoder) がある。Autoencoderとは入力に入れた画像を、一度多次元ベクトルとして特徴化し、その多次元ベクトルから再び、入力と同じ画像を作る操作を指す。画像を多次元ベクトルに変換するネットワークをencoder、多次元ベクトルから画像に戻すネットワークをdecoderと呼ぶ (encoder, decoderは必ずしもネットワークでなくても良い)。

VAEはさらに、encoderが作り出す多次元ベクトルがランダムに近い分布になるよう学習する。すると、画像からエンコードされていない、ランダムな多次元画像に対しても画像を生成できるようになる。
