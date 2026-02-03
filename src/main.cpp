#include <sil/sil.hpp>
#include <glm/vec3.hpp>
#include <random.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numbers>

/**
 * Conserve uniquement la composante verte de chaque pixel de l'image.
 * Les composantes rouge et bleue sont mises à zéro.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void keep_green_only(sil::Image& img)
{
    for (glm::vec3& colors : img.pixels())
    {
        // On garde uniquement la composante verte
        colors = glm::vec3{0.f, colors.g, 0.f};
    }
}

/**
 * Échange les composantes rouge et bleue de chaque pixel de l'image.
 * La composante verte reste inchangée.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void channels_swap(sil::Image& img)
{
    for (glm::vec3& colors : img.pixels())
    {
        // On échange la composante rouge et la composante bleue
        std::swap(colors.r, colors.b);
    }
}

/**
 * Convertit l'image en niveaux de gris en utilisant la formule de luminance relative au système sRGB.
 * La nouvelle valeur de chaque composante (R, G, B) est calculée comme suit :
 * gray = 0.299 * R + 0.587 * G + 0.114 * B
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void black_and_white(sil::Image& img)
{
    for (glm::vec3& colors : img.pixels())
    {
        float gray = 0.299f * colors.r + 0.587f * colors.g + 0.114f * colors.b; // Formule de luminance relative au système sRGB (y = 0.299 * R + 0.587 * G + 0.114 * B)
        colors = glm::vec3{gray, gray, gray};
    }
}

/**
 * Applique un effet de négatif à l'image en inversant les valeurs de chaque composante de couleur.
 * On sait que les couleurs sont dans l'espace sRGB et donc entre 0 et 1, on peut donc faire la négation donc :
 * new_color = 1.0 - original_color
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void negative(sil::Image& img)
{
    for (glm::vec3& colors : img.pixels())
    {
        colors = glm::vec3{1.f, 1.f, 1.f} - colors;
    }
}

/**
 * Remplit l'image avec un dégradé horizontal allant du noir à gauche (0, 0, 0) au blanc à droite (1, 1, 1).
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void gradient(sil::Image& img)
{
    for (int x{0}; x < img.width(); x++)
    {
        float t = static_cast<float>(x) / img.width();
        glm::vec3 color{t, t, t};
        
        for (int y{0}; y < img.height(); y++)
        {
            img.pixel(x, y) = color;
        }
    }
}

enum class Mirror
{
    Horizontal,
    Vertical,
    Both
};

/**
 * Miroir l'image horizontalement.
 * Chaque pixel (x, y) est échangé avec son symétrique par rapport à l'axe vertical passant par le milieu de l'image, c'est-à-dire le pixel (width - 1 - x, y).
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void mirror(sil::Image& img, Mirror direction = Mirror::Horizontal)
{
    int width = img.width();
    int height = img.height();
    for (int y{0}; y < height; y++)
    {
        for (int x{0}; x < width / 2; x++)  // On ne fait que la moitié de l'image en largeur (puisqu'on échange deux pixels à chaque fois)
        {
            if (direction == Mirror::Horizontal)
            {
                std::swap(img.pixel(x, y), img.pixel(width - 1 - x, y));
            }
            else if (direction == Mirror::Vertical)
            {
                std::swap(img.pixel(x, y), img.pixel(x, height - 1 - y));
            }
            else if (direction == Mirror::Both)
            {
                std::swap(img.pixel(x, y), img.pixel(width - 1 - x, height - 1 - y));
            }
        }
    }
}

/**
 * Ajoute du bruit aléatoire à chaque pixel de l'image.
 * On modifie aléatoirement la couleur d'environ 20% des pixels en leur assignant une couleur aléatoire.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void noisy(sil::Image& img)
{
    for (glm::vec3& colors : img.pixels())
    {
        const int random = random_int(0, 5);
        if(random == 0)
        {
            const float red = random_float(0, 100) / 100.f;
            const float green = random_float(0, 100) / 100.f;
            const float blue = random_float(0, 100) / 100.f;
            colors = glm::vec3{red, green, blue}; // Pixel aléatoire
        }
    }
}

/**
 * Fait pivoter l'image de 90 degrés dans le sens des aiguilles d'une montre.
 * La nouvelle position du pixel (x, y) devient (height - 1 - y, x) dans l'image pivotée.
 * On crée une nouvelle image pour stocker le résultat, puis on remplace l'ancienne image par la nouvelle.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void rotate90(sil::Image& img)
{
    int width = img.width();
    int height = img.height();
    sil::Image rotated_image{height, width};

    for (int y{0}; y < height; y++)
    {
        for (int x{0}; x < width; x++)
        {
            rotated_image.pixel(height - 1 - y, x) = img.pixel(x, y);
        }
    }

    img = std::move(rotated_image); // On remplace l'ancienne image par la nouvelle équivaut à img.pixels() = new_img.pixels();
}

/**
 * Applique un effet de séparation des canaux RGB en décalant la composante rouge vers la gauche et la composante bleue vers la droite.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void splitRGB(sil::Image& img)
{
    int width = img.width();
    int height = img.height();
    sil::Image split_image{width, height};
    const int offset = 25;

    for (int y{0}; y < height; y++)
    {
        for (int x{0}; x < width; x++)
        {
            float red = img.pixel(std::max(x - offset, 0), y).r;
            float blue = img.pixel(std::min(x + offset, width - 1), y).b;
            split_image.pixel(x, y) = glm::vec3{red, img.pixel(x, y).g, blue};
        }
    }

    img = std::move(split_image);
}

enum class Brightness
{
    Darker,
    Brighter
};

/**
 * Modifie la luminosité de l'image en fonction du mode spécifié.
 * Si le mode est Darker, chaque composante de couleur est élevée au carré pour assombrir l'image.
 * Si le mode est Brighter, chaque composante de couleur est transformée par la racine carrée pour éclaircir l'image.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param mode Mode de modification de la luminosité (Brightness::Darker ou Brightness::Brighter).
 */
