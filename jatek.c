//TODO: Kód rendezése
//TODO: Kommentezés
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <time.h>
#include "menu.h"
#include "jatek.h"

int konami=0/*cheat*/,blinkyporg=0/*Blinky animáció*/,pacporg=0/*Pac animáció*/,egerx,egery,voltmentes=0,nyeresvizsgalo,pont=0, pontrender=0;

char palya[32][32], temp='X',pontkiir[50];/*Szellem pontjának lementéséhez kell menü és játék között*/
const double meret=0.3125;
typedef enum iranyenum{fel,jobbra,le,balra,allo} iranyenum;
typedef enum cheat{alap,fel1,fel2,le1,le2,bal,jobb,bal2,jobb2,bvolt,avolt} cheat; //Cheat állapotgépe
iranyenum jelenlegi, kovetkezo,blinkyirany,blinkykovirany;
cheat cheatall=alap;

TTF_Font  *szovegfont, *pontfont;
SDL_Rect jatekhova = {0,0,0,0};
SDL_Color jatekfeher = {255,255,255}, jatekfekete = {0,0,0};
SDL_TimerID id, extraid/*Extra pont generálás*/;

typedef struct pozicio{int x,y,sebx,seby,csuszx,csuszy;}pozicio;
pozicio koordinata,terkeppoz,blinkypoz,blinkykoordpoz;

Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    pontrender++;
    if(pontrender>8)
        pontrender=0;
    return ms;
}

Uint32 extraidozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_PEEKEVENT;
    SDL_PushEvent(&ev);
    pontrender++;
    if(pontrender>8)
        pontrender=0;
    return ms;
}

