#include <iostream>
#include <string>
#include <vector>
using namespace std;

class BMP
{
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

public:
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
    void TF()
    {
        float error = 0.0f;

        for (int i = 0; i < this->info.height; i++) //высота
        {
            for (int j = 0; j < this->info.width; j++) //ширина
            {
                RGB *data = &this->rgb_arr[(i * this->info.width) + j]; //берём i пиксель
                uint8_t S = (data->r + data->g + data->b) / 3;          //определяем средний цвет

                if (S < (0xFF / 2)) //если пиксель меньше 128 тогда он чёрный
                {
                    error = (S - 0x00) / 42; //вычеслем ошибку для применени метода дизеринга
                    S = 0x00;
                }
                else //иначе белый
                {
                    error = (S - 0xFF) / 42;
                    S = 0xFF;
                }

                *data = RGB{S, S, S}; //создаём "пиксель" которому мы уже сменили цветы

                //накладывание шума
                if (j < this->info.width - 1) //справа
                {
                    dizering(data, 0, 1, i, j, S, 8, error);
                }
                if (j < this->info.width - 2) //справа + 1
                {
                    dizering(data, 0, 2, i, j, S, 4, error);
                }
                if (j < this->info.width - 1 && i < this->info.height - 1) //снизу справа
                {
                    dizering(data, 1, 1, i, j, S, 4, error);
                }
                if (j < this->info.width - 2 && i < this->info.height - 1) //снизу справа + 1
                {
                    dizering(data, 1, 2, i, j, S, 2, error);
                }
                if (j < this->info.width - 1 && i < this->info.height - 2) //снизу + 1 справа
                {
                    dizering(data, 2, 1, i, j, S, 2, error);
                }
                if (j < this->info.width - 2 && i < this->info.height - 2) //снизу + 1 справа + 1
                {
                    dizering(data, 2, 2, i, j, S, 1, error);
                }
                if (i < this->info.height - 1) //снизу
                {
                    dizering(data, 1, 0, i, j, S, 8, error);
                }
                if (i < this->info.height - 2) //снизу + 1
                {
                    dizering(data, 2, 0, i, j, S, 4, error);
                }
                if (j > 1 && i < this->info.height - 2) //снизу слева + 1
                {
                    dizering(data, 1, -2, i, j, S, 2, error);
                }
                if (j > 0 && i < this->info.height - 1) //снизу слева
                {
                    dizering(data, 1, -1, i, j, S, 4, error);
                }
                if (j > 1 && i < this->info.height - 2) //снизу + 1 слева + 1
                {
                    dizering(data, 2, -2, i, j, S, 1, error);
                }
                if (j > 0 && i < this->info.height - 2) //снизу + 1 слева
                {
                    dizering(data, 2, -1, i, j, S, 2, error);
                }
            }
        }
    }

private:
    void dizering(RGB *data, int depth, int LR, int i, int j, uint8_t S, int coefficent, int error)
    {
        data = &this->rgb_arr[((i + depth) * this->info.width) + j + LR];
        S = (data->r + data->g + data->b) / 3;
        S += coefficent * error;
        *data = RGB{S, S, S};
    }
};

int main(int argc, char *argv[])
{
    BMP test(argv[argc - 1]);
    test.OpenRB();
    test.TF();
    test.Write();
    // test.STDOUT();
}
