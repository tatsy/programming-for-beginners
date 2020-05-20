---
layout: post
title: GANの学習安定化テクニック
---

2014年にGAN, 2015年にDCGANが発表されて以降、様々な学習安定化のテクニックが提案されてきた。ここでは、その代表例を、簡単に紹介する。ぜひ、各自の課題の参考にしてほしい。

---

## GANの学習が難しい原因と問題

GANはGeneratorがDiscriminatorを騙せるようなきれいな画像を作り出すことができれば、よりよい画像を作ることができるが、ここにはよく知られた問題が2つある。

1つ目の問題はGeneratorあるいはDiscriminatorの一方だけが早く学習していまい、もう一方のネットワークの学習が進みづらくなる問題である。この問題は初期においてはDiscriminatorの学習が進みすぎて、Generatorがどのような画像を作っても簡単に見破ってしまい、結果としてGeneratorが学習というものだ。ただ、最近はGeneratorを安定して学習させる技術がいろいろ出てきたことで、以下にも説明するTTUR (two time-scale update rule)にもあるようにDiscriminatorの方を早く学習させるのが主流となっている。

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
lossD = criterion(y_real, torch.ones_like(y_real) * 0.9) + criterion(y_fake, torch.zeros_like(y_fake))
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

これまでのDCGANなどは交差エントロピーを用いて、本物か偽物かの判断を行っており、これは多次元空間のなかで、本物と偽物の間の境界線を引く問題に相当します。一般にDiscriminatorは本物のサンプルが集まっているところに決定境界を引きますが、このときGeneratorがDiscriminatorが本物と判定する決定境界の内側にありながらも、本物のサンプルとは遠いサンプルを生成した時、sigmoid関数を用いている場合には、そのサンプルを本物に近づける方向に学習が進みません (論文、図1を参照)。

LSGANはこの問題を防ぐためにGeneratorが仮に決定境界の内側のサンプルを生成した場合に、それを境界線付近に戻すように誤差を設定することでDiscriminatorの学習が上手くいかなくなる問題を防いでいます。

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

この論文は強化学習で用いられるactor-critic学習の知見をGANに応用したものである。Actor-critic学習とは、強化学習において、行動選択をするactorとその行動の結果を評価するcriticを用いる手法で、GANのgeneratorとdiscriminatorの関係と良く似ている (criticという言葉はWGANの時点で登場している)。

強化学習の分野でactorとcriticの学習でcriticをactorに比べて速く学習させると、両者の関係がゲーム理論のナッシュ均衡に近づくという知見に基づき、GANにおいてもdiscriminatorをgeneratorより速く学習させるようにすれば、より学習が安定し、収束が早まることを示している。

この論文では、元論文でdiscriminatorをgeneratorよりも多く学習させていたWGAN-GPで、それぞれを同じ頻度で学習するようにし、discriminatorに大きな学習率を用いると、学習がより速く進むことを示した(論文、図7を参照)。

また、これまでGANの生成した画像を評価する指標として広く用いたれてきたinception scoreに変わる指標としてFrechet Inception Distance (FID)という指標を新たに提案している。Inception scoreがImageNetで学習したInception v3モデルに画像を通したときに得られる画像識別のラベルの分散が少ない(=特定の物体らしく見える)ときにスコアが高くなる指標だが、実際の画像に見られるバラエティの多さについては評価していない。そこで、FIDはInception v3が与える識別ラベルの平均と分散を教師、生成画像の両方で計算し、そこから定義されるガウス分布の距離をFrechet距離により測ることで、データの多様性についても評価している。

## Progressive Growing of GANs (PGGAN)

Karras et al. 2017, "Progressive Growing of GANs for Improved Quality, Stability, and Variation" \\
<https://arxiv.org/abs/1710.10196>{: target="_blank" }

