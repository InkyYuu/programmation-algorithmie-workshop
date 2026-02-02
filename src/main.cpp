#include <sil/sil.hpp>
#include <glm/vec3.hpp>
#include <random.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

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

    img = std::move(rotated_image); // On remplace l'ancienne image par la nouvelle
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
}