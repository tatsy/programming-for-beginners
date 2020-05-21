---
layout: page
title: "C++プログラミング"
permalink: "/cpp/"
---

## マーチング・キューブ法によるメッシュ生成

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

* [ボリュームデータの読み込み]({% link _cpp/read-volume.md %})
* [大津の二値化による境界値の決定]({% link _cpp/otsu-binarize.md %})
* [マーチング・キューブ法]({% link _cpp/march-cubes.md %})

## 点群データからのメッシュ復元

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


* [アルゴリズムの概要]({% link _cpp/point-to-surface.md %})
* [k-d木による最近傍探索]({% link _cpp/kdtree.md %})
* [RBFによる関数の補間]({% link _cpp/radial-basis-function.md %})
