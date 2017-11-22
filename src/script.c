/*------------------------------------------------------------------------\\
||                                                                        ||
||  File:    script.c                                                     ||
||  Purpose: Handling of Program Scripts for Tapestries                   ||
||  Author:  (c) 1996 Ant Whitehead (ant@solace.mh.se)                    ||
||  Credits: Some portions of code have been adapted from the MobProgs    ||
||           Released in the Merc2.2 distribution.                        ||
||                                                                        ||
\\------------------------------------------------------------------------*/


char *next_line( char *script )
{
    char	*pointer = script;

    while( *pointer != '\n' && *pointer != '\0' )
	pointer++;

    if( *pointer == '\n' )
	*pointer++ = '\0';

    if( *pointer == '\r' )
	*pointer++ = '\0';

    return( pointer );
}


bool string_eval( char *lhs, char *opr, char *rhs )
{
    if( !str_cmp( opr, "==" ) )			/* exact match		*/
	return( bool )( !str_cmp( lhs, rhs ) );

    if( !str_cmp( opr, "!=" ) )			/* NOT exact match	*/
	return( bool )( str_cmp( lhs, rhs ) );

    if( !str_cmp( opr, "/" ) )			/* exact substring	*/
	return ( bool )( !str_infix( rhs, lhs ) );

    if( !str_cmp( opr, "!/" ) )			/* NOT exact substring	*/
	return( bool )( str_infix( rhs, lhs ) );

    bug( "Unknown scripting operator(strings)\n\r", 0 );
    return 0;
}

bool value_eval( int lhs, char *opr, int rhs )
{
    if( !str_cmp( opr, "==" ) )
	return( lhs == rhs );
    if( !str_cmp( opr, "!=" ) )
	return( lhs != rhs );
    if( !str_cmp( opr, ">" ) )
	return( lhs > rhs );
    if( !str_cmp( opr, "<" ) )
	return( lhs < rhs );
    if( !str_cmp( opr, ">=" ) )
	return( lhs <= rhs );
    if( !str_cmp( opr, ">=" ) )
	return( lhs >= rhs );
    if( !str_cmp( opr, "&" ) )
	return( lhs & rhs );
    if( !str_cmp( opr, "|" ) )
	return( lhs | rhs );

    bug( "Unknown scripting operator(values)\n\r", 0 );
    return 0;
}

/* This function performs the evaluation of the if checks.  It is
 * here that you can add any ifchecks which you so desire. Hopefully
 * it is clear from what follows how one would go about adding your
 * own. The syntax for an if check is: ifchck ( arg ) [opr val]
 * where the parenthesis are required and the opr and val fields are
 * optional but if one is there then both must be. The spaces are all
 * optional. The evaluation of the opr expressions is farmed out
 * to reduce the redundancy of the mammoth if statement list.
 * If there are errors, then return -1 otherwise return boolean 1,0
 */