void brightness(sil::Image& img, Brightness mode)
{
    for (glm::vec3& colors : img.pixels())
    {
        if (mode == Brightness::Darker)        {
            colors = glm::vec3{colors.r * colors.r, colors.g * colors.g, colors.b * colors.b};
        }

        else if (mode == Brightness::Brighter)
        {
            colors = glm::vec3{std::sqrt(colors.r), std::sqrt(colors.g), std::sqrt(colors.b)};
        }
    }
}

/**
 * Dessine un disque blanc sur un fond noir.
 * Le rayon et le centre du disque peuvent être spécifiés.
 * Si le centre n'est pas spécifié, il est placé au centre de l'image.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param radius Rayon du disque (par défaut 100.f).
 * @param centerX Coordonnée x du centre du disque (par défaut -1, ce qui signifie centré horizontalement).
 * @param centerY Coordonnée y du centre du disque (par défaut -1, ce qui signifie centré verticalement).
 */
void disk(sil::Image& img, float radius = 100.f, int centerX = -1, int centerY = -1)
{
    int width = img.width();
    int height = img.height();
    if (centerX == -1) centerX = width / 2;
    if (centerY == -1) centerY = height / 2;

    for (int y{0}; y < height; y++)
    {
        for (int x{0}; x < width; x++)
        {

            // Formule de la distance entre deux points : d = sqrt((x2 - x1)² + (y2 - y1)²)
            float dx = x - centerX; // x2 - x1
            float dy = y - centerY; // y2 - y1
            float distance = std::sqrt(dx * dx + dy * dy); // sqrt((x2 - x1)² + (y2 - y1)²
            if (distance < radius)
            {
                img.pixel(x, y) = glm::vec3{1.f, 1.f, 1.f};
            }
        }
    }
}

/**
 * Dessine un cercle blanc sur un fond noir.
 * Le rayon, l'épaisseur et le centre du cercle peuvent être spécifiés.
 * Si le centre n'est pas spécifié, il est placé au centre de l'image.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param radius Rayon du cercle (par défaut 100.f).
 * @param thickness Épaisseur du cercle (par défaut 3.f).
 * @param centerX Coordonnée x du centre du cercle (par défaut -1, ce qui signifie centré horizontalement).
 * @param centerY Coordonnée y du centre du cercle (par défaut -1, ce qui signifie centré verticalement).
 */
