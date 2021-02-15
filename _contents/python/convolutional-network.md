---
layout: post
title: 畳み込みニューラルネット
---

画像生成について触れる前に、畳み込みニューラルネットワーク (CNN) を用いた画像分類を行ってみる。データセットは以前ロジスティック回帰の回でも用いたMNISTを用いる。また、本資料では深層学習のフレームワークとして、比較的分かりやすく、2020年現在開発がアクティブに進んでいるPyTorchを用いる。

## データセット・クラスの用意

PyTorchに限らず、TensorFlowなどの多くの深層学習フレームワークにおいて、データセットからのデータの読み出しを抽象化するクラスが用意されている。PyTorchでは `torch.utils.data.Dataset` というクラスがそれにあたる。このクラスを継承したサブクラスでデータを読み出し、そのインスタンスを `torch.utils.data.DataLoader` クラスの引数に与えることで、データを柔軟に読み出すことが可能となる。最も簡単な例では以下のようになる。

```python
# torch.utils.data.Datasetを継承したクラス
dataset = MyDataset()  
# ミニバッチのサイズを32とし, 4スレッドで並列にデータを読み出し、データ順序をランダムに入れ替える
data_loader = torch.utils.data.DataLoader(dataset, batch_size=32, num_workers=4, shuffle=True)
```

`torch.utils.data.Dataset` クラスでは、データセットの大きさ (データの数) を返すメソッド `___len__(self)` と、1つ分のデータを返すメソッド `__getitem__(self, idx)` を実装すると、並列にデータを読み出ししたり、データの順序をランダムにしたりといった操作をクラス内で行ってくれる。例えばMNISTを用いる場合なら、以下のような実装になるだろう。

```python
class MnistDataset(torch.utils.data.Dataset):
    def __init__(self, root_dir, mode='train'):
        super(MnistDataset, self).__init__()

        self.root_dir = root_dir
        self.mode = mode

        if self.mode == 'train':
            self.image_file = 'train-images-idx3-ubyte'
            self.label_file = 'train-labels-idx1-ubyte'
        elif self.mode == 'test':
            self.image_file = 't10k-images-idx3-ubyte'
            self.label_file = 't10k-labels-idx1-ubyte'
        else:
            raise Exception('MNIST dataset mode must be "train" or "test"')
        
        self.image_data = self._load_images(os.path.join(self.root_dir, self.image_file))
        self.label_data = self._load_labels(os.path.join(self.root_dir, self.label_file))

    def __len__(self):
        return len(self.image_data)

    def __getitem__(self, idx):
        return {
            'images': self.image_data[idx],
            'labels': self.label_data[idx]
        }

    def _load_images(self, filename):
        with open(filename, 'rb') as fp:
            magic = struct.unpack('>i', fp.read(4))[0]
            if magic != 2051:
                raise Exception('Magic number does not match!')

            n_images, height, width = struct.unpack('>iii', fp.read(4 * 3))

            n_pixels = n_images * height * width
            pixels = struct.unpack('>' + 'B' * n_pixels, fp.read(n_pixels))
            pixels = np.asarray(pixels, dtype='uint8').reshape((n_images, 1, height, width))

            # 画像サイズを2べきにしておく
            pixels = np.pad(pixels, [(0, 0), (0, 0), (2, 2), (2, 2)], mode='constant', constant_values=0)
            pixels = (pixels / 255.0).astype('float32')

        return pixels

    def _load_labels(self, filename):
        with open(filename, 'rb') as fp:
            magic = struct.unpack('>i', fp.read(4))[0]
            if magic != 2049:
                raise Exception('Magic number does not match!')

            n_labels = struct.unpack('>i', fp.read(4))[0]
            labels = struct.unpack('>' + 'B' * n_labels, fp.read(n_labels))

            # 誤差関数用にlongで表しておく
            labels = np.asarray(labels, dtype='int64')

        return labels
```

この例では `__getitem__` の戻り値が `dict` 型になっている。この場合、取り出し側では `dict` の各要素ごとにバッチにまとめた値が取り出せる。例えば、

```python
for data in data_loader:
    images = data['images']  # 画像が32個分
    labels = data['labels']  # ラベルが32個分
```

のような形でデータを取り出せる。

## モジュール・クラスの用意

PyTorchでは深層学習のネットワークを `torch.nn.Module` というクラスのサブクラスとして定義する。このサブクラスに `forward(self, ...)` というメソッドを実装すると、データをネットワークに通す操作と、誤差逆伝搬によりパラメータの更新幅を求める計算をほぼ自動化できる。

