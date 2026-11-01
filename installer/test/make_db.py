"""3.65 app.db semasina benzeyen test veritabani uretir."""
import sqlite3, sys
def make(path, full_main=False, extra_nested=False):
    db = sqlite3.connect(path)
    db.executescript("""
    CREATE TABLE tbl_appinfo_page (pageId INTEGER PRIMARY KEY, pageNo INT NOT NULL UNIQUE, themeFile TEXT, bgColor INT, texWidth INT, texHeight INT, imageWidth INT, imageHeight INT, flags INT, reserved01, reserved02, reserved03, reserved04, reserved05);
    CREATE TABLE tbl_appinfo_icon (pageId REFERENCES tbl_appinfo_page(pageId) ON DELETE RESTRICT NOT NULL, pos INT NOT NULL, iconPath TEXT, title TEXT, type INT NOT NULL, command TEXT, titleId TEXT, icon0Type INT NOT NULL, parentalLockLv INT, status INT, reserved01, reserved02, reserved03, reserved04, reserved05, PRIMARY KEY(pageId, pos));
    CREATE TABLE tbl_appinfo (titleId TEXT NOT NULL, key INT NOT NULL, val TEXT, PRIMARY KEY(titleId, key));
    CREATE TRIGGER tgr_deletePage2 AFTER DELETE ON tbl_appinfo_page BEGIN UPDATE tbl_appinfo_page SET pageNo = pageNo - 1 WHERE pageNo > old.pageNo; END;
    """)
    # ana sayfalar 0,1 ; klasor sayfalari negatif
    db.execute("INSERT INTO tbl_appinfo_page(pageId,pageNo) VALUES (100,0),(101,1),(200,-11),(201,-12),(202,-13),(203,-14)")
    def app(page,pos,tid,title):
        db.execute("INSERT INTO tbl_appinfo_icon(pageId,pos,title,type,titleId,icon0Type) VALUES (?,?,?,0,?,0)",(page,pos,title,tid))
        db.execute("INSERT INTO tbl_appinfo VALUES (?,0,?)",(tid,title))
    def folder(page,pos,tid,title,content_no):
        db.execute("INSERT INTO tbl_appinfo_icon(pageId,pos,title,type,titleId,icon0Type,reserved01,reserved05) VALUES (?,?,?,5,?,7,?,x'00')",(page,pos,title,tid,content_no))
    app(100,0,"PCSG00001","Ayarlar"); app(100,1,"VITASHELL","VitaShell")
    folder(100,2,"FLDR00001","Test A",-11)       # ana menu, Test A
    folder(200,0,"FLDR00003","Test B",-12)       # Test A icinde
    app(200,1,"PCSE00002","Oyun 1")
    folder(200,2,None,"Görünmez Ç",-13)            # Test A icinde, titleId bos (2.28 oncesi)
    app(201,0,"PCSE00003","Oyun 2")              # Test B icinde
    folder(101,0,"","Eski Klasör",-14)            # ana menu, titleId ''
    if extra_nested:
        folder(201,1,"FLDR00007","Derin",-15)    # Test B icinde (3. seviye)
        db.execute("INSERT INTO tbl_appinfo_page(pageId,pageNo) VALUES (204,-15)")
    if full_main:
        for pos in range(3,10): app(100,pos,"APPA%05d"%pos,"a%d"%pos)
        for pos in range(1,10): app(101,pos,"APPB%05d"%pos,"b%d"%pos)
    db.commit(); db.close()
if __name__ == "__main__":
    make(sys.argv[1], "full" in sys.argv, "deep" in sys.argv)
