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
} ;

static Triplet calcPointSurface(Grille_quadruplet pts_surface_bezier, double u, double v)
{
	Table_quadruplet T;
	T.nb = pts_surface_bezier.nb;
	ALLOUER(T.table, pts_surface_bezier.nb);
	Triplet P;

	for(int i = 0; i < pts_surface_bezier.nb; i++){
		T[i] = calcPoint(pts_surface_bezier.grille[i], v);
	}

	P = calcPoint(T,u);
	free(T);

	return P;
}

static Triplet calcPoint(Table_quadruplet pts_surface_bezier, double u)
{
	Table_quadruplet T;
	T.nb = pts_surface_bezier.nb;
	ALLOUER(T.table, pts_surface_bezier.nb);
	Triplet P;

	for(int i=0 ; i < pts_surface_bezier.nb ; i++) {
		//coord homogenes
		T.table[i].x  = pts_surface_bezier.table[i].x * pts_surface_bezier.table[i].h;
		T.table[i].y  = pts_surface_bezier.table[i].y * pts_surface_bezier.table[i].h;
		T.table[i].z  = pts_surface_bezier.table[i].z * pts_surface_bezier.table[i].h;
		T.table[i].h  = pts_surface_bezier.table[i].h;
	}
	
	for(int rang = 1; rang < pts_surface_bezier.nb ; rang++) {
		//de casteljau
		
		for(int i=0 ; i < pts_surface_bezier.nb - rang ; i++) {
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

static void affiche_surface_bezier(struct surface_bezier *o)
{
	glBegin(GL_QUAD_STRIP) ;
	
	for (int i = 0; i < o->nb_pts_row - 1; ++i)
	{
		for (int j = 0; j < o->nb_pts_col - 1; ++j)
		{
			glVertex3f(o->affichage.grille[i].table[j].x, o->affichage.grille[i].table[j].y, o->affichage.grille[i].table[j].z);
			glVertex3f(o->affichage.grille[i + 1].table[j].x, o->affichage.grille[i + 1].table[j].y, o->affichage.grille[i + 1].table[j].z);
			glVertex3f(o->affichage.grille[i].table[j + 1].x, o->affichage.grille[i].table[j + 1].y, o->affichage.grille[i].table[j + 1].z);
			glVertex3f(o->affichage.grille[i + 1].table[j + 1].x, o->affichage.grille[i + 1].table[j + 1].y, o->affichage.grille[i + 1].table[j + 1].z);		
		
			++j;
		}
		++i;
	}

	glEnd();

	//affichage polynome controle
	if (o->polygone_ctrl) {
		glBegin(GL_QUAD_STRIP) ;
		
		for (int i = 0; i < o->nb_pts_row - 1; ++i)
		{
			for (int j = 0; j < o->nb_pts_col - 1; ++j)
			{
				glVertex3f(o->table_surface_bezier.grille[i].table[j].x, o->table_surface_bezier.grille[i].table[j].y, o->table_surface_bezier.grille[i].table[j].z);
				glVertex3f(o->table_surface_bezier.grille[i + 1].table[j].x, o->table_surface_bezier.grille[i + 1].table[j].y, o->table_surface_bezier.grille[i + 1].table[j].z);
				glVertex3f(o->table_surface_bezier.grille[i].table[j + 1].x, o->table_surface_bezier.grille[i].table[j + 1].y, o->table_surface_bezier.grille[i].table[j + 1].z);
				glVertex3f(o->table_surface_bezier.grille[i + 1].table[j + 1].x, o->table_surface_bezier.grille[i + 1].table[j + 1].y, o->table_surface_bezier.grille[i + 1].table[j + 1].z);		
			
				++j;
			}
			++i;
		}
	
		glEnd();
	}
}

static void changement(struct surface_bezier *o)
{
	double u = 0.f;
	double v = 0.f;
	double pas_row = 1.f/(o->nb_pts_row -1);
	double pas_col = 1.f/(o->nb_pts_col -1);
	
	ALLOUER(o->affichage.grille, o->nb_pts_row);
	o->affichage.nb = o->nb_pts_row;

	for (int i=0; i< o->nb_pts_row; ++i){
		ALLOUER(o->affichage.grille[i], o->nb_pts_col);
		o->affichage.grille[i].nb = o->nb_pts_col;
	}
	
	if (o->nb_pts_col < 2)
		o->nb_pts_col = 10;
	if (o->nb_pts_row < 2)
		o->nb_pts_row = 10;
	
	for(int k = 0 ; k < o->nb_pts_row ; k++)
	{
		for(int g = 0 ; g < o->nb_pts_col ; g++)
		{
			o->affichage.grille[k].table[g] = calcPointSurface(o->surface_bezier, u, v);
			v += pas_col
		}
		u += pas_row;
	}
	
}

CLASSE(surface_bezier, struct surface_bezier,      
       CHAMP(table_surface_bezier, L_table_point P_grille_quadruplet Sauve Extrait)   
       CHAMP(nb_pts, LABEL("Nombre de points") L_entier  Edite Sauve DEFAUT("10") )
       CHAMP(polygone_ctrl, LABEL("afficher le polygone de contrôle") L_booleen  Edite Sauve DEFAUT("0") )
       CHANGEMENT(changement)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_surface_bezier))
       
       MENU("TP_PERSO/surface_bezier")
       EVENEMENT("Ctrl+SBZ")
       )
       
