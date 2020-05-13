---
layout: post
title: GANの学習安定化テクニック
---

2014年にGAN, 2015年にDCGANが発表されて以降、様々な学習安定化のテクニックが提案されてきた。ここでは、その代表例を、簡単に紹介する。ぜひ、各自の課題の参考にしてほしい。

---

## GANの学習が難しい原因と問題

GANはGeneratorがDiscriminatorを騙せるようなきれいな画像を作り出すことができれば、よりよい画像を作ることができるわけですが、ここにはよく知られた問題が2つあります。

1つ目の問題はGeneratorあるいはDiscriminatorの一方だけが早く学習していまい、もう一方のネットワークの学習が進みづらくなる問題です。この問題は初期においてはDiscriminatorの学習が進みすぎて、Generatorがどのような画像を作っても簡単に見破ってしまい、結果としてGeneratorが学習しなくなる問題でした。ただ、最近はGeneratorを安定して学習させる技術がいろいろ出てきたことで、以下にも説明するTTUR (two time-scale update rule)にもあるようにDiscriminatorの方を早く学習させるのが主流です。

2つ目の問題はmode collapseやmissing modeと呼ばれる問題で、Generatorが最も得意な画像、例えば数字なら0、だけを作ってDiscriminatorを騙すように学習してしまうことを指します。Generatorは1-9の数字は全く作ることなく、よりよい0を作ることを目指して学習してしまうわけです。これを防ぐためには、Discriminatorが1枚1枚の画像だけではなく、バッチの中のデータの多様性などを判断できるようにするとよく、初期においてはfeature matchingやminibatch discriminationなどが提案されました。

## Label Smoothing

Goodfellow 2016, "NIPS 2016 Tutorial: Generative Adversarial Networks" \\
<https://arxiv.org/abs/1701.00160>{: target="_blank" }

## Feature Matching

Salimans et al. 2016, "Improved Techniques for Training GANs" \\
<https://arxiv.org/abs/1606.03498>{: target="_blank" }

## Minibatch Discrimination

Salimans et al. 2016, "Improved Techniques for Training GANs" \\
<https://arxiv.org/abs/1606.03498>{: target="_blank" }

## Unrolled GAN

Metz et al. 2016, "Unrolled Generative Adversarial Networks" \\
<https://arxiv.org/abs/1611.02163>{: target="_blank" }

## Boundary Equibillium GAN (BEGAN)

Berthelot et al. 2017, "BEGAN: Boundary Equilibrium Generative Adversarial Networks" \\
<https://arxiv.org/abs/1703.10717>{: target="_blank" }


## Wasserstein GAN (& WGAN-GP)

Arjovsky et al. 2017, "Wasserstein GAN" \\
<https://arxiv.org/abs/1701.07875>{: target="_blank" }

Gulrajani et al.2017, "Improved Training of Wasserstein GANs" \\
<https://arxiv.org/abs/1704.00028>{: target="_blank" }

## Least-Squares GAN (LSGAN)

Mao et al. 2017, "Least Squares Generative Adversarial Networks" \\
<https://arxiv.org/abs/1611.04076>{: target="_blank" }

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