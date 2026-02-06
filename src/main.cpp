#include <sil/sil.hpp>
#include <glm/vec3.hpp>
#include <random.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numbers>
#include <complex>

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

/**
 * Génère le fractal de Mandelbrot et le dessine dans l'image fournie.
 * Chaque pixel de l'image est coloré en fonction du nombre d'itérations nécessaires pour déterminer si le point complexe correspondant appartient à l'ensemble de Mandelbrot.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param iterations Nombre maximum d'itérations pour déterminer l'appartenance à l'ensemble de Mandelbrot (par défaut 100).
 */
void mandelbrotFractal(sil::Image& img, int iterations = 100)
{
    int width = img.width();
    int height = img.height();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x) {
            std::complex<float> c(
                (static_cast<float>(x) / width) * 3.5f - 2.5f,
                (static_cast<float>(y) / height) * 2.0f - 1.0f
            );
            std::complex<float> z = 0;
            int n = 0;

            while (std::abs(z) <= 2.0f && n < iterations)
            {
                z = z * z + c;
                ++n;
            }

            float t = static_cast<float>(n) / iterations;
            img.pixel(x, y) = glm::vec3{t, t, t};
        }
    }
}

enum class Kernel
{
    Identity,
    Blur,
    Sharpen,
    EdgeDetection,
    BoxBlur
};

std::vector<std::vector<float>> getKernel(Kernel type) {
    switch (type) {
        case Kernel::Identity:
            return {
                {0, 0, 0},
                {0, 1, 0},
                {0, 0, 0}
            };

        case Kernel::Blur:
            return {
                {0.0625f, 0.125f, 0.0625f},
                {0.125f, 0.25f, 0.125f},
                {0.0625f, 0.125f, 0.0625f}
            };

        case Kernel::Sharpen:
            return {
                { 0, -1,  0},
                {-1,  5, -1},
                { 0, -1,  0}
            };

        case Kernel::EdgeDetection:
            return {
                {-1, -1, -1},
                {-1,  8, -1},
                {-1, -1, -1}
            };
    }

    return {};
}

/**
 * Applique une convolution de flou à l'image en utilisant un noyau de moyenne mobile de taille spécifiée.
 * La convolution est effectuée en deux passes : d'abord horizontalement, puis verticalement.
 * Le résultat final est une image floutée, où chaque pixel est la moyenne des pixels environnants dans un carré de taille "size x size".
 * 
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param size Taille du noyau de flou (par défaut 100). 
 */
void blur_convolution(sil::Image& img, int size = 100) {
    if (size <= 1) return;

    const int w = img.width();
    const int h = img.height();
    const int half = size / 2;

    sil::Image temp{w, h};

    for (int y = 0; y < h; ++y) {
        glm::vec3 sum{0.f};

        for (int i = -half; i < -half + size; ++i) {
            int sx = std::clamp(i, 0, w - 1);
            sum += img.pixel(sx, y);
        }

        temp.pixel(0, y) = sum / static_cast<float>(size);

        for (int x = 1; x < w; ++x) {
            int removeX = std::clamp(x - half - 1, 0, w - 1);
            int addX    = std::clamp(x - half + size - 1, 0, w - 1);

            sum -= img.pixel(removeX, y);
            sum += img.pixel(addX, y);

            temp.pixel(x, y) = sum / static_cast<float>(size);
        }
    }

    sil::Image out{w, h};

    for (int x = 0; x < w; ++x) {
        glm::vec3 sum{0.f};

        for (int j = -half; j < -half + size; ++j) {
            int sy = std::clamp(j, 0, h - 1);
            sum += temp.pixel(x, sy);
        }

        out.pixel(x, 0) = sum / static_cast<float>(size);

        for (int y = 1; y < h; ++y) {
            int removeY = std::clamp(y - half - 1, 0, h - 1);
            int addY    = std::clamp(y - half + size - 1, 0, h - 1);

            sum -= temp.pixel(x, removeY);
            sum += temp.pixel(x, addY);

            out.pixel(x, y) = sum / static_cast<float>(size);
        }
    }

    img = std::move(out);
}


/**
 * Applique une convolution à l'image en utilisant un noyau de convolution spécifié.
 * La convolution est effectuée en parcourant chaque pixel de l'image (sauf les bords) et en calculant la nouvelle valeur du pixel en fonction des pixels voisins et du noyau.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param kernel Type de noyau de convolution à appliquer (Kernel::Identity, Kernel::Blur, Kernel::Sharpen, Kernel::EdgeDetection).
 */
