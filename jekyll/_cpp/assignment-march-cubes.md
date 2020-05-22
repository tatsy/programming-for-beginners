---
layout: post
title: "マーチング・キューブ法とその拡張"
---

<table class="images">
<tr>
  <td style="text-align: center; width: 50%;">{{ '**ボリューム断面**' | markdownify }}</td>
  <td style="text-align: center; width: 50%;">{{ '**復元メッシュ**' | markdownify }}</td>
</tr>
<tr>
  <td>{% include lightbox.html src="/public/images/march_cubes/shell_section_140.jpg" style="width: 100%;" %}</td>
  <td>{% include lightbox.html src="/public/images/march_cubes/shell_mesh.jpg" style="width: 100%;" %}</td>
</tr>
</table>

これまでに解説した内容を元にテンプレートプログラムの内容を改変して、マーチング・キューブ法を実装しよう。変更するべき内容は以下の通り。

* `volume.h`内にあるボリュームデータを読み取る関数`Volume::load`を完成させる。
* `mcubes.cpp`内にある大津の二値化により等値面を張るための閾値を決定する`getThresholdOtsu`関数を完成させる。
* `mcubes.cpp`内にある`marchCubes`関数にあるfor文の内部に三角形取得するソースコードを追加する。

なお、これらの箇所には、最初の時点では `NOT_IMPL_ERROR()` と書かれており、この状態で実行すると、実装されていない箇所がエラーとともに表示されるようになっている。