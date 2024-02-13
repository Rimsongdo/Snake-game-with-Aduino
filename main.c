
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "pic18f45k22.h"
#include "Config.h" 
#include "time.h"
#include "lcd.h"


#define _XTAL_FREQ  8000000

#define LCD_EN PORTCbits.RC1
#define LCD_RS PORTCbits.RC0

// Définition des directions
#define HAUT 0
#define BAS 1
#define GAUCHE 2
#define DROITE 3

#define MAX_LONGUEUR_SERPENT 100

int x=7;
int y=1;
int score=0;

int pseudo_aleatoire(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

/*
void delay() {
    unsigned int i;
    for (i = 0; i < 100; i++) {
        // Vous pouvez ajuster ce délai selon votre besoin
    }
}

void SendCommand(unsigned char command) {
    LCD_RS = 0;
    delay();
    LCD_EN = 1;
    delay();
    PORTB = command;
    delay();
    LCD_EN = 0;
    delay();
}

void SendData(unsigned char lcddata) {
    LCD_RS = 1;
    delay();
    LCD_EN = 1;
    delay();
    PORTB = lcddata;
    delay();
    LCD_EN = 0;
    delay();
}

void print(const char *str) {
    while (*str != '\0') {
        SendData(*str);
        str++;
    }
}

void SendString(const char *str) {
    while (*str) {
        SendData(*str++);
    }
}


void ClearLCD() {
    unsigned char P = 0x01; // Commande pour effacer l'affichage
    SendCommand(P);
}


void SetCursor(unsigned char row, unsigned char col) {
    unsigned char position;

    // Calculer la position en fonction de la ligne et de la colonne
    if (row == 0)
        position = 0x80 + col;
    else if (row == 1)
        position = 0xC0 + col;
    else if (row == 2)
        position = 0x94 + col; // Adresse pour la troisième ligne
    else if (row == 3)
        position = 0xD4 + col; // Adresse pour la quatrième ligne
    else
        return; // Ligne non supportée

    // Envoyer la commande pour définir la position du curseur
    SendCommand(position);
}*/




// Structure du serpent
typedef struct Snake {
  int position_x;
  int position_y;
  int longueur;
  int direction;
  struct Segment *segments;
} Snake;

// Structure du segment
typedef struct Segment {
  int position_x;
  int position_y;
} Segment;

// Fonction pour initialiser le serpent
Snake serpent;
Segment segments_serpent[MAX_LONGUEUR_SERPENT];


void initialiser_serpent(Snake *serpent) {
  serpent->position_x = 0;
  serpent->position_y = 3;
  serpent->longueur = 1;
  serpent->direction = DROITE;
  serpent->segments = segments_serpent;
}

int nouvelle_position_x ;
int nouvelle_position_y;



void deplacer_serpent(Snake *serpent) {
  // Sauvegarder les coordonnées de la tête avant de les modifier
  int ancienne_position_x = serpent->position_x;
  int ancienne_position_y = serpent->position_y;

  // Déterminer la nouvelle position de la tête
  nouvelle_position_x = ancienne_position_x;
  nouvelle_position_y = ancienne_position_y;
  switch (serpent->direction) {
    case HAUT:
      nouvelle_position_y--;
      break;
    case BAS:
      nouvelle_position_y++;
      break;
    case GAUCHE:
      nouvelle_position_x--;
      break;
    case DROITE:
      nouvelle_position_x++;
      break;
  }

  // Mettre à jour la position de la tête
  serpent->position_x = nouvelle_position_x;
  serpent->position_y = nouvelle_position_y;

  // Déplacer les segments du serpent
  for (int i = serpent->longueur - 1; i > 0; i--) {
    segments_serpent[i].position_x = segments_serpent[i - 1].position_x;
    segments_serpent[i].position_y = segments_serpent[i - 1].position_y;
  }

  // Mettre à jour la position du segment initial avec les anciennes coordonnées de la tête
  segments_serpent[0].position_x = ancienne_position_x;
  segments_serpent[0].position_y = ancienne_position_y;
}




// Fonction pour afficher le serpent
void afficher_serpent(Snake *serpent) {
  // Effacer l'écran
  ClearLCD();
  
  // Définir la position du curseur pour la tête du serpent
  SetCursor(serpent->position_y, serpent->position_x);

  // Afficher la tête du serpent
  print("*");

  // Définir la position du curseur pour chaque segment du serpent
  for (int i = 0; i < serpent->longueur; i++) {
    SetCursor(serpent->segments[i].position_y, serpent->segments[i].position_x);
    
    // Afficher le segment du serpent
    print("o");
  }
}
void init_ports()
{
    ANSELD=0;
    TRISD=1;
}



void snake_move(Snake *serpent)
{
    unsigned char rigth=PORTDbits.RD4;
    unsigned char left=PORTDbits.RD1;
    unsigned char up=PORTDbits.RD2;
    unsigned char down=PORTDbits.RD3;
    if(rigth==1)
    {
        serpent->direction=DROITE;
    }
    if(down==1)
    {
        serpent->direction=BAS;
    }
    if(left==1)
    {
        serpent->direction=GAUCHE;
    }
    if(up==1)
    {
        serpent->direction=HAUT;
    }
}

int detecter_collision(Snake *serpent) {
    // Vérifier les collisions avec les bords de l'écran
    if (serpent->position_x < 0 || serpent->position_x >= 20 || serpent->position_y < 0 || serpent->position_y >= 4) {
        
        return 1; // Collision avec les bords de l'écran
    }

    // Vérifier les collisions avec le corps du serpent
    for (int i = 0; i < serpent->longueur; i++) {
        if (serpent->position_x == serpent->segments[i].position_x && serpent->position_y == serpent->segments[i].position_y) {
            return 1; // Collision avec le corps du serpent
        }
    }

    return 0; // Pas de collision détectée
}




void generer_proie() {
  do {
    x = pseudo_aleatoire(0,18);
    y = pseudo_aleatoire(0, 3);
    // Vérifier que la proie n'est pas générée sur une partie du corps du serpent
    for (int i = 0; i < serpent.longueur; i++) {
      if ((x == serpent.segments[i].position_x && y == serpent.segments[i].position_y)) {
        // Si la proie est générée sur une partie du corps du serpent, réessayer avec de nouvelles coordonnées
        x = -1; // Marquer les coordonnées comme invalides
        y = -1;
        break;
      }
    }
  } while (x == -1||y==-1); // Continuer à générer de nouvelles coordonnées jusqu'à ce qu'elles soient valides
}

void afficher_proie()
{
  SetCursor(y,x);
  print("*");
}









void main() {
    // Initialisation du serpent et des ports
    
   
    unsigned char graine = 52; // Par exemple, vous pouvez utiliser n'importe quelle valeur ici

    // Utiliser la valeur de graine comme graine pour srand()
    srand((unsigned int)graine);
    unsigned char P;
    TRISB = 0;
    TRISCbits.RC0 = 0;
    TRISCbits.RC1 = 0;
    P = 0X38;
    SendCommand(P);
    P = 0X0C;
    SendCommand(P);
    P = 0X01;
    SendCommand(P);
    init_ports();
    initialiser_serpent(&serpent);

    int a = 2; // Variable de contrôle pour le jeu

    // Boucle principale du jeu
    while (1) {
        // Vérifier si le jeu est en cours
        while(a==2)
        {
            if(PORTDbits.RD6==0)
        
            {

                SetCursor(0, 5);
                print("SNAKE GAME");
                SetCursor(1,5);
                print("----------");
                SetCursor(2, 0);
                print("Press START to play!");
                SetCursor(3, 0);
                print("********************");
                

            }
            else
            {
                a=0;
                break;
            }
            
        }
        
        
       
        
        
        if (a == 0) {
            
            

            // Déplacer le serpent
            snake_move(&serpent); 
            deplacer_serpent(&serpent);
            if (detecter_collision(&serpent)) {
                // Afficher "GAME OVER" si une collision est détectée
               
               
                a = 1; 
                // Mettre fin au jeu
            }

            // Gérer les collisions
            
            // Afficher le serpent
            afficher_serpent(&serpent);
            afficher_proie();
            if(serpent.position_x==0&&serpent.position_y==0)
            {
                SetCursor(0,0);
                print(" ");
            }
            __delay_ms(750);
            if (serpent.position_x == x && serpent.position_y == y)
            {
                SetCursor(y,x);
                print(" "); // Effacer la proie de l'écran
      
                // Augmenter la longueur du serpent en ajoutant un segment
                serpent.longueur++;
                score++;

                // Générer une nouvelle proie
                generer_proie();
              }
        } else {
            ClearLCD();
            // Le jeu est terminé, afficher "GAME OVER" en permanence
            while (1) {
                
                SetCursor(1, 5);
                print("GAME OVER");
                SetCursor(3, 3);
                
                print("SCORE:");
                
                char s[3];
                sprintf(s,"%d",score);
                SetCursor(3,11);
                print(s);
                if(PORTDbits.RD6==1)
                {
                    a=2;
                    break;
                }
                
                // Vous pouvez ajouter ici une pause avant de relancer le jeu
            }
        }
    }
}