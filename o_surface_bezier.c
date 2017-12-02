#include <GL/gl.h>
#include <math.h>
#include "o_objet.h"
#include "u_table.h"
#include "t_geometrie.h"

struct surface_bezier
{
  Grille_quadruplet table_surface_bezier;
  int nb_pts_col;
  int nb_pts_row;
  Grille_triplet affichage;
  Booleen polygone_ctrl;
  Booleen affichage_surface;
} ;

static Triplet calcPoint(Table_quadruplet pts_table_bezier, double u)
{
	Table_quadruplet T;
	T.nb = pts_table_bezier.nb;
	ALLOUER(T.table, pts_table_bezier.nb);
	Triplet P;

	for(int i=0 ; i < pts_table_bezier.nb ; i++) {
		//coord homogenes
		T.table[i].x  = pts_table_bezier.table[i].x * pts_table_bezier.table[i].h;
		T.table[i].y  = pts_table_bezier.table[i].y * pts_table_bezier.table[i].h;
		T.table[i].z  = pts_table_bezier.table[i].z * pts_table_bezier.table[i].h;
		T.table[i].h  = pts_table_bezier.table[i].h;
	}
	
	for(int rang = 1; rang < pts_table_bezier.nb ; rang++) {
		//de casteljau
		
		for(int i=0 ; i < pts_table_bezier.nb - rang ; i++) {
			T.table[i].x = T.table[i].x * (1-u) + T.table[i+1].x * u;
			T.table[i].y = T.table[i].y * (1-u) + T.table[i+1].y * u;
			T.table[i].z = T.table[i].z * (1-u) + T.table[i+1].z * u;
			T.table[i].h = T.table[i].h * (1-u) + T.table[i+1].h * u;
		}
	}
	//projection en cartesien
	
	T.table[0].x  = T.table[0].x / T.table[0].h;
	T.table[0].y  = T.table[0].y / T.table[0].h;
	T.table[0].z  = T.table[0].z / T.table[0].h;
	
	P.x = T.table[0].x;
	P.y = T.table[0].y;
	P.z = T.table[0].z;
	free(T.table);
	return P;
}

static Triplet calcPointSurface(Grille_quadruplet pts_surface_bezier, double u, double v)
{
	Table_quadruplet T;
	T.nb = pts_surface_bezier.nb_colonnes;
	ALLOUER(T.table, pts_surface_bezier.nb_colonnes);
	
	Table_quadruplet TableColonne;
	TableColonne.nb = pts_surface_bezier.nb_lignes;
	ALLOUER(TableColonne.table, pts_surface_bezier.nb_lignes);
	
	Triplet P;
	for(int j = 0; j < pts_surface_bezier.nb_colonnes; j++){
		for(int i = 0; i < pts_surface_bezier.nb_lignes; i++){
			TableColonne.table[i].x = pts_surface_bezier.grille[i][j].x;
			TableColonne.table[i].y = pts_surface_bezier.grille[i][j].y;
			TableColonne.table[i].z = pts_surface_bezier.grille[i][j].z;
			TableColonne.table[i].h = pts_surface_bezier.grille[i][j].h;
		}
		
		P = calcPoint(TableColonne, v);
		
		T.table[j].x = P.x;
		T.table[j].y = P.y;
		T.table[j].z = P.z;
		T.table[j].h = 1.; 
	}

	P = calcPoint(T,u);
	
	free(TableColonne.table);
	free(T.table);
	return P;
}