int jatek_indit(void)
{
    pontfont = TTF_OpenFont("ttf/times.ttf", 20);
    if(!pontfont)
        {fprintf(stderr, "Nem sikerult megnyitni a pontfontot (Times)!\n");
        exit(4);}
    SDL_Event event;

    konami=0;  //Menüből visszatérve nullázódjon
    srand(time(NULL));
    voltmentes=0; // Ha menüből visszatért akkor már lehet változás a játéktéren, ezért újra figyelmeztet hogy ments.
    koordinata.sebx=0, koordinata.seby=0;
    jelenlegi=kovetkezo=allo;
    int kilep=0,sor,oszlop,pozvisszax,pozvisszay;

    if(temp!='X')
    {
        pozvisszax=blinkypoz.x;
        pozvisszay=blinkypoz.y;
        palya[14][16]=' ';
    }

    for(sor=0;sor<32;sor++)
    {
        for(oszlop=0;oszlop<32;oszlop++)
        {
            if(palya[sor][oszlop]=='9')
            {
                koordinata.x=40 + 20*oszlop;
                koordinata.y=40 + 20*sor;
            }
            if(palya[sor][oszlop]=='B')
            {
                blinkykoordpoz.x=40 + 20*oszlop;
                blinkykoordpoz.y=40 + 20*sor;
            }
        }
    }
    if(temp!='X')
    {
        palya[pozvisszay][pozvisszax]=temp;
        palya[14][16]='B';
    }

    SDL_RemoveTimer(id);  //Menüből visszatérve ne adódjanak össze
    SDL_RemoveTimer(extraid);
    id = SDL_AddTimer(45, idozit, NULL);
    extraid= SDL_AddTimer(400,extraidozit,NULL);

    SDL_FillRect(screen,NULL, 0x000000);  //Fekete képernyõ
    SDL_Flip(screen);
    while(!kilep)
    {
        if(zene==0)
            Mix_PauseMusic();
        else
            Mix_ResumeMusic();
        if(mute==1)
            Mix_VolumeMusic(20);  //CHUNKOKKAL KIEGÉSZÍTENI!
        else
            Mix_VolumeMusic(100);
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                kilep = 1;
                return kilepni;
                break;

            case SDL_PEEKEVENT:
//------------------------- Peekeventenként új irányt számolunk Blinkynek, és extra pontokat sorsolunk a pályára -----------------
                extrapontok();
                blinkykovirany=rand()%4;
                    if((blinkykovirany==fel && blinkyirany== le ) || (blinkykovirany==le && blinkyirany== fel ) ||          //Ha visszafordulna újat sorsol
                      (blinkykovirany==jobbra && blinkyirany== balra ) || (blinkykovirany==balra && blinkyirany== jobbra ))
                            blinkykovirany=rand()%4;

                break;

            case SDL_USEREVENT:
//------------------------------------ Pacman régi pozíciójának törlése a pályáról ----------------------------------------------
                if(jelenlegi==fel)
                    palya[terkeppoz.csuszy][terkeppoz.x]=' ';
                else if(jelenlegi==le)
                    palya[terkeppoz.csuszy][terkeppoz.x]=' ';
                else if(jelenlegi==jobbra)
                    palya[terkeppoz.y][terkeppoz.csuszx]=' ';
                else if(jelenlegi==balra)
                    palya[terkeppoz.y][terkeppoz.csuszx]=' ';
//---------------------- jelenlegi térképpozíció kiszámolása, pálya szél és falak vizsgálata -------------------------------------
                terkeppoz.x=(koordinata.x-40)/20;
                terkeppoz.y=(koordinata.y-40)/20;
                terkeppoz.csuszx=(koordinata.x-30)/20;
                terkeppoz.csuszy=(koordinata.y-30)/20;
                blinkypoz.x=(blinkykoordpoz.x-40)/20;
                blinkypoz.y=(blinkykoordpoz.y-40)/20;
                blinkypoz.csuszx=(blinkykoordpoz.x-30)/20;
                blinkypoz.csuszy=(blinkykoordpoz.y-30)/20;
                palyaszele();
                falvizsgalo();

                if(pacporg==4) //Pörgetések nullázása, hogy előröl kezdje
                    pacporg=0;
                if(blinkyporg>1)
                    blinkyporg=0;
//-------------------------------------------- Pacman beillesztése az új helyre --------------------------------------------------
                if(jelenlegi==fel)
                {
                    pontrendszer(terkeppoz.csuszy,terkeppoz.x);
                    palya[terkeppoz.csuszy][terkeppoz.x]='9';
                }
                else if(jelenlegi==le)
                {
                    pontrendszer(terkeppoz.csuszy,terkeppoz.x);
                    palya[terkeppoz.csuszy][terkeppoz.x]='9';
                }
                else if(jelenlegi==jobbra)
                {
                    pontrendszer(terkeppoz.y,terkeppoz.csuszx);
                    palya[terkeppoz.y][terkeppoz.csuszx]='9';
                }
                else if(jelenlegi==balra)
                {
                    pontrendszer(terkeppoz.y,terkeppoz.csuszx);
                    palya[terkeppoz.y][terkeppoz.csuszx]='9';
                }
                else
                {
                    pontrendszer(terkeppoz.y,terkeppoz.x);
                    palya[terkeppoz.y][terkeppoz.x]='9';
                }
//----------------------------------------- Nyerés és vesztés vizsgálata --------------------------------------------------------
                if (blinkypoz.x==terkeppoz.x && blinkypoz.y==terkeppoz.y)
                    return vesztett;
                kirajzol();
                if(nyeresvizsgalo==0)  //Nincs már pacdot, nyertünk!
                {
                    SDL_RemoveTimer(id);
                    SDL_RemoveTimer(extraid);
                    return nyert;
                }

                pacporg++;
                blinkyporg++;
//------------------------------------------------- Új koordináták kiszámolása --------------------------------------------------
                blinkykoord();
                kovkoordinataszamolo();

                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE :
                        temp=palya[blinkypoz.y][blinkypoz.x];  //Ideiglenesbe eltároljuk mi volt blinky alatt, és ott tároljuk blinky-t
                        palya[blinkypoz.y][blinkypoz.x]='B';
                        return almenu_start();
                        break;

                    case SDLK_UP :                      //cheat csak nyilakra működik, de az irányváltás mindkettőre, ezért nincs break
                        if(cheatall==fel1)
                            cheatall=fel2;
                        else
                            cheatall=fel1;
                    case 'w':
                        kovetkezo=fel;
                        break;

                    case SDLK_DOWN :
                        if(cheatall==fel2)          //A beírandó cheat: fel,fel,le,le,bal,jobb,bal,jobb,b,a Ezeknek van itt az állapottáblája
                            cheatall=le1;
                        else if(cheatall==le1)
                            cheatall=le2;
                    case 's':
                        kovetkezo=le;
                        break;

                    case SDLK_RIGHT :
                        if(cheatall==bal)
                            cheatall=jobb;
                        else if(cheatall==bal2)
                            cheatall=jobb2;
                    case 'd':
                        kovetkezo=jobbra;
                        break;

                    case SDLK_LEFT :
                        if(cheatall==le2)
                            cheatall=bal;
                        else if(cheatall==jobb)
                            cheatall=bal2;
                            kovetkezo=balra;
                        break;
                    case 'a':
                        if(cheatall==bvolt)
                            konami=(konami+1)%2;
                        kovetkezo=balra;
                        break;

                    case 'b':
                        if(cheatall==jobb2)
                            cheatall=bvolt;
                        break;
                    case 'x':                      //Tesztelés miatt X-re instant nyerés
                        return nyert;
                    break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    egerx = event.button.x;
                    egery = event.button.y;

                    if (egerx>330 && egerx<390 && egery>10 && egery<30)  //pause
                    {
                        return almenu_start();
                    }
                    else if(egerx>690 && egerx<710 && egery>690 && egery<710)
                    {
                        if(zene==1)
                            zene=0;
                        else if(zene==0)
                            zene=1;
                    }

                    else if(egerx>660 && egerx<680 && egery>690 && egery<710)
                    {
                        if(mute==1)
                            mute=0;
                        else if(mute==0)
                            mute=1;
                    }
                }
                break;
        }
    }
    SDL_Flip(screen);
}

