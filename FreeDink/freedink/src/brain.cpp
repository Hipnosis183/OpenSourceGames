/**
 * Brains - Sprites AI

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2014, 2015  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "brain.h"
#include "live_sprites_manager.h"
#include "game_engine.h" /* smooth_follow */
#include "dinkc.h"
#include "gfx_sprites.h"
#include "log.h"

/*bool*/int check_for_kill_script(int i)
{
	if (spr[i].script > 0)
	{
		//if (  (spr[i].brain == 0) | (spr[i].brain == 5) | (spr[i].brain == 6) | (spr[i].brain == 7))
		
		if (locate(spr[i].script, "DIE")) run_script(spr[i].script);
		
		return(/*true*/1);	
	}
	
	return(/*false*/0);
}

void add_kill_sprite(int h)
{
	if ( (spr[h].dir > 9) || (spr[h].dir < 1) )
	{
		log_error("Changing sprites dir from %d (!?) to 3.", spr[h].dir);
		spr[h].dir = 3;
		
	}
	
	
	int dir = spr[h].dir;
	int base = spr[h].base_die;
	
	//Msg("Base die is %d", base);
	if (base == -1) 
	{
		
	  if (seq[spr[h].base_walk+5].is_active)
		{
			add_exp(spr[h].exp, h);
			
			int crap2 = add_sprite(spr[h].x,spr[h].y,5,spr[h].base_walk +5,1);
			spr[crap2].speed = 0;
			spr[crap2].seq = spr[h].base_walk + 5;   
			// set corpse size to the original sprite size
			spr[crap2].size = spr[h].size;
			return;
		} else
		{
			dir = 0;
			base = 164;
			
		}
	}
	
	
	
	if (!seq[base+dir].is_active)
	{  
		
		if (dir == 1) dir = 9;
		else if (dir == 3) dir = 7;			
		else if (dir == 7) dir = 3;			
		else if (dir == 9) dir = 1;			
		
		else if (dir == 4) dir = 6;			
		else if (dir == 6) dir = 4;			
		else if (dir == 8) dir = 2;			
		else if (dir == 2) dir = 8;			
		
		
	}
	if (!seq[base+dir].is_active)
		
	{
		log_error("Can't make a death sprite for dir %d!", base+dir);
	}
	
	
	
	int crap2 = add_sprite(spr[h].x,spr[h].y,5,base +dir,1);
	spr[crap2].speed = 0;
	spr[crap2].base_walk = 0;
	spr[crap2].seq = base + dir;
	
	if (base == 164) spr[crap2].brain = 7;
	
	spr[crap2].size = spr[h].size;
	
	add_exp(spr[h].exp, h);
	
}


int get_distance_and_dir_smooth(int h, int h1, int *dir)
{
  unsigned int x_diff = abs(spr[h].x - spr[h1].x);
  unsigned int y_diff = abs(spr[h].y - spr[h1].y);

  if (spr[h].x < spr[h1].x)
    {
      if (spr[h].y < spr[h1].y)
	{
	  // 6, 3, 2
	  if (y_diff * 4 < x_diff)
	    *dir = 6;
	  else if (x_diff * 4 < y_diff)
	    *dir = 2;
	  else
	    *dir = 3;
	}
      else if (spr[h].y > spr[h1].y)
	{
	  // 4, 9, 8
	  if (y_diff * 4 < x_diff)
	    *dir = 6;
	  else if (x_diff * 4 < y_diff)
	    *dir = 8;
	  else
	    *dir = 9;
	}
      else
	{
	  *dir = 6;
	}
    }
  else if (spr[h].x > spr[h1].x)
    {
      if (spr[h].y < spr[h1].y)
	{
	  // 4, 1, 2
	  if (y_diff * 4 < x_diff)
	    *dir = 4;
	  else if (x_diff * 4 < y_diff)
	    *dir = 2;
	  else
	    *dir = 1;
	}
      else if (spr[h].y > spr[h1].y)
	{
	  // 4, 7, 8
	  if (y_diff * 4 < x_diff)
	    *dir = 4;
	  else if (x_diff * 4 < y_diff)
	    *dir = 8;
	  else
	    *dir = 7;
	}
      else
	{
	  *dir = 4;
	}
    }
  else
    {
      if (spr[h].y < spr[h1].y)
	*dir = 2;
      else if (spr[h].y > spr[h1].y)
	*dir = 8;
    }

  return (x_diff > y_diff) ? x_diff : y_diff;
}

