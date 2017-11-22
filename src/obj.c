#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

/* free lists */
WEAPON_DATA		*weapon_free;
ARMOUR_DATA		*armour_free;
CONT_DATA		*cont_free;
DRINK_DATA		*drink_free;
FOOD_DATA		*food_free;
SCROLL_DATA		*scroll_free;
PILL_DATA		*pill_free;
POTION_DATA		*potion_free;
WAND_DATA		*wand_free;
STAFF_DATA		*staff_free;
LIGHT_DATA		*light_free;
MONEY_DATA		*money_free;

WEAPON_DATA *new_weapon( void )
{
    WEAPON_DATA *weapon;

    if( !weapon_free )
    {
	top_weapon++;
	weapon = alloc_perm( sizeof( *weapon ) );
    }
    else
    {
	weapon 		= weapon_free;
	weapon_free	= weapon_free->next;
    }
    weapon->next	= NULL;

    return weapon;
}

ARMOUR_DATA *new_armour( void )
{
    ARMOUR_DATA *armour;

    if( !armour_free )
    {
	top_armour++;
	armour = alloc_perm( sizeof( *armour ) );
    }
    else
    {
	armour		= armour_free;
	armour_free	= armour_free->next;
    }
    armour->next	= NULL;

    return armour;
}

CONT_DATA *new_cont( void )
{
    CONT_DATA *cont;

    if( !cont_free )
    {
	top_cont++;
	cont = alloc_perm( sizeof( *cont ) );
    }
    else
    {
	cont		= cont_free;
	cont_free	= cont_free->next;
    }
    cont->next	= NULL;

    return cont;
}

DRINK_DATA *new_drink( void )
{
    DRINK_DATA *drink;

    if( !drink_free )
    {
	top_drink++;
	drink = alloc_perm( sizeof( *drink ) );
    }
    else
    {
	drink		= drink_free;
	drink_free	= drink_free->next;
    }
    drink->next	= NULL;

    return drink;
}

FOOD_DATA *new_food( void )
{
    FOOD_DATA *food;

    if( !food_free )
    {
	top_food++;
	food = alloc_perm( sizeof( *food ) );
    }
    else
    {
	food		= food_free;
	food_free	= food_free->next;
    }
    food->next	= NULL;

    return food;
}

POTION_DATA *new_potion( void )
{
    POTION_DATA *potion;
    int i;

    if( !potion_free )
    {
	top_potion++;
	potion = alloc_perm( sizeof( *potion ) );
	for( i = 0 ; i < 4 ; i++ )
	    potion->spell[ i ] = -1;
    }
    else
    {
	potion		= potion_free;
	potion_free	= potion_free->next;
    }
    potion->next	= NULL;

    return potion;
}

PILL_DATA *new_pill( void )
{
    PILL_DATA *pill;
    int i;

    if( !pill_free )
    {
	top_pill++;
	pill = alloc_perm( sizeof( *pill ) );
	for( i = 0 ; i < 4 ; i++ )
	    pill->spell[ i ] = -1;
    }
    else
    {
	pill		= pill_free;
	pill_free	= pill_free->next;
    }
    pill->next	= NULL;

    return pill;
}

SCROLL_DATA *new_scroll( void )
{
    SCROLL_DATA *scroll;
    int i;

    if( !scroll_free )
    {
	top_scroll++;
	scroll = alloc_perm( sizeof( *scroll ) );
	for( i = 0 ; i < 4 ; i++ )
	    scroll->spell[ i ] = -1;
    }
    else
    {
	scroll		= scroll_free;
	scroll_free	= scroll_free->next;
    }
    scroll->next	= NULL;

    return scroll;
}

STAFF_DATA *new_staff( void )
{
    STAFF_DATA *staff;

    if( !staff_free )
    {
	top_staff++;
	staff = alloc_perm( sizeof( *staff ) );
	staff->spell = -1;
    }
    else
    {
	staff		= staff_free;
	staff_free	= staff_free->next;
    }
    staff->next	= NULL;

    return staff;
}

WAND_DATA *new_wand( void )
{
    WAND_DATA *wand;

    if( !wand_free )
    {
	top_wand++;
	wand = alloc_perm( sizeof( *wand ) );
	wand->spell = -1;
    }
    else
    {
	wand		= wand_free;
	wand_free	= wand_free->next;
    }
    wand->next	= NULL;

    return wand;
}

LIGHT_DATA *new_light( void )
{
    LIGHT_DATA *light;

    if( !light_free )
    {
	top_light++;
	light = alloc_perm( sizeof( *light ) );
    }
    else
    {
	light		= light_free;
	light_free	= light_free->next;
    }
    light->next	= NULL;

    return light;
}

MONEY_DATA *new_money( void )
{
    MONEY_DATA *money;

    if( !money_free )
    {
	top_money++;
	money = alloc_perm( sizeof( *money ) );
    }
    else
    {
	money		= money_free;
	money_free	= money_free->next;
    }
    money->next	= NULL;

    return money;
}

