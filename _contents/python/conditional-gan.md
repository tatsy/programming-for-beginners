---
layout: post
title: 条件付き画像生成
---

前回はランダムノイズから、ランダムな画像を生成するGANについて解説した。一方で、例えば数字なら、0や5といった特定の数字を狙って生成したいという需要があるだろう。

このような特定の条件を満たす画像を生成する技術はいくつかあるが、今回は前回のGANを拡張したConditional GANについて解説する。

## GANからの変更点

GANからConditional GANへの拡張は単純で、GeneratorとDiscriminatorの両方の入力に対して、どの種類の画像を生成したいのかを意味する余剰次元を追加するだけだ。

例えば、MNISTのデータを利用している場合、画像の種類は10種類なので、追加する余剰次元は10である。もし5という数字を生成したければ、ランダムベクトルに対して `[0, 0, 0, 0, 0, 1, 0, 0, 0, 0]` という10次元ベクトルを追加して、それをGeneratorに渡す。

```python
z = torch.randn([n_batches, sample_dims], dtype=torch.float32, device=device)
z = torch.cat([z, c_onehot], dim=1)  # c_onehotが10次元の余剰次元を表すベクトル
x_fake = netG(z)
```

Discriminatorの場合は入力が画像なので、画像の各画素に対して、上記の10次元ベクトルを追加する。

```python
c_onehot_tile = c_onehot.view(num_batch, -1, 1, 1).repeat(1, 1, height, width)
x_and_c = torch.cat([x, c_onehot_tile], dim=1)
y = netD(x_and_c)
```

前回のDCGANのプログラムを参考に、この書き換えには各自で取り組んでほしい。

## 学習の結果

以下は前回同様10エポック(Colabで13分程度)学習して得られる生成画像である。このように、短い学習時間でも、十分にそれらしい画像が得られていることが分かる。

{% include lightbox.html src="/public/images/gan/mnist_x_fake_class_tile.jpg" %}

## まとめコード

<https://github.com/tatsy/programming-for-beginners/blob/master/_programs/python/gan/DCGAN_mnist_class.ipynb>
