//TODO: Kommentek bővítése.
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "menu.h"
#include "jatek.h"

#define VERZIO "V1.5A"

TTF_Font *cimfont, *szovegfont, *leirasfont, *verziofont, *eredmenyfont;
typedef struct ListaElem{char listanev[100]; int listapont;struct ListaElem * kov;} ListaElem;
SDL_Rect hova = {0,0,0,0};
SDL_Rect srchang = {0,0,61,61};

int menu_kezdo (void)
{
    pont=0;
//-------------------------------------------- EGYEDI BETÛTÍPUS FÜGGVÉNY INICIALIZÁLÁS --------------------------------------------
    int TTF_init();
    if(TTF_Init()==-1)
        {fprintf(stderr, "Nem sikerult inicializalni a fontot! %s\n", TTF_GetError());
        exit(4);}
    SDL_Rect dsthang = {screen->w-screen->w*0.12, screen->w-screen->w*0.1, 0, 0 };
    szovegfont = TTF_OpenFont("ttf/szoveg.ttf", screen->w/15);
    cimfont = TTF_OpenFont("ttf/pacman.ttf", screen->w*0.1);
    leirasfont = TTF_OpenFont("ttf/times.ttf", screen->w*0.043);
    verziofont = TTF_OpenFont("ttf/times.ttf", 14);
    eredmenyfont = TTF_OpenFont("ttf/times.ttf", 35);

    TTF_SetFontOutline(szovegfont, 1);
//---------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------ HELYI VÁLTOZÓK ÉRTÉKADÁSA ------------------------------------------------------
    FILE *newgamefp,*loadgamefp;
    newgamefp=fopen("files/terkep.txt","r");
    loadgamefp=fopen("files/save.sav","r");

    char menusorok[5][12]={"New Game", "Load Game","Highscore","Manual","Exit"};  //Ezek a sorok kerülnek majd kiiratásra
    int egerx,egery,kilep;
    int i=0, besor, beoszlop;
    char c;
    SDL_Event event;

    if(!newgamefp)
    {fprintf(stderr,"Nem sikerult megnyitni a terkep.txt-t!");
        exit(6);}

//---------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------- HÁTTÉR KIRAJZOLÁSA -----------------------------------------------------------
    menu_hatter_kirajzol();
    boxRGBA(screen, 120, 396, 600, 684, 255, 255, 255, 125);  //Menü háttér doboz


    /*
        Külön függvényben történik a kirajzolás, a kód rövidítése érdekében.
        Hogy a menü pontok eltûnjenek, mindig ûjra kéne rajzolni, ez sok sor lenne.
        Ehelyett a menu_hatter_kirajzol kirajzolja a háttérképet, címet, és a szürke dobozt, a hangjelet és a logót.
    */

//---------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------- MENÜ SZÖVEG KIÍRÁSA ---------------------------------------------------------

    for(i=0;i<5;i++)
    {
        szoveg = TTF_RenderUTF8_Blended(szovegfont, menusorok[i], feher);   //Adott sorok kiírása, pozicionálása, felszabaditása
        hova.x = (720-szoveg->w)/2;                                   // creen -> w képernyõ szélességét adja meg
        hova.y = 403.2+i*60;             //Minden egyes sort 50 pixellel lejjeb ír ki
        SDL_BlitSurface(szoveg, NULL, screen, &hova);
        SDL_FreeSurface(szoveg);
    }
    SDL_Flip(screen);

//---------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------- EGÉRMOZGÁS ÉS KATTINTÁS -------------------------------------------------------

/*
  Mágikus számoknak tűnhetnek a lent következők, de nem azok. Pontos számítások következményei, csak koordinátákat nem tudtam szebben megadni.
*/

    kilep=0;
    while (!kilep)
    {
        if(zene==0)
            Mix_PauseMusic();
        else
            Mix_ResumeMusic();

        SDL_WaitEvent(&event);  //Vár egy eventet
        if(event.type==SDL_QUIT)  //X-re kattintás
        {
            kilep=1;
            return kilepni;
        }
        if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) //Bal kattintás
        {
            egerx = event.button.x;
            egery = event.button.y;
            if (egerx>237 && egerx<483 && egery>395 && egery<440)  //Új játék
            {
                for(besor=0;besor<32;besor++)
                {
                    for(beoszlop=0;beoszlop<32;beoszlop++)
                    {
                        if((c=fgetc(newgamefp))!='\n')
                            palya[besor][beoszlop]=c;
                        else if(c=fgetc(newgamefp))// az előző karakter \n volt, ezért olvastatjuk be a következőt is.
                            palya[besor][beoszlop]=c;

                    }
                }
                return Ujjatek;
                kilep=1;
            }
            if (egerx>220 && egerx<495 && egery>450 && egery<505)  //Betöltés
            {
                for(besor=0;besor<32;besor++)
                {
                    for(beoszlop=0;beoszlop<32;beoszlop++)
                    {
                        if((c=fgetc(loadgamefp))!='\n')
                            palya[besor][beoszlop]=c;
                        else if(c=fgetc(loadgamefp))// az előző karakter \n volt, ezért olvastatjuk be a következőt is.
                            palya[besor][beoszlop]=c;

                    }
                }
                fscanf(loadgamefp,"%d",&pont);
                return Ujjatek;
                kilep=1;

            }
            if (egerx>220 && egerx<495 && egery>515 && egery<570)  //Eredmények
            {
                kilep=1;
                if(menu_eredmenyek()==kilepni)
                    return kilepni;
                //HA nem megy bele, mással tér vissza, kezdődik a menü előröl.
                return menu_kezdo();

            }
            if (egerx>225 && egerx<455 && egery>575 && egery<630)  //Leiras
            {
                kilep=1;
                if(menu_leiras()==kilepni)
                    return kilepni;
                //HA nem megy bele, mással tér vissza, kezdődik a menü előröl.
                return menu_kezdo();
            }
            if (egerx>288 && egerx<432 && egery>635 && egery<685)  //Kilépés
            {
                kilep=1;
                return kilepni;
            }
            if (egerx>633 && egerx<699 && egery>648 && egery<713)  //hang
            {
                if(zene==1)
                    {
                        zene=0;
                        SDL_BlitSurface(music_off,&srchang,screen,&dsthang);
                    }
                    else if(zene==0)
                    {
                        zene=1;
                        SDL_BlitSurface(music_on,&srchang,screen,&dsthang);
                    }
            SDL_Flip(screen);
            }

        } //If vége
    }//While vége

}

