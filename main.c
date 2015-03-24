#include <SDL.h>                              //Hibakódok:
#include <SDL_gfxPrimitives.h>                //1: Ablaknyitási hiba 2: Képbetöltési hiba. 3: TTF betöltés hiba 4: Font nyitás hiba
#include <SDL_image.h>                        //5: Hangnyitási hiba 6: Térkép ill. mentés nyitási hiba 7:Malloc hiba
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "jatek.h"

void nyertel(void);
void vesztettel(void);
int highscore(void);
void highscorefajlba(char*nev);

typedef struct ListaElem{char nev[100]; int pont; struct ListaElem * kov;} ListaElem;


SDL_Surface *screen, *music_on, *music_off, *music_on_small, *music_off_small, *sound_on, *sound_off, *szoveg, *hatter,
            *cim, *logo, *wall, *sprite, *cherry, *bell, *key, *melon, *orange, *pacdot, *powerdot, *pause, *youwin, *youlose,
            *blinky;
SDL_Color feher,fekete;
SDL_Color feher = {255,255,255}, fekete = {0,0,0};

int zene=0,mute=0;                                                       //Azért globális, hogy a két játék között is tárolja a hangot

int main(int argc, char *argv[])
{
//-------------------------------------------------INICIALIZÁCIÓ---------------------------------------------------------------
    SDL_Surface *ikon;                                      //Ablak, surfacek, videó, hang inicializálása

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    ikon = IMG_Load("images/ikon.png");                                     //Ikon betöltése
    if(!ikon)
        {fprintf(stderr, "Nem sikerult megnyitni az ikon.png-t!\n");
        exit(2);}
    SDL_WM_SetIcon(ikon,NULL);
    SDL_WM_SetCaption("PacMan", "PacMan");         //Cím


    screen=SDL_SetVideoMode(720,720, 0, SDL_ANYFORMAT);         //Ablak*Ablak méretű lesz, ENUM-ban tárolva a méret
        if (!screen)                                                 //Képernyõ megnyitásának ellenõrzése
        {fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
            exit(1);}

    Mix_OpenAudio(44000,AUDIO_S16SYS,2,4096);
    Mix_Music *zene;
    zene=Mix_LoadMUS("./music/hatter.mp3");
    Mix_PlayMusic(zene,-1);





//---------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------- SURFACE ÉRTÉKEK MEGADÁSA --------------------------------------------------

    pacdot=IMG_Load("images/pacdot.png");
    powerdot=IMG_Load("images/powerdot.png");
    wall=IMG_Load("images/fal.png");
    sprite=IMG_Load("images/pac_sprite.png");
    pacdot=IMG_Load("images/pacdot.png");
    cherry=IMG_Load("images/cherry.png");
    bell=IMG_Load("images/bell.png");
    key=IMG_Load("images/key.png");
    melon=IMG_Load("images/melon.png");
    orange=IMG_Load("images/orange.png");
    music_on=IMG_Load("images/music_on.png");
    music_on_small=IMG_Load("images/music_on_small.png");
    music_off=IMG_Load("images/music_off.png");
    music_off_small=IMG_Load("images/music_off_small.png");
    sound_on=IMG_Load("images/sound_on_small.png");
    sound_off=IMG_Load("images/sound_off_small.png");
    hatter=IMG_Load("images/menu_hatter.png");
    logo=IMG_Load("images/logo.png");
    pause=IMG_Load("images/pause.png");
    youwin=IMG_Load("images/youwin.png");
    youlose=IMG_Load("images/youlose.png");
    blinky=IMG_Load("images/blinky_sprite.png");

//---------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------ MEGNYITÁSI HIBÁK ELLENÕRZÉSE ---------------------------------------------------

    if(!music_on)
        {fprintf(stderr, "Nem sikerult megnyitni a music_on ikont!\n");
        exit(2);}
    if(!music_off)
        {fprintf(stderr, "Nem sikerult megnyitni a music_off ikont!\n");
        exit(2);}
    if(!sound_on)
        {fprintf(stderr, "Nem sikerult megnyitni a sound_on ikont!\n");
        exit(2);}
    if(!sound_off)
        {fprintf(stderr, "Nem sikerult megnyitni a sound_off ikont!\n");
        exit(2);}
    if(!music_on_small)
        {fprintf(stderr, "Nem sikerult megnyitni a music_on_small ikont!\n");
        exit(2);}
    if(!music_off_small)
        {fprintf(stderr, "Nem sikerult megnyitni a music_off_small ikont!\n");
        exit(2);}
    if(!hatter)
        {fprintf(stderr, "Nem sikerult megnyitni az menu hátteret!\n");
        exit(2);}
    if(!cherry)
        {fprintf(stderr, "Nem sikerult megnyitni a cseresznye ikont!\n");
        exit(2);}
    if(!bell)
        {fprintf(stderr, "Nem sikerult megnyitni a harang ikont!\n");
        exit(2);}
    if(!key)
        {fprintf(stderr, "Nem sikerult megnyitni a kulcs ikont!\n");
        exit(2);}
    if(!melon)
        {fprintf(stderr, "Nem sikerult megnyitni a dinnye ikont!\n");
        exit(2);}
    if(!orange)
        {fprintf(stderr, "Nem sikerult megnyitni a narancs ikont!\n");
        exit(2);}
    if(!sprite)
        {fprintf(stderr, "Nem sikerult megnyitni a pac_prite.png-t!\n");
        exit(2);}
    if(!wall)
        {fprintf(stderr, "Nem sikerult megnyitni a fal ikont!\n");
        exit(2);}
    if(!logo)
        {fprintf(stderr, "Nem sikerult megnyitni a logó ikont!\n");
        exit(2);}
    if(!pause)
        {fprintf(stderr, "Nem sikerult megnyitni a logó ikont!\n");
        exit(2);}
    if(!blinky)
        {fprintf(stderr,"Nem sikerült megnyitni a blinky_sprite-ot!\n");
        exit(2);}
    if(!youwin)
        {fprintf(stderr,"Nem sikerült megnyitni a youwin-t!\n");
        exit(2);}
    if(!youlose)
        {fprintf(stderr,"Nem sikerült megnyitni a youlose-t!\n");
        exit(2);}

//---------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------- JÁTÉK INDULÁSA ----------------------------------------------------------

    jatekallapot=menuben;
    static int kilep=0;
    while(!kilep)
    {
        switch(jatekallapot)
        {
            case menuben:
                mute=0;
                menu=menu_kezdo();
                if(menu==Ujjatek)
                    jatekallapot=jatekban;
                else if(menu==kilepni)
                    kilep=1;

                break;
            case jatekban:
                jatek=jatek_indit();
                if(jatek==kilepni)
                    kilep=1;
                else if(jatek==nyert)
                {
                    nyertel();
                    if(highscore()==kilepni)
                        return 0;
                    jatekallapot=menuben;
                }
                else if(jatek==vesztett)
                {
                    vesztettel();
                    if(highscore()==kilepni)
                        return 0;
                    jatekallapot=menuben;
                }
                else if(jatek==menube)
                    jatekallapot=menuben;
                break;
        }
    }
    Mix_FreeMusic(zene);
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}
void nyertel(void)
{
    SDL_Delay(500);
    boxRGBA(screen,0,0,720,720,0,0,0,150);
    SDL_Rect src={0,0,720,720};
    int i;
    for(i=0;i<50;i++)
    {
        SDL_Rect dst={screen->w/2-i*6,screen->h/2-i*6,0,0};
        SDL_BlitSurface(rotozoomSurface(youwin,0.0,(double)(i*12)/720,0),&src,screen,&dst);
        SDL_Flip(screen);
        SDL_Delay(20);
    }
    SDL_Delay(2000);



}
void vesztettel(void)
{
    SDL_Delay(500);
    boxRGBA(screen,0,0,720,720,0,0,0,150);
    SDL_Rect src={0,0,720,720};
    int i;
    for(i=0;i<50;i++)
    {
        SDL_Rect dst={screen->w/2-i*6,screen->h/2-i*6,0,0};
        SDL_BlitSurface(rotozoomSurface(youlose,0.0,(double)(i*12)/720,0),&src,screen,&dst);
        SDL_Flip(screen);
        SDL_Delay(20);
    }
    SDL_Delay(2000);

}