PGGANは通常のGANのトレーニングでは難しかった高解像度画像の生成を"progressive"なトレーニングにより実現する手法である。基本的なアイディアは単純で、最初は4x4のような小さな画像からGANをトレーニングし始め、それが十分に収束したら、そのgeneratorに逆畳み込み層を、discriminatorに畳み込み層を追加して、8x8の画像でトレーニングを行う。

これを順次繰り返していき、徐々に大きな画像をトレーニングしていくと、最初から高解像度画像をトレーニングするよりも、安定して高解像度画像を生成するネットワークをトレーニングできる。

この論文の動画は、一般の技術ニュースなどでも広く取り上げられたので、もし見たことがない人がいれば一見することを勧める。

<https://www.youtube.com/watch?v=XOxxPcy5Gr4>{: target="_blank" }

## Spectral Normalization (SNGAN)

Miyato et al. 2018, "Spectral Normalization for Generative Adversarial Networks" \\
<https://arxiv.org/abs/1802.05957>{: target="_blank" }

Preferred Networkの研究者によって発表されたGANのトレーニング安定化に関する論文。この論文ではWGANなどで議論されていたdiscriminatorのリプシッツ連続性をスペクトルノルムによる重み係数の正規化によって実現する。スペクトルノルムとは行列に対するノルム(=大きさ)の尺度の一つで、一般的な(L2ノルムをベクトルノルムに使う)定義では最大特異値に一致する。

畳み込みを始めとするニューラルネットのレイヤーは行列の掛け算に対応するので、この行列に対してスペクトルノルムを計算し、そのノルムによって、重みを正規化する。SNGANではdiscriminatorからbatch normalizationを除き、その代わりにspectral normalizationが用いているが、これらは同時に使えないという訳ではなく、以下に説明するself-attention GANなどでは両方同時に使われている。

## Hinge Loss (vs Softplus)

Miyato et al. 2018, "Spectral Normalization for Generative Adversarial Networks" \\
<https://arxiv.org/abs/1802.05957>{: target="_blank" }

これも上記のSNGANの論文で用いられているテクニックで、WGANで用いられていたロスをhingeロスという閾値計算の入ったロスに変更すると、inception scoreやFIDの観点から画像の性質が向上することが報告されている。実装は非常に単純でReLUを用いて以下のように書ける (generatorのロスは変更なし)。

```python
import torch.nn.functional as F
lossD = F.relu(1.0 - y_real).mean() + F.relu(1.0 + y_fake).mean()
lossG = -torch.mean(y_fake)
```

## Self-Attention (SAGAN)

Zhang et al. 2018, "Self-Attention Generative Adversarial Networks" \\
<https://arxiv.org/abs/1805.08318>{: target="_blank" }

この論文では元々機械翻訳の分野で用いられていたattentionの考え方をGANに応用している。画像生成の分野でも画像に説明をつけるcaptioningなどでは広く用いられてきた考え方だが、この論文では"self-attention"と言っているように、「日本語から英語」、「画像から説明文」のようなドメインを変更するものではなく、自分自身の特徴マップを用いてattentionを計算する。

Attentionを画像1枚から計算するために、self-attentionにおいては、自分自身の特徴マップを異なるレイヤーによって2つ生成し、片方を転置して掛け算することでattentionを生成する。論文自体はそれほど長くなく、self-attentionの考え方も難しくないので、詳細についてはぜひ論文を見てみてほしい。

## BigGAN

Brock et al. 2018, "Large Scale GAN Training for High Fidelity Natural Image Synthesis" \\
<https://arxiv.org/abs/1809.11096>{: target="_blank" }

この論文は上記のSAGANをベースとし、以下のようなネットワーク構造を提案している。

* ロスにはhingeロスを使う
* generatorにはconditonal batch normalizationをdiscriminatorにはprojectionを使う
* optimizationにはTTURを使う
* 重みの初期値はXavier initializationや正規分布ではなくorthogonal initializationを使う

Conditional batch normalizationやprojection discriminatorはGANそのものではなく、CGANのような条件付き画像生成で用いるテクニックなので、詳細は以下の論文を参考にしてほしい。