void menu_hatter_kirajzol(void)
{
    SDL_FillRect(screen,NULL, 0x000000);  //Fekete képernyõ
    SDL_Flip(screen);
//------------------------------------------------------ MENÜ HÁTTÉR  -------------------------------------------------------------
    SDL_Rect src = {0,0,720,720};             //forrás kép adatai
    SDL_Rect srclogo = {0,0,100,100};
    SDL_Rect dst = {0,0, 0, 0 };              //Cél meghatározása, mindig a kép közepén lesz
    SDL_BlitSurface(hatter,&src,screen,&dst);
//---------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------- CÍMEK, SZÖVEGEK -------------------------------------------------------------
    cim = TTF_RenderUTF8_Blended(cimfont, "PACMAN", feher);  //Cím kiírása, pozicionálása
    hova.x = (720-cim->w)/2;
    hova.y = 11;
    SDL_BlitSurface(cim, NULL, screen, &hova);
    SDL_FreeSurface(cim);

    szoveg= TTF_RenderUTF8_Blended(verziofont, VERZIO, feher);
    hova.x=(720-szoveg->w)-5;
    hova.y=5;
    SDL_BlitSurface(szoveg, NULL, screen, &hova);
    SDL_FreeSurface(szoveg);

    SDL_Rect dsthang = {633.6, 648, 0, 0 };  //Hang kirajzolás poz.
    SDL_Rect dstlogo={10,620,110,720,255,255,255,0};
    SDL_BlitSurface(logo,&srclogo,screen,&dstlogo);
    if(zene==1)
        SDL_BlitSurface(music_on,&srchang,screen,&dsthang);
    if(zene==0)
        SDL_BlitSurface(music_off,&srchang,screen,&dsthang);

    SDL_Flip(screen);
}