int highscore(void)
{
    SDL_EnableUNICODE(1);
    static SDL_Rect hova={0,0,0,0};
    SDL_Event event;
    int i, hossz=0,kilep=0;
    char kiirando[5][28]={"Highscore","Your name:","","","Press ENTER"};

    char *nev=(char*)malloc(sizeof(char));
    char *tmp=(char*)malloc(sizeof(char));
    if(nev==NULL || tmp==NULL)
        {fprintf(stderr,"Nem tudtam helyet foglalni a névnek!");
            exit(7);}
    nev[0]='\0';

    SDL_FillRect(screen,NULL,0x000000);
    for(i=0;i<5;i++)
    {
    szoveg = TTF_RenderUTF8_Blended(szovegfont, kiirando[i], jatekfeher);
    hova.x = (screen->w-szoveg->w)/2;
    hova.y = (screen->h)*0.20+(i*screen->h)*0.17;
    SDL_BlitSurface(szoveg, NULL, screen, &hova);
    SDL_FreeSurface(szoveg);
    SDL_Flip(screen);
    }

    while(!kilep)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym==SDLK_RETURN)
                {
                    kilep=1;
                    break;
                }

                if( event.key.keysym.unicode == (Uint16)' ' ||
                   ( ( event.key.keysym.unicode >= (Uint16)'0' ) && ( event.key.keysym.unicode <= (Uint16)'9' ) ) ||
                   ( ( event.key.keysym.unicode >= (Uint16)'A' ) && ( event.key.keysym.unicode <= (Uint16)'Z' ) ) ||
                   ( ( event.key.keysym.unicode >= (Uint16)'a' ) && ( event.key.keysym.unicode <= (Uint16)'z' ) ) )
                {
                    hossz++;


                    nev = (char*)realloc(nev,hossz*sizeof(char)+1);
                        if(nev==NULL)
                        {fprintf(stderr,"Nem tudtam helyet foglalni a névnek!");
                            exit(7);
                            }
                    sprintf(tmp,"%c",(char)event.key.keysym.unicode);
                    strcat(nev,tmp);
                }
                if( event.key.keysym.sym==SDLK_BACKSPACE)
                {
                    boxRGBA(screen,0,380,720,430,0,0,0,255);
                    hossz=1;
                    nev = (char*)realloc(nev,hossz*sizeof(char));
                    nev[0]='\0';
                }

                szoveg = TTF_RenderUTF8_Shaded(szovegfont, nev, jatekfeher,jatekfekete);
                hova.x = (screen->w-szoveg->w)/2;
                hova.y = (screen->h)*0.20+(2*screen->h)*0.17;

                SDL_BlitSurface(szoveg, NULL, screen, &hova);
                SDL_FreeSurface(szoveg);
                SDL_Flip(screen);

                break;
            case SDL_QUIT:
                kilep=1;
                return kilepni;
                break;
        }
    }
    printf("%s",nev);
    highscorefajlba(nev);
}