int jatek_menu(void)
{
    SDL_Event event;
    int i, ESC;
    char kiirando[5][16]={"Resume", "Save", "Quit to menu", "Quit to desktop"};
    int TTF_init();                                                  //Egyedi betûstílus fuggüvény inicializálás
        if(TTF_Init()==-1)
        {printf("TTF_Init: %s\n", SDL_GetError());
        exit(3);}
    szovegfont = TTF_OpenFont("ttf/szoveg.ttf", screen->h/14);  //Címfont meghatározása, .ttf fájl és magasság.
    if (!szovegfont)
        {fprintf(stderr, "Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(4);}
    TTF_SetFontOutline(szovegfont, 1);  //Körvonal a szövegfonthoz
    boxRGBA(screen, screen->w/6, screen->h*0.15, screen->w - screen->w/6, screen->h - screen->h*0.15, 0, 0, 0, 180);  //Menü háttér alap doboz
    boxRGBA(screen, screen->w/6, screen->h*0.15, screen->w - screen->w/6, screen->h - screen->h*0.15, 255, 255, 255, 125);  //Menü háttér doboz
    for(i=0;i<4;i++)
    {
        szoveg = TTF_RenderUTF8_Blended(szovegfont, kiirando[i], jatekfeher);  //Adott sorok kiírása, pozicionálása
        jatekhova.x = (screen->w-szoveg->w)/2;
        jatekhova.y = (screen->h)*0.20+(i*screen->h)*0.17;
        SDL_BlitSurface(szoveg, NULL, screen, &jatekhova);
        SDL_FreeSurface(szoveg);
        SDL_Flip(screen);
    }
    ESC=1;
    while(ESC!=0)
    {
        if(zene==0)
            Mix_PauseMusic();
        else if (zene==1)
            Mix_ResumeMusic();
        if(mute==1)
            Mix_VolumeMusic(20);  //CHUNKOKKAL KIEGÉSZÍTENI!
        else
            Mix_VolumeMusic(100);
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                ESC = 1;
                return kilepni;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    egerx = event.button.x;
                    egery = event.button.y;
                    if (egerx>(screen->w)*0.35 && egerx<(screen->w)*0.64 && egery> (screen->h)*0.18 && egery< (screen->h)*0.27)  //resume
                    {
                        ESC=0;
                    }
                    if (egerx>(screen->w)*0.4 && egerx<(screen->w)*0.6 && egery> (screen->h)*0.35 && egery< (screen->h)*0.425)  //save
                    {
                        kiment();
                        voltmentes=1;
                        szoveg = TTF_RenderUTF8_Blended(szovegfont, "d", jatekfeher);
                        jatekhova.x = 420;
                        jatekhova.y = 266;
                        SDL_BlitSurface(szoveg, NULL, screen, &jatekhova);
                        SDL_FreeSurface(szoveg);
                        SDL_Flip(screen);
                    }
                    if (egerx>(screen->w)*0.24 && egerx<(screen->w)*0.75 && egery> (screen->h)*0.52 && egery< (screen->h)*0.6)  //Menube
                    {
                        if(voltmentes==0)
                        {
                            int vissza=kilepes();
                            if(vissza==1){
                                ESC=0;
                                return menube;}
                            else if(vissza==0);
                            {
                                kirajzol();
                                ESC=0;
                                return jatekmenube;
                            }
                        }
                        else if(voltmentes==1)
                        {
                            ESC=0;
                            return menube;
                        }
                    }
                    if (egerx>(screen->w)*0.18 && egerx<(screen->w)*0.81 && egery> (screen->h)*0.69 && egery< (screen->h)*0.79)  //Kilepes
                    {
                        ESC=0;
                        if(voltmentes==0 && kilepes()==1)
                        {
                            return kilepni;
                        }else if(voltmentes==1)
                        {
                            return kilepni;
                        }
                    }
                    SDL_Rect srchang ={0,0,20,20};
                    SDL_Rect dstzene ={690,690,0,0};
                    SDL_Rect dstmute ={660,690,0,0};
                    if(egerx>690 && egerx<710 && egery>690 && egery<710)
                    {
                        if(zene==1)
                        {
                            zene=0;
                            SDL_BlitSurface(music_off_small,&srchang,screen,&dstzene);
                        }
                        else if(zene==0)
                        {
                            zene=1;
                            SDL_BlitSurface(music_on_small,&srchang,screen,&dstzene);
                        }
                    }
                    if(egerx>660 && egerx<680 && egery>690 && egery<710)
                    {
                        if(mute==1)
                        {
                            mute=0;
                            SDL_BlitSurface(sound_on,&srchang,screen,&dstmute);
                        }
                        else if(mute==0)
                        {
                            mute=1;
                            SDL_BlitSurface(sound_off,&srchang,screen,&dstmute);
                        }
                    }
                    SDL_Flip(screen);
                }
                break;

            case SDL_KEYDOWN:
                if(event.key.keysym.sym==SDLK_ESCAPE)
                    ESC=0;
                break;
        }
    }
}

