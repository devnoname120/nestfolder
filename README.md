# nestfolder 1.0.1

English: [README.en.md](README.en.md)

PS Vita ana ekranında (LiveArea) **klasör içinde klasör** kullanmayı sağlayan taiHEN eklentisi.

> Yalnızca **3.65 retail** firmware'de denendi. SceShell farklıysa eklenti hiçbir kancayı takmaz ve ana ekrana dokunmaz.

## Özellikler

- **İç içe klasör:** bir klasörün içindeki klasörü açabilirsin. En fazla **2 seviye** var: üst klasör ve onun içinde bir klasör. Daha derindeki bir klasöre dokunmak yoksayılır.
- **Geri dönüş:** iç klasördeyken O tuşu ya da boşluğa dokunmak üst klasöre döner.
- **Klasörü klasöre koyma:** ana menüde, düzenleme modunda bir klasörü başka bir klasörün üstüne bırakabilirsin. titleId'si boş klasörler (eklentiden önce oluşturulmuş ve kurulum uygulamasıyla titleId verilmemiş) bırakılamaz, yerine döner.
- **Klasör içinden alt klasöre uygulama atma:** Test A'nın içindeyken bir uygulamayı Test B'nin üstüne bırakırsan uygulama Test B'ye girer. Klasör bırakılamaz, yerine döner; böylece 3. seviye oluşmaz.
- **İç klasörü düzenleme:** iç klasörün içinde öğelerin yerini değiştirebilir, uygulama çıkarabilir, klasörün adını değiştirebilirsin. Yeni ad kalıcıdır.
- **Kurulum uygulaması:** `nestfolder_kurulum.vpk` eklentiyi kurar, günceller, kaldırır; ilk açılışta `app.db` ve `config.txt` dosyalarını bir kez yedekler.
- **Üst klasörün ikonunda iç klasör:** üst klasörün küçük resminde, diğer uygulamalar gibi iç klasörün küçültülmüş ikonu da görünür. `ux0:data/nestfolder/nosubicon` ile kapatılabilir.
- **İç klasör ikonu güncellenir:** iç klasörün içeriği değişince ikonu yeniden çizilip kaydedilir ve yeniden başlatmadan görünür. İç klasörde değişiklik yapıp üst klasöre dönünce ikon hemen yenilenir. Üst klasör açıkken iç klasörün üstüne uygulama bırakırsan yeni ikon, üst klasörü bir sonraki açışında görünür (Shell içeriği klasör kapanırken kaydeder). Üst klasörün (ana menüdeki klasör) ikonu hemen yenilenir.
- **Otomatik titleId:** yeni bir klasör oluşturduğun anda eklenti ona `FLDR00001`, `FLDR00002` gibi benzersiz bir titleId verir. Klasörü yeniden başlatmadan hemen başka bir klasöre koyabilirsin.

## Kurulum

### Kurulum uygulamasıyla (önerilen)

1. HENkaku ayarlarında **Enable Unsafe Homebrew** açık olmalı (uygulama `ur0:` altına yazar).
2. `nestfolder_kurulum.vpk` dosyasını VitaShell ile kur ve **nestfolder Kurulum** balonunu aç.
3. İlk açılışta uygulama `ur0:shell/db/app.db` ve `config.txt` dosyalarını bir kez `ux0:data/nestfolder/first_backup/` içine yedekler. Bu yedek bir daha üzerine yazılmaz; uygulamayı silsen de kalır.
4. Menüden bir seçenek seç. Önce ayrıntılı bir **onay ekranı** çıkar: işlemin ne olduğu, nelerin değişeceği ve nelerin değişmeyeceği yazar. Onay tuşu ile devam edilir, geri tuşu ile hiçbir şey yapılmadan menüye dönülür.

Uygulama Vita'nın sistem dili Türkçeyse Türkçe, değilse İngilizce açılır; ayrı bir dil ayarı yoktur. Onay ve geri tuşları sistemin ayarına uyar (çoğu modelde X onay, O geri; Japon modellerde tersi).