void highscorefajlba(char *nev)
{




    /*
    char neve[100], temp[5][100];
    int pontja,lep=0,i;
    ListaElem *uj;
    ListaElem *tmp,*tmp2;
    ListaElem *futo;
    ListaElem *cik;
    ListaElem *hslista=NULL;

    FILE *highscorefp;
    highscorefp=fopen("highscore.txt","r");
    while(fscanf(highscorefp,"%s %d\n", &neve, &pontja )!=EOF)
    {
        uj=(ListaElem*)malloc(sizeof(ListaElem));
        strcpy(uj->listanev,neve);
        uj->listapont=pontja;
        uj->kov=hslista;
        hslista=uj;
    }
    fclose(highscorefp);

    highscorefp=fopen("highscore.txt","w+a");

    for(tmp2=hslista;(tmp2!=NULL)&&(lep<5);tmp2=tmp2->kov,++lep);

        if((tmp2==NULL)&&(lep<1))
        {
            uj=(ListaElem*)malloc(sizeof(ListaElem));
            strcpy(uj->listanev,nev);
            uj->listapont=pont;
            uj->kov=hslista;
            hslista=uj;
        }
        else
            if((tmp2==NULL)&&(lep<5))
        {
            for(cik=hslista,futo=hslista->kov;(futo!=NULL)&&!((cik->listapont<pont)&&(futo->listapont>pont));cik=cik->kov, futo=futo->kov);
            if(futo!=NULL)
                {uj=(ListaElem*)malloc(sizeof(ListaElem));
                strcpy(uj->listanev,nev);
                uj->listapont=pont;
                cik->kov=uj;
                uj->kov=futo;
                }

        }
        else {
            for(cik=hslista,futo=hslista->kov;(futo!=NULL)&&!((cik->listapont<pont)&&(futo->listapont>pont));cik=cik->kov, futo=futo->kov);
            if(futo!=NULL)
            {
                    uj=(ListaElem*)malloc(sizeof(ListaElem));
                    strcpy(uj->listanev,nev);
                    uj->listapont=pont;
                    uj->kov=futo;
                    cik->kov=uj;
                    tmp=hslista;
                    hslista=hslista->kov;
                    free(tmp);
            }
        }


        for(cik=hslista,i=0;cik!=NULL;cik=cik->kov,i++)
        {
            sprintf(temp[i],"%d.%s %d points",i+1,cik->listanev,cik->listapont);
            printf("%s",temp[i]);
        }

        for(i=4;i!=0;i--)
            fputs(temp[i],highscorefp);
        fclose(highscorefp);*/
}