void kirajzol(void)
{
    int sor,oszlop;
    nyeresvizsgalo=0;
    SDL_Rect feltoltdst={40,40,680,680};
    SDL_FillRect(screen,&feltoltdst, 0x000000);  //Fekete képernyõ
    SDL_Rect src = {jelenlegi*4*20+pacporg*20,0,20,20};
    SDL_Rect srcallo = {0,0,20,20};
    SDL_Rect dst = {koordinata.x,koordinata.y, 0, 0 };
    SDL_Rect dstblinky = {blinkykoordpoz.x,blinkykoordpoz.y};

    SDL_Rect srcblinky = {blinkyirany*2*20+blinkyporg*20,0,20,20};
    SDL_Rect srcblinkyallo={40+blinkyporg*4*20,0,20,20};

    SDL_Rect srcpause ={0,0,59,20};
    SDL_Rect dstpause ={330,10,0,0};
    SDL_BlitSurface(pause,&srcpause,screen,&dstpause);

    SDL_Rect srcikon ={0,0,20,20};
    SDL_Rect dstzene ={690,690,0,0};
    SDL_Rect dstmute ={660,690,0,0};
    if(zene==1)
        SDL_BlitSurface(music_on_small,&srcikon,screen,&dstzene);
    else if(zene==0)
        SDL_BlitSurface(music_off_small,&srcikon,screen,&dstzene);
    if(mute==1)
        SDL_BlitSurface(sound_off,&srcikon,screen,&dstmute);
    else if(mute==0)
        SDL_BlitSurface(sound_on,&srcikon,screen,&dstmute);

    for(sor=0;sor<32;sor++)
    {
        for(oszlop=0;oszlop<32;oszlop++)
        {
            if(palya[sor][oszlop]=='9')
            {
                if(koordinata.sebx==0 && koordinata.seby==0)
                {
                    SDL_BlitSurface(sprite,&srcallo,screen,&dst);
                }
                else
                {
                    SDL_BlitSurface(sprite,&src,screen,&dst);
                }
            }
            if(palya[sor][oszlop]=='B')
            {
                if(blinkykoordpoz.sebx==0 && blinkykoordpoz.seby==0)
                    SDL_BlitSurface(blinky,&srcblinkyallo,screen,&dstblinky);
                else
                    SDL_BlitSurface(blinky,&srcblinky,screen,&dstblinky);
            }
            SDL_Rect src = {0,0,20,20};
            SDL_Rect dst = {40+ 20*oszlop,40+20*sor, 0, 0 };
            if(palya[sor][oszlop]=='#')
                SDL_BlitSurface(wall,&src,screen,&dst);
            if(palya[sor][oszlop]=='1')
            {
                SDL_BlitSurface(pacdot,&src,screen,&dst);
                nyeresvizsgalo++;
            }
            if(palya[sor][oszlop]=='2')
            {
                SDL_BlitSurface(powerdot,&src,screen,&dst);
                nyeresvizsgalo++;
            }
            if(palya[sor][oszlop]=='3')
                SDL_BlitSurface(cherry,&src,screen,&dst);
            if(palya[sor][oszlop]=='4')
                SDL_BlitSurface(orange,&src,screen,&dst);
            if(palya[sor][oszlop]=='5')
                SDL_BlitSurface(melon,&src,screen,&dst);
            if(palya[sor][oszlop]=='6')
                SDL_BlitSurface(bell,&src,screen,&dst);
            if(palya[sor][oszlop]=='7')
                SDL_BlitSurface(key,&src,screen,&dst);
        }
    }
    SDL_Flip(screen);
}