| Menü | Ne yapar |
|---|---|
| Kur / güncelle | `nestfolder.suprx` dosyasını `ur0:tai/` içine kopyalar, etkin `config.txt` dosyasında `*main` altına satırı ekler, titleId'si boş eski klasörlere sormadan titleId verir. |
| Eski klasörlere titleId ver | Yalnızca titleId'si boş klasörlere `FLDRnnnnn` verir. eski sürümlerde bir klasörün içine bırakılıp görünmez olan klasörler de geri gelir. |
| Kaldır | İç klasörleri ana menüdeki boş yerlere çıkarır (önce üst klasörün sayfası denenir), eklentinin klasörlere verdiği titleId'leri (`FLDR` + 5 rakam) temizler, `config.txt` satırını ve `ur0:tai/nestfolder.suprx` dosyasını siler. Uygulamaların ve başka biçimdeki titleId'lere dokunulmaz. Ana menüde yeterli boş yer yoksa hiçbir şeye dokunmaz. Yedekler ve loglar silinmez. |
| İlk yedeğe dön | `app.db` ve `config.txt` dosyalarını ilk yedekten geri yükler. İlk yedekten sonra kurulan uygulamaların balonları kaybolabilir. |
| Çıkış | Uygulamadan çıkar. |

Onaylanan işlem önce bir çalışma kopyasında hazırlanır ve ekranda ne yapılacağı listelenir. Ardından 5 saniyelik geri sayım başlar; geri tuşu ile iptal edilirse hiçbir dosya değişmez. Geri sayım bitince değişiklikler yazılır, doğrulanır ve Vita kendiliğinden yeniden başlar. Yapılacak bir değişiklik yoksa yeniden başlatma olmaz.

Ayrıntılar:

- `ux0:tai/config.txt` varsa taiHEN onu kullanır; satır oraya eklenir, `ur0:tai/config.txt` içindeki eski satır temizlenir. Yorum satırlarına dokunulmaz, CRLF/LF korunur.
- `app.db` yazılmadan hemen önce, çalışma kopyası alındığından beri değişip değişmediğine bakılır. Shell bu arada dosyayı değiştirdiyse yazma yapılmaz, işlemi tekrar çalıştırman istenir.
- Shell o an `app.db`'ye yazıyorsa (açık bir SQLite günlüğü varsa) uygulama bekler, yazma bitmezse işlem yapmaz.
- Yazılan dosya okunup karşılaştırılır; tutmazsa işlem öncesi `app.db` geri yazılır ve Vita yeniden başlatılmaz.
- Çalışma dosyaları `ux0:data/nestfolder/installer/` içindedir (`orig.db`: son işlemden önceki `app.db`).
- Önceki test sürümlerinin `ilk_yedek/` ve `kurulum/` klasörleri ilk açılışta kendiliğinden `first_backup/` ve `installer/` adına taşınır; mevcut ilk yedek korunur.

### Elle

1. `nestfolder.suprx` dosyasını `ur0:tai/` klasörüne kopyala.
2. `ur0:tai/config.txt` dosyasında `*main` başlığının altına şu satırı ekle:
   ```
   *main
   ur0:tai/nestfolder.suprx
   ```
3. Vita'yı yeniden başlat.

Elle kurulumdan önce `ur0:shell/db/app.db` dosyasını bir kez yedekle; eklenti kendisi yedek almaz.

Eklenti, ana ekranın kurulduğunu kendisi algılayıp yaklaşık 1 saniye sonra devreye girer; sabit bir bekleme yoktur.

## Bilinen sınırlar

| Durum | Ne oluyor | Geçici çözüm |
|---|---|---|
| İç klasörden dışarı uygulama taşımak | Uygulama üst klasöre değil **ana menüye** gider (çökme yok). | Uygulamayı ana menüden üst klasöre tekrar sürükle. |
| 2.6'dan önce oluşturulmuş klasörler | titleId'leri boş olabilir. Böyle bir klasör başka bir klasöre bırakılamaz, yerine döner. | Kurulum uygulamasında **Eski klasörlere titleId ver** (Kur / güncelle bunu zaten yapar). |

## Ayar dosyaları

Hepsi boş bir dosya ya da klasör olabilir. Değişiklik yeniden başlatınca geçerli olur.

| Yol | Etkisi |
|---|---|
| `ux0:data/nestfolder/disable` | Eklenti hiçbir şey yapmaz. |
| `ux0:data/nestfolder/nosubicon` | Üst klasörün ikonunda iç klasörün ikonunu çizmez (eski görünüm). |
| `ux0:data/nestfolder/delay20` | Açılışta eski davranış: algılama yerine sabit 20 saniye bekler. Erken devreye girmek sorun çıkarırsa kullan. |
| `ux0:data/nestfolder/snapshot` | İç klasör açılışında eski (2.7) davranış. Yalnızca iç klasör açılışında sorun çıkarsa kullan; bu modda iç klasörü düzenlemek çökmeye yol açar. |

