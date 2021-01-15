---
layout: post
title: シェルの設定
---

## Windows

Windowsのシェルは2021年現在、過渡期にあって、いくつかのシェル環境が混在している。著者のお勧めは今のところオープンソースのPowerShell。

* コマンドプロンプト (`cmd.exe`)
  * Windowsに昔からあるシェル
* PowerShell (`pwsh.exe`)
  * コマンドプロンプトの改善のために導入されたシェル。デフォルトでも入っているが[オープンソース版](https://docs.microsoft.com/ja-jp/powershell/){: target="_blank" }を使うのが良い.
* Linux Subsystem
  * Windows上にUbuntuなどのLinux仮想環境を作って、その上で動かすシェル。Linuxのシェルと同じように使えるが、WindowsとLinux仮想環境でファイルシステムが共有されていないなどの問題があり、やや使いづらい。

### PowerShellの設定

オープンソースのPowerShellにはoh-my-poshというプラグインがあり、これを使うことでシェルが格段に使いやすくなる。

* <https://github.com/JanDeDobbeleer/oh-my-posh>

また[Nerdフォント](https://www.nerdfonts.com/){: target="_blank" }と呼ばれるemojiがサポートされたフォントを使うことで、見た目もきれいになり楽しい。ただし、PowerShellをそのまま使う場合にはフォントの設定がうまくいかないので、Windows TerminalというWindowsの歴史の中で最も使いやすいシェルを導入する。

### Windows Terminal

Windows Terminalは今までのPowerShellなどで手が届いていなかった細かなオプションの設定やタブの利用などが可能となったモダンなシェル環境である。Windows Terminal自体はシェルではなく、コマンドプロンプトやPowerShellなどのシェルを間接的に呼び出して実行する。

Windows Terminalは表示に使うフォントを設定ファイルで管理できるので、適当なNerdフォントを設定する。

## Mac/Linux

MacとLinuxの場合はデフォルトのシェルが`bash`というシェルになっている。例えばMacでターミナルを立ち上げると、`bash`のソフトウェアが走って、ターミナル上に入力を受け付ける画面を表示する。

筆者はあまり`bash`は使ったことがないのだけど、拡張性などを考えて`zsh`をよく使っている。というわけで`zsh`を使う方法を紹介する。

MacだったらHomebrew, Linuxなら`apt-get`(Debian系)とか`yum` (CentOS系)で`zsh`を入れる。`zsh`には多数のプラグインが用意されていて、細かな設定が可能なのだが、最初から細かい設定をするのは少々骨が折れるので大人しく[oh-my-zsh](https://ohmyz.sh/){: target="_blank" }を使う。

ちなみに著者は[zplug](https://github.com/zplug/zplug){: target="_blank" }という`zsh`のプラグインマネージャを使っているので、興味がある人は調べてみると良い。
