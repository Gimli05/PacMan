int jatek_indit(void);
int jatek_menu(void);
void kirajzol(void);
int kilepes(void);
int almenu_start(void);
void palyaszele(void);
void falvizsgalo(void);
void pontrendszer(int y,int x);
void kovkoordinataszamolo(void);
void kiment(void);
void extrapontok(void);
void blinkykoord(void);

extern SDL_Surface *screen, *music_on_small, *music_off_small, *sound_on, *sound_off, *szoveg, *hatter, *cim, *wall, *sprite,
                   *cherry, *bell, *key, *melon, *orange, *pacdot, *powerdot, *logo, *pause, *blinky;
extern int zene,mute,pont;
extern char palya[32][32];
SDL_Color feher,fekete;