## Log

Eklenti `ux0:data/nestfolder/log.txt` dosyasına yalnızca kısa bilgiler yazar. Her açılışta yeni bir log başlar; bir önceki açılışın logu `log.prev.txt` olarak saklanır. Yani dosyalar büyümez, bir çökmeden sonra yeniden başlattığında çökmenin olduğu oturumun logu `log.prev.txt` içinde kalır.

Log satırları İngilizcedir:

- `[nest] nestfolder 1.0.1`: eklenti yüklendi.
- `[nest] Shell ready, active after N ms`: ana ekran algılandı, eklenti açılıştan N ms sonra devreye girdi.
- `[nest] hooks: folder=ok list=ok drop=ok icon=ok refresh=ok subicon=ok titleId=ok`: hangi özelliklerin devrede olduğu. `-` görünen özellik devre dışıdır.
- `[tid] new folder -> FLDRnnnnn`: yeni klasöre atanan titleId.
- Hata durumunda, örneğin imza eşleşmezse ya da SceShell 3.65 değilse, nedeni.

Sorun bildirirken `log.txt` ve sorun önceki oturumda olduysa `log.prev.txt` dosyasını gönder.

## Yedek

Eklenti açılışta `app.db` yedeği **almaz**. Yedeği kurulum uygulaması alır:

- `ux0:data/nestfolder/first_backup/`: uygulamanın ilk açılışındaki `app.db`, `config_ur0.txt`, varsa `config_ux0.txt` (`done.txt`: yedeğin tamamlandığını gösterir). Bir kez alınır, üzerine yazılmaz.
- `ux0:data/nestfolder/installer/orig.db`: son işlemden hemen önceki `app.db`.

Eski test sürümlerinin aldığı `ux0:data/nestfolder/backup/app.db.1..3` dosyaları silinmez, istersen elle silebilirsin.

## Kurtarma

- Açılışta sorun olursa Vita'yı **L tuşu basılı** açarak eklentileri devre dışı bırak, sonra `config.txt`'den satırı kaldır.
- Ana ekran düzeni bozulursa kurulum uygulamasında **İlk yedeğe dön**'ü seç. Uygulama açılmıyorsa VitaShell ile `ux0:data/nestfolder/first_backup/app.db` (ya da son işlemden önceki hal için `ux0:data/nestfolder/installer/orig.db`) dosyasını `ur0:shell/db/` içine **`app.db` adıyla** kopyala ve yeniden başlat.
- Eklenti kaldırılmadan iç içe klasöre dokunmak Shell'i kilitler; eklentiyi kaldırmak için kurulum uygulamasının **Kaldır** seçeneğini kullan, o iç klasörleri önce ana menüye çıkarır.

## Derleme

