#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Ogrenci{
    char ogrenciNo[8];
    char ad[15];
    char soyad[15];
    int kayitSirasi;
    char ogrenimTuru[3];
    struct Ogrenci *sonraki;
}liste;

//fonksiyon prototipleri
liste *mergeSort(liste *, char []);
liste *geriBirlestir(liste *, liste *, char []);
void listeBol(liste *, liste **, liste **);
void fazlaKayitSil(liste *);
liste *kayitSiraGuncelle(liste *);
void numaraAtamasi(liste *);
liste *numarayaGoreSirala(liste *);
void siniflaraDagit(liste *, clock_t *);
liste *ogrenciEkle(liste *, clock_t *);


int main()
{
    clock_t fonkSure, girdi, girdiToplam;  //clock tick sayisini okuyabilmek icin clock_t tipinde tanimlanir.
    double gecenSure;
    girdiToplam = clock();
    int a=0;
    int ogrKayit = 1;
    char giris[5][15];
    liste *bas;    //Ogrenci bilgilerini saklamak icin linked listin ilk dugumu olusturulur.
    liste *aktif;
    bas = malloc(sizeof(liste));
    bas->sonraki = NULL;
    aktif = bas;
    FILE *fp = fopen("ogrenci-kayit-bilgileri.txt", "r");    //Okumak icin dosya acilir.
    while(fgetc(fp) != EOF){    //Acilan dosyanin sonuna kadar gidilir, okunan degerler linked liste atanir.
        if(a == 0){
            a++;
            fscanf(fp, "%s %s %s %s %s", giris[0], giris[1], giris[2], giris[3], giris[4]);
        }
        else{
            aktif->sonraki = malloc(sizeof(liste));    //Okunan her ogrenci icin listede dinamik olarak yer acilir.
            aktif = aktif->sonraki;
        }
        fscanf(fp, "%s %s %s %d %s", aktif->ogrenciNo, aktif->ad, aktif->soyad, &aktif->kayitSirasi, aktif->ogrenimTuru);
    }
    aktif->sonraki = NULL;
    fclose(fp);    //Actigimiz dosya geri kapatilir.
    fonkSure = clock();
    fazlaKayitSil(bas);    //Dosyadan okunan veriye istenilen adimlar uygulanir, her adim yapilirken gecen sure hesaplanir.
    fonkSure = clock() - fonkSure;
    gecenSure = (double)(fonkSure)/CLOCKS_PER_SEC;
    printf("fazlaKayitSil()'in calismasi %lf saniye surdu.\n\n", gecenSure);
    fonkSure = clock();
    bas = kayitSiraGuncelle(bas);
    fonkSure = clock() - fonkSure;
    gecenSure = (double)(fonkSure)/CLOCKS_PER_SEC;
    printf("Kayit siralari guncellenmistir.\n\n");
    printf("kayitSiraGuncelle()'nin calismasi %lf saniye surdu.\n\n", gecenSure);
    fonkSure = clock();
    numaraAtamasi(bas);
    fonkSure = clock() - fonkSure;
    gecenSure = (double)(fonkSure)/CLOCKS_PER_SEC;
    printf("Numarasi olmayan ogrencilere numara atamasi yapilmistir.\n\n");
    printf("numaraAtamasi()'in calismasi %lf saniye surdu.\n\n", gecenSure);
    fonkSure = clock();
    bas = numarayaGoreSirala(bas);
    fonkSure = clock() - fonkSure;
    gecenSure = (double)(fonkSure)/CLOCKS_PER_SEC;
    printf("Liste ogrenci numaralarina gore siralanmistir.\n\n");
    printf("numarayaGoreSirala()'nin calismasi %lf saniye surdu.\n\n", gecenSure);
    siniflaraDagit(bas, &girdi);
    girdiToplam += girdi;
    while(ogrKayit == 1){
        girdi = clock();
        printf("\nYeni ogrenci kaydetmek istiyormusunuz?(Istiyorsaniz lutfen 1 giriniz.): ");
        scanf("%d", &ogrKayit);
        girdi = clock() - girdi;
        girdiToplam += girdi;
        if(ogrKayit == 1){
            bas = ogrenciEkle(bas, &girdi);
            girdiToplam += girdi;
        }
    }
    fp = fopen("ogrenci-kayit-bilgileri-guncel.txt", "w");    //Ogrenci listesinin guncel halini yazdirmak icin yeni bir dosya acilir.
    fprintf(fp, "%-15s %-15s %-15s %15s %15s\n", giris[0], giris[1], giris[2], giris[3], giris[4]);
    while(bas != NULL){
       fprintf(fp, "%-15s %-15s %-15s %15d %15s\n", bas->ogrenciNo, bas->ad, bas->soyad, bas->kayitSirasi, bas->ogrenimTuru);  //Liste actigimiz dosyaya yazdirilir.
        aktif = bas;
        bas = bas->sonraki;
        free(aktif);    //Linked list olustururken dinamik olarak atadigimiz bellek bosaltilir.
    }
    fclose(fp);    //Actigimiz dosya geri kapatilir.
    fonkSure = clock() - girdiToplam;  //clock() program calismaya basladigindan itibaren gecen clock tick sayisini verir.
    gecenSure = (double)(fonkSure)/CLOCKS_PER_SEC;  //calisma suresi gecen clock tick sayisi, saniye basi clock tick sayisina bolerek bulunur.
    printf("\nUygulamanin toplam calisma suresi: %lf saniye\n", gecenSure);
    return 0;
}