もっとも単純なネットワークとしてYan LeCun氏 (2019年のチューリング賞受賞者だ！) がニューラルネットの黎明期に提案したLeNet5の構造を実装してみる。なお、現代的な深層学習のフレームワークに合わせるため一部実装を変更してある。LeNet5についての説明は[こちら](https://pytorch.org/tutorials/beginner/blitz/neural_networks_tutorial.html){: target="_blank" }のウェブページを参考にしてほしい。

```python
import torch
import torch.nn as nn
import torch.nn.functional as F

class LeNet5(nn.Module):
    def __init__(self):
        super(LeNet5, self).__init__()

        self.net = nn.Sequential(
            nn.Conv2d(1, 6, kernel_size=5, stride=1, padding=0),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Sigmoid(),
            nn.Conv2d(6, 16, kernel_size=5, stride=1, padding=0),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Sigmoid()
        )

        self.fc = nn.Sequential(
            nn.Linear(5 * 5 * 16, 120),
            nn.Sigmoid(),
            nn.Linear(120, 84),
            nn.Sigmoid(),
            nn.Linear(84, 10)
        )

    def forward(self, x):
        n_batches, _, _, _ = x.size()
        x = self.net(x)
        x = x.view(n_batches, -1)
        y = self.fc(x)
        return torch.log_softmax(y, dim=1)
```

上記の例は、なるべくオリジナルのLeNet5に忠実に、活性化関数にsigmoid関数を用いており、バッチ正規化などの処理も入れていない。また、ロスの計算を安定させるため、softmax関数ではなく対数softmax関数を用いている。

## Optimizerの用意

深層学習においては、ネットワークを構成するパラメータの更新を最急降下法などの勾配法により行う。このようなパラメータの最適化計算を行うクラスをOptimizerと呼ぶ。識別タスクの場合には確率的最急降下法 (SGD, stochastic gradient descent) や、これを勾配方向が急激に変わらないように調整したMomentum SGDなどがよく使われている印象がある。一方、後述する画像生成のタスクにおいては、学習履歴を参照し、あまりパラメータが更新されていない部分を大きく更新するようなAdamなどのOptimizerが使われることが多い。これらのOptimizerが結果に少なからず影響を与えていることは間違いないが、一般にはOptimizerを調整することで結果を良くしようと試みることは、あまり賢い選択とは言えないだろう。

Optimizerには様々ないくつかのパラメータがあるが、基本はOptimizerに更新させるパラメータのリストと、ステップ幅 (learning rate, lrと略されることが多い) の2つを与えれば良い。

```python
net = LeNet5()
optim = torch.optim.SGD(net.parameters(), lr=1.0e-3)
```

## 学習ループ

ニューラルネットワークの学習は、上記のOptimizerによってネットワークのパラメータを何度も更新することによって行われる。一般に、1度パラメータを更新する処理を**ステップ**と呼び、データセット内の全てのデータを1度ずつ学習に用いることを**エポック**と呼ぶ。

多くの場合、この2つのループをネストさせて二重ループで学習ループを実装する。

```python
for epoch in range(n_epochs):
    # エポックに関するループ
    for data in data_loader:
        # ミニバッチを用いたパラメータ更新
        ...
```

1ステップの処理は、

* 勾配の初期化
* ネットワークへのデータ転送
* ロスの評価
* 誤差逆伝播
* パラメータの更新

の5つの処理からなる。これらをまとめたコードは以下の通りだ。

```python
# データの取り出し
images = data['images']
labels = data['labels']

# トレーニングモードに変更
net.train()

# 勾配の初期化
net.zero_grad()

# ネットワークへのデータ転送
y = net(images)

# 誤差の評価
loss = criterion(y, labels)

# 誤差逆伝搬
loss.backward()

# パラメータの更新
optim.step()
```

より複雑なネットワークになればネットワークへのデータ転送や誤差の評価は複雑にはなるが、基本的な流れはほとんど変わらない。なお上記のコードに現れる `criterion` は誤差を評価する損失関数で、ネットワークの最終出力に`log_softmax`用いた場合には `nn.NLLLoss` (非負対数尤度, Non-Negative Likelihood)を用いる。 (効率は落ちるが通常の`softmax`を使った場合には `nn.CrossEntropyLoss` を使う)。

```python
criterion = nn.NLLLoss()
```

## 学習の結果とネットワークの改良

さて、上記のMNISTの例だが、おそらく現状の設定のままでは、なかなかロスが下がらず、精度も上がらないのではないかと思う。これにはいくつかの原因があるが、代表的なものは勾配消失と誤差関数の振動である。

1つめの問題は活性化関数にsigmoidを使っていることが原因で、sigmoid関数は入力の値が0から遠くなると勾配が急激に0に近づくため、sigmoidが何段も重なると、誤差逆伝搬の途中で伝搬される勾配が0に限りなく小さくなってしまい、学習が進みづらくなってしまう。現在は、ネットワーク途中の活性化関数にsigmoidを使うことはなく、一般にはReLU (rectified linear unit)という閾値関数を使う。また、各畳み込み層で学習しているバイアスの成分が入力データに対して大きく変化しなくて済むように、正規化層を畳み込みと活性化の間に入れると、さらに学習の効率が向上する。多くの場合は、バッチ内のデータの平均と分散を使って正規化するバッチ正規化 (batch normalization)を使うことが多い。

次に誤差関数の振動だが、通常のSGDを使うと、ミニバッチ内のデータが十分にデータ全体を近似していない場合に、あまり良い勾配が得られず、そのために誤差関数が良い方向に収束していかないことがある。これを防ぐために、慣性 (momentum)を使う方法や、各パラメータに対する勾配の大きさを元に更新に使う勾配量を調整する方法が多数提案されている。代表的なものはSGD + momentum (PyTorchの場合はSGDの引数にmomentumを入れる), RMSprop, AdaGrad, AdaDelta, Adamなどだ。

上記の改良を加えて上手く学習ができれば、1エポックも回せば軽く90%以上の精度が得られるようになるだろう。これ以外にも、様々な学習のテクニックがあるが、それらについては、ネット上にも多くの記事や実装があるので、各自調べてみてほしい。
