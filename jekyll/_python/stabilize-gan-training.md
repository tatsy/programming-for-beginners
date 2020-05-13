---
layout: post
title: GANの学習安定化テクニック
---

2014年にGAN, 2015年にDCGANが発表されて以降、様々な学習安定化のテクニックが提案されてきた。ここでは、その代表例を、簡単に紹介する。ぜひ、各自の課題の参考にしてほしい。

---

## GANの学習が難しい原因と問題

GANはGeneratorがDiscriminatorを騙せるようなきれいな画像を作り出すことができれば、よりよい画像を作ることができるが、ここにはよく知られた問題が2つある。

1つ目の問題はGeneratorあるいはDiscriminatorの一方だけが早く学習していまい、もう一方のネットワークの学習が進みづらくなる問題である。この問題は初期においてはDiscriminatorの学習が進みすぎて、Generatorがどのような画像を作っても簡単に見破ってしまい、結果としてGeneratorが学習しなくなる。ただ、最近はGeneratorを安定して学習させる技術がいろいろ出てきたことで、以下にも説明するTTUR (two time-scale update rule)にもあるようにDiscriminatorの方を早く学習させるのが主流となっている。

2つ目の問題はmode collapseやmissing modeと呼ばれる問題で、Generatorが最も得意な画像、例えば数字なら0、だけを作ってDiscriminatorを騙すように学習してしまうことを指す。Generatorは1-9の数字は全く作ることなく、よりよい0を作ることを目指して学習してしまう。これを防ぐためには、Discriminatorが1枚1枚の画像だけではなく、バッチの中のデータの多様性などを判断できるようにするとよく、初期においてはfeature matchingやminibatch discriminationなどが提案された。

## Label Smoothing

Salimans et al. 2016, "Improved Techniques for Training GANs" \\
<https://arxiv.org/abs/1606.03498>{: target="_blank" }

一般にDiscriminatorは、ある画像が与えられたときの「本物らしさ」と「偽物らしさ」の比を学習する (論文3.4項を参照)。これらを学習する時、ターゲットとなる本物ラベル値 $\alpha$と偽物ラベル値$\beta$を1, 0とする代わりに少し中央に寄せて、0.9, 0.1とすると学習が安定することが知られていた。このやり方を**two-sided label smoothing**と言う。

GANの場合には、Generatorから出力される画像が、信頼できる画像ばかりとは限らないので、その画像までも正しいと学習しないように偽物のラベルについては$\beta=0$のままとして$\alpha$だけを0.9のように小さな値に変更するほうが良いとされている。このやり方を**one-sided label smoothing**という。

実際の実装では、これまで

```python
criterion = nn.BCEWithLogitsLoss()
lossD = criterion(y_real, torch.ones_like(y_real)) + criterion(y_fake, torch.ones_like(y_fake))
```

のように学習していたものを、

```python
lossD = criterion(y_real, torch.ones_like(y_real) * 0.9) + criterion(y_fake, torch.ones_like(y_fake))
```

のように書き換えるだけでよく、実装も簡単である。

## Feature Matching

Salimans et al. 2016, "Improved Techniques for Training GANs" \\
<https://arxiv.org/abs/1606.03498>{: target="_blank" }

これは、GeneratorがDiscriminatorに対して学習しづらいことを改善する問題で、偽物サンプルに対する学習値が1になるようにGeneratorを学習することに加えて、Generatorに対しても本物らしさを学習させようとするものである。

Discriminatorは画像が入ってきたときに0と1の間のラベル値を出力するが、その最終出力の前に得られている特徴量を使い、偽物サンプルの特徴量と本物サンプルの特徴量がより近くなるようにGeneratorを学習させる。こうすることで、より安定してGeneratorを学習させることができるようになる。

また、特徴量の近さを図るときには、ミニバッチの中での特徴量の平均を取るため、Generatorが偏ったサンプルを出力してしまうmode collapseにも効果があると考えられる。

## Minibatch Discrimination

Salimans et al. 2016, "Improved Techniques for Training GANs" \\
<https://arxiv.org/abs/1606.03498>{: target="_blank" }

この方法はGeneratorが特定のカテゴリの画像だけを作るようになってしまうmode collapseを防ぐための方法で、DiscriminatorがGeneratorが偏ったサンプルを出力しているのかどうかを判別できるようにする手法だ。

論文中の式は、一度特徴ベクトルを線形変換する操作が入るが、基本的にやっていることは、ミニバッチの中にあるサンプル同士の距離の計算である。その距離の情報を特徴ベクトルに追加して、次のレイヤーに送ることで、似たようなサンプルばかりがミニバッチに含まれていることが、Disciminatorによって判別できるようになる。

## Unrolled GAN

Metz et al. 2016, "Unrolled Generative Adversarial Networks" \\
<https://arxiv.org/abs/1611.02163>{: target="_blank" }

これはDiscriminatorがGeneratorに対して速く学習してしまうことを防ぐ方法の一種で、Generatorが常にDiscriminatorの学習を後手から学習するがために、上手く学習しない問題を解決するものである。

Unrolled GANではDiscriminatorを一定のステップ$K$だけ先に進めて学習を行い、その先に学習を進めたDiscriminatorの予想を元にGeneratorを学習する。こうすることで、Discriminatorは相対的に学習していないGeneratorの情報で、Generatorは相対的に学習が進んだDiscriminatorの情報を使って学習できることになり、両者の不均衡が緩和されるというものである。

## Boundary Equibillium GAN (BEGAN)