liste *mergeSort(liste *bas, char islem[]){
    //Liste merge sort algoritmasi kullanilarak siralanir.
    if ((bas == NULL) || (bas->sonraki == NULL)){    //Listenin uzunlugu 2'den kucuk ise aynen geri dondurulur.
        return bas;
    }
    liste *onParca;
    liste *arkaParca;
    listeBol(bas, &onParca, &arkaParca);    //Liste on ve arka parca olmak uzere 2 parcaya ayrilir.
    return geriBirlestir(mergeSort(onParca, islem), mergeSort(arkaParca, islem), islem);  //Liste rekursif olarak parcalarina ayrilir, siralanir ve geri birlestirilir.
}

liste *geriBirlestir(liste *onParca, liste *arkaParca, char islem[]){
    //Daha onceden parcalara ayrilan liste, kucukten buyuge siralanarak geri birlestirilir.
    if (onParca == NULL){  //Eger parcalardan birisi bos ise listenin sonuna diger parca atanir.
        return arkaParca;
    }
    else if (arkaParca == NULL){
        return onParca;
    }
    liste *sonuc = NULL;
    if(strcmp(islem, "kayitSirasi") == 0){   //Parcalarin ilk elemanlarindan baslayarak kiyaslama yapilir. Kucuk olan eleman alinir, sonraki eleman ile islem tekrarlanir.
        if (onParca->kayitSirasi <= arkaParca->kayitSirasi){
            sonuc = onParca;
            sonuc->sonraki = geriBirlestir(onParca->sonraki, arkaParca, islem);
        }
        else{
            sonuc = arkaParca;
            sonuc->sonraki = geriBirlestir(onParca, arkaParca->sonraki, islem);
        }
    }
    else if(strcmp(islem, "ogrenciNo") == 0){
        if (strcmp(onParca->ogrenciNo, arkaParca->ogrenciNo) <= 0){
            sonuc = onParca;
            sonuc->sonraki = geriBirlestir(onParca->sonraki, arkaParca, islem);
        }
        else{
            sonuc = arkaParca;
            sonuc->sonraki = geriBirlestir(onParca, arkaParca->sonraki, islem);
        }
    }
  return sonuc;
}

