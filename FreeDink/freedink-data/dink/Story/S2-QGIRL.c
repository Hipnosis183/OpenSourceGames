void main ( void )
{
debug("Making girl");
preload_seq(331);
preload_seq(333);
preload_seq(337);
preload_seq(339);
sp_base_walk(&current_sprite, 330);
sp_speed(&current_sprite, 1);
//sp_timing(&current_sprite, 66);
sp_pseq(&current_sprite, 331);
sp_pframe(&current_sprite, 1);
sp_brain(&current_sprite, 16);
}

 void talk(void)
 {
  say("`#Hi, Dink.", &current_sprite);
 }