/* free code for fields -- puts the field on the free list, and clears it */
void free_weapon( OBJ_DATA *obj )
{
    int i;

    if( !obj->weapon )
	return;
  
    if( obj->weapon == obj->pIndexData->weapon )
    {
	obj->weapon = NULL;
	return;
    }

    obj->weapon->type 		= 0;
    obj->weapon->message	= 0;
    obj->weapon->flags		= 0;
    obj->weapon->hit		= 0;
     
    for( i = 0 ; i < 3 ; i++ )
	obj->weapon->damage[i]	= 0;

    obj->weapon->next 	= weapon_free;
    weapon_free		= obj->weapon;
    obj->weapon		= NULL;
}

void free_weapon_index( OBJ_INDEX_DATA *obj )
{
    int i;

    if( !obj->weapon )
	return;
  
    obj->weapon->type 		= 0;
    obj->weapon->message	= 0;
    obj->weapon->flags		= 0;
    obj->weapon->hit		= 0;
     
    for( i = 0 ; i < 3 ; i++ )
	obj->weapon->damage[i]	= 0;

    obj->weapon->next 	= weapon_free;
    weapon_free		= obj->weapon;
    obj->weapon		= NULL;
}

void free_armour( OBJ_DATA *obj )
{
    int i;

    if( !obj->armour )
	return;

    if( obj->armour == obj->pIndexData->armour )
    {	
	obj->armour = NULL;
	return;
    }

    for( i = 0 ; i < 4 ; i++ )
	obj->armour->ac[i]	= 0;

    obj->armour->next	= armour_free;
    armour_free		= obj->armour;
    obj->armour		= NULL;
}

void free_armour_index( OBJ_INDEX_DATA *obj )
{
    int i;

    if( !obj->armour )
	return;

    for( i = 0 ; i < 4 ; i++ )
	obj->armour->ac[i]	= 0;

    obj->armour->next	= armour_free;
    armour_free		= obj->armour;
    obj->armour		= NULL;
}

void free_cont( OBJ_DATA *obj )
{

    if( !obj->cont )
	return;

    if( obj->cont == obj->pIndexData->cont )
    {
	obj->cont = NULL;
	return;
    }

    obj->cont->closed	= 0;
    obj->cont->key	= 0;
    obj->cont->capacity	= 0;
    obj->cont->single	= 0;

    obj->cont->next	= cont_free;
    cont_free		= obj->cont;
    obj->cont		= NULL;
}

void free_cont_index( OBJ_INDEX_DATA *obj )
{

    if( !obj->cont )
	return;

    obj->cont->closed	= 0;
    obj->cont->key	= 0;
    obj->cont->capacity	= 0;
    obj->cont->single	= 0;

    obj->cont->next	= cont_free;
    cont_free		= obj->cont;
    obj->cont		= NULL;
}

void free_drink( OBJ_DATA *obj )
{

    if( !obj->drink )
	return;

    if( obj->drink == obj->pIndexData->drink )
    {
	obj->drink = NULL;
	return;
    }

    obj->drink->volume	= 0;
    obj->drink->limit	= 0;
    obj->drink->liquid	= 0;
    obj->drink->poison	= 0;

    obj->drink->next	= drink_free;
    drink_free		= obj->drink;
    obj->drink		= NULL;
}

void free_drink_index( OBJ_INDEX_DATA *obj )
{

    if( !obj->drink )
	return;

    obj->drink->volume	= 0;
    obj->drink->limit	= 0;
    obj->drink->liquid	= 0;
    obj->drink->poison	= 0;

    obj->drink->next	= drink_free;
    drink_free		= obj->drink;
    obj->drink		= NULL;
}

void free_food( OBJ_DATA *obj )
{
    if( !obj->food )
	return;

    if( obj->food == obj->pIndexData->food )
    {
	obj->food = NULL;
	return;
    }

    if( obj->food == obj->pIndexData->food )

    obj->food->bite	= 0;
    obj->food->poison	= 0;

    obj->food->next	= food_free;
    food_free		= obj->food;
    obj->food		= NULL;
}

void free_food_index( OBJ_INDEX_DATA *obj )
{
    if( !obj->food )
	return;

    obj->food->bite	= 0;
    obj->food->poison	= 0;

    obj->food->next	= food_free;
    food_free		= obj->food;
    obj->food		= NULL;
}

void free_potion( OBJ_DATA *obj )
{
    int i;

    if( !obj->potion )
	return;

    if( obj->potion == obj->pIndexData->potion )
    {
	obj->potion = NULL;
	return;
    }

    obj->potion->level	= 0;

    for( i = 0 ; i < 4 ; i++ )
	obj->potion->spell[i]	= -1;

    obj->potion->next	= potion_free;
    potion_free		= obj->potion;
    obj->potion		= NULL;
}