void listeBol(liste *bas, liste **onParcaRef, liste **arkaParcaRef){
    //Verilen listenin dugumlerini on ve arka olmak uzere iki parcaya boler, ve referans
                                            //parametreleri kullanarak bu iki liste geri dondurulur.
    //Eger listenin uzunlugu tek ise fazla olan dugum on parcaya gider.
    if ((bas == NULL) || (bas->sonraki == NULL)){    //Listenin uzunlugu 2'den kucuk ise aynen geri dondurulur.
        *onParcaRef = bas;
        *arkaParcaRef = NULL;
        return;
    }
    liste *slow;
    liste *fast;
    slow = bas;
    fast = bas->sonraki;
    //Fast 2'ser dugum ilerlerken, slow 1'er dugum ilerler. Boylece fast listenin sonuna ulastiginda slow ile
    while ((fast != NULL) && (fast->sonraki != NULL)){                                   //listenin ortasi bulunmus olur.
        fast = fast->sonraki->sonraki;
        slow = slow->sonraki;
    }
    *onParcaRef = bas;
    *arkaParcaRef = slow->sonraki;
    slow->sonraki = NULL;
    return;
}

void fazlaKayitSil(liste *bas){
    //Birden fazla kayit edilmis ogrencilerin fazla kayitlari silinir.
    liste *aktif, *aktif2;
    aktif = bas;
    aktif2 = aktif->sonraki;
    printf("2 defa ismi gecen ogrencilerin tekrarlari silinmistir.\n");
    printf("Isimler:\n");
    while(aktif->sonraki != NULL){    //Liste boyunca dolasilarak fazlalik olan kayitlar bulunur.
        int sayac = 0;
        while(aktif2->sonraki != NULL){
            if((strcmp(aktif->sonraki->ad, aktif2->sonraki->ad)==0)&&(strcmp(aktif->sonraki->soyad, aktif2->sonraki->soyad)==0)){
                if(sayac == 0){
                    printf("%s %s\n", aktif->sonraki->ad, aktif->sonraki->soyad);
                    sayac++;
                }
                liste *silinecek;
                silinecek = aktif2->sonraki;
                if(aktif2->sonraki->sonraki != NULL){
                    //Eger bulunan fazlalik listenin sonunda degil ise fazlaligin bulundugu dugume bakan pointer bir sonraki dugume esitlenir.
                    aktif2->sonraki = aktif2->sonraki->sonraki;
                    free(silinecek);   //Fazlaligin bulundugu bellek alani bosaltilir.
                }
                else{
                    //Eger fazlalik listenin sonunda ise fazlaligin bulundugu dugume bakan pointer NULL'a esitlenir.
                    aktif2->sonraki = NULL;
                    free(silinecek);  //Fazlaligin bulundugu bellek alani bosaltilir.
                }
            }
            else{
                aktif2 = aktif2->sonraki;
            }
        }
        aktif = aktif->sonraki;
        aktif2 = aktif->sonraki;
    }
    printf("\n");
    return;
}

liste *kayitSiraGuncelle(liste *bas){
    int i = 1;
    mergeSort(bas, "kayitSirasi");    //Liste ogrenci kayit siralarina gore siralanir.
    liste *aktif;
    liste *aktif2;
    aktif = bas;
    aktif2 = bas;
    while(aktif->sonraki != NULL){    //Ogrencilerin kayit onceliklerine gore kayit sira numaralari guncellenir.
        if(aktif->kayitSirasi != 0){
            aktif->kayitSirasi = i;
            i++;
        }
        aktif = aktif->sonraki;
    }
    if(aktif->kayitSirasi != 0){
        aktif->kayitSirasi = i;
        i++;
    }
    while(aktif2->kayitSirasi == 0){     //Kayit sira numarasi bulunmayan yeni ogrencilerin dugumleri listenin sonuna
        if(strcmp(aktif2->ogrenciNo, "-") == 0){                               //aktarilir ve kayit sira numarasi atamasi yapilir.
            aktif2->kayitSirasi = i;
            i++;
            if(aktif2 == bas){
                bas = bas->sonraki;
                aktif->sonraki = aktif2;
                aktif = aktif->sonraki;
                aktif->sonraki = NULL;
                aktif2 = bas;
            }
            else if(aktif2->sonraki != NULL){
                liste *onceki;
                onceki = bas;
                while(onceki->sonraki != aktif2){
                    onceki = onceki->sonraki;
                }
                onceki->sonraki = aktif2->sonraki;
                aktif->sonraki = aktif2;
                aktif = aktif->sonraki;
                aktif->sonraki = NULL;
                aktif2 = onceki;
            }
        }
        aktif2 = aktif2->sonraki;
    }
    return bas;
}