* Conditional batch normalization \\
  Vries et al. 2017, "Modulating early visual processing by language" \\
  <https://arxiv.org/abs/1707.00683>{: target="_blank" }
* Projection discriminator \\
  Miyato and Koyama 2018, "cGANs with Projection Discriminator" \\

また、この論文では上記のSAGANをベースにしたネットワークに加えて、2つの新しい知見を提供している。

1つ目がtruncated trickと呼ばれる方法で、GANで画像生成のもととなるランダムベクトルをサンプリングする際に、ランダムベクトルのノルムが一定以上になったら、そのサンプルを棄却して、サンプリングし直すというものである。このリサンプリングに用いる閾値を0.5から1程度にすると、画像のリアルさ(fidelity)とバリエーションのバランスが良くなることが報告されている(論文、図2を参照)。

2つ目はorthogonal regularizationと呼ばれるテクニックで、各レイヤーの重み行列の各行が基底ベクトルになるように正則化するというもの。この考え方自体は以前からあったものの、従来のやり方 (論文、式(2))のやり方では、重みが過度に制約されてしまうという問題があったため、提案法では、この定義を緩めたものとして新たな定式化を提案している(論文、式(3))。

上記の方法で学習すると512x512のような比較的高解像度の画像であってもPGGANのようなテクニック無しで、安定的に画像生成ができることが報告されている。

## Style GAN

Karras et al. 2019, "A Style-Based Generator Architecture for Generative Adversarial Networks" \\
<https://arxiv.org/abs/1812.04948>{: target="_blank" }

この方法は同著者の従来法であるPGGANの発展形で、各レイヤーで画像をアップサンプルするときに、Styleと呼ばれる画像特徴のようなものとランダムノイズの両方を都度入力している。こうすることで、(やや解釈が入るが) 各レイヤーは画像の大きさに即した特徴の学習に集中することができ、結果として生成される画像に細かな特徴が再現されるようになる。

論文の図1がこれまでのGANとStyle GANの比較になっているが相違点は主に4つある。1つ目にこれまでのGANはランダムベクトルをそのままネットワークに入れていたが、Style GANの入力は固定のベクトル (学習時に変更される) であり、ランダムノイズではない。

2つ目の違いとして、入力のランダムベクトル (latent vector) は、それをスタイルベクトルに変換するmapping networkに入力される。このネットワークで変換された特徴が、各レイヤーに入力されることで、各レイヤーはその時々で必要な特徴をスタイルベクトルから抜き出せば良くなり、より画像に特徴を反映しやすく成る。

3つ目の違いとしてランダムベクトル (latent vector)とは別のノイズベクトルが各レイヤーに入力されている。これはlatent vectorとは無関係のもので、全体の画像の印象はそのままに、髪の毛の毛先の方向が変わるといったランダムなバリエーションを生成するのに役立つと説明されている (論文、3.2項と図4を参照)。

最後に各レイヤーでスタイルベクトルはAdaIN (adaptive instance normalization)という元々は絵画風の効果を付与するstyle transferの研究で用いられていたレイヤーを用いている。スタイルベクトルはスケーリングとバイアスの成分からなっていて、AdaINではスタイルベクトルを用いて入力の特徴マップを変換する (論文、式(1))。

上記の工夫を取り入れたことで、Style GANは非常に高解像度な画像の生成に成功している。余談になるが、2020年初頭に話題になった[手塚治虫の漫画風画像の生成](https://blogs.nvidia.co.jp/2020/04/27/osamu-tezuka-ai-supporterd-manga/){: target="_blank" }に用いられたネットワークがこのStyle GANである。


## その他の参考になる資料

* Goodfellow 2016, "NIPS 2016 Tutorial: Generative Adversarial Networks" \\
  <https://arxiv.org/abs/1701.00160>{: target="_blank" }
* Shall we GANs? (SSII 2019 チュートリアル) \\
  <https://www.slideshare.net/SSII_Slides/ssii2019ts11>{: target="_blank" }
