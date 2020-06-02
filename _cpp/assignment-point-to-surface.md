---
layout: post
title: "課題: 点群データからのメッシュ復元"
---

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**入力点群**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**復元メッシュ**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/surf_recon/buddha_point.jpg" style="width: 100%;" %}</td>
  <td>{% include lightbox.html src="/public/images/surf_recon/buddha_recon.jpg" style="width: 100%;" %}</td>
</tr>
</table>

## 課題1: 点群データからのメッシュ復元

これまでに解説した内容を元にテンプレート・プログラムの内容を改変して、点群データからメッシュを復元してみよう。

* [テンプレート・プログラム](#)

変更するべき内容はいずれも`surface_recon.cpp`のファイル内にあり、それぞれは以下の通り。

* 入力の頂点位置を$[-1, 1]^3$の領域に入るように正規化し、法線方向に沿ってオフセットした点とともに配列に格納する (`xyz`が頂点位置、`vals`が陰関数の参照値になる)
* 疎行列`AA`とベクトル`bb`に値を入れて、RBF補間のための線形問題を定義する。
* マーチング・キューブ法を実行するためのボリュームデータを作成して、実際の曲面を抽出する。

なお、前回同様、これらの箇所には最初の時点では`NOT_IMPL_ERROR()`と書かれており、この状態で実行すると、実装されていない箇所がエラーとともに表示されるようになっている。

## 課題2: RBF関数の変更とその効果について調査

今回の実装ではコンパクトサポートのC1級連続Wendland関数を用いた。これをより高次のWendland関数の他、ガウス関数や薄膜スプライン関数などに変更すると、メッシュの復元結果はどのように変化するかを調べよう。

また、今回の実装ではRBFに、さらに1次の多項式項をつけて曲面を補間したが、これを2次の多項式へと拡張し、$x^2, y^2, z^2, xy, yz, zx$の項を追加すると、メッシュの復元結果はどう変化するかを調べよう。

さらなる発展として、コンパクトサポートのRBFを用いながらも、階層的な処理によって穴のないメッシュとより少ない頂点での補間を実現している以下の論文を実装してみよう。

Ohtake et al. 2003, "A Multi-scale Approach to 3D Scattered Data Interpolation with Compactly Supported Basis Functions"