void numaraAtamasi(liste *bas){
    //Daha oncesinde kayit sira numaralarina gore siralanmis ogrencilere numara atamasi yapilir.
    liste *aktif;
    aktif = bas;
    int birogr=1, ikiogr=1, bugfix=1 ;
    //Eger bugfix silinirse 2. olarak tanimlanan deger ve ona bagli atamalar tamamen hatali sonuclar vermektedir.
    while(aktif != NULL){
        if(strcmp(aktif->ogrenciNo, "-") == 0){
            char num[3];
            strcpy(aktif->ogrenciNo, "17");
            if(strcmp(aktif->ogrenimTuru, "I") == 0){
                strcat(aktif->ogrenciNo, "01");
                if(birogr<10){
                    strcat(aktif->ogrenciNo, "00");
                }
                else if(birogr<100){
                    strcat(aktif->ogrenciNo, "0");
                }
                itoa(birogr, num, 10);
                strcat(aktif->ogrenciNo, num);
                birogr++;
            }
            else if(strcmp(aktif->ogrenimTuru, "II") == 0){
               strcat(aktif->ogrenciNo, "02");
                if(ikiogr<10){
                    strcat(aktif->ogrenciNo, "00");
                }
                else if(ikiogr<100){
                    strcat(aktif->ogrenciNo, "0");
                }
                itoa(ikiogr, num, 10);
                strcat(aktif->ogrenciNo, num);
                ikiogr++;
            }
        }
        else if(strncmp(aktif->ogrenciNo, "17", 2) == 0){
            if(strcmp(aktif->ogrenimTuru, "I") == 0){
                birogr++;
            }
            else if(strcmp(aktif->ogrenimTuru, "II") == 0){
                ikiogr++;
            }
        }
        aktif = aktif->sonraki;
    }
    return;
}

liste *numarayaGoreSirala(liste *bas){
    //Liste ogrenci numaralarina gore siralanir.
    bas = mergeSort(bas, "ogrenciNo");
    return bas;
}

