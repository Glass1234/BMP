#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
using namespace std;
//kek
class BMP
{
public:
    // BMP() {}
    BMP(string image)
    {
        this->image = image;
    }
    void OpenRB()
    {
        FILE *image = fopen(this->image.c_str(), "rb");
        if (image)
        {
            fread(&this->head, sizeof(HEAD), 1, image);                                 //заполняем структуры
            fread(&this->info, sizeof(INFO), 1, image);                                 //v
            this->rgb_arr.resize(info.sizeimage / sizeof(RGB));                         //
            fread(&this->rgb_arr[0], sizeof(RGB), info.sizeimage / sizeof(RGB), image); //x
            fclose(image);
        }
        else
        {
            cout << "ERROR" << endl;
        }
    }
    void Write()
    {
        FILE *image = fopen("new_image.bmp", "wb");
        if (image)
        {
            fwrite(&this->head, sizeof(HEAD), 1, image);
            fwrite(&this->info, sizeof(INFO), 1, image);
            fwrite(&this->rgb_arr[0], sizeof(RGB), info.sizeimage / sizeof(RGB), image);
            fclose(image);
        }
        else
        {
            cout << "ERROR" << endl;
        }
    }
    void STDOUT()
    {
        cout << "------------------------debag------------------------" << endl;

        cout << "HEAD: " << sizeof(HEAD) << '-';
        cout << sizeof(HEAD().type) + sizeof(HEAD().size) + sizeof(HEAD().reserv) + sizeof(HEAD().offbits) << endl;

        cout << "INFO: " << sizeof(INFO) << '-';
        cout << sizeof(INFO().size) + sizeof(INFO().width) + sizeof(INFO().height) + sizeof(INFO().planes) + sizeof(INFO().bitcount) + sizeof(INFO().compression) + sizeof(INFO().sizeimage) + sizeof(INFO().unused) << endl;

        cout << "RGB: " << sizeof(RGB) << '-';
        cout << sizeof(RGB().b) + sizeof(RGB().g) + sizeof(RGB().r) << endl;

        cout << "size_rgb:";
        cout << rgb_arr.size() << endl;

        // sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Works!");
        // sf::Image buffer;
        // buffer.create(this->info.width, this->info.height, sf::Color(0, 0, 0));

        // for (int i = 0; i < this->info.height; i++)
        // {
        //     for (int j = 0; j < this->info.width; j++)
        //     {
        //         RGB data = rgb_arr[(i * this->info.width) + j];
        //         buffer.setPixel(j, (this->info.height - 1) - i, sf::Color(data.r, data.g, data.b));
        //     }
        // }

        // sf::Rect<int> rectangle(0, 0, this->info.width, this->info.height);
        // sf::Texture bufferTexture;
        // bufferTexture.loadFromImage(buffer, rectangle);
        // sf::Sprite bufferSprite(bufferTexture);
        // bufferSprite.setScale(0.8, 0.8);
        // while (window.isOpen())
        // {
        //     sf::Event event;
        //     while (window.pollEvent(event))
        //     {
        //         if (event.type == sf::Event::Closed)
        //             window.close();
        //     }
        //     window.draw(bufferSprite);
        //     window.display();
        // }
        cout << "-----------------------------------------------------" << endl;
    }
    void TF()
    {
        float error = 0.0f;

        for (int i = 0; i < this->info.height; i++)
        {
            for (int j = 0; j < this->info.width; j++)
            {
                RGB *data = &this->rgb_arr[(i * this->info.width) + j];
                uint8_t S = (data->r + data->g + data->b) / 3;

                if (S < (0xFF / 2))
                {
                    error = (S - 0x00) / 16;
                    S = 0x00;
                }
                else
                {
                    error = (S - 0xFF) / 16;
                    S = 0xFF;
                }

                *data = RGB{S, S, S};

                int x = j;
                int y = (this->info.height - 1) - i;

                if (j < this->info.width - 1)
                {
                    data = &this->rgb_arr[(i * this->info.width) + j + 1];
                    S = (data->r + data->g + data->b) / 3;
                    S += 7 * error;
                    *data = RGB{S, S, S};
                }

                if (j < this->info.width - 1 && i < this->info.height - 1)
                {
                    data = &this->rgb_arr[((i + 1) * this->info.width) + j + 1];
                    S = (data->r + data->g + data->b) / 3;
                    S += 1 * error;
                    *data = RGB{S, S, S};
                }

                if (i < this->info.height - 1)
                {
                    data = &this->rgb_arr[((i + 1) * this->info.width) + j];
                    S = (data->r + data->g + data->b) / 3;
                    S += 5 * error;
                    *data = RGB{S, S, S};
                }

                if (j < this->info.width + 1 && i < this->info.height - 1)
                {
                    data = &this->rgb_arr[((i + 1) * this->info.width) + j - 1];
                    S = (data->r + data->g + data->b) / 3;
                    S += 3 * error;
                    *data = RGB{S, S, S};
                }
            }
        }
    }

private:
#pragma pack(push, 1)
    struct HEAD
    {
        int16_t type;
        int32_t size;
        int16_t reserv[2];
        int32_t offbits;
    };
    struct INFO
    {
        int32_t size;
        int32_t width;
        int32_t height;
        int16_t planes;
        int16_t bitcount;
        int32_t compression;
        int32_t sizeimage;
        int32_t unused[25];
    };
    struct RGB
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t unused = 0xff;
    };
#pragma pack(pop)

    HEAD head;
    INFO info;
    RGB rgb;
    vector<RGB> rgb_arr;
    string image;
};

int main(int argc, char *argv[])
{
    BMP test(argv[argc - 1]);
    test.OpenRB();
    test.TF();
    test.Write();
    // test.STDOUT();
}