[VitaSDK](https://vitasdk.org) gerekir.

```
mkdir build && cd build
cmake ..
make
```

`build/` içinde `nestfolder.suprx` ve eklentiyi içine gömen `nestfolder_kurulum.vpk` oluşur.

### Windows (WSL)

1. PowerShell'i yönetici olarak açıp `wsl --install -d Ubuntu` çalıştır, bilgisayarı yeniden başlat.
2. Ubuntu'da VitaSDK'yı kur:
   ```bash
   sudo apt update && sudo apt install -y make git cmake python3 curl unzip bzip2 xz-utils
   echo 'export VITASDK=/usr/local/vitasdk' >> ~/.bashrc
   echo 'export PATH=$VITASDK/bin:$PATH' >> ~/.bashrc
   source ~/.bashrc
   git clone https://github.com/vitasdk/vdpm && cd vdpm && ./bootstrap-vitasdk.sh && ./install-all.sh && cd ~
   ```
3. Projeyi Linux tarafına aç ve derle (`KULLANICI` yerine Windows kullanıcı adını yaz):
   ```bash
   unzip -o /mnt/c/Users/KULLANICI/Downloads/nestfolder_1.0.0.zip -d ~/nestfolder
   cd ~/nestfolder && rm -rf build && mkdir build && cd build && cmake .. && make
   cp nestfolder_kurulum.vpk nestfolder.suprx /mnt/c/Users/KULLANICI/Downloads/
   ```

### Kurulum uygulamasının kaynakları (`installer/`)

| Dosya | İçerik |
|---|---|
| `src/main.c` | Ekran, menü, dosya işlemleri, geri sayım, yeniden başlatma (yalnızca Vita) |
| `src/db.c` | app.db işlemleri: titleId verme, iç klasörleri çıkarma, durum. Vita'nın sistem SQLite modülünü (SceSqlite) kullanır; günlük bellekte tutulur. |
| `src/vfs_sceio.c` | SQLite için `sceIo` tabanlı dosya katmanı. SceSqlite'ın kendi katmanı yazarken yan dosya (`-journal`) açamadığı için gerekli. |
| `src/cfg.c` | `config.txt` düzenleme |
| `src/lang.h` | İki dil: `TR("English", "Türkçe")`; sistem dili Türkçeyse Türkçe |
| `src/font.h` | Türkçe karakterli 12x24 ekran fontu (`tools/make_font.py`, DejaVu Sans Mono) |
| `sce_sys/` | LiveArea resimleri (`tools/make_livearea.py`) |
| `test/` | PC testleri: `db.c`/`cfg.c` birim testi ve Vita API'lerini taklit eden katmanla tüm uygulamanın simülasyonu |

PC testleri (Linux, `libsqlite3-dev` gerekir):

```
cd installer/test
gcc -o test_main test_main.c ../src/db.c ../src/cfg.c -lsqlite3
gcc -idirafter $VITASDK/arm-vita-eabi/include -Wl,--wrap=sqlite3_open_v2 \
    -o sim ../src/main.c ../src/db.c ../src/cfg.c vita_shim.c -lsqlite3
python3 make_db.py t.db && ./test_main stats t.db
```

## Nasıl çalışır (kısaca)

- **İç içe açma:** Shell, klasör içinde klasör açarken önce açık klasörü kapatıp öğelerini yok eder; bu, seçilen öğeyi de sildiği için kilitlenir. Eklenti açma/kapatma fonksiyonlarına kanca atar ve alt klasörü, Shell'e verdiği kendi kopyasından açar. Kopya gerçek klasör kaydını gösterir ve kaydın silinmemesi için onun referans sayacını artırır. Kopyanın içerik kabı, ad değiştirme kaydının ihtiyaç duyduğu sayfa kimliğini döndürür. Kopya, klasör adının kendi kopyasını taşır: 2.26'ya kadar gerçek öğenin metin belleğini paylaşıyordu; gerçek öğe silinince bu bellek serbest kalıyor ve ad değiştirme onu ikinci kez serbest bırakıp ara sıra kilitlenmeye yol açıyordu.
- **İkon yenileme (1.0.1):** Shell klasör resmini `"<içerik sayfa no>:<rowid>"` anahtarıyla bir resim önbelleğinde tutar. Çizim işi bitince (`0xB3146`) öğeyi yalnızca ana menü sayfalarında arar (`0xAFE82` → her sayfa için `0xD3996`) ve bulursa `0xCF7DE(öğe, 1)` ile önbellekten atıp yeniden yükler. İç klasörün öğesi açık klasörde durduğu için Shell onu bulamaz. Eklenti, çizimini istediği klasörlerin rowid'ini izler; iç klasörün öğesi üst klasör açılırken kurucusunda (`0xCFB92`) resmini `0xCFD50`'den `0xCF7DE(öğe, 0)` ile önbellekten aldığı anda, çizim bitmişse bir kez `0xCF7DE(öğe, 1)` ile zorla yeniler (öğenin bu anda widget'ı ve `+0x40` weak_ptr'ı yoktur; çağrı yeri ile doğrulanır). Ardından üst klasör bir kez daha çizdirilir ki onun küçük resmine iç klasörün güncel hali girsin. `0xCF7DE(öğe, 1)` önce resme kendi referansını alıp öğenin resim alanını sıfırlar, sonra kaydı önbellekten atar; 2.23'teki kilitlenme bu sıranın izlenmemesinden geliyordu.
- **Üst klasör ikonunda iç klasör (1.0.1):** küçük resim işi (`0xB7FE0`) alt öğelerin resimlerini satırın `iconPath` yolundan `0xBD3A6(yol, &resim, 0, 0, 0)` ile önbelleğe alır, yüklenmelerini bekler ve çizer. Klasör satırlarının yolu yoktur. Eklenti `0xBD3A6`'ya kanca atar; dönüş adresi bu döngüyü (`0xB8234`) gösteriyorsa alt kaydı çağıranın yığınından bulur (`[sp+0x20]` = pos, `[sp+0x44]` = liste) ve kayıt bir klasörse isteği klasörlerin anahtarı (`"%d:%lld"` = içerik sayfası:rowid) ve yükleyicisiyle (`0xCF7DE`'deki parametrelerin aynısı, yükleyici `0xD2160`) yapar. Resim önbellekte ana menü ve klasör içindeki öğeyle ortaktır.
- **İç klasör ikonu:** Shell klasör ikonunun küçük resmini arka planda, klasörün içeriğini app.db'den okuyarak çizer ve app.db'ye kaydeder. Klasör içeriği app.db'ye ise klasör kapanırken yazılır. Normal Vita'da klasör içinde klasör olmadığı için Shell iç klasör için çizim istemez. Eklenti bu isteği içerik yazıldıktan sonra gönderir: iç klasör kapandıktan hemen sonra, üst klasörün içinden alt klasöre uygulama atıldıysa da üst klasör kapandıktan sonra. Ekrandaki resim Shell'in bellekteki önbelleğinden geldiği için yeni resim yeniden başlatınca görünür.
- **Bırakma kontrolü:** Shell'in "sürüklenen öğe klasör mü?" sorusuna eklenti cevap verir. Ana menüde her şey serbesttir, yalnızca titleId'si boş klasörler başka bir klasöre bırakılamaz; bir klasörün içindeyken yalnızca uygulamalar alt klasöre bırakılabilir.
- **titleId:** Shell yeni klasör kaydını hazırlayıp kaydetme işine gönderirken eklenti araya girer ve boş titleId alanına `FLDRnnnnn` yazar. Hem app.db satırı hem ekrandaki ikon bu kayıttan oluşur. Kullanılan numaralar açılışta app.db dosyası taranarak bulunur, SQLite kullanılmaz.
- **Güvenlik:** her kanca noktasındaki kod önce 3.65 retail SceShell ile karşılaştırılır. Eşleşmezse o özellik devreye alınmaz.