static void affiche_surface_bezier(struct surface_bezier *o)
{

	//affichage surface
	if (o->affichage_surface) {
		for (int i = 0; i < o->nb_pts_row -1; ++i)
		{
			for (int j = 0; j < o->nb_pts_col -1; ++j)
			{
				
				glBegin(GL_TRIANGLES);
				glVertex3f(o->affichage.grille[i][j].x, o->affichage.grille[i][j].y, o->affichage.grille[i][j].z);
				glVertex3f(o->affichage.grille[i][j + 1].x, o->affichage.grille[i][j + 1].y, o->affichage.grille[i][j + 1].z);	
				glVertex3f(o->affichage.grille[i + 1][j].x, o->affichage.grille[i + 1][j].y, o->affichage.grille[i + 1][j].z);
				glEnd();
				
				glBegin(GL_TRIANGLES);
				glVertex3f(o->affichage.grille[i][j + 1].x, o->affichage.grille[i][j + 1].y, o->affichage.grille[i][j + 1].z);
				glVertex3f(o->affichage.grille[i + 1][j].x, o->affichage.grille[i + 1][j].y, o->affichage.grille[i + 1][j].z);
				glVertex3f(o->affichage.grille[i + 1][j + 1].x, o->affichage.grille[i + 1][j + 1].y, o->affichage.grille[i + 1][j + 1].z);
				glEnd();
			}
		}
	}
	
	for (int i = 0; i < o->nb_pts_row; ++i)
	{
		for (int j = 0; j < o->nb_pts_col; ++j)
		{
			glBegin(GL_POINTS);
			glVertex3f(o->affichage.grille[i][j].x, o->affichage.grille[i][j].y, o->affichage.grille[i][j].z);
			glEnd();
		}
	}

	glEnd();

	//affichage polynome controle
	if (o->polygone_ctrl) {
		
		
		for (int i = 0; i < o->table_surface_bezier.nb_lignes - 1; ++i)
		{
			for (int j = 0; j < o->table_surface_bezier.nb_colonnes - 1; ++j)
			{
				glBegin(GL_TRIANGLES);
				glVertex3f(o->table_surface_bezier.grille[i][j].x, o->table_surface_bezier.grille[i][j].y, o->table_surface_bezier.grille[i][j].z);
				glVertex3f(o->table_surface_bezier.grille[i][j + 1].x, o->table_surface_bezier.grille[i][j + 1].y, o->table_surface_bezier.grille[i][j + 1].z);
				glVertex3f(o->table_surface_bezier.grille[i + 1][j].x, o->table_surface_bezier.grille[i + 1][j].y, o->table_surface_bezier.grille[i + 1][j].z);	
				glEnd();
				
				glBegin(GL_TRIANGLES);
				glVertex3f(o->table_surface_bezier.grille[i][j + 1].x, o->table_surface_bezier.grille[i][j + 1].y, o->table_surface_bezier.grille[i][j + 1].z);
				glVertex3f(o->table_surface_bezier.grille[i + 1][j].x, o->table_surface_bezier.grille[i + 1][j].y, o->table_surface_bezier.grille[i + 1][j].z);
				glVertex3f(o->table_surface_bezier.grille[i + 1][j + 1].x, o->table_surface_bezier.grille[i + 1][j + 1].y, o->table_surface_bezier.grille[i + 1][j + 1].z);
				glEnd();
			}
		}
		
	}
	

}

static void changement(struct surface_bezier *o)
{	
	if ( ! (UN_CHAMP_CHANGE(o)||CREATION(o)) )
    return ;

	double u = 0.f;
	double v = 0.f;
	double pas_row = 1.f/(o->nb_pts_row -1);
	double pas_col = 1.f/(o->nb_pts_col -1);

	if (CREATION(o))
	{
		ALLOUER(o->affichage.grille, o->nb_pts_row);
		o->affichage.nb_lignes = o->nb_pts_row;
		o->affichage.nb_colonnes = o->nb_pts_col;

		for (int i=0; i< o->nb_pts_row; ++i){
			ALLOUER(o->affichage.grille[i], o->nb_pts_col);
		}
	}

	if (CHAMP_CHANGE(o, table_surface_bezier) || CHAMP_CHANGE(o, nb_pts_row) || CHAMP_CHANGE(o, nb_pts_col) )
	{
		for (int i=0; i< o->nb_pts_row; ++i){
			free(o->affichage.grille[i]);
		}
		
		free(o->affichage.grille);
		
		ALLOUER(o->affichage.grille, o->nb_pts_row);
		o->affichage.nb_lignes = o->nb_pts_row;
		o->affichage.nb_colonnes = o->nb_pts_col;

		for (int i=0; i< o->nb_pts_row; ++i){
			ALLOUER(o->affichage.grille[i], o->nb_pts_col);
		}
		
		if (o->nb_pts_col < 2)
			o->nb_pts_col = 10;
		if (o->nb_pts_row < 2)
			o->nb_pts_row = 10;
		
		for(int k = 0 ; k < o->nb_pts_row ; k++)
		{
			for(int g = 0 ; g < o->nb_pts_col ; g++)
			{
				o->affichage.grille[k][g] = calcPointSurface(o->table_surface_bezier, u, v);
				v += pas_col;
			}
			v =0;
			u += pas_row;
		}
	}
	

	
}

CLASSE(surface_bezier, struct surface_bezier,      
       CHAMP(table_surface_bezier, L_grille_point P_grille_quadruplet Sauve Extrait)   
       CHAMP(nb_pts_col, LABEL("Nombre de points sur les lignes") L_entier  Edite Sauve DEFAUT("10") )
       CHAMP(nb_pts_row, LABEL("Nombre de points sur les colones") L_entier  Edite Sauve DEFAUT("10") )
       CHAMP(polygone_ctrl, LABEL("afficher le polygone de contrôle") L_booleen  Edite Sauve DEFAUT("0") )
       CHAMP(affichage_surface, LABEL("afficher la surface") L_booleen  Edite Sauve DEFAUT("0") )
       CHANGEMENT(changement)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_surface_bezier))
       
       MENU("TP_PERSO/surface_bezier")
       EVENEMENT("Ctrl+SBZ")
       )