void circle(sil::Image& img, float radius = 100.f, float thickness = 3.f, int centerX = -1, int centerY = -1)
{
    int width = img.width();
    int height = img.height();
    if (centerX == -1) centerX = width / 2;
    if (centerY == -1) centerY = height / 2;

    for (int y{0}; y < height; y++)
    {
        for (int x{0}; x < width; x++)
        {
            // Formule de la distance entre deux points : d = sqrt((x2 - x1)² + (y2 - y1)²)
            float dx = x - centerX; // x2 - x1
            float dy = y - centerY; // y2 - y1
            float distance = std::sqrt(dx * dx + dy * dy); // sqrt((x2 - x1)² + (y2 - y1)²
            if (distance < radius + thickness && distance > radius - thickness)
            {
                img.pixel(x, y) = glm::vec3{1.f, 1.f, 1.f};
            }
        }
    }
}

/**
 * Crée une animation en dessinant un disque blanc se déplaçant horizontalement sur un fond noir.
 * Le disque se déplace de la gauche vers la droite de l'image.
 * Le nombre de secondes et le nombre d'images par seconde (ips) peuvent être spécifiés.
 * Le centre vertical du disque peut également être spécifié.
 *
 * @param centerY Coordonnée y du centre du disque (par défaut -1, ce qui signifie centré verticalement).
 * @param seconds Durée de l'animation en secondes (par défaut 3 secondes).
 * @param ips Images par seconde (par défaut 25 ips).
 */
void animation(int centerY = -1, int seconds = 3, int ips = 25)
{
    const int width = 500;
    const int height = 500;

    if (centerY == -1) centerY = height / 2;

    for (int x{0}; x < width; x += width / (seconds * ips))
    {
        sil::Image img{width, height};
        disk(img, 100.f, x, centerY);
        img.save("output/animation/frame_" + std::to_string(x) + ".png");
    }
}

/**
 * Dessine une rosace composée de plusieurs cercles blancs sur un fond noir.
 * Le nombre de cercles, la "serréité" (tightness) et le rayon des cercles peuvent être spécifiés.
 * Les cercles sont disposés de manière circulaire autour du centre de l'image.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param circles Nombre de cercles dans la rosette (par défaut 6).
 * @param tightness Facteur de serréité des cercles (par défaut 0.5f).
 * @param radius Rayon des cercles (par défaut 100.f).
 */
void rosette(sil::Image& img, int circles = 6, float tightness = 0.5f, float radius = 100.f)
{
    int width = img.width();
    int height = img.height();
    float centerX = width / 2.f;
    float centerY = height / 2.f;

    float offset = radius * 2 * tightness;

    for (int i = 0; i < circles; ++i)
    {
        // Calcul de l'angle actuel en radians (formule : angle = (2 * π * i) / nombre_de_cercles)
        float angle = (2.0f * std::numbers::pi * i) / circles;

        // Conversion coordonnées polaires en coordonnées cartésiennes (formules : x = centerX + offset * cos(angle) et y = centerY + offset * sin(angle))
        float cx = centerX + offset * std::cos(angle);
        float cy = centerY + offset * std::sin(angle);

        circle(img, radius, 3.f, static_cast<int>(cx), static_cast<int>(cy));
    }

    // Cercle central
    circle(img, radius, 3.f, static_cast<int>(centerX), static_cast<int>(centerY));
}

/**
 * Applique un effet de mosaïque à l'image en répétant l'image plusieurs fois.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param copies Nombre de copies de l'image (par défaut 5).
 */
void mosaic(sil::Image& img, int copies = 5)
{
    int width = img.width();
    int height = img.height();
    sil::Image mosaic_image{width*5, height*5};

    for (int y{0}; y < height*5; y++)
    {
        for (int x{0}; x < width*5; x++)
        {
            mosaic_image.pixel(x, y) = img.pixel(x % width, y % height);
        }
    }

    img = std::move(mosaic_image);
}