int get_distance_and_dir_nosmooth(int h, int h1, int *dir)
{
  int distancex = 5000;
  int distancey = 5000;
  /* Arbitrarily set to 6 to avoid uninitialized values; don't set to
     5, because *dir is added to e.g. base_attack to get the right
     sequence - with 5, you get the dead/corpse sequence instead of an
     attack sequence.. */
  int dirx = 6;
  int diry = 6;

  if ((spr[h].x > spr[h1].x) && ((spr[h].x - spr[h1].x) < distancex))
    {
      distancex = spr[h].x - spr[h1].x;
      dirx = 4;
    }
  if ((spr[h].x < spr[h1].x) && ((spr[h1].x - spr[h].x) < distancex))
    {
      distancex = spr[h1].x - spr[h].x;
      dirx = 6;
    }

  if ((spr[h].y > spr[h1].y) && ((spr[h].y - spr[h1].y) < distancey))
    {
      distancey = spr[h].y - spr[h1].y;
      diry = 8;
    }
  if ((spr[h].y < spr[h1].y) && ((spr[h1].y - spr[h].y) < distancey))
    {
      distancey = spr[h1].y - spr[h].y;
      diry = 2;
    }

  if (distancex > distancey)
    {
      *dir = dirx;
      return distancex;
    }
  else
    {
      *dir = diry;
      return distancey;
    }
}
int get_distance_and_dir(int h, int h1, int *dir)
{
  if (smooth_follow == 1)
    return get_distance_and_dir_smooth(h, h1, dir);
  else
    return get_distance_and_dir_nosmooth(h, h1, dir);
}


void change_dir_to_diag( int *dir)
{
	if (*dir == 8) *dir = 7;
	if (*dir == 4) *dir = 1;
	if (*dir == 2) *dir = 3;
	if (*dir == 6) *dir = 9;
}



void process_follow(int h)
{
	if (spr[h].follow > 299)
	{
		log_error("Sprite %d cannot 'follow' sprite %d??",h,spr[h].follow);
		return;
	}
	
	if (!spr[spr[h].follow].active)
	{
		log_debug("Killing follow");
		spr[h].follow = 0;
		return;
	}
	
	int dir;
	int distance = get_distance_and_dir(h, spr[h].follow, &dir);
	
	if (distance < 40) return;
	
	changedir(dir,h,spr[h].base_walk);
	automove(h);
}



void done_moving(int h)
{
	
	spr[h].move_active = /*false*/0;
	
	spr[h].move_nohard = /*false*/0;
	
	if (spr[h].move_script > 0)
	{
		//	Msg("mover running script %d..", spr[h].move_script);
		run_script(spr[h].move_script);
	}
	
	
	
}

void process_move(int h)
{
	
	//	Msg("Proccesing sprite %d, dir %d (script is %d)", h, spr[h].dir, spr[h].move_script);
	
	
	
	if ((spr[h].move_dir == 4) | (spr[h].move_dir == 1) | (spr[h].move_dir == 7) )
	{
		if (spr[h].x <= spr[h].move_num)
		{
			//done moving
			done_moving(h);
			return;
		}
		changedir(spr[h].move_dir,h,spr[h].base_walk);		
		automove(h);	
	}
	
	if ( (spr[h].move_dir == 6) | (spr[h].move_dir == 9) | (spr[h].move_dir == 3))
	{
		if (spr[h].x >= spr[h].move_num)
		{
			//done moving
			done_moving(h);
			return;
		}
		changedir(spr[h].move_dir,h,spr[h].base_walk);		
		automove(h);	
	}
	
	
	if (spr[h].move_dir == 2)
	{
		if (spr[h].y >= spr[h].move_num)
		{
			//done moving
			done_moving(h);
			return;
		}
		changedir(spr[h].move_dir,h,spr[h].base_walk);		
		automove(h);	
	}
	
	
	if (spr[h].move_dir == 8)
	{
		if (spr[h].y <= spr[h].move_num)
		{
			//done moving
			done_moving(h);
			return;
		}
		changedir(spr[h].move_dir,h,spr[h].base_walk);		
		automove(h);	
	}
}

void draw_damage(int h)
{
	int crap2 = add_sprite(spr[h].x,spr[h].y,8,0,0);
	
	spr[crap2].y -= k[seq[spr[h].pseq].frame[spr[h].pframe]].yoffset;
	spr[crap2].x -= k[seq[spr[h].pseq].frame[spr[h].pframe]].xoffset;
	spr[crap2].y -= k[seq[spr[h].pseq].frame[spr[h].pframe]].box.bottom / 3;
	spr[crap2].x += k[seq[spr[h].pseq].frame[spr[h].pframe]].box.right / 5;
	
	spr[crap2].speed = 1;
	spr[crap2].hard = 1;
	spr[crap2].brain_parm = h;  
	spr[crap2].my = -1;
	spr[crap2].kill_ttl = 1000;
	spr[crap2].dir = 8;
	spr[crap2].damage = spr[h].damage;
}