int menu_leiras(void)
{
    menu_hatter_kirajzol();

    SDL_Event event;
    int egerx,egery,kilep,i;
    char leiras[14][50]={"- Don't get caught","- Eat Pac-Dots to gain score","- Eat Power-Dots to be able to eat ghosts",  //Leírás kiírandó szövegei
                         "- Move with ↑ ↓ ← → or W,S,A,D"," ","Score system:","  10 points","  50 points",
                         "    100 points", "    500 points", "      1000 points", "      2500 points",
                         "      5000 points","Eaten ghosts in a row: 200,400,800,1600 points"};

//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------- LEÍRÁS ----------------------------------------------------------------------
    boxRGBA(screen,70,108,650,620, 0, 0, 0, 200);  //Háttér takaró doboz
    //boxRGBA(screen,70,396,650,620, 0, 0, 0, 225);  //Háttér takaró doboz
    szoveg = TTF_RenderUTF8_Blended(szovegfont, "Back", feher);     //"Back" renderelése, pozícionálása, kiírása
        hova.x = (720-szoveg->w)/2;
        hova.y = 641;
        SDL_BlitSurface(szoveg, NULL, screen, &hova);
    for(i=0;i<5;i++)
    {
        szoveg = TTF_RenderUTF8_Blended(leirasfont, leiras[i], feher);     //Adott i-dik sor kiírása, pozicionálása
        hova.x = 100;
        hova.y = 122+i*36;
        SDL_BlitSurface(szoveg, NULL, screen, &hova);
    }
    hova.x = (720-szoveg->w)/2;
    for(i=5;i<14;i++)
    {
        szoveg = TTF_RenderUTF8_Blended(leirasfont, leiras[i], feher);     //Adott i-dik sor kiírása, pozicionálása
        hova.x = (720-szoveg->w)/2;
        hova.y = 122+i*36;
        SDL_BlitSurface(szoveg, NULL, screen, &hova);
    }
    SDL_Rect src = {1,1,20,20};                                                             //Leírás képeinek kirajzolása
    SDL_Rect dstpac =   {285,345, 0, 0 };    //Nem mágikus számok, paraméteresen adják meg a pozíciókat
    SDL_BlitSurface(pacdot,&src,screen,&dstpac);
    SDL_Rect dstpower = {285,381, 0, 0 };
    SDL_BlitSurface(powerdot,&src,screen,&dstpower);
    SDL_Rect dstch =    {285,417, 0, 0 };
    SDL_BlitSurface(cherry,&src,screen,&dstch);
    SDL_Rect dsto =     {285,455, 0, 0 };
    SDL_BlitSurface(orange,&src,screen,&dsto);
    SDL_Rect dstm =     {285,489, 0, 0 };
    SDL_BlitSurface(melon,&src,screen,&dstm);
    SDL_Rect dstb =     {285,525, 0, 0 };
    SDL_BlitSurface(bell,&src,screen,&dstb);
    SDL_Rect dstk =     {285,561, 0, 0 };
    SDL_BlitSurface(key,&src,screen,&dstk);

    SDL_FreeSurface(szoveg);
    SDL_Flip(screen);
//--------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------ HANG ÉS KILÉPÉS EVENT----------------------------------------------------------------
    SDL_Rect dsthang = {633.6, 648, 0, 0 };

    egerx=egery=kilep=0;
    while (!kilep)
    {
        if(zene==0)
            Mix_PauseMusic();
        else
            Mix_ResumeMusic();

        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            kilep=1;
            return kilepni;
        }
        if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            egerx = event.button.x;
            egery = event.button.y;
            if (egerx>288 && egerx<432 && egery> 633.6 && egery< 684)  //Vissza
            {
                kilep=1;
                return mas;
            }
            if (egerx>633 && egerx<699 && egery>648 && egery<713)  //hang
            {
                    if(zene==1)
                    {
                        zene=0;
                        SDL_BlitSurface(music_off,&srchang,screen,&dsthang);
                    }
                    else if(zene==0)
                    {
                        zene=1;
                        SDL_BlitSurface(music_on,&srchang,screen,&dsthang);
                    }
                SDL_Flip(screen);
            }
        }
    }
}
int menu_eredmenyek(void)
{
    menu_hatter_kirajzol();
    boxRGBA(screen, 120, 396, 600, 684, 255, 255, 255, 125);  //Menü háttér doboz
    SDL_Event event;
    int egerx,egery,kilep;
    int i=0;
    char kiirando[5][10]={"Highscore", "", "", "", "Back"};  //Ezek a sorok kerülnek majd kiiratásra
//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------- HIGHSCORE SZÖVEGEK KIÍRÁSA --------------------------------------------------------
    for(i=0;i<5;i++)
    {
        szoveg = TTF_RenderUTF8_Blended(szovegfont, kiirando[i], feher);  //Adott sorok kiírása, pozicionálása
        hova.x = (720-szoveg->w)/2;
        hova.y = 403+(i*60);
        SDL_BlitSurface(szoveg, NULL, screen, &hova);
        SDL_FreeSurface(szoveg);
        SDL_Flip(screen);
    }
//--------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------ HANG ÉS KILÉPÉS EVENT----------------------------------------------------------------
    SDL_Rect dsthang = {633, 648, 0, 0 };

    char hs[5][50]={"No data found.","No data found.","No data found.","No data found.","No data found."};

    FILE *highscorefp;
    highscorefp=fopen("highscore.txt","r");

    for(i=0;i<5;i++)
        fgets(hs[i],100,highscorefp);
    fclose(highscorefp);
    for(i=0;i<5;i++)
    {

        szoveg = TTF_RenderUTF8_Blended(eredmenyfont,hs[i], feher);  //Adott sorok kiírása, pozicionálása
        hova.x = (720-szoveg->w)/2;
        hova.y = 440+(i*40);
        SDL_BlitSurface(szoveg, NULL, screen, &hova);
        SDL_FreeSurface(szoveg);
        SDL_Flip(screen);
    }

    egerx=egery=kilep=0;
    while (!kilep)
    {
        if(zene==0)
            Mix_PauseMusic();
        else
            Mix_ResumeMusic();

        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            return kilepni;
        }
        if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            egerx = event.button.x;
            egery = event.button.y;
            if (egerx>288 && egerx<432 && egery> 634 && egery<684)  //Vissza
            {
                kilep=1;
                return mas;
            }
            if (egerx>633 && egerx<699 && egery>648 && egery<713)  //hang
            {
                if(zene==1)
                    {
                        zene=0;
                        SDL_BlitSurface(music_off,&srchang,screen,&dsthang);
                    }
                    else if(zene==0)
                    {
                        zene=1;
                        SDL_BlitSurface(music_on,&srchang,screen,&dsthang);
                    }
                SDL_Flip(screen);
            }
        }
    }
}
