#include <iostream>
#include <string>
#include <vector>
using namespace std;

class BMP
{
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
    void STDOUT()
    {
        cout << "------------------------debag------------------------" << endl;

        cout << "HEAD: " << sizeof(HEAD) << '-';
        cout << sizeof(HEAD().type) + sizeof(HEAD().size) + sizeof(HEAD().reserv) + sizeof(HEAD().offbits) << endl;

        cout << "INFO: " << sizeof(INFO) << '-';
        cout << sizeof(INFO().size) + sizeof(INFO().width) + sizeof(INFO().height) + sizeof(INFO().planes) + sizeof(INFO().bitcount) + sizeof(INFO().compression) + sizeof(INFO().sizeimage) + sizeof(INFO().unused) << endl;

        cout << "RGB: " << sizeof(RGB) << '-';
        cout << sizeof(RGB().b) + sizeof(RGB().g) + sizeof(RGB().r) + sizeof(RGB().unused) << endl;

        cout << "size_rgb:";
        cout << rgb_arr.size() << endl;

        cout << "-----------------------------------------------------" << endl;
    }
    void TF()
    {
        float error = 0.0f;

        for (int i = 0; i < this->info.height; i++)
        {
            for (int j = 0; j < this->info.width; j++)
            {
                RGB *data = &this->rgb_arr[(i * this->info.width) + j]; //берём i пиксель
                uint8_t S = (data->r + data->g + data->b) / 3;          //определяем средний цвет

                if (S < (0xFF / 2)) //если пиксель меньше 128 тогда он чёрный
                {
                    error = (S - 0x00) / 16; //вычеслем ошибку для применени метода дизеринга
                    S = 0x00;
                }
                else //иначе белый
                {
                    error = (S - 0xFF) / 16;
                    S = 0xFF;
                }

                *data = RGB{S, S, S}; //создаём "пиксель" которому мы уже сменили цветы

                //накладывание шума
                if (j < this->info.width - 1) //если пиксель не вылез за границы
                {
                    data = &this->rgb_arr[(i * this->info.width) + j + 1]; //берём следующий пиксель
                    S = (data->r + data->g + data->b) / 3;                 //находим его цвет
                    S += 7 * error;                                        //делаем пропорцию
                    *data = RGB{S, S, S};                                  //записываем готовый результат
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