void changedir( int dir1, int k,int base)
        {
                int hspeed;
                int speed_hold = spr[k].speed;
                if (k > 1) if (spr[k].brain != 9) if (spr[k].brain != 10)
                {
                        hspeed = spr[k].speed * (base_timing / 4);
                        if (hspeed > 49)
                        {
                                log_debug("Speed was %d", hspeed);
                                spr[k].speed = 49;
                        } else
                                spr[k].speed = hspeed;
                }
                int old_seq = spr[k].seq;
                spr[k].dir = dir1;

                if (dir1 == 1)
                {
                        spr[k].mx = (0 - spr[k].speed ) + (spr[k].speed / 3);
                        spr[k].my = spr[k].speed - (spr[k].speed / 3);

                        if (base != -1)
                        {


                                spr[k].seq = base + 1;
                                if (!seq[spr[k].seq].is_active)
                                {
                                        spr[k].seq = base + 9;

                                }

                        }

                        if (old_seq != spr[k].seq)
                        {
                                spr[k].frame = 0;
                                spr[k].delay = 0;
                        }


                }

                if (dir1 == 2)
                {
                        spr[k].mx = 0;
                        spr[k].my = spr[k].speed;
                        if (base != -1)
                                spr[k].seq = base + 2;

                        if (!seq[spr[k].seq].is_active && seq[base+3].is_active)
			  spr[k].seq = base + 3;
                        if (!seq[spr[k].seq].is_active && seq[base+1].is_active)
			  spr[k].seq = base + 1;


                        if (old_seq != spr[k].seq)
                        {
                                spr[k].frame = 0;
                                spr[k].delay = 0;
                        }


                }
                if (dir1 == 3)
                {
                        spr[k].mx = spr[k].speed - (spr[k].speed / 3);
                        spr[k].my = spr[k].speed - (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 3;
                                if (!seq[spr[k].seq].is_active)
                                        spr[k].seq = base + 7;

                        }

                        if (old_seq != spr[k].seq)
                        {
                                spr[k].frame = 0;
                                spr[k].delay = 0;
                        }


                }

                if (dir1 == 4)
                {

                        //Msg("Changing %d to four..",k);
                        spr[k].mx = (0 - spr[k].speed);
                        spr[k].my = 0;
                        if (base != -1)
                                spr[k].seq = base + 4;
                        if (!seq[spr[k].seq].is_active && seq[base+7].is_active)
			  spr[k].seq = base + 7;
                        if (!seq[spr[k].seq].is_active && seq[base+1].is_active)
			  spr[k].seq = base + 1;
                }

                if (dir1 == 6)
                {
                        spr[k].mx = spr[k].speed;
                        spr[k].my = 0;
                        if (base != -1)
                                spr[k].seq = base + 6;

                        if (!seq[spr[k].seq].is_active && seq[base+3].is_active)
			  spr[k].seq = base + 3;
                        if (!seq[spr[k].seq].is_active && seq[base+9].is_active)
			  spr[k].seq = base + 9;

                }

                if (dir1 == 7)
                {
                        spr[k].mx = (0 - spr[k].speed) + (spr[k].speed / 3);
                        spr[k].my = (0 - spr[k].speed)+ (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 7;


                                if (!seq[spr[k].seq].is_active)
				  spr[k].seq = base + 3;
                        }

                }
                if (dir1 == 8)
                {
                        spr[k].mx = 0;
                        spr[k].my = (0 - spr[k].speed);
                        if (base != -1)
                                spr[k].seq = base + 8;

                        if (!seq[spr[k].seq].is_active && seq[base+7].is_active)
			  spr[k].seq = base + 7;
                        if (!seq[spr[k].seq].is_active && seq[base+9].is_active)
			  spr[k].seq = base + 9;

                }


                if (dir1 == 9)
                {
                        spr[k].mx = spr[k].speed- (spr[k].speed / 3);
                        spr[k].my = (0 - spr[k].speed)+ (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 9;
                                if (!seq[spr[k].seq].is_active)
                                        spr[k].seq = base + 1;
                        }
                }



                if (old_seq != spr[k].seq)
                {
                        spr[k].frame = 0;
                        spr[k].delay = 0;
                }


                if (!seq[spr[k].seq].is_active)
                {
                        //spr[k].mx = 0;
                        //spr[k].my = 0;
                        spr[k].seq = old_seq;

                }

                //Msg("Leaving with %d..", spr[k].dir);

                //Msg("Changedir: Tried to switch sprite %d to dir %d",k,dir1);

                spr[k].speed = speed_hold;

}