Berthelot et al. 2017, "BEGAN: Boundary Equilibrium Generative Adversarial Networks" \\
<https://arxiv.org/abs/1703.10717>{: target="_blank" }

この手法もDiscriminatorとGeneratorの学習の不均衡を緩和する方法の一種で、Discriminatorの現在の誤差とGeneratorの現在の誤差を比較し、Discriminatorの誤差が相対的に小さくなったら、よりGeneratorの学習が進むように誤差の量を調整するというアプローチを取っている。

## Wasserstein GAN (& WGAN-GP)

Arjovsky et al. 2017, "Wasserstein GAN" \\
<https://arxiv.org/abs/1701.07875>{: target="_blank" }

Gulrajani et al.2017, "Improved Training of Wasserstein GANs" \\
<https://arxiv.org/abs/1704.00028>{: target="_blank" }

この方法はGANの学習を安定化させるためにWasserstein距離という確率密度分布同士の距離を測る指標を導入している。

Wasserstein GANの基本的な考え方は誤差関数にリプシッツ連続性 (平たく言うと、勾配が大きくなりすぎないということ) を導入すると、学習が安定することを解いている。実際の実装では、誤差関数に交差エントロピーを用いるのをやめ、

```python
lossD = torch.mean(y_real) - torch.mean(y_fake)
lossG = torch.mean(y_fake)
```

とし、パラメータ更新の際に勾配が一定以上の大きさの場合にクリッピングする処理を入れることで、上記のリプシッツ連続性を実現している。

さらに、しかしながら、上記のWGANでは、クリッピングをする際の閾値が小さすぎれば学習が遅くなり、一方で閾値が大きすぎれば元々のGANとほとんど同じになってしまい、閾値の調整が難しいという問題があった。

WGANの考え方を進めたWGAN-GP (GP = gradient penalty) では、勾配をクリッピングする代わりに、勾配の大きさに対して誤差をかけて、勾配が大きくなりすぎないように制約することで、元々のWGANの問題を解決している。

なお、上記のWGAN、WGAN-GPにおいては、Discriminatorが何度か学習するごとにGeneratorを更新するという考え方が、導入されており、これ以後、Discriminatorの学習頻度を上げることが一般化していく。

## Least-Squares GAN (LSGAN)

Mao et al. 2017, "Least Squares Generative Adversarial Networks" \\
<https://arxiv.org/abs/1611.04076>{: target="_blank" }

LSGANは、Discriminatorが本物か偽物かを判別する情報に加えて、どのくらい本物らしいのか、というのを二乗誤差によって図ることで、よりよいサンプルの生成を目指す手法です。

これまでのDCGANなどは交差エントロピーを用いて、本物か偽物かの判断を行っており、これは多次元空間のなかで、本物と偽物の間の境界線を引く問題に相当します。もしGeneratorが十分に本物に近いものを生成するようになると、この境界線の十分内側の領域に偽物のサンプルが生成されます。

すると、Discriminatorにとっては、それが偽物なのか本物なのかの判断が難しくなり、Generatorの学習が進むに従って、Discriminatorが学習しづらくなっていきます。

LSGANはこの問題を防ぐためにGeneratorが仮に決定境界の十分内側のサンプルを生成した場合に、それを境界線付近に戻すように誤差を設定することでDiscriminatorの学習が上手くいかなくなる問題を防いでいます。

実装自体もとても簡単で、誤差関数を交差エントロピーから以下のような二乗誤差に置き換えるだけです。

```python
a, b, c = -1.0, 1.0, 0.0
criterion = nn.MSELoss()
lossD = 0.5 * criterion(y_true, torch.ones_like(y_true) * b) + \
        0.5 * criterion(y_fake, torch.ones_like(y_fake) * a)
lossG = criterion(y_fake, torch.ones_like(y_fake) * c)
```

## Two time-scale Update Rule (TTUR)

Heusel et al. 2017, "GANs Trained by a Two Time-Scale Update Rule Converge to a Local Nash Equilibrium" \\
<https://arxiv.org/abs/1706.08500>{: target="_blank" }

## Progressive Growing of GANs (PGGAN)

Karras et al. 2017, "Progressive Growing of GANs for Improved Quality, Stability, and Variation" \\
<https://arxiv.org/abs/1710.10196>{: target="_blank" }

## Spectral Normalization (SNGAN)

Miyato et al. 2018, "Spectral Normalization for Generative Adversarial Networks" \\
<https://arxiv.org/abs/1802.05957>{: target="_blank" }

## Hinge Loss (vs Softplus)

Miyato et al. 2018, "Spectral Normalization for Generative Adversarial Networks" \\
<https://arxiv.org/abs/1802.05957>{: target="_blank" }


## Self-Attention (SAGAN)

Zhang et al. 2018, "Self-Attention Generative Adversarial Networks" \\
<https://arxiv.org/abs/1805.08318>{: target="_blank" }

## BigGAN

Brock et al. 2018, "Large Scale GAN Training for High Fidelity Natural Image Synthesis" \\
<https://arxiv.org/abs/1809.11096>{: target="_blank" }

## Style GAN

Karras et al. 2019, "A Style-Based Generator Architecture for Generative Adversarial Networks" \\
<https://arxiv.org/abs/1812.04948>{: target="_blank" }


## その他の参考になる資料

Goodfellow 2016, "NIPS 2016 Tutorial: Generative Adversarial Networks" \\
<https://arxiv.org/abs/1701.00160>{: target="_blank" }