void siniflaraDagit(liste *bas, clock_t *girdisuresi){
    //Listedeki ogrencilerin kullanici tarafindan girilen siniflara dagitimi yapilir.
    clock_t fonksure, fonksure2, fonksure3;
    double gecensure;
    fonksure = clock();
    int i, j;
    int secenek;
    int sinifSayisi, totalKapasite=0;
    int birOgretim=0, ikiOgretim=0;
    int *sinif;
    liste *aktif;
    aktif = bas;
    fonksure2 = clock();
    printf("Sinif sayisini giriniz: ");
    scanf("%d", &sinifSayisi);
    sinif = malloc(sinifSayisi * sizeof(int));  //Sinif sayisina gore dinamik olarak dizi acilir. Kapasite atamalari yapilir.
    printf("Sinif kapasitelerini giriniz:\n");
    for(i=0; i<sinifSayisi; i++){
        printf("%d. Sinif: ", i+1);
        scanf("%d", &sinif[i]);
        totalKapasite += sinif[i];
    }
    fonksure2 = clock() - fonksure2;
    while(aktif != NULL){                       //Birinci ve ikinci ogretimde kacar ogrenci oldugu hesaplanir.
        if(strcmp(aktif->ogrenimTuru, "I")==0){
            birOgretim++;
        }
        else if(strcmp(aktif->ogrenimTuru, "II")==0){
            ikiOgretim++;
        }
        aktif = aktif->sonraki;
    }
    printf("Dersi alan ogrenci sayisi:\n");
    printf("1. Ogretim: %d\n2. Ogretim: %d\n", birOgretim, ikiOgretim);
    printf("Girilen siniflarin toplam kapasitesi: %d\n", totalKapasite);
    while((totalKapasite<birOgretim) || (totalKapasite<ikiOgretim)){  //Eger siniflarin toplam kapasitesi ogrenci sayisindan az ise, fazladan sinif girisi alinir.
        printf("\nToplam sinif kapasitesi yetersiz. Lutfen ek sinif giriniz.\n");
        sinifSayisi++;
        sinif = realloc(sinif, sinifSayisi * sizeof(int));  //Dinamik olarak sinif sayisi arttirilir.
        fonksure3 = clock();
        printf("%d. Sinif: ", sinifSayisi);
        scanf("%d", &sinif[sinifSayisi-1]);
        fonksure3 = clock() - fonksure3;
        fonksure2 += fonksure3;
        totalKapasite += sinif[sinifSayisi-1];
   }
    FILE *fp;
    aktif = bas;
    int sinifFinal[sinifSayisi];
    for(i=0; i<sinifSayisi; i++){
        sinifFinal[i] = 0;
    }
    fonksure3 = clock();
    printf("\nEsit sinif dagilimi icin 1, en az sinif dagilimi icin 2 giriniz: ");
    scanf("%d", &secenek);
    fonksure3 = clock() - fonksure3;
    fonksure2 += fonksure3;
    if(secenek == 1){                          //Kullanicinin secimine gore siniflara atama gerceklestirilir.
        int esitOgrSayisi, a, bosSinifSayisi;
        printf("\n1. Ogretim: ");              //1. Ogretimler icin sinif atamasi yapilir.
        bosSinifSayisi = sinifSayisi;
        esitOgrSayisi = birOgretim/bosSinifSayisi;
        do{
            a = 0;
            for(i=0;i<sinifSayisi;i++){
                if((sinif[i] < esitOgrSayisi) && (sinifFinal[i] == 0)){ //Esit ogrenci atamasi icin ogrenci sayisi/sinif sayisi ortalamasi altinda kalan
                    sinifFinal[i] = sinif[i];                                                    //siniflara ilk atama yapilir devamina yeni ortalama atanir.
                    birOgretim -= sinif[i];
                    a++;
                    bosSinifSayisi--;
                }
            }
            esitOgrSayisi = birOgretim/bosSinifSayisi;
        }while(a != 0);
        birOgretim -= bosSinifSayisi*(birOgretim/bosSinifSayisi);
        for(i=0; i<sinifSayisi; i++){
            if(sinifFinal[i] == 0){
                sinifFinal[i] = esitOgrSayisi;
                if(birOgretim != 0){
                    sinifFinal[i]++;
                    birOgretim--;
                }
            }
        }
        for(i=0; i<sinifSayisi; i++){  //Sinif sayisina gore dinamik olarak dosyalar acilir, ogrenciler dosyaya yazdirilir.
            char dosyaAdi[20];
            char sinifNo[3];
            strcpy(dosyaAdi, "sinif");
            itoa(i+1, sinifNo , 10);
            strcat(dosyaAdi, sinifNo);
            strcat(dosyaAdi, "ogr1.txt");
            fp = fopen(dosyaAdi, "w");
            for(j=0; j<sinifFinal[i]; j++){
                while(strcmp(aktif->ogrenimTuru, "II") == 0){
                    aktif = aktif->sonraki;
                }
                fprintf(fp, "%-15s %-15s %-15s %15d %15s\n", aktif->ogrenciNo, aktif->ad, aktif->soyad, aktif->kayitSirasi, aktif->ogrenimTuru);
                aktif = aktif->sonraki;
            }
            fclose(fp);
            printf("%d  ", sinifFinal[i]);
        }

        for(i=0; i<sinifSayisi; i++){
            sinifFinal[i] = 0;
        }
        aktif = bas;

        printf("\n2. Ogretim: ");   //2. Ogretimler icinde yukaridaki islemler tekrarlanir.
        bosSinifSayisi = sinifSayisi;
        esitOgrSayisi = ikiOgretim/bosSinifSayisi;
        do{
           a = 0;
            for(i=0; i<sinifSayisi; i++){
                if((sinif[i] < esitOgrSayisi) && (sinifFinal[i] == 0)){
                    sinifFinal[i] = sinif[i];
                    ikiOgretim -= sinif[i];
                    a++;
                    bosSinifSayisi--;
                }
            }
            esitOgrSayisi = ikiOgretim/bosSinifSayisi;
        }while(a != 0);
        ikiOgretim -= bosSinifSayisi*(ikiOgretim/bosSinifSayisi);
        for(i=0; i<sinifSayisi; i++){
            if(sinifFinal[i] == 0){
                sinifFinal[i] = esitOgrSayisi;
                if(ikiOgretim != 0){
                    sinifFinal[i]++;
                    ikiOgretim--;
                }
            }
        }
        for(i=0; i<sinifSayisi; i++){    //Sinif sayisina gore dinamik olarak dosyalar acilir, ogrenciler dosyaya yazdirilir.
            char dosyaAdi[20];
            char sinifNo[3];
            strcpy(dosyaAdi, "sinif");
            itoa(i+1, sinifNo , 10);
            strcat(dosyaAdi, sinifNo);
            strcat(dosyaAdi, "ogr2.txt");
            fp = fopen(dosyaAdi, "w");
            for(j=0; j<sinifFinal[i]; j++){
                while(strcmp(aktif->ogrenimTuru, "I") == 0){
                    aktif = aktif->sonraki;
                }
                fprintf(fp, "%-15s %-15s %-15s %15d %15s\n", aktif->ogrenciNo, aktif->ad, aktif->soyad, aktif->kayitSirasi, aktif->ogrenimTuru);
                aktif = aktif->sonraki;
            }
            fclose(fp);
            printf("%d  ", sinifFinal[i]);
        }
    }
    else if(secenek == 2){
        int max, maxK;
        printf("\n1. Ogretim: ");    //1. Ogretimler icin sinif atamasi yapilir.
        for(i=0; i<sinifSayisi; i++){
            max=0;
            for(j=0; j<sinifSayisi; j++){
                if((max < sinif[j]) && (sinifFinal[j] == 0)){
                    max = sinif[j];
                    maxK = j;
                }                      //En az sinif dagilimi icin ogrenci kapasitesi en buyuk olan siniflardan baslayarak ogrenci atamasi yapilir.
            }
            if(birOgretim >= max){
                sinifFinal[maxK] = max;
                birOgretim -= max;
            }
            else{
               sinifFinal[maxK] = birOgretim;
                birOgretim = 0;
            }
        }
        for(i=0; i<sinifSayisi; i++){      //Sinif sayisina gore dinamik olarak dosyalar acilir, ogrenciler dosyaya yazdirilir.
            char dosyaAdi[20];
            char sinifNo[3];
            strcpy(dosyaAdi, "sinif");
            itoa(i+1, sinifNo , 10);
            strcat(dosyaAdi, sinifNo);
            strcat(dosyaAdi, "ogr1.txt");
            fp = fopen(dosyaAdi, "w");
            for(j=0; j<sinifFinal[i]; j++){
                while(strcmp(aktif->ogrenimTuru, "II") == 0){
                    aktif = aktif->sonraki;
                }
                fprintf(fp, "%-15s %-15s %-15s %15d %15s\n", aktif->ogrenciNo, aktif->ad, aktif->soyad, aktif->kayitSirasi, aktif->ogrenimTuru);
                aktif = aktif->sonraki;
            }
            fclose(fp);
            printf("%d  ", sinifFinal[i]);
        }

        for(i=0; i<sinifSayisi; i++){
            sinifFinal[i] = 0;
        }
        aktif = bas;

        printf("\n2. Ogretim: ");     //2. Ogretimler icinde yukaridaki islemler tekrarlanir.
        for(i=0; i<sinifSayisi; i++){
            max=0;
            for(j=0; j<sinifSayisi; j++){
                if((max < sinif[j]) && (sinifFinal[j] == 0)){
                    max = sinif[j];
                    maxK = j;
                }
            }
            if(ikiOgretim >= max){
            sinifFinal[maxK] = max;
            ikiOgretim -= max;
            }
            else{
                sinifFinal[maxK] = ikiOgretim;
                ikiOgretim = 0;
            }
        }
        for(i=0; i<sinifSayisi; i++){       //Sinif sayisina gore dinamik olarak dosyalar acilir, ogrenciler dosyaya yazdirilir.
            char dosyaAdi[20];
            char sinifNo[3];
            strcpy(dosyaAdi, "sinif");
            itoa(i+1, sinifNo , 10);
            strcat(dosyaAdi, sinifNo);
            strcat(dosyaAdi, "ogr2.txt");
            fp = fopen(dosyaAdi, "w");
            for(j=0; j<sinifFinal[i]; j++){
                while(strcmp(aktif->ogrenimTuru, "I") == 0){
                    aktif = aktif->sonraki;
                }
                fprintf(fp, "%-15s %-15s %-15s %15d %15s\n", aktif->ogrenciNo, aktif->ad, aktif->soyad, aktif->kayitSirasi, aktif->ogrenimTuru);
                aktif = aktif->sonraki;
            }
            fclose(fp);
            printf("%d  ", sinifFinal[i]);
        }
    }
    free(sinif);   //Dinamik olarak actigimiz sinif dizisinin bellekte kapladigi alan bosaltilir.
    printf("\n\nTum ogrenciler yerlestirilebilmistir.\n\n");
    *girdisuresi = fonksure2;
    fonksure = clock() - fonksure - fonksure2;
    gecensure = (double)(fonksure)/CLOCKS_PER_SEC;   //Fonksiyonun calisma suresi hesaplanir.
    printf("siniflaraDagit()'in calismasi %lf saniye surdu.\n", gecensure);
    return;
}