void convolution(sil::Image& img, Kernel kernel) {
    if (kernel == Kernel::BoxBlur) {
        blur_convolution(img);
        return;
    }
    std::vector<std::vector<float>> k = getKernel(kernel);
    sil::Image original = img;

    for (int y = 1; y < img.height() - 1; ++y) {
        for (int x = 1; x < img.width() - 1; ++x) {
            glm::vec3 newColor{0.f, 0.f, 0.f};

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    glm::vec3 neighborColor = original.pixel(x + kx, y + ky);
                    newColor += neighborColor * k[ky + 1][kx + 1];
                }
            }

            img.pixel(x, y) = newColor;
        }
    }
}

/**
 * Applique un effet de différence de gaussienne à l'image en utilisant deux convolutions de flou avec des tailles de noyau différentes.
 * La différence de gaussienne est obtenue en soustrayant l'image floutée avec un noyau plus grand de l'image floutée avec un noyau plus petit.
 * 
 * @param img Image à modifier (type sil::Image), modifiée en place.
 */
void gaussienne_difference(sil::Image& img) {
    sil::Image blurred1 = img;
    sil::Image blurred2 = img;

    blur_convolution(blurred1, 1);
    blur_convolution(blurred2, 3);

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            glm::vec3 c1 = blurred1.pixel(x, y);
            glm::vec3 c2 = blurred2.pixel(x, y);
            img.pixel(x, y) = glm::vec3{
                std::clamp(c1.r - c2.r > 0.03f ? 1.f : c1.r - c2.r, 0.f, 1.f),
                std::clamp(c1.g - c2.g > 0.03f ? 1.f : c1.g - c2.g, 0.f, 1.f),
                std::clamp(c1.b - c2.b > 0.03f ? 1.f : c1.b - c2.b, 0.f, 1.f)
            };
        }
    }
}

/**
 * Applique un filtre de Kuwahara à l'image pour réduire le bruit tout en préservant les bords.
 * Pour chaque pixel, le filtre divise la région environnante en quatre sous-régions et calcule la moyenne et la variance de chaque sous-région.
 * Le pixel est ensuite remplacé par la moyenne de la sous-région ayant la plus faible variance.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param radius Rayon de la région environnante à considérer pour le filtrage (par défaut 4).
 */
void kuwahara(sil::Image& img, int radius = 4) {
    const int w = img.width();
    const int h = img.height();
    if (radius <= 0) return;

    sil::Image original = img;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {

            glm::vec3 bestMean{0.f};
            float bestVar = std::numeric_limits<float>::infinity();

            for (int q = 0; q < 4; ++q) {
                int dx0 = (q == 0 || q == 2) ? -radius : 0;
                int dx1 = (q == 0 || q == 2) ? 0       : radius;
                int dy0 = (q == 0 || q == 1) ? -radius : 0;
                int dy1 = (q == 0 || q == 1) ? 0       : radius;

                glm::vec3 mean{0.f};
                float meanL = 0.f;
                int count = 0;

                for (int dy = dy0; dy <= dy1; ++dy) {
                    for (int dx = dx0; dx <= dx1; ++dx) {
                        int sx = std::clamp(x + dx, 0, w - 1);
                        int sy = std::clamp(y + dy, 0, h - 1);
                        const glm::vec3 c = original.pixel(sx, sy);
                        mean += c;
                        meanL += 0.299f * c.r + 0.587f * c.g + 0.114f * c.b;
                        ++count;
                    }
                }

                if (count == 0) continue;
                mean /= static_cast<float>(count);
                meanL /= static_cast<float>(count);

                float var = 0.f;
                for (int dy = dy0; dy <= dy1; ++dy) {
                    for (int dx = dx0; dx <= dx1; ++dx) {
                        int sx = std::clamp(x + dx, 0, w - 1);
                        int sy = std::clamp(y + dy, 0, h - 1);
                        float l = 0.299f * original.pixel(sx, sy).r + 0.587f * original.pixel(sx, sy).g + 0.114f * original.pixel(sx, sy).b;
                        float d = l - meanL;
                        var += d * d;
                    }
                }
                var /= static_cast<float>(count);

                if (var < bestVar) {
                    bestVar = var;
                    bestMean = mean;
                }
            }

            img.pixel(x, y) = bestMean;
        }
    }
}