int kilepes(void)
{
    SDL_Event event;
    static int i,kilep;
    char kiirando[2][21]={"Quit without saving?", "Yes       No"};
    boxRGBA(screen, screen->w/11, screen->h*0.35, screen->w - screen->w/10, screen->h - screen->h*0.35, 0, 0, 0, 255);  //Menü háttér alap doboz
    boxRGBA(screen, screen->w/11, screen->h*0.35, screen->w - screen->w/10, screen->h - screen->h*0.35, 125, 125, 255, 125);  //Menü háttér doboz
    for (i=0;i<2;i++)
    {
        szoveg = TTF_RenderUTF8_Blended(szovegfont, kiirando[i], jatekfeher);  //Adott sorok kiírása, pozicionálása, felszabaditása
        jatekhova.x = (screen->w-szoveg->w)/2+1;                           // screen -> w képernyõ szélességét adja meg
        jatekhova.y = (screen->h)*0.40+(i*screen->h)*0.17;                   //Minden egyes sort 50 pixellel lejjeb ír ki
        SDL_BlitSurface(szoveg, NULL, screen, &jatekhova);
        SDL_FreeSurface(szoveg);
        SDL_Flip(screen);
    }

    kilep=0;
    while (!kilep)
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)
        {
            kilep=1;
            return 1;
        }
        if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            egerx = event.button.x;
            egery = event.button.y;
            if (egerx>(screen->w)*0.25 && egerx<(screen->w)*0.41 && egery> (screen->h)*0.55 && egery< (screen->h)*0.75)  //Eredmények
            {
                kilep=1;
                return 1;
            }
            if (egerx>(screen->w)*0.64 && egerx<(screen->w)*0.75 && egery> (screen->h)*0.55 && egery< (screen->h)*0.75)  //Eredmények
            {
                kilep=1;
                return 0;
            }
        }
    }
}

int almenu_start(void)
{
    jatekmenu=jatek_menu();
    if(jatekmenu==kilepni)
        return kilepni;
    else if(jatekmenu==menube)
        return menube;
    else if(jatekmenu==jatekmenube)
        almenu_start();
}

void palyaszele(void)
{
    if(jelenlegi==jobbra && terkeppoz.x > 30 )
        koordinata.x=(terkeppoz.x=0)*20+40;

    else if(jelenlegi==le && terkeppoz.y > 30 )
        koordinata.y=(terkeppoz.y=1)*20+40;

    else if(jelenlegi==balra && terkeppoz.x <1  && (koordinata.x-40)%20==0 )
        koordinata.x=(terkeppoz.x=31)*20+40;

    else if(jelenlegi==fel && terkeppoz.y <1 && (koordinata.y-40)%20==0)
        koordinata.y=(terkeppoz.y=31)*20+40;

    if(blinkyirany==jobbra && blinkypoz.x > 30 )
        blinkykoordpoz.x=(blinkypoz.x=0)*20+40;

    else if(blinkyirany==le && blinkypoz.y > 30 )
        blinkykoordpoz.y=(blinkypoz.y=1)*20+40;

    else if(blinkyirany==balra && blinkypoz.x <1  && (blinkykoordpoz.x-40)%20==0 )
        blinkykoordpoz.x=(blinkypoz.x=31)*20+40;

    else if(blinkyirany==fel && blinkypoz.y <1 && (blinkykoordpoz.y-40)%20==0)
        blinkykoordpoz.y=(blinkypoz.y=31)*20+40;
}