liste *ogrenciEkle(liste *bas, clock_t *girdisuresi){
    clock_t fonksure, fonksure2;
    double gecensure;
    fonksure = clock();
    char ogrNo[8], ogrAd[15], ogrSoyad[15], ogrOgrenimTuru[3];
    liste *aktif2;
    aktif2 = bas;
    fonksure2 = clock();
    printf("Ogrencinin numarasini giriniz: ");
    scanf("%s", ogrNo);
    printf("Ogrencinin adini giriniz: ");
    scanf("%s", ogrAd);
    printf("Ogrencinin soyadini giriniz: ");
    scanf("%s", ogrSoyad);
    printf("Ogrencinin ogrenim turunu giriniz: ");
    scanf("%s", ogrOgrenimTuru);
    fonksure2 = clock() - fonksure2;
    while(aktif2 != NULL){          //Girilen ogrenci listede daha once var mi kontrol edilir.
        if((strcmp(ogrAd, aktif2->ad) == 0) && (strcmp(ogrSoyad, aktif2->soyad) == 0)){
            break;
        }
        aktif2 = aktif2->sonraki;
    }
    if(aktif2 != NULL){  //Var ise ogrenci kayidi yapilmadan cikilir.
        printf("Girmek istediginiz ogrenci listede bulunmaktadir. Isleminiz iptal edilmistir.\n\n");
    }
    else{            //Yok ise ogrenci listenin sonuna eklenir, bilgileri atanir.
        liste *aktif;
        aktif = bas;
        while(aktif->sonraki != NULL){
            aktif = aktif->sonraki;
        }
        aktif->sonraki = malloc(sizeof(liste));
        aktif = aktif->sonraki;
        aktif->sonraki = NULL;
        strcpy( aktif->ogrenciNo, ogrNo );
        strcpy( aktif->ad, ogrAd );
        strcpy( aktif->soyad, ogrSoyad );
        aktif->kayitSirasi = 0;
        strcpy( aktif->ogrenimTuru, ogrOgrenimTuru );
        printf("Girmek istediginiz ogrenci listeye eklenmistir.\n\n");
    }
    bas = kayitSiraGuncelle(bas);  //Ogrenci kayit edildikten sonra listeler guncellenir.
    numaraAtamasi(bas);
    bas = numarayaGoreSirala(bas);
    *girdisuresi = fonksure2;
    fonksure = clock() - fonksure - fonksure2;
    gecensure = (double)(fonksure)/CLOCKS_PER_SEC;  //Fonksiyonun calisma suresi hesaplanir.
    printf("ogrenciEkle()'nin calismasi %lf saniye surdu.\n", gecensure);
    return bas;
}