bool mprog_do_ifchck( char *ifchck, CHAR_DATA *mob, CHAR_DATA *actor,
		     OBJ_DATA *obj, void *vo, CHAR_DATA *rndm)
{

  char buf[ MAX_INPUT_LENGTH ];
  char arg[ MAX_INPUT_LENGTH ];
  char opr[ MAX_INPUT_LENGTH ];
  char val[ MAX_INPUT_LENGTH ];
  CHAR_DATA *vict = (CHAR_DATA *) vo;
  OBJ_DATA *v_obj = (OBJ_DATA  *) vo;
  char     *bufpt = buf;
  char     *argpt = arg;
  char     *oprpt = opr;
  char     *valpt = val;
  char     *point = ifchck;
  int       lhsvl;
  int       rhsvl;

  if ( *point == '\0' ) 
    {
      bug ( "Mob: %d null ifchck", mob->pIndexData->vnum ); 
      return -1;
    }   
  /* skip leading spaces */
  while ( *point == ' ' )
    point++;

  /* get whatever comes before the left paren.. ignore spaces */
  while ( *point != '(' ) 
    if ( *point == '\0' ) 
      {
	bug ( "Mob: %d ifchck syntax error", mob->pIndexData->vnum ); 
	return -1;
      }   
    else
      if ( *point == ' ' )
	point++;
      else 
	*bufpt++ = *point++; 

  *bufpt = '\0';
  point++;

  /* get whatever is in between the parens.. ignore spaces */
  while ( *point != ')' ) 
    if ( *point == '\0' ) 
      {
	bug ( "Mob: %d ifchck syntax error", mob->pIndexData->vnum ); 
	return -1;
      }   
    else
      if ( *point == ' ' )
	point++;
      else 
	*argpt++ = *point++; 

  *argpt = '\0';
  point++;

  /* check to see if there is an operator */
  while ( *point == ' ' )
    point++;
  if ( *point == '\0' ) 
    {
      *opr = '\0';
      *val = '\0';
    }   
  else /* there should be an operator and value, so get them */
    {
      while ( ( *point != ' ' ) && ( !isalnum( *point ) ) ) 
	if ( *point == '\0' ) 
	  {
	    bug ( "Mob: %d ifchck operator without value",
		 mob->pIndexData->vnum ); 
	    return -1;
	  }   
	else
	  *oprpt++ = *point++; 

      *oprpt = '\0';
 
      /* finished with operator, skip spaces and then get the value */
      while ( *point == ' ' )
	point++;
      for( ; ; )
	{
	  if ( ( *point != ' ' ) && ( *point == '\0' ) )
	    break;
	  else
	    *valpt++ = *point++; 
	}

      *valpt = '\0';
    }
  bufpt = buf;
  argpt = arg;
  oprpt = opr;
  valpt = val;

  /* Ok... now buf contains the ifchck, arg contains the inside of the
   *  parentheses, opr contains an operator if one is present, and val
   *  has the value if an operator was present.
   *  So.. basically use if statements and run over all known ifchecks
   *  Once inside, use the argument and expand the lhs. Then if need be
   *  send the lhs,opr,rhs off to be evaluated.
   */

  if ( !str_cmp( buf, "rand" ) )
    {
      return ( number_percent() <= atoi(arg) );
    }

  if ( !str_cmp( buf, "ispc" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return 0;
	case 'n': if ( actor )
 	             return ( !IS_NPC( actor ) );
	          else return -1;
	case 't': if ( vict )
                     return ( !IS_NPC( vict ) );
	          else return -1;
	case 'r': if ( rndm )
                     return ( !IS_NPC( rndm ) );
	          else return -1;
	default:
	  bug ( "Mob: %d bad argument to 'ispc'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "isnpc" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return 1;
	case 'n': if ( actor )
	             return IS_NPC( actor );
	          else return -1;
	case 't': if ( vict )
                     return IS_NPC( vict );
	          else return -1;
	case 'r': if ( rndm )
	             return IS_NPC( rndm );
	          else return -1;
	default:
	  bug ("Mob: %d bad argument to 'isnpc'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "isgood" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return IS_GOOD( mob );
	case 'n': if ( actor )
	             return IS_GOOD( actor );
	          else return -1;
	case 't': if ( vict )
	             return IS_GOOD( vict );
	          else return -1;
	case 'r': if ( rndm )
	             return IS_GOOD( rndm );
	          else return -1;
	default:
	  bug ( "Mob: %d bad argument to 'isgood'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "isfight" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return ( mob->fighting ) ? 1 : 0;
	case 'n': if ( actor )
	             return ( actor->fighting ) ? 1 : 0;
	          else return -1;
	case 't': if ( vict )
	             return ( vict->fighting ) ? 1 : 0;
	          else return -1;
	case 'r': if ( rndm )
	             return ( rndm->fighting ) ? 1 : 0;
	          else return -1;
	default:
	  bug ( "Mob: %d bad argument to 'isfight'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "isimmort" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return ( get_trust( mob ) > LEVEL_IMMORTAL );
	case 'n': if ( actor )
	             return ( get_trust( actor ) > LEVEL_IMMORTAL );
  	          else return -1;
	case 't': if ( vict )
	             return ( get_trust( vict ) > LEVEL_IMMORTAL );
                  else return -1;
	case 'r': if ( rndm )
	             return ( get_trust( rndm ) > LEVEL_IMMORTAL );
                  else return -1;
	default:
	  bug ( "Mob: %d bad argument to 'isimmort'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "ischarmed" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return ( IS_AFFECTED( mob, AFF_CHARM ) ) ? 1 : 0;
	case 'n': if ( actor )
	             return ( IS_AFFECTED( actor, AFF_CHARM ) ) ? 1 : 0;
	          else return -1;
	case 't': if ( vict )
	             return ( IS_AFFECTED( vict, AFF_CHARM ) ) ? 1 : 0;
	          else return -1;
	case 'r': if ( rndm )
	             return ( IS_AFFECTED( rndm, AFF_CHARM ) ) ? 1 : 0;
	          else return -1;
	default:
	  bug ( "Mob: %d bad argument to 'ischarmed'",
	       mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "isfollow" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return ( mob->master != NULL
			  && mob->master->in_room == mob->in_room );
	case 'n': if ( actor )
	             return ( actor->master != NULL
			     && actor->master->in_room == actor->in_room );
	          else return -1;
	case 't': if ( vict )
	             return ( vict->master != NULL
			     && vict->master->in_room == vict->in_room );
	          else return -1;
	case 'r': if ( rndm )
	             return ( rndm->master != NULL
			     && rndm->master->in_room == rndm->in_room );
	          else return -1;
	default:
	  bug ( "Mob: %d bad argument to 'isfollow'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "isaffected" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return ( mob->affected_by & atoi( arg ) );
	case 'n': if ( actor )
	             return ( actor->affected_by & atoi( arg ) );
	          else return -1;
	case 't': if ( vict )
	             return ( vict->affected_by & atoi( arg ) );
	          else return -1;
	case 'r': if ( rndm )
	             return ( rndm->affected_by & atoi( arg ) );
	          else return -1;
	default:
	  bug ( "Mob: %d bad argument to 'isaffected'",
	       mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "hitprcnt" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = mob->hit / mob->max_hit;
	          rhsvl = atoi( val );
         	  return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    lhsvl = actor->hit / actor->max_hit;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 't': if ( vict )
	          {
		    lhsvl = vict->hit / vict->max_hit;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'r': if ( rndm )
	          {
		    lhsvl = rndm->hit / rndm->max_hit;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'hitprcnt'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "inroom" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = mob->in_room->vnum;
	          rhsvl = atoi(val);
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    lhsvl = actor->in_room->vnum;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 't': if ( vict )
	          {
		    lhsvl = vict->in_room->vnum;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'r': if ( rndm )
	          {
		    lhsvl = rndm->in_room->vnum;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'inroom'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "sex" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = mob->sex;
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    lhsvl = actor->sex;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 't': if ( vict )
	          {
		    lhsvl = vict->sex;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'r': if ( rndm )
	          {
		    lhsvl = rndm->sex;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'sex'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "position" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = mob->position;
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    lhsvl = actor->position;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 't': if ( vict )
	          {
		    lhsvl = vict->position;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'r': if ( rndm )
	          {
		    lhsvl = rndm->position;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'position'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "level" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = get_trust( mob );
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    lhsvl = get_trust( actor );
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else 
		    return -1;
	case 't': if ( vict )
	          {
		    lhsvl = get_trust( vict );
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'r': if ( rndm )
	          {
		    lhsvl = get_trust( rndm );
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'level'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "class" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = mob->class->number;
	          rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    lhsvl = actor->class->number;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else 
		    return -1;
	case 't': if ( vict )
	          {
		    lhsvl = vict->class->number;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'r': if ( rndm )
	          {
		    lhsvl = rndm->class->number;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'class'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "goldamt" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = mob->gold;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    lhsvl = actor->gold;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 't': if ( vict )
	          {
		    lhsvl = vict->gold;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'r': if ( rndm )
	          {
		    lhsvl = rndm->gold;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'goldamt'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "objtype" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'o': if ( obj )
	          {
		    lhsvl = obj->item_type;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	         else
		   return -1;
	case 'p': if ( v_obj )
	          {
		    lhsvl = v_obj->item_type;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'objtype'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "number" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': lhsvl = mob->gold;
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
		    if IS_NPC( actor )
		    {
		      lhsvl = actor->pIndexData->vnum;
		      rhsvl = atoi( val );
		      return mprog_veval( lhsvl, opr, rhsvl );
		    }
		  }
	          else
		    return -1;
	case 't': if ( vict )
	          {
		    if IS_NPC( actor )
		    {
		      lhsvl = vict->pIndexData->vnum;
		      rhsvl = atoi( val );
		      return mprog_veval( lhsvl, opr, rhsvl );
		    }
		  }
                  else
		    return -1;
	case 'r': if ( rndm )
	          {
		    if IS_NPC( actor )
		    {
		      lhsvl = rndm->pIndexData->vnum;
		      rhsvl = atoi( val );
		      return mprog_veval( lhsvl, opr, rhsvl );
		    }
		  }
	         else return -1;
	case 'o': if ( obj )
	          {
		    lhsvl = obj->pIndexData->vnum;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	case 'p': if ( v_obj )
	          {
		    lhsvl = v_obj->pIndexData->vnum;
		    rhsvl = atoi( val );
		    return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'number'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  if ( !str_cmp( buf, "name" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
	{
	case 'i': return mprog_seval( mob->name, opr, val );
	case 'n': if ( actor )
	            return mprog_seval( actor->name, opr, val );
	          else
		    return -1;
	case 't': if ( vict )
	            return mprog_seval( vict->name, opr, val );
	          else
		    return -1;
	case 'r': if ( rndm )
	            return mprog_seval( rndm->name, opr, val );
	          else
		    return -1;
	case 'o': if ( obj )
	            return mprog_seval( obj->name, opr, val );
	          else
		    return -1;
	case 'p': if ( v_obj )
	            return mprog_seval( v_obj->name, opr, val );
	          else
		    return -1;
	default:
	  bug ( "Mob: %d bad argument to 'name'", mob->pIndexData->vnum ); 
	  return -1;
	}
    }

  /* Ok... all the ifchcks are done, so if we didnt find ours then something
   * odd happened.  So report the bug and abort the MOBprogram (return error)
   */
  bug ( "Mob: %d unknown ifchck", mob->pIndexData->vnum ); 
  return -1;

}