/**
 * Applique un tramage (dithering) à une valeur de couleur d'un canal (R, G ou B) en utilisant un motif de Bayer 4x4.
 * Le tramage est une technique de quantification qui permet de simuler des niveaux de couleur intermédiaires en utilisant des motifs de pixels.
 * 
 * @param value Valeur de couleur du canal à tramer (entre 0 et 1).
 * @param x Coordonnée x du pixel (utilisée pour déterminer le seuil de tramage à partir du motif de Bayer).
 * @param y Coordonnée y du pixel (utilisée pour déterminer le seuil de tramage à partir du motif de Bayer).
 */
float dither_channel(float value, int x, int y)
{
    static const int bayer[4][4] = {
        { 0,  8,  2, 10},
        {12,  4, 14,  6},
        { 3, 11,  1,  9},
        {15,  7, 13,  5}
    };

    value = std::clamp(value, 0.f, 1.f);

    float threshold = (bayer[y % 4][x % 4] + 0.5f) / 16.f;

    return value > threshold ? 1.f : 0.f;
}


/**
 * Applique un tramage (dithering) en couleur à l'image.
 * Chaque canal RGB est quantifié avec un motif de Bayer 4x4.
 *
 * @param img Image à modifier (modifiée en place)
 * @param color Si true, applique le tramage à chaque canal RGB, sinon convertit l'image en niveaux de gris avant d'appliquer le tramage.
 */
void dithering(sil::Image& img, bool color = true)
{
    int width = img.width();
    int height = img.height();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            glm::vec3 colorVec = img.pixel(x, y);

            if (color) {
                colorVec.r = dither_channel(colorVec.r, x, y);
                colorVec.g = dither_channel(colorVec.g, x, y);
                colorVec.b = dither_channel(colorVec.b, x, y);
            } else {
                float gray = 0.299f * colorVec.r + 0.587f * colorVec.g + 0.114f * colorVec.b;
                gray = dither_channel(gray, x, y);
                colorVec = glm::vec3{gray, gray, gray};
            }

            img.pixel(x, y) = colorVec;
        }
    }
}


/* ----- Effets personnels ----- */

/**
 * Applique un effet de pixelisation à l'image en regroupant les pixels en blocs et en remplaçant chaque bloc par la couleur moyenne de ses pixels.
 *
 * @param img Image à modifier (type sil::Image), modifiée en place.
 * @param blockSize Taille des blocs de pixels (par défaut 8).
 */
void pixelated(sil::Image& img, int blockSize = 8) // Effet 8 bits
{
    int width = img.width();
    int height = img.height();

    for (int y = 0; y < height; y += blockSize)
    {
        for (int x = 0; x < width; x += blockSize)
        {
            // Calcul de la couleur moyenne du bloc
            glm::vec3 avgColor{0.f, 0.f, 0.f};
            int pixelCount = 0;

            for (int dy = 0; dy < blockSize; ++dy)
            {
                for (int dx = 0; dx < blockSize; ++dx)
                {
                    if (x + dx < width && y + dy < height)
                    {
                        avgColor += img.pixel(x + dx, y + dy);
                        pixelCount++;
                    }
                }
            }

            if (pixelCount > 0)
            {
                avgColor /= static_cast<float>(pixelCount);
            }

            // Application de la couleur moyenne à tous les pixels du bloc
            for (int dy = 0; dy < blockSize; ++dy)
            {
                for (int dx = 0; dx < blockSize; ++dx)
                {
                    if (x + dx < width && y + dy < height)
                    {
                        img.pixel(x + dx, y + dy) = avgColor;
                    }
                }
            }
        }
    }
}

/* 
    Image différentielle 
    Effet vu lors de ma 3ème année de BUT Info pour un exercice en C (création de notre propre format d'image et de compression)
*/

/**
 * Calcule les différences entre chaque pixel et le pixel précédent dans l'image.
 * Le premier pixel reste inchangé.
 *
 * @param img Image source (type sil::Image).
 * @return Vecteur contenant les pixels différentiels.
 */