void automove (int j)
{
	
	char kindx,kindy;
	int speedx = 0;
	int speedy = 0;
	
	
	
	
	if (spr[j].mx != 0)
	{ 
		if (spr[j].mx < 0)
			kindx = '-'; else kindx = '+';
		if (kindx == '-') speedx = (spr[j].mx - (spr[j].mx * 2)); else
			speedx = spr[j].mx;
	} else kindx = '0';
	
	if (spr[j].my != 0)
	{ 
		if (spr[j].my < 0)
			kindy = '-'; else kindy = '+';
		if (kindy == '-') speedy = (spr[j].my - (spr[j].my * 2)); else
			speedy = spr[j].my;
		
	} else kindy = '0';
	
	int speed = speedx;
	if (speedy > speedx) speed = speedy;
	if (speed > 0)
		move(j,speed,kindx,kindy);
	//move(j, 1, '+','+'); 
	
}

void move(int u, int amount, char kind,  char kindy)
{
	int mx = 0;
	int my = 0;	
	int i;
	/*bool*/int clearx;
	/*bool*/int cleary;
	clearx = /*false*/0;
	cleary = /*false*/0;
	
	for (i = 1; i <= amount; i++)
	{
		spr[u].moveman++;
		if (mx >= spr[u].mx) clearx = /*true*/1;
		if (my >= spr[u].my) clearx = /*true*/1;
		
		if ((clearx) && (cleary))
		{
			mx = 0;
			my = 0;
			clearx = /*false*/0;
			cleary = /*false*/0;
			
		}
		
		
		if (kind == '+')
		{
			if (mx < spr[u].mx)
				spr[u].x++;
			mx++;
			
		}
		if (kind == '-')
		{
			
			
			if (mx < (spr[u].mx - (spr[u].mx * 2)))
				spr[u].x--;
			mx++;
		}
		
		if (kindy == '+')
		{
			
			if (my < spr[u].my)
				spr[u].y++;
			my++;
		}
		if (kindy == '-')
		{
			
			if (my < (spr[u].my - (spr[u].my * 2)))
				spr[u].y--;
			my++;
		}
		
		spr[u].lpx[spr[u].moveman] = spr[u].x;
        spr[u].lpy[spr[u].moveman] = spr[u].y;
	}
	
	
}


int autoreverse(int j)
{
	//Msg("reversing die %d",spr[j].dir);
	int r = ((rand() % 2)+1);	
	if ( (spr[j].dir == 1) || (spr[j].dir == 2) ) 
	{
		if (r == 1)
			return(8);
		if (r == 2)
			return(6);
		
	}
	
	if ( (spr[j].dir == 3) || (spr[j].dir == 6) ) 
	{
		if (r == 1)
			return(2);
		if (r == 2)
			
			return(4);
		
	}
	
    if ( (spr[j].dir == 9) || (spr[j].dir == 8) ) 
	{
		if (r == 1)
			return(2);
		if (r == 2)
			
			return(6);
		
		
	}
	
    if ( (spr[j].dir == 7) || (spr[j].dir == 4) ) 
	{
		if (r == 1)
			return(8);
		if (r == 2)
			return(6);
		
	}
	
	return(0);
}


int autoreverse_diag(int j)
{
	if (spr[j].dir == 0) spr[j].dir = 7;
	int r = ((rand() % 2)+1);	
	
	if ( (spr[j].dir == 1) || (spr[j].dir == 3) ) 
	{
		
		if (r == 1)
			return(9);
		if (r == 2)
			return(7);
	}
	
	if ( (spr[j].dir == 3) || (spr[j].dir == 6) ) 
	{
		if (r == 1)
			return(7);
		if (r == 2)
			return(1);
		
	}
	
    if ( (spr[j].dir == 9) || (spr[j].dir == 8) ) 
	{
		if (r == 1)
			return(1);
		if (r == 2)
			return(7);
	}
	
    if ( (spr[j].dir == 7) || (spr[j].dir == 4) ) 
	{
		if (r == 1)
			return(3);
		if (r == 2)
			return(9);
		
	}
	
	log_debug("Auto Reverse Diag was sent a dir %d sprite, base %d walk.",spr[j].dir, spr[j].base_walk);
	return(0);
}
