#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define len(x)((int)log10(x)+1)//Sýkýþtýrýlmýþ metin uzunluðu için karmaþýklýk O (n)dir (basit len)

//LZ77 ALGORITMASI
struct token
{
    uint8_t offset_uzunluk;
    char c;
};

int kapasite = 1 << 3;

char* dosya_okuma(char* dosya, int* boy)
{

    FILE* fp = fopen(dosya, "rb");
    char metin[100];
    char* kaynak_yazi;

    if(fp!=NULL)
    {
        while(!feof(fp))
        {
            fgets(metin,100,fp);
        }


        *boy = ftell(fp);
        kaynak_yazi = malloc(*boy);

        rewind(fp);
        for(int i=0; i < *boy; i++)
            kaynak_yazi[i] = (char) fgetc(fp);
    }

    else
    {
        printf("Dosya bulunamadi.");
        exit(0);
    }

    fclose(fp);

    return kaynak_yazi;
}

//SIKISTIRMA KISMI
struct token *encode(char *metin, int maks, int *tokenSayi)
{
    int offsetbits=5, lenghtbits=(8 - offsetbits);
    int offsetmask=((1 << offsetbits) - 1);
    int lenghtmask=((1 << lenghtbits) - 1);

    int tokenToplam = 0;
    char *ileriTampon, *aramaTampon;
    struct token a;

    struct token *encoded = malloc(kapasite * sizeof(struct token));

    for (ileriTampon = metin; ileriTampon < metin + maks; ileriTampon++)
    {
        aramaTampon = ileriTampon - offsetmask;
        if (aramaTampon < metin)
            aramaTampon = metin;

        int max_uzunluk = 0;
        char *max_konum = ileriTampon;

        for (; aramaTampon < ileriTampon; aramaTampon++)
        {
            int uzunluk = eslesmeboyu(aramaTampon, ileriTampon, lenghtmask);

            if (uzunluk > max_uzunluk)
            {
                max_uzunluk = uzunluk;
                max_konum = aramaTampon;
            }
        }

        if (ileriTampon + max_uzunluk >= metin + maks)
        {
            max_uzunluk = metin + maks - ileriTampon - 1;
        }

        int x=ileriTampon - max_konum;
        int y=max_uzunluk;

        a.offset_uzunluk = (x<< lenghtbits | y);
        ileriTampon += max_uzunluk;
        a.c = *ileriTampon;

        if (tokenToplam + 1 > kapasite)
        {
            kapasite = kapasite << 1;
            encoded = realloc(encoded, kapasite * sizeof(struct token));
        }

        encoded[tokenToplam++] = a;
    }

    if (tokenToplam)
        *tokenSayi = tokenToplam;

    return encoded;
}

int eslesmeboyu(char* met1, char* met2, int sinir)
{
    int boy = 0;
    while(*met1++ == *met2++ && boy < sinir)
        boy++;

    return boy;
}

void dosya_yaz_tokens(char* dosya, struct token* encoded, int i)
{
    FILE* fp;
    if (fp = fopen(dosya, "wb"))
    {
        fwrite(encoded, sizeof(struct token), i, fp);
        fclose(fp);
    }
}

void karakter_kopyala(char* m1, char* m2, int boyut)
{
    while(boyut--)
    {
        strcmp(*m1,*m2);
        m1++;
        m2++;
    }

}

//Huffman Kodlamasi

struct node
{
    int deger;
    char harf;
    struct node *sol,*sag;
};

typedef struct node Dugum;

int frekans ()
{
    FILE* fp = fopen("metin.txt", "rb");
    char metin[100];

    if(fp!=NULL)
    {
        while(!feof(fp))
            fgets(metin,100,fp);
    }

    int c=0, alfabe[30]= {0};

    while(metin[c]!='\0')
    {

        if(metin[c]>= 'a' && metin[c]<= 'z')
            alfabe[metin[c]-'a']++;
        c++;
    }

    for(c=0; c<20; c++)
    {
        if(alfabe[c]!=0)
        {
            c++;
        }

    }

}
int kucukBul (Dugum *dizi[], int farkli)
{
    int enKucuk;
    int i = 0;

    while (dizi[i]->deger==-1)
        i++;
    enKucuk=i;
    if (i==farkli)
    {
        i++;
        while (dizi[i]->deger==-1)
            i++;
        enKucuk=i;
    }

    for (i=1; i<30; i++)
    {
        if (dizi[i]->deger==-1)
            continue;
        if (i==farkli)
            continue;
        if (dizi[i]->deger<dizi[enKucuk]->deger)
            enKucuk = i;
    }


    return enKucuk;
}

