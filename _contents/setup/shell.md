---
layout: post
title: シェルの設定
---

シェルの設定は任意であるが、今後シェルを使う機会が増えていくであろうことを考えると、最低限の設定をして使いやすくしておくことが望ましい (設定しないままだと使いづらく、使うのをすぐやめてしまうことが予想されるため)。

## Windows

Windowsのシェルは2021年現在、過渡期にあって、いくつかのシェル環境が混在している。この中では、オープンソース版のPowerShellかWindows Subsystem 2 (WSL2)のどちらかを使うのが良いと思われる。

* コマンドプロンプト (`cmd.exe`)
  * Windowsに昔からあるシェル。最初の選択肢にはならないと思うが、Windowsのシステム設定変更などでたまに使う時がある。
* PowerShell (`pwsh.exe`)
  * コマンドプロンプトの改善のために導入されたシェル。デフォルトでも入っているが[オープンソース版](https://docs.microsoft.com/ja-jp/powershell/){: target="_blank" }もあり、こちらの方が機能が多い。
* Windows Subsystem for Linux (WSL)
  * Windows上にUbuntuなどのLinux仮想環境を作って、その上で動かすシェル。Linuxのシェルと同じように使えるが、WindowsとLinux仮想環境でファイルシステムが分かれているので、その分ディスクを圧迫するのが気にはなる。

以下ではPowerShellの設定方法について述べるが、WSLを使う場合は、その下に示すMac/Linux用の設定に従うと良い。

### PowerShellのインストール

GitHubのリポジトリからWindows用のインストーラ(拡張子が`.msi`になっているもの)で、特にこだわりがなければ最新のものをダウンロード、インストールする。

<https://github.com/PowerShell/PowerShell/releases>{: target="_blank" }

インストールの途中で、右クリックのメニューに「PowerShellで開く」という項目を追加するかどうかを尋ねられるので、ここのチェックは入れておくことを推奨する。

{% include lightbox.html src="/public/images/setup/powershell_setup.jpg" %}


### Chocolateyのインストール

PowerShellでは[Chocolatey](https://chocolatey.org/){: target="_blank" }というパッケージマネージャが使用可能で、これを使うと、ややブラックボックスにはなるものの、ライブラリやプログラムのインストールが自動化できる。今回はPowerShellの設定のためだけにChocolateyを使うが、個人的にはプログラムのライブラリなどは、個別に手動でインストールした方が良い気がする。

Chocolateyのインストールについては、以下のページの手順に従う。ところどころ、管理者としてPowerShellを起動していないと失敗することがあるので注意すること。

<https://chocolatey.org/install>{: target="_blank" }

### oh-my-poshのインストール

オープンソースのPowerShellには[oh-my-posh](https://github.com/JanDeDobbeleer/oh-my-posh)というプラグインがあり、これを使うことでシェルが格段に使いやすくなる。

* <https://github.com/JanDeDobbeleer/oh-my-posh>

インストールにあたっては、まずPowerShellのスクリプト実行設定を確認する。

```powershell
Get-ExecutionPolicy
```

この結果は、特に変更していなければ`RemoteSigned`となっているはずなので、以下のようにタイプして、設定を`Bypass`に設定し直す。なおChocolateyをインストールした場合には、この設定はすでに済んでいると思われる。

```powershell
Set-ExecutionPolicy Bypass -Scope Process
```

設定後、`oh-my-posh`と必要なGitのモジュールをインストールする。

```powershell
Install-Module posh-git -Scope CurrentUser
Install-Module oh-my-posh -Scope CurrentUser
```

これで必要なモジュールのインストールは完了なので、次回立ち上げ時以降、自動的にoh-my-poshが働くように設定ファイルを編集する。

```powershell
notepad $profile
```

もし設定ファイルが存在しなければ、作成するかどうかを尋ねられるので、作成する。するとNotepad(メモ帳)でファイルが開くので以下の内容を追記して保存する。

```powershell
Import-Module posh-git
Import-Module oh-my-posh
Set-Theme Paradox
```

なお`Set-Theme`の部分は[oh-my-posh](https://github.com/JanDeDobbeleer/oh-my-posh#themes){: target="_blank" }のGitリポジトリにあるものを見て、好きなものを使用すると良い。

### Nerdフォントのインストール

[Nerdフォント](https://www.nerdfonts.com/){: target="_blank" }と呼ばれるemojiがサポートされたフォントを使うと、シェルの見た目がきれいになり使う意欲が湧く(個人差あり)。ただし、PowerShellをそのまま使う場合にはフォントの設定がうまくいかない可能性があるので、ここではフォントをインストールするにとどめて、設定については、以下のWindows Terminalというシェルで行う。

<https://www.nerdfonts.com/>{: target="_blank" }

なお、日本語のフォルダ名なども合わせてフォントを置き換えたい場合には**Cica**と呼ばれるフォントを使うのが良い。

<https://github.com/miiton/Cica>{: target="_blank" }

こちらもGitHubの[リリースページ](https://github.com/miiton/Cica/releases){: target="_blank" }からフォントがダウンロードできる。

### Windows Terminal

Windows Terminalは今までのPowerShellなどで手が届いていなかった細かなオプションの設定やタブの利用などが可能となったモダンなシェル環境である。Windows Terminal自体はシェルではなく、コマンドプロンプトやPowerShellなどのシェルを間接的に呼び出して実行する。

Windows Terminalは表示に使うフォントを設定ファイルで管理できるので、適当なNerdフォントを設定する。設定についてはWindows Terminalを立ち上げて、画面上部、タブの右隣にある「+」ボタンをクリックし、そこから「設定」を選ぶ。

するとエディタで設定ファイルが開くので、設定ファイル内の`Windows.Terminal.PowershellCore`という場所を探して、以下のように設定する。

```json
{
  "guid": {...},
  "hidden": false,
  "name": "PowerShell",
  "source": "Windows.Terminal.PowershellCore",
  "colorScheme": "One Half Dark (好きなカラーテーマを設定)", 
  "fontFace": "Delugia Nerd Font (好きなNerdフォントを設定)",
  "fontSize": 12, 
}
```

すると、以下の画像のように画面が設定されて、Anacondaの開発環境名 (以下の画像では`base`)やGitリポジトリのブランチ名 (以下の画像では`master`)などが表示されるようになる。

---

## Mac/Linux

MacやLinuxにはデフォルトで「ターミナル」というソフトウェアが用意されているが、おすすめはMacなら[iTerm](https://iterm2.com/){: target="_blank"}、Linuxなら[Byobu](https://www.byobu.org/)というソフトウェアを使うと、タブの数を増やしたり、画面を分割したりできる。

これらのターミナル環境の中では、MacとLinuxともに`bash`というシェルを使用するように設定されている(ターミナルはCUI操作を行うソフト、シェルはコマンドを受け取ってOS上で命令を実行するソフトウェアのこと)。例えばMacでターミナルを立ち上げると、`bash`のソフトウェアが走って、ターミナル上に入力を受け付ける画面を表示する。

筆者はあまり`bash`は使ったことがなく、より拡張性などが優れている(と思われる)`zsh`をよく使っている。というわけで`zsh`を使う方法を紹介する。最低限の設定であれば非常に簡単で、まずは以下の手順で`zsh`と`oh-my-zsh`をインストールすると良い。

### Zshのインストール

Macなら[Homebrew](https://brew.sh/index_ja){: target="_blank" }, Linuxなら`apt-get`(Debian系)とか`yum` (CentOS系)で`zsh`を入れる。ちなみに、最近は`Fish`というシェルも人気らしく、興味があれば下の補足の項を見て欲しい。

### Oh-my-zshのインストール

`zsh`には多数のプラグインが用意されていて、細かな設定が可能ではあるのだが、最初から細かい設定をするのは少々骨が折れる。まだ設定に慣れないうちは、最も設定が簡単で、使い勝手もそれなりに良い[oh-my-zsh](https://ohmyz.sh/){: target="_blank" }を使うのが良いと思う。

インストール方法は非常に簡単で、上記のサイトをおとづれて、`zsh`を立ちあげた状態でインストール用のスクリプトを実行する。

```shell
sh -c "$(curl -fsSL https://raw.github.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
```

もしかするとCurl (Web上のファイルをCLI経由でインストールするソフトウェア)が見つからないと言われるかもしれないので、その場合は`zsh`と同様にHomebrew等でインストールする。

### 補足: zplug

ちなみに著者は[zplug](https://github.com/zplug/zplug){: target="_blank" }という`zsh`のプラグインマネージャを使っているので、興味がある人は調べてみると良い。

参考までの`zplug`の設定ファイルを掲載しておく。

```shell
# zsh/zplug settings
source "${HOME}/.zplug/init.zsh"

zplug "sorin-ionescu/prezto"

zplug "modules/prompt", from:prezto
zplug "modules/git", from:prezto
zplug "modules/autosuggestions", from:prezto
zplug "modules/completion", from:prezto
zplug "modules/command-not-found", from:prezto
zplug "modules/directory", from:prezto
zplug "modules/environment", from:prezto
zplug "modules/fasd", from:prezto
zplug "modules/history", from:prezto

zplug "mafredri/zsh-async"
zplug "wting/autojump"
zplug "rimraf/k"
zplug "b4b4r07/enhancd", use:init.sh
zplug "junegunn/fzf-bin", as:command, from:gh-r, rename-to:"fzf", frozen:1
zplug "b4b4r07/zsh-gomi", as:command, use:bin/gomi, on:junegunn/fzf-bin
zplug "mollifier/cd-gitroot"

zplug "romkatv/powerlevel10k", as:theme, depth:1

zplug "zsh-users/zsh-syntax-highlighting", defer:2
zplug "modules/history-substring-search", from:prezto, defer:2

# Install plugins if there are plugins that have not been installed
if ! zplug check --verbose; then
    printf "Install? [y/N]: "
    if read -q; then
        echo; zplug install
    fi
fi

# Then, source plugins and add commands to $PATH
zplug load --verbose

# Aliases
if [ "$(uname)" = "Darwin" ]; then
    alias ls='ls -G'
    alias la='ls -la -G'
    alias ll='ls -la -G'
else
    alias ls='ls --color=auto'
    alias la='ls -la --color=auto'
    alias ll='ls -la --color=auto'
fi
```

### 補足: fish

最近、注目を集めている新しめのシェルにfishというものがある (2021年1月現在)。

<https://fishshell.com/>{: target="_blank" }

fishはインストールした時点である程度の補完機能がついていて、そのままでもそれなりに使えるというのが特徴で、zsh + oh-my-zshの代わりにこちらを使ってみるのも良いかもしれない。

fishのインストールは簡単でMacであればHomebrew, Ubuntu系であればPPAを追加することでインストール可能(Ubuntu 19.04以降はPPA不要)

```shell
# Mac
brew install fish

# Ubuntu 19.04より前
sudo apt-add-repository ppa:fish-shell/release-3
sudo apt update -y
sudo apt install fish -y

# Ubuntu 19.04以降
sudo apt install fish -y
```

また、fishにもzshと同様に[oh-my-fish](https://github.com/oh-my-fish/oh-my-fish){: target="_blank" }というツールが用意されており、こちらを追加でインストールすることで、テーマなどの設定が簡単になる。

```shell
# fish shellを起動した状態で
curl -L https://get.oh-my.fish | fish
```
