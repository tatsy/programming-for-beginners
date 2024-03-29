---
layout: post
title: 数独を解く
---

ご存知の通り、数独というのは9x9のマスの中に1-9の数字を一定のルールのもとで入れていくパズルである。このルールとはすなわち、

* 同じ行に同じ数字が存在しない
* 同じ列に同じ数字が存在しない
* 3x3の9領域に同じ数字が存在しない

の3つである。数独の解き方にはいろいろなやり方があるが、9x9の問題であれば、バックトラック法と呼ばれる最も簡単なアルゴリズムでも十分高速に解くことができる(一般的なパズル本の最難問が1秒以下で解ける)。

## 問題の配列化

今回は数独の問題が以下の形式のテキストとして与えられることとする。Pythonは複数行のテキストを以下のように定義することができる。

```python
problem = '''
-35-9--48
--9--8--3
-4-6-5--1
----74---
-2-----6-
---15----
8--9-2-7-
9--5--2--
61--4-53-
'''
```

これを扱いやすくするためにNumPyの配列に直してみる。

まずはテキストを行ごとの配列に変換する。行は `\n' という文字で区切られているので、これで文字列を分割する。

```python
problem = problem.split('\n')
```

ただし、このやり方だと空行が入る可能性があるので、以下のリスト内包表記を用いて取り除く。

```python
problem = [line for line in problem if line != '']
```

すると`problem`の長さは9となるはずだ。各行を文字ごとに分割するには、同じようにリスト内包表記を用いると簡単だ。以下は二重ループを用いたリスト内包表記の例だ。

```python
problem = [c for line in problem for c in line]
```

すると今度は`problem`が9x9の二次元配列になる。あとは各文字を数字に置き換える。まだ埋まっていないセルを表す'-'を0に置き換えたいので、以下のようにハッシュ(Pythonの用語では`dict`)を使うと良い。

```python
numbers = {'-': 0}
numbers.update({str(i): i for i in range(1, 10)})
print(numbers)
```

この出力は以下のようになる。

```python
{'-': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9}
```

あとはこれを使って文字を数字に置き換える。再び二重ループのリスト内包表記で、

```python
problem = [numbers[c] for line in problem for c in line]
```

とすると、先ほどまでの文字が数字に置き換わるはずだ。なお、ここまで繰り返しリスト内包表記も用いたが、これらをまとめて1行で書くこともできる。

```python
problem = [numbers[c] for line in problem.split('\n') if line != '' for c in line]
```

## 使用済みの数字を調べる

問題が数字の配列として表せたところで、使用済みの数字を調べる方法を見ていく。今 `(i, j)` にある数字に何が使えるのかを調べたいとする。

当然ながら`i`行目にある数字は `problem[i, :]` という配列で、`j`列目にある数字は`problem[:, j]`という配列で表せる。また3x3のブロックについては、ブロックの右上の座標が`(3 * (i // 3), 3 * (j // 3))`と表せることに注目する (Python 3.xでは`//`は整数の割り算を表す)。これを用いれば、以下のようにしてブロック内の数字が得られる。

```python
k = 3 * (i // 3)
l = 3 * (j // 3)
blk_nums = problem[k:k+3, l:l+3]
```

これらの配列の中では0が重複して現れるので、1つの数字が1回ずつしか現れないように`set`を用いる。`set`は引数に一次元配列しか取らないので、NumPyのtolist関数を使って、上記のNumPyの配列をPythonの1次元配列に置き換えておく。なお`blk_nums`は二次元配列であることに注意すること。

```python
used_nums = set(row_nums.tolist() + col_nums.tolist() + blk_nums.flatten().tolist())
```

まだ使える数字は1-9の数字の中で、この`set`にあらわれていない数字なので、集合演算の関数 `difference` を使って0-9の数字の集合と、使用済みの数字の集合の差分を取る。

```python
nums = set([i for i in range(10)]).difference(used_nums)
```

例として、最初に示した問題の`(0, 0)`成分に対して上記のコードを実行すると `{1, 2, 7}` という集合が得られるはずである。

## バックトラック法

バックトラック法について考える前に、数独をコンピュータに解かせる最も単純な方法を考えてみよう。上記の使用済みの数字を調べる方法を少し変更すれば、各行、各列、各ブロックで数字が正しく1度だけ使われているかどうかを調べることができそうだ。とすれば、全ての空きマスに適当に数字を入れて、その結果が正しいかどうかを順に試せば良さそうだ。

ただ、当然ながらこれでは計算に時間がかかりすぎる。各マスに入れられる数字の候補は9通りでマスが81マスあるので、最悪の場合には$9^{81}$個の候補について調べる必要がある。

一方で、実際にはいくつかのマスにはすでに数字が入っており、上記のように何も考えずに全てのパターンを試すのは非常に効率が悪いことが分かるだろう。そこで、可能な候補についてだけ、しらみ潰しに調べることを考える。これを実現するのがバックトラック法だ。

これでも効率がとても良いとは言えないが、実際には最初の方の数字がある程度決まると、後の方の数字は急速に候補が少なくなるため、実用的にはそれなりに高速に動作する。少なくとも9x9の問題であれば、十分に速い(16x16の問題になるとそうはいかない！)

### 深さ優先探索

バックトラック法は深さ優先探索の一種で、数独の例で言えば、候補となる数字を仮に入れる操作をできる限り繰り返し、もし最後のマスまで数字が入れば成功、途中で数字が入れられなくなったら失敗として違う候補を試す。

深さ優先探索の実装方法は再帰関数を使う方法とFILO (first in last out, 最初に入れたものが最後に取り出されるということ) データ構造 (一般にはスタックと呼ぶ) を用いる方法の2つがある。今回はより単純な再帰関数を使う方法で実装をすすめる。

再帰関数を使う場合、ある関数の中では、

1. 左上から0が入っているマスを探す
    * もし0が入っているマスがなければ成功
1. 0が入っているマスを見つけたら、
    * 上記の方法で、候補の数字を調べる
    * 候補の数字を仮に入れて、関数を再帰呼び出しする
    * 試行が失敗したら、仮に入れた数字を0に戻し、次の候補を入れて、再度再帰呼び出し (以後繰り返し)

という処理を行う。

以下にスケルトン・プログラムを用意したので、これを参考にプログラムを作って見てほしい。

```python
def sudoku_solve(problem):
    rows, cols = problem.shape
    all_nums = set([i for i in range(0, 10)])
    for i in range(rows):
        for j in range(cols):
            if problem[i, j] == 0:
                # 候補の数字を調べる (以下のnumsに代入する)
                nums = None

                # 候補の数字を仮入れして再帰呼び出し
                success = False
                for n in nums:
                    # 数字を代入
                    problem[i, j] = n
                    if sudoku_solve(problem):
                        success = True
                        break

                    # 失敗したら元に戻す
                    problem[i, j] = 0

                if not success:
                    return False
        
    return True
```

### 処理の効率化

上記の方法でも、最初に示した数独の問題であれば数十ミリ秒で解くことができる。だが、次の問題はどうだろう？

```python
# https://www.j-cast.com/premium/2018/10/19341452.html?p=all
problem = '''
---------
3--2-9--7
94--1--82
-5-6-2-3-
--63-74--
---------
---------
-82-3-59-
1--9-4--6
'''
```

この問題だと、おそらく1秒以上は時間がかかるはずだ。実は、上記のコードは次の0のマスを探すまでの処理の効率が非常に悪い。0のマスは左上から右下に向かって探しているのだから、今新しく`(i, j)`というマスに数字を入れたら `(i, j+1)` 以降の数字だけを調べれば良いのである。

答えは示さないので、上記のプログラムを更新したマスより後のものからだけ0を探すように更新しよう。すると、高難易度の問題でも1秒以内には解けるようになる(コンピュータの速さにもよる)。

その他にも、候補の数が少ないものから埋めていくなどの効率化が考えられる。余裕があれば試してみよう。確認用に難易度の異なる4問を示しておく。

```python
# レベル1
problem = '''
-35-9--48
--9--8--3
-4-6-5--1
----74---
-2-----6-
---15----
8--9-2-7-
9--5--2--
61--4-53-
'''
```

```python
# レベル2
# https://si-coding.net/sudoku10.html
problem = '''
------1-4
4---78---
-3------2
5--4-7-3-
2------5-
9---264--
-27------
--56----7
-6--13---
'''
```

```python
# レベル3
# https://www.j-cast.com/premium/2018/10/19341452.html?p=all
problem = '''
---------
3--2-9--7
94--1--82
-5-6-2-3-
--63-74--
---------
---------
-82-3-59-
1--9-4--6
'''
```

```python
# 世界一難しい？
# https://www.conceptispuzzles.com/ja/index.aspx?uri=info/article/424
problem = '''
8--------
--36-----
-7--9-2--
-5---7---
----457--
---1---3-
--1----68
--85---1-
-9----4--
'''
```
