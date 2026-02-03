# 🐣 S1 | Prog: Workshop

## Exercices réalisés

### ✔ Ne garder que le vert

![Green Only](output/green_only.png)

### ✔ Echanger les canaux

![Channels Swap](output/channels_swap.png)

### ✔ Noir & Blanc

![Black & White](output/black_and_white.png)

### ✔ Négatif

![Negative](output/negative.png)

### ✔ Dégradé

![Gradient](output/gradient.png)

### ✔ Mirror

![Mirror](output/mirror.png)

<div style="background-color: #fff3cd; border-left: 4px solid #ffc107; padding: 10px; margin: 10px 0; border-radius: 5px; font-family: Arial, sans-serif; color: #856404;">
    💡 Il est possible de changer l'orentation du miroir en modifiant le paramètre de la fonction <strong>mirror</strong>, par exemple <strong>mirror(img, Mirror::Vertical)</strong> pour un miroir vertical <i>(par défaut, le miroir est horizontal)</i>.
</div>

### ✔ Image bruitée

![Noisy](output/noisy.png)

### ✔ Rotation de 90°

![Rotated 90°](output/rotate90.png)

### ✔ RGB Split

![Split RGB](output/splitRGB.png)

### ✔ Luminosité

![Darker](output/darker.png)
![Brighter](output/brighter.png)

<div style="background-color: #fff3cd; border-left: 4px solid #ffc107; padding: 10px; margin: 10px 0; border-radius: 5px; font-family: Arial, sans-serif; color: #856404;">
    💡 Il est possible de modifier la luminosité en changeant la valeur du paramètre <strong>mode</strong>, par exemple <strong>brightness(img, Brightness::Darker)</strong> pour une image plus sombre <i>(par défaut, l'image est éclaircie)</i>.
</div>

### ✔ Disque

![Disk](output/disk.png)

<div style="background-color: #fff3cd; border-left: 4px solid #ffc107; padding: 10px; margin: 10px 0; border-radius: 5px; font-family: Arial, sans-serif; color: #856404;">
    💡 Il est possible de modifier le rayon et la position du disque en changeant les paramètres de la fonction <strong>disk</strong>, par exemple <strong>disk(img, 150.f, 250, 250)</strong> pour un disque de rayon 150 centré en (250, 250) <i>(par défaut, le rayon est de 100 et le disque est centré)</i>.
</div>

### ✔ Cercle

![Circle](output/circle.png)

<div style="background-color: #fff3cd; border-left: 4px solid #ffc107; padding: 10px; margin: 10px 0; border-radius: 5px; font-family: Arial, sans-serif; color: #856404;">
    💡 Il est possible de modifier le rayon, l'épaisseur et la position du cercle en changeant les paramètres de la fonction <strong>circle</strong>, par exemple <strong>circle(img, 150.f, 5.f, 250, 250)</strong> pour un cercle de rayon 150, d'épaisseur 5 centré en (250, 250) <i>(par défaut, le rayon est de 100, l'épaisseur est de 3 et le cercle est centré)</i>.
</div>

### ✔ Animation

![Animation](output/animation.gif)

<div style="background-color: #fff3cd; border-left: 4px solid #ffc107; padding: 10px; margin: 10px 0; border-radius: 5px; font-family: Arial, sans-serif; color: #856404;">
    💡 Il est possible de modifier la coordonnée Y, la durée et le nombre d'images par seconde de l'animation en changeant les paramètres de la fonction <strong>animation</strong>, par exemple <strong>animation(5, 5, 30)</strong> pour une animation de 5 secondes à 30 images par seconde avec un cercle à Y 5 <i>(par défaut, la durée est de 3 secondes à 25 images par seconde avec un cercle centré verticalement)</i>.
</div>

### ✔ Rosace

![Rosette](output/rosette.png)

<div style="background-color: #fff3cd; border-left: 4px solid #ffc107; padding: 10px; margin: 10px 0; border-radius: 5px; font-family: Arial, sans-serif; color: #856404;">
    💡 Il est possible de modifier le nombre de branches de la rosace, l'épaisseur des cercles et leur rayon en changeant les paramètres de la fonction <strong>rosette</strong>, par exemple <strong>rosette(img, 12, 1, 150)</strong> pour une rosace à 12 branches avec des cercles d'épaisseur 1 et de rayon 150 <i>(par défaut, la rosace a 6 branches avec des cercles d'épaisseur 0.5 et de rayon 100)</i>.
</div>

### ✔ Mosaïque

![Mosaic](output/mosaic.png)

<div style="background-color: #fff3cd; border-left: 4px solid #ffc107; padding: 10px; margin: 10px 0; border-radius: 5px; font-family: Arial, sans-serif; color: #856404;">
    💡 Il est possible de modifier le nombre de tuiles (horizontalement et verticalement) en changeant le paramètre de la fonction <strong>mosaic</strong>, par exemple <strong>mosaic(img, 10)</strong> pour une mosaïque avec 10 tuiles sur chaque ligne et sur chaque colonne <i>(par défaut, le nombre de 5)</i>.
</div>

### ✔ Mosaïque miroir

![Mosaic Mirror](output/mosaic_mirror.png)

### ✔ Glitch

![Glitch](output/glitch.png)

### ✔ Tri de pixels

![Pixel Sort](output/pixel_sort.png)