void free_potion_index( OBJ_INDEX_DATA *obj )
{
    int i;

    if( !obj->potion )
	return;

    obj->potion->level	= 0;

    for( i = 0 ; i < 4 ; i++ )
	obj->potion->spell[i]	= -1;

    obj->potion->next	= potion_free;
    potion_free		= obj->potion;
    obj->potion		= NULL;
}

void free_scroll( OBJ_DATA *obj )
{
    int i;

    if( !obj->scroll )
	return;

    if( obj->scroll == obj->pIndexData->scroll )
    {
	obj->scroll = NULL;
	return;
    }

    obj->scroll->level	= 0;

    for( i = 0 ; i < 4 ; i++ )
	obj->scroll->spell[i]	= -1;

    obj->scroll->next	= scroll_free;
    scroll_free		= obj->scroll;
    obj->scroll		= NULL;
}

void free_scroll_index( OBJ_INDEX_DATA *obj )
{
    int i;

    if( !obj->scroll )
	return;

    obj->scroll->level	= 0;

    for( i = 0 ; i < 4 ; i++ )
	obj->scroll->spell[i]	= -1;

    obj->scroll->next	= scroll_free;
    scroll_free		= obj->scroll;
    obj->scroll		= NULL;
}

void free_pill( OBJ_DATA *obj )
{
    int i;

    if( !obj->pill )
	return;

    if( obj->pill == obj->pIndexData->pill )
    {
	obj->pill = NULL;
	return;
    }

    obj->pill->level	= 0;

    for( i = 0 ; i < 4 ; i++ )
	obj->pill->spell[i]	= -1;

    obj->pill->next	= pill_free;
    pill_free		= obj->pill;
    obj->pill		= NULL;
}

void free_pill_index( OBJ_INDEX_DATA *obj )
{
    int i;

    if( !obj->pill )
	return;

    obj->pill->level	= 0;

    for( i = 0 ; i < 4 ; i++ )
	obj->pill->spell[i]	= -1;

    obj->pill->next	= pill_free;
    pill_free		= obj->pill;
    obj->pill		= NULL;
}

void free_staff( OBJ_DATA *obj )
{
    if( !obj->staff )
	return;

    if( obj->staff == obj->pIndexData->staff )
    {
	obj->staff = NULL;
	return;
    }

    obj->staff->level	= 0;
    obj->staff->charges	= 0;
    obj->staff->max	= 0;
    obj->staff->spell	= -1;

    obj->staff->next	= staff_free;
    staff_free		= obj->staff;
    obj->staff		= NULL;
}

void free_staff_index( OBJ_INDEX_DATA *obj )
{
    if( !obj->staff )
	return;

    obj->staff->level	= 0;
    obj->staff->charges	= 0;
    obj->staff->max	= 0;
    obj->staff->spell	= -1;

    obj->staff->next	= staff_free;
    staff_free		= obj->staff;
    obj->staff		= NULL;
}

void free_wand( OBJ_DATA *obj )
{
    if( !obj->wand )
	return;

    if( obj->wand == obj->pIndexData->wand )
    {
	obj->wand = NULL;
	return;
    }

    obj->wand->level	= 0;
    obj->wand->charges	= 0;
    obj->wand->max	= 0;
    obj->wand->spell	= -1;

    obj->wand->next	= wand_free;
    wand_free		= obj->wand;
    obj->wand		= NULL;
}

void free_wand_index( OBJ_INDEX_DATA *obj )
{
    if( !obj->wand )
	return;

    obj->wand->level	= 0;
    obj->wand->charges	= 0;
    obj->wand->max	= 0;
    obj->wand->spell	= -1;

    obj->wand->next	= wand_free;
    wand_free		= obj->wand;
    obj->wand		= NULL;
}

void free_light( OBJ_DATA *obj )
{
    if( !obj->light )
	return;

    if( obj->light == obj->pIndexData->light )
    {
	obj->light = NULL;
	return;
    }

    obj->light->duration	= 0;
    obj->light->intensity	= 0;

    obj->light->next	= light_free;
    light_free		= obj->light;
    obj->light		= NULL;
}

void free_light_index( OBJ_INDEX_DATA *obj )
{
    if( !obj->light )
	return;

    obj->light->duration	= 0;
    obj->light->intensity	= 0;

    obj->light->next	= light_free;
    light_free		= obj->light;
    obj->light		= NULL;
}

void free_money( OBJ_DATA *obj )
{
    if( !obj->money )
	return;

    if( obj->money == obj->pIndexData->money )
    {
	obj->money = NULL;
	return;
    }

    obj->money->gold	= 0;
    obj->money->silver	= 0;
    obj->money->copper	= 0;

    obj->money->next	= money_free;
    money_free		= obj->money;
    obj->money		= NULL;
}

void free_money_index( OBJ_INDEX_DATA *obj )
{
    if( !obj->money )
	return;

    obj->money->gold	= 0;
    obj->money->silver	= 0;
    obj->money->copper	= 0;

    obj->money->next	= money_free;
    money_free		= obj->money;
    obj->money		= NULL;
}