void falvizsgalo(void)
{
//------------------------------------------------ KÖVETKEZŐ LÉPÉST VIZSGÁLJA -------------------------------------------
    if(kovetkezo==jobbra && (palya[terkeppoz.y][terkeppoz.x+1]!='#' || konami==1) && (koordinata.x-40)%20==0 && (koordinata.y-40)%20==0)
    {                                  /*(koordinata.x-40)%20==0 && (koordinata.xy-40)%20==0 feltétel miatt csak egységenként fog tudni mozogni */
        koordinata.sebx=4;
        koordinata.seby=0;
        jelenlegi=kovetkezo;
    }
    if(kovetkezo==balra && (palya[terkeppoz.y][terkeppoz.x-1]!='#' || konami==1) && (koordinata.x-40)%20==0 && (koordinata.y-40)%20==0)
    {
         koordinata.sebx=-4;
         koordinata.seby=0;
         jelenlegi=kovetkezo;
    }
    if(kovetkezo==fel && (palya[terkeppoz.y-1][terkeppoz.x]!='#' || konami==1) && (koordinata.x-40)%20==0 && (koordinata.y-40)%20==0)
    {
         koordinata.sebx=0;
         koordinata.seby=-4;
         jelenlegi=kovetkezo;
    }
    if(kovetkezo==le && (palya[terkeppoz.y+1][terkeppoz.x]!='#' || konami==1) && (koordinata.x-40)%20==0 && (koordinata.y-40)%20==0)
    {
         koordinata.sebx=0;
         koordinata.seby=4;
         jelenlegi=kovetkezo;
    }

//--------------------------------------------------------------------------------------------------------------------
//------------------------------- JELENLEGI IRÁNYBA TÖRTÉNŐ LÉPÉST VIZSGÁLJA -----------------------------------------

    if(((jelenlegi==jobbra && palya[terkeppoz.y][terkeppoz.x+1]=='#' && konami!=1) || (jelenlegi==balra && palya[terkeppoz.y][terkeppoz.x-1]=='#' && konami!=1) ||
      (jelenlegi==fel && palya[terkeppoz.y-1][terkeppoz.x]=='#' && konami!=1) || (jelenlegi==le && palya[terkeppoz.y+1][terkeppoz.x]=='#' && konami!=1)) &&
      (koordinata.x-40)%20==0 && (koordinata.y-40)%20==0)
        jelenlegi=allo;

    if(jelenlegi==allo)
    {
         koordinata.sebx=0;
         koordinata.seby=0;
    }
}

void pontrendszer(int y,int x)
{
    if(palya[y][x]=='1')
        pont+=10;
    else if(palya[y][x]=='2')
        pont+=50;
    else if(palya[y][x]=='3')
        pont+=100;
    else if(palya[y][x]=='4')
        pont+=500;
    else if(palya[y][x]=='5')
        pont+=1000;
    else if(palya[y][x]=='6')
        pont+=2500;
    else if(palya[y][x]=='7')
        pont+=5000;
    else if(palya[y][x]=='#')
        pont+=15000;

    sprintf(pontkiir,"Score: %d",pont);
    if(pontrender==7)
    {
        szoveg = TTF_RenderUTF8_Shaded(pontfont, pontkiir, jatekfeher, jatekfekete);
        jatekhova.x = 10;
        jatekhova.y = 10;
        SDL_BlitSurface(szoveg, NULL, screen, &jatekhova);
        SDL_FreeSurface(szoveg);
        SDL_Flip(screen);
    }
}

void kovkoordinataszamolo(void)
{
    if(jelenlegi!=allo)
    {
        koordinata.x+=koordinata.sebx;
        koordinata.y+=koordinata.seby;
    }
    if(blinkyirany!=allo)
    {
        blinkykoordpoz.x+=blinkykoordpoz.sebx;
        blinkykoordpoz.y+=blinkykoordpoz.seby;
    }
}