//huffman agacini olusturur ve adresini döndürür/
void olustur_HuffmanTree(Dugum **tree)
{
    Dugum *temp;
    Dugum *dizi[30];
    int i, altDugum = 30;
    int kucuk1,kucuk2;

    for (i=0; i<30; i++)
    {
        dizi[i] = malloc(sizeof(Dugum));
        dizi[i]->deger = frekans();
        dizi[i]->harf = i;
        dizi[i]->sol = NULL;
        dizi[i]->sag = NULL;
    }

    while (altDugum>1)
    {
        kucuk1=kucukBul(dizi,-1);
        kucuk2=kucukBul(dizi,kucuk1);
        temp = dizi[kucuk1];
        dizi[kucuk1] = malloc(sizeof(Dugum));
        dizi[kucuk1]->deger=temp->deger+dizi[kucuk2]->deger;
        dizi[kucuk1]->harf=127;
        dizi[kucuk1]->sol=dizi[kucuk2];
        dizi[kucuk1]->sag=temp;
        dizi[kucuk2]->deger=-1;
        altDugum--;
    }

    *tree = dizi[kucuk1];

    return;
}

// tabloyu her harf için bitlerle olusturur
//binary için +1 de sola 0, +2 de saga 1 yaz
void tablo_olustur(int codeTable[], Dugum *tree, int Code)
{
    if (tree->harf<30)
        codeTable[(int)tree->harf] = Code;
    else
    {
        tablo_olustur(codeTable, tree->sol, Code*10+1);
        tablo_olustur(codeTable, tree->sag, Code*10+2);
    }

    return;
}

void sikistirilmis_dosya(FILE *girdi, FILE *cikti, int tablo[])
{
    int boyut, k, kalan_bitler = 8;
    int ilkhal_Bit= 0, sikistirilmis_Bit = 0;
    char a, bit, n = 0;

    while ((a=fgetc(girdi))!=10)
    {
        ilkhal_Bit++;

        if (a==32)
        {
            boyut = len(tablo[29]);
            k = tablo[29];
        }
        else
        {
            boyut=len(tablo[a-97]);
            k = tablo[a-97];
        }

        while (boyut>0)
        {
            sikistirilmis_Bit++;

            bit = k % 10 - 1;
            k /= 10;
            n = n | bit;

            kalan_bitler--;
            boyut--;

            if (kalan_bitler==0)
            {
                fputc(n,cikti);
                n = 0;
                kalan_bitler = 8;
            }
            n = n << 1;
        }

    }

    if (kalan_bitler!=8)
    {
        n = n << (kalan_bitler-1);
        fputc(n,cikti);
    }

    fprintf(stderr,"Sikistirmadan onceki bit degeri = %d \n",ilkhal_Bit*8);
    fprintf(stderr,"Sikistirilmis halin bit degeri = %d \n",sikistirilmis_Bit);
    fprintf(stderr,"Sikistirilma orani: %.2f%% \n",((float)sikistirilmis_Bit/(ilkhal_Bit*8))*100);

    return;
}

int main()
{
    char* ana_metin;
    struct token* encoded_text;

    int encoded_size;
    int tokensayisi;

    int uzunluk;
    int ilkboyut;

    Dugum *tree;
    int tablo[30], tablo2[30];
    FILE *metin, *cikti;
    int i;


    ana_metin = dosya_okuma("metin.txt", &uzunluk);
    ilkboyut = uzunluk;

    encoded_text = encode(ana_metin, uzunluk, &tokensayisi);
    dosya_yaz_tokens("lz77_encoded.txt", encoded_text, tokensayisi);

    encoded_size = tokensayisi * sizeof(struct token);

    printf("\n-------LZ77 Algoritmasi ile sikistirma--------\n");

    printf("Sikistirma oncesi boyut: %d\n Sikistirma boyut: %d\n", ilkboyut, encoded_size);
    printf("Sikistirilma orani: %.02f%\n", (float)(ilkboyut - encoded_size) / (float)ilkboyut * 100.0);

    metin = fopen("metin.txt", "r");
    cikti = fopen("huffman_output.txt","w");

    printf("\n-------Huffman Kodlamasi ile sikistirma--------\n");

    olustur_HuffmanTree(&tree);
    tablo_olustur(tablo, tree, 0);
    sikistirilmis_dosya(metin,cikti,tablo2);


    return 0;
}