std::vector<glm::vec3> pixel_to_diff(sil::Image& img)
{
    int width = img.width();
    int height = img.height();
    std::vector<glm::vec3> differential;
    differential.reserve(width * height);
    std::vector<glm::vec3> image_pixels = img.pixels(); // Copie des pixels de l'image (tableau 1D)

    for (int x = 0; x < width * height; ++x)
    {
        if (x == 0){
            differential.push_back(image_pixels[0]);
            continue;
        } else {
            differential.push_back(image_pixels[x] - image_pixels[x - 1]);
        }        
    }

    return differential;
}

/**
 * Sauvegarde les données différentielles dans un fichier CSV.
 *
 * @param data Vecteur contenant les pixels différentiels (type glm::vec3).
 * @param filename Nom du fichier CSV où les données seront sauvegardées.
 */
void save_differential_data_in_csv(std::vector<glm::vec3> data, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << filename << std::endl;
        return;
    }

    file << "R,G,B\n"; // En-tête du fichier CSV
    file << std::fixed << std::setprecision(6);

    for (const auto& color : data)
    {
        file << color.r << "," << color.g << "," << color.b << "\n";
    }

    file.close();
    std::cout << "Data saved in " << filename << std::endl;
}

/**
 * Créer une image différentielle à partir de l'image source.
 * L'image différentielle est obtenue en calculant la différence entre chaque pixel et le pixel précédent.
 * Le résultat est ensuite converti en une image où les valeurs absolues des différences sont utilisées.
 * Seul le premier pixel reste inchangé.
 *
 * @param img Image source (type sil::Image), modifiée en place pour contenir l'image différentielle.
 */
void differential(sil::Image& img, bool save_csv = true, const std::string& csv_filename = "../output/differential.csv")
{
    int width = img.width();
    int height = img.height();
    std::vector<glm::vec3> differential = pixel_to_diff(img);
    sil::Image differential_image{width, height};

    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            glm::vec3 colors = glm::abs(differential[y * width + x]);
            glm::vec3 base = {1.0f, 1.0f, 1.0f};
            differential_image.pixel(x, y) = base - colors;
        }
    }

    if (save_csv){
        save_differential_data_in_csv(differential, csv_filename);
    }
    img = std::move(differential_image);
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
    image.save("output/darker.jpg");

    image = sil::Image{"images/photo.jpg"};
    brightness(image, Brightness::Brighter);
    image.save("output/brighter.jpg");
    
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

    image = sil::Image{500, 500};
    mandelbrotFractal(image);
    image.save("output/mandelbrot.png");

    image = sil::Image{"images/logo.png"};
    convolution(image, Kernel::Identity);
    image.save("output/convolution_identity.png");

    image = sil::Image{"images/logo.png"};
    convolution(image, Kernel::Blur);
    image.save("output/convolution_blur.png");

    image = sil::Image{"images/logo.png"};
    convolution(image, Kernel::Sharpen);
    image.save("output/convolution_sharpen.png");

    image = sil::Image{"images/logo.png"};
    convolution(image, Kernel::EdgeDetection);
    image.save("output/convolution_edge_detection.png");

    image = sil::Image{"images/logo.png"};
    convolution(image, Kernel::BoxBlur);
    image.save("output/convolution_blur_box.png");

    image = sil::Image{"images/inky.png"};
    gaussienne_difference(image);
    image.save("output/gaussienne_difference.png");

    image = sil::Image{"images/logo.png"};
    pixelated(image);
    image.save("output/pixelated.png");

    save_differential_data_in_csv(sil::Image{"images/inky.png"}.pixels(), "../output/differential_original.csv");

    image = sil::Image{"images/logo.png"};
    differential(image, false);
    image.save("output/differential.png");

    image = sil::Image{"images/inky.png"};
    differential(image, true, "../output/differential_inky.csv");
    image.save("output/differential_inky.png");

    image = sil::Image{"images/inky_mono.png"};
    differential(image, true, "../output/differential_inky_mono.csv");
    image.save("output/differential_inky_mono.png");

    image = sil::Image{"images/inky.png"};
    kuwahara(image);
    image.save("output/kuwahara.png");

    image = sil::Image{"images/inky.png"};
    dithering(image);
    image.save("output/dithering_color.png");

    image = sil::Image{"images/photo.jpg"};
    dithering(image, false);
    image.save("output/dithering_mono.jpg");    

    return 0;
}