void kiment(void)
{
    int i,j;
    FILE *savegamefp;
    savegamefp=fopen("files/save.sav","w+t");
    if(!savegamefp)
    {fprintf(stderr,"Nem sikerult megnyitni a save.sav-t!");
        exit(6);}
    for(j=0;j<32;j++)
    {
        for(i=0;i<32;i++)
        {
            fprintf(savegamefp,"%c",palya[j][i]);
        }
        fprintf(savegamefp,"\n");
    }
    fprintf(savegamefp,"\n%d", pont);
   fclose(savegamefp);
}

void extrapontok(void)
{
    int random, randx,randy, counter =0;
    char beilleszt='0';
    random=rand()%10000+1; //1->10000-ig generálok számokat.
    randx=rand()%32;
    randy=rand()%31;// legelső roba ne generáljon
    if(random%2500==0)//Aranykulcs, 4 ilyen szám van
        beilleszt='7';
    else if(random%1250==0)//Harang, 8 ilyen szám van
        beilleszt='6';
    else if(random%500==0)//Dinnye, 20 ilyen szám van
        beilleszt='5';
    else if(random%250==0)//Narancs, 40 ilyen szám van
        beilleszt='4';
    else if(random%50==0)//Cseresznye, 200 ilyen van
        beilleszt='3';

    if(beilleszt!='0')
    {
        do
        {
            if(palya[randy+1][randx]=='1' || palya[randy+1][randx]==' ')
            {
                palya[randy+1][randx]=beilleszt;
            }
            else
            {
                randx=rand()%32;
                randy=rand()%32;
            }
            counter ++;
        }while(palya[randy+1][randx]!=beilleszt && counter <10);
    }
}


void blinkykoord(void)
{
    if(blinkypoz.x > 13 && blinkypoz.x<17 && blinkypoz.y>13 && blinkypoz.y<17)
        blinkykovirany=fel;

//------------------------------------------------ KÖVETKEZŐ LÉPÉST VIZSGÁLJA -------------------------------------------
if(blinkykovirany==jobbra && (palya[blinkypoz.y][blinkypoz.x+1]!='#') && (blinkykoordpoz.x-40)%20==0 && (blinkykoordpoz.y-40)%20==0)
    {
        blinkykoordpoz.sebx=10;
        blinkykoordpoz.seby=0;
        blinkyirany=blinkykovirany;
    }
    if(blinkykovirany==balra && (palya[blinkypoz.y][blinkypoz.x-1]!='#') && (blinkykoordpoz.x-40)%20==0 && (blinkykoordpoz.y-40)%20==0)
    {
         blinkykoordpoz.sebx=-10;
         blinkykoordpoz.seby=0;
         blinkyirany=blinkykovirany;
    }
    if(blinkykovirany==fel && (palya[blinkypoz.y-1][blinkypoz.x]!='#') && (blinkykoordpoz.x-40)%20==0 && (blinkykoordpoz.y-40)%20==0)
    {
         blinkykoordpoz.sebx=0;
         blinkykoordpoz.seby=-10;
         blinkyirany=blinkykovirany;
    }
    if(blinkykovirany==le && (palya[blinkypoz.y+1][blinkypoz.x]!='#') && (blinkykoordpoz.x-40)%20==0 && (blinkykoordpoz.y-40)%20==0)
    {
         blinkykoordpoz.sebx=0;
         blinkykoordpoz.seby=10;
         blinkyirany=blinkykovirany;
    }

//--------------------------------------------------------------------------------------------------------------------
//------------------------------- blinkyirany IRÁNYBA TÖRTÉNŐ LÉPÉST VIZSGÁLJA -----------------------------------------

    if(((blinkyirany==jobbra && palya[blinkypoz.y][blinkypoz.x+1]=='#') || (blinkyirany==balra && palya[blinkypoz.y][blinkypoz.x-1]=='#') ||
      (blinkyirany==fel && palya[blinkypoz.y-1][blinkypoz.x]=='#') || (blinkyirany==le && palya[blinkypoz.y+1][blinkypoz.x]=='#')) &&
      (blinkykoordpoz.x-40)%20==0 && (blinkykoordpoz.y-40)%20==0)
         blinkyirany=allo;

    if(blinkyirany==allo)
    {
         blinkykoordpoz.sebx=0;
         blinkykoordpoz.seby=0;
    }
}