/**
 * Applique un effet de mosaïque avec miroir à l'image en répétant l'image plusieurs fois et en inversant alternativement les lignes.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void mosaic_mirror(sil::Image& img)
{
    int width = img.width();
    int height = img.height();
    sil::Image mosaic_image{width*5, height*5};

    for (int y{0}; y < height*5; y++)
    {
        for (int x{0}; x < width*5; x++)
        {
            // Détermination de la tuile courante
            int tile_x = x / width;
            int tile_y = y / height;

            // Coordonnées dans l'image source
            int src_x = x % width;
            int src_y = y % height;

            // Effet de miroir horizontal sur les colonnes impaires
            if (tile_x % 2 == 1)
                src_x = width - 1 - src_x;

            // Effet de miroir vertical sur les lignes impaires
            if (tile_y % 2 == 1)
                src_y = height - 1 - src_y;

            mosaic_image.pixel(x, y) = img.pixel(src_x, src_y);
        }
    }

    img = std::move(mosaic_image);
}

/**
 * Applique un effet de glitch à l'image en déplaçant aléatoirement des blocs de pixels.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void glitch(sil::Image& img)
{
    int width = img.width();
    int height = img.height();

    for (int x = 0; x < width; x ++)
    {
        for (int y = 0; y < height; y ++)
        {
            const int random = random_int(0, 1500);
            if(random == 0) {
                int rectWidth = random_int(10, 50);
                int rectHeight = random_int(1, 10);
                int x2 = random_int(0, width - rectWidth);
                int y2 = random_int(0, height - rectHeight);

                for (int dx = 0; dx < rectWidth; dx++) {
                    for (int dy = 0; dy < rectHeight; dy++) {
                        if (x + dx < width && y + dy < height && x2 + dx < width && y2 + dy < height) {
                            std::swap(img.pixel(x + dx, y + dy), img.pixel(x2 + dx, y2 + dy));
                        }
                    }
                }
            }
        }
    }
}

/**
 * Trie les pixels de l'image par segments aléatoires en fonction de leur luminosité totale.
 * Parcourt les pixels de l'image et, avec une probabilité de 1 sur 75, sélectionne un segment de pixels de longueur aléatoire entre 20 et 75 pixels.
 * Trie ensuite ce segment de pixels en fonction de la somme de leurs composantes R, G et B (luminosité totale).
 * 
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void pixelSort(sil::Image& img)
{
    std::vector<glm::vec3> pixels = img.pixels();
    int pixelIndex = 0;
    while (pixelIndex < img.width() * img.height())
    {
        int random = random_int(0, 75);
        if (random == 0) {
            int randomLength = random_int(20, 75);
            int endIndex = std::min(pixelIndex + randomLength, img.width() * img.height());
            std::sort(pixels.begin() + pixelIndex, pixels.begin() + endIndex, [](const glm::vec3& a, const glm::vec3& b) {
                return (a.r + a.g + a.b) < (b.r + b.g + b.b); // Tri par luminosité totale (addition des composantes R, G et B)
            });
            pixelIndex = endIndex;
        }
        else {
            pixelIndex++;
        }
    }
    img.pixels() = pixels;
}

int main()
{
    sil::Image image{"images/logo.png"};
    keep_green_only(image);
    image.save("output/green_only.png");

    image = sil::Image{"images/logo.png"};
    channels_swap(image);
    image.save("output/channels_swap.png");

    image = sil::Image{"images/logo.png"};
    black_and_white(image);
    image.save("output/black_and_white.png");

    image = sil::Image{"images/logo.png"};
    negative(image);
    image.save("output/negative.png");

    image = sil::Image{300, 200};
    gradient(image);
    image.save("output/gradient.png");

    image = sil::Image{"images/logo.png"};
    mirror(image);
    image.save("output/mirror.png");

    image = sil::Image{"images/logo.png"};
    noisy(image);
    image.save("output/noisy.png");

    image = sil::Image{"images/logo.png"};
    rotate90(image);
    image.save("output/rotate90.png");

    image = sil::Image{"images/logo.png"};
    splitRGB(image);
    image.save("output/splitRGB.png");

    image = sil::Image{"images/photo.jpg"};
    brightness(image, Brightness::Darker);
    image.save("output/darker.png");

    image = sil::Image{"images/photo.jpg"};
    brightness(image, Brightness::Brighter);
    image.save("output/brighter.png");

    image = sil::Image{500, 500};
    disk(image);
    image.save("output/disk.png");

    image = sil::Image{500, 500};
    circle(image);
    image.save("output/circle.png");

    animation();

    image = sil::Image{500, 500};
    rosette(image);
    image.save("output/rosette.png");

    image = sil::Image{"images/logo.png"};
    mosaic(image);
    image.save("output/mosaic.png");

    image = sil::Image{"images/logo.png"};
    mosaic_mirror(image);
    image.save("output/mosaic_mirror.png");

    image = sil::Image{"images/logo.png"};
    glitch(image);
    image.save("output/glitch.png");

    image = sil::Image{"images/logo.png"};
    pixelSort(image);
    image.save("output/pixel_sort.png");

    return 0;
}