## Değişiklikler

- **1.0.1:** Kaldır, eklentinin klasörlere verdiği titleId'leri de temizliyor (iç klasörler ana menüye çıkarıldıktan sonra). İç klasörün ikonu yeniden başlatmadan yenileniyor; üst klasörün ikonunda iç klasörün küçültülmüş ikonu görünüyor (`nosubicon` ile kapatılabilir). Kurulum uygulaması 1.0.1 yalnızca yeni eklentiyi içeriyor.
- **1.0.0 (eklenti ve kurulum uygulaması):** İlk sürüm numarası. Kurulum uygulaması iki dilli (sistem dili Türkçeyse Türkçe, değilse İngilizce), onay/geri tuşları sistem ayarına uyuyor, her seçenekten önce ayrıntılı onay ekranı var. Veri klasörleri İngilizce: `first_backup/`, `installer/` (eskileri otomatik taşınır). Eklentinin log satırları İngilizce.

Geliştirme sürümleri (cihaz testleri):

- **Kurulum 1.1:** Vita'da titleId verme ve kaldırma "unable to open database file" hatası veriyordu: SceSqlite yazmaya başlarken günlük dosyası açamıyordu. Dosyalar artık kendi `sceIo` katmanımızla açılıyor, günlük bellekte tutuluyor.
- **2.29:** Eklentideki otomatik `app.db` yedeği kaldırıldı (`nobackup` ayarı da). Yedeği artık kurulum uygulaması bir kez alıyor. Kurulum uygulaması eklendi (`installer/`, `nestfolder_kurulum.vpk`). Derleme: `vita_create_self` bayraklarının bir hedeften diğerine sızması düzeltildi.
- **2.28:** titleId'si boş klasör başka bir klasöre bırakılamıyor.
- **2.27:** İç klasörün adını değiştirirken ara sıra oluşan kilitlenme düzeltildi.

## Lisans ve sorumluluk

- Proje kodu `LICENSE` dosyasındaki lisansla dağıtılır. Üçüncü taraf parçalar (font vb.) için `THIRD_PARTY_NOTICES.md`.
- Bu proje bir yapay zekâ asistanı (Anthropic'in Claude'u) ile birlikte yazıldı; yön, kararlar ve her sürümün cihaz testi proje sahibine ait.
- Sony ile bağlantısı yoktur. PlayStation, PS Vita ve LiveArea, Sony Interactive Entertainment'ın ticari markalarıdır.
- Kullanım riski kullanıcıya aittir: eklenti ana ekranın çalışmasını değiştirir, kurulum uygulaması `app.db` dosyasını düzenler.
- Depoya kişisel `app.db` ve firmware/bellek dökümleri eklenmemelidir.
