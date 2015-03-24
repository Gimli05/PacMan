int menu_kezdo(void);
void menu_hatter_kirajzol(void);
int menu_leiras(void);
int menu_eredmenyek(void);

extern SDL_Surface *screen, *music_on, *music_off, *szoveg, *hatter, *cim,
                   *cherry, *bell, *key, *melon, *orange, *pacdot, *powerdot, *logo;
extern TTF_Font *szovegfont;
extern SDL_Color jatekfeher, jatekfekete;
extern int zene;
extern char palya[32][32];
typedef enum visszateres{Ujjatek,kilepni,mas,menube,jatekmenube,nyert,vesztett}visszateres;
typedef enum gamestate{menuben,jatekban}gamestate;
visszateres menu, jatek, jatekmenu,jatekalmenu;
gamestate jatekallapot;
SDL_Color feher,fekete